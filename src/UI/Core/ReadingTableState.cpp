#include "ReadingTableState.h"
#include <math.h>
void ReadingTableState::Refresh(const BufferClass& buffer)
{
    if(_hold) return;
    _buffer=&buffer;_generation=buffer.GetResetGeneration();
    _count=buffer.GetCount();
    _firstIndex=buffer.GetTotalSamples()-_count+1;_origin=buffer.GetStartTimeMs();
    _offset=LastOffset();_selected=-1;_pageUnavailable=false;
    const auto samples=buffer.View(PageCapacity);
    _cacheCount=samples.Count();_cacheOffset=_count-_cacheCount;
    for(uint16_t i=0;i<_cacheCount;++i) _samples[i]=samples[i];
}
void ReadingTableState::SetLive(const BufferClass& buffer) {_hold=false;Refresh(buffer);}

bool ReadingTableState::LoadPage(uint16_t offset)
{
    const uint16_t end=offset+Rows<_count?offset+Rows:_count;
    if(offset>=_cacheOffset && end<=_cacheOffset+_cacheCount) return true;
    // Never silently mix generations or substitute newer readings for overwritten ones.
    if(!_buffer || _generation!=_buffer->GetResetGeneration()) return false;
    const auto live=_buffer->View();
    const uint64_t firstLive=_buffer->GetTotalSamples()-live.Count()+1;
    if(Index(offset)<firstLive || Index(end)>_buffer->GetTotalSamples()+1) return false;
    uint16_t start=offset/PageCapacity*PageCapacity;
    if(end>start+PageCapacity) start=offset;
    if(_count>PageCapacity && start>_count-PageCapacity) start=_count-PageCapacity;
    if(Index(start)<firstLive) start=static_cast<uint16_t>(firstLive-_firstIndex);
    _cacheOffset=start;
    _cacheCount=_count-start<PageCapacity?_count-start:PageCapacity;
    const uint16_t source=static_cast<uint16_t>(Index(start)-firstLive);
    for(uint16_t i=0;i<_cacheCount;++i) _samples[i]=live[source+i];
    _selected=-1;
    return true;
}
bool ReadingTableState::Navigate(uint16_t offset)
{
    Hold();
    if(!LoadPage(offset)) {_pageUnavailable=true;return false;}
    _pageUnavailable=false;_offset=offset;return true;
}
void ReadingTableState::Page(int direction)
{
    uint16_t next=_offset;
    if(direction<0) next=_offset>=Rows?_offset-Rows:0;
    else if(direction>0 && _offset<LastOffset()) {
        next=_offset+Rows;
        if(next>LastOffset()) next=LastOffset();
    }
    Navigate(next);
}
void ReadingTableState::First() {Navigate(0);}
void ReadingTableState::Last() {Navigate(LastOffset());}
bool ReadingTableState::Select(uint16_t index)
{
    if(index>=_count) return false;
    // Keep selection within the frozen page when possible, including its last few rows.
    uint16_t offset=(index/Rows)*Rows;
    if(offset>LastOffset()) offset=LastOffset();
    if(index>=_cacheOffset && index<_cacheOffset+_cacheCount && _cacheCount>=Rows) {
        if(offset<_cacheOffset) offset=_cacheOffset;
        if(offset+Rows>_cacheOffset+_cacheCount) offset=_cacheOffset+_cacheCount-Rows;
    }
    if(!Navigate(offset)) return false;
    _selected=index;return true;
}
bool ReadingTableState::SelectTime(double fraction)
{
    if(!_cacheCount || !isfinite(fraction)) return false;
    fraction=fmax(0,fmin(1,fraction));
    const uint16_t first=_cacheOffset,last=first+_cacheCount-1;
    const double target=TimeMs(first)+fraction*(static_cast<double>(TimeMs(last))-TimeMs(first));
    uint16_t low=first,high=last+1;
    while(low<high) {const uint16_t mid=(low+high)/2;if(TimeMs(mid)<target) low=mid+1;else high=mid;}
    if(low>last) --low;
    else if(low>first && target-TimeMs(low-1)<=TimeMs(low)-target) --low;
    return Select(low);
}
