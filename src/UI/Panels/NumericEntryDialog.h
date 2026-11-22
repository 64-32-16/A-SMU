#pragma once
#include "../Core/Panel.h"
#include "../Controls/Button.h"

// Reusable numeric entry in the SET LIMIT style. Owner validates/commits the value.
class NumericEntryDialog : public Panel {
public:
    using AcceptHandler=bool (*)(void*,double);
    NumericEntryDialog();
    void Open(const char* title,double value,AcceptHandler accept,void* context);
    void Close();
    void Draw() override;
    void TouchEnd(int16_t x,int16_t y) override;
private:
    void Key(unsigned key);
    Button _keys[17];
    char _title[80]={},_input[32]={};
    bool _replace=true,_invalid=false;
    AcceptHandler _accept=nullptr;
    void* _context=nullptr;
};
