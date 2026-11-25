#pragma once
#include "../../Core/Buffer.h"

// One bounded page snapshot; the rest of the history stays in the central ring.
class ReadingTableState {
public:
    static constexpr uint16_t Rows=8, PageCapacity=50;
    void Refresh(const BufferClass& buffer);
    void SetLive(const BufferClass& buffer);
    void Hold() { _hold=true; }
    void Page(int direction);
    void First();
    void Last();
    bool Select(uint16_t index);
    bool SelectTime(double fraction);
    bool IsHeld() const {return _hold;}
    bool PageUnavailable() const {return _pageUnavailable;}
    uint16_t Count() const {return _count;}
    uint16_t Offset() const {return _offset;}
    int Selected() const {return _selected;}
    uint16_t LastOffset() const {return _count>Rows?_count-Rows:0;}
    uint64_t Index(uint16_t index) const {return _firstIndex+index;}
    uint32_t TimeMs(uint16_t index) const {return static_cast<uint32_t>(Sample(index).TimeMs-_origin);}
    uint32_t Origin() const {return _origin;}
    // Logical indices are relative to the retained history at the last LIVE refresh.
    // Only the cached interval can be dereferenced; navigation loads it before use.
    const BufferSample& Sample(uint16_t index) const {return _samples[index-_cacheOffset];}
    BufferView CachedSamples() const {return BufferView(_samples,_cacheCount);}
    uint16_t CacheOffset() const {return _cacheOffset;}
    uint16_t CachedCount() const {return _cacheCount;}
private:
    bool Navigate(uint16_t offset);
    bool LoadPage(uint16_t offset);
    BufferSample _samples[PageCapacity];
    const BufferClass* _buffer=nullptr;
    uint32_t _generation=0;
    uint16_t _cacheOffset=0,_cacheCount=0;
    uint16_t _count=0,_offset=0;
    uint64_t _firstIndex=1;
    uint32_t _origin=0;
    int _selected=-1;
    bool _hold=false,_pageUnavailable=false;
};
