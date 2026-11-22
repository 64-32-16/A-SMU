#pragma once
#include "Buffer.h"

enum class HistogramScale : uint8_t { SmartScale=0, AutoBin=1, Fit=2, Off=3 };
struct HistogramSettings {
    static constexpr uint16_t MaxBins=64;
    BufferQuantity quantity=BufferQuantity::Voltage;
    bool enabled=true;
    uint16_t bins=20;
    double minimum=-1, maximum=1;
    HistogramScale method=HistogramScale::SmartScale;
    uint16_t yMaximum=10;
};
struct HistogramResult {
    uint16_t counts[HistogramSettings::MaxBins+2]={};
    uint16_t bins=20, valid=0, invalid=0, peak=0, yMaximum=10;
    double minimum=-1, maximum=1, width=0.1, dataMin=0, dataMax=0;
    bool hasData=false;
    // index 0 = low outlier, 1..bins = regular bins, bins+1 = high outlier.
    int BinAt(double value) const;
};
bool ValidHistogramSettings(const HistogramSettings& settings);
void CalculateHistogram(const BufferSample* samples,uint16_t count,
                        const HistogramSettings& settings,HistogramResult& result);
