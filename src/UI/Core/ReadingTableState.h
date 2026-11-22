#pragma once
#include "../../Core/Buffer.h"

// Presentation snapshot: Hold is independent of acquisition and buffer ownership.
class ReadingTableState {
public:
    static constexpr uint16_t Rows=8;
    void Refresh(const BufferClass& buffer);
    void SetLive(const BufferClass& buffer);
    void Hold() { _hold=true; }
    void Page(int direction);
    void First();
    void Last();
    bool Select(uint16_t index);
    bool SelectTime(double fraction);
    bool IsHeld() const {return _hold;}
    uint16_t Count() const {return _count;}
    uint16_t Offset() const {return _offset;}
    int Selected() const {return _selected;}
    uint16_t LastOffset() const {return _count>Rows?_count-Rows:0;}
    uint64_t Index(uint16_t index) const {return _firstIndex+index;}
    uint32_t TimeMs(uint16_t index) const {return static_cast<uint32_t>(_samples[index].TimeMs-_origin);}
    uint32_t Origin() const {return _origin;}
    const BufferSample* Samples() const {return _samples;}
private:
    BufferSample _samples[BufferClass::Capacity];
    uint16_t _count=0,_offset=0;
    uint64_t _firstIndex=1;
    uint32_t _origin=0;
    int _selected=-1;
    bool _hold=false;
};
