#ifndef _DAC_OUTPUT_H_INCLUDED
#define _DAC_OUTPUT_H_INCLUDED

class DacOutput
{
public:
    virtual ~DacOutput() {}

    virtual void Begin() = 0;
    virtual void SetVSet(float volts) = 0;
    virtual void SetILimit(float volts) = 0;
    virtual void ZeroOutputs() = 0;
};

#endif
