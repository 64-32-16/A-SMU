#include "UiTiming.h"
#if ASMU_UI_TIMING
namespace UiTiming {
Samples samples[Count];
uint32_t lastPoll=0,lastReport=0,gapsOver20ms=0;
bool havePoll=false;
char pending[768];
size_t pendingSize=0,pendingOffset=0;
unsigned lastPlotSamples=0,lastPlotBytes=0;
bool lastPlotVector=false;
Geometry geometry;
}
#endif
