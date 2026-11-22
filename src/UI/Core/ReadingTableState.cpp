#include "ReadingTableState.h"
#include <math.h>
void ReadingTableState::Refresh(const BufferClass& buffer)
{
    if(_hold) return;
    _count=buffer.CopyLatest(_samples,BufferClass::Capacity);
    _firstIndex=buffer.GetTotalSamples()-_count+1;_origin=buffer.GetStartTimeMs();
    _offset=LastOffset();_selected=-1;
}
void ReadingTableState::SetLive(const BufferClass& buffer) {_hold=false;Refresh(buffer);}
void ReadingTableState::Page(int direction)
{
    Hold();
    if(direction<0) _offset=_offset>=Rows?_offset-Rows:0;
    else if(direction>0 && _offset<LastOffset()) {
        const uint16_t next=_offset+Rows;
        _offset=next<LastOffset()?next:LastOffset();
    }
}
void ReadingTableState::First() {Hold();_offset=0;}
void ReadingTableState::Last() {Hold();_offset=LastOffset();}
bool ReadingTableState::Select(uint16_t index)
{
    if(index>=_count) return false;
    Hold();_selected=index;
    const uint16_t page=(index/Rows)*Rows;
    _offset=page<LastOffset()?page:LastOffset();return true;
}
bool ReadingTableState::SelectTime(double fraction)
{
    if(!_count || !isfinite(fraction)) return false;
    fraction=fmax(0,fmin(1,fraction));
    const double target=TimeMs(0)+fraction*(static_cast<double>(TimeMs(_count-1))-TimeMs(0));
    uint16_t low=0,high=_count;
    while(low<high) {const uint16_t mid=(low+high)/2;if(TimeMs(mid)<target) low=mid+1;else high=mid;}
    if(low==_count) --low;
    else if(low && target-TimeMs(low-1)<=TimeMs(low)-target) --low;
    return Select(low);
}
