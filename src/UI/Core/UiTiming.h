#pragma once

// Set to 0 in platformio.ini to remove all instrumentation and storage.
#ifndef ASMU_UI_TIMING
#define ASMU_UI_TIMING 0
#endif

#if ASMU_UI_TIMING
#include <Arduino.h>
#include <stdio.h>

namespace UiTiming {
enum Metric { Gap, System, Input, Dispatch, Begin, Draw, Swap,
              MainGraph, PlotPrepare, PlotRaster, PlotUpload, PlotVector, PlotCommands, Count };
struct Samples {
    uint64_t sum = 0;
    uint32_t count = 0;
    uint32_t maximum = 0;
    void Add(uint32_t us) {
        sum += us;
        ++count;
        if (us > maximum) maximum = us;
    }
};
// Main-loop only. No allocation, sample history or access from acquisition.
// Shared across main.cpp, GraphPanel.cpp and GDClass.cpp. Header-local copies
// would silently hide the detailed measurements from the serial report.
extern Samples samples[Count];
extern uint32_t lastPoll, lastReport, gapsOver20ms;
extern bool havePoll;
extern char pending[768];
extern size_t pendingSize, pendingOffset;
extern unsigned lastPlotSamples, lastPlotBytes;
extern bool lastPlotVector;
struct Geometry {
    double xStart=0;
    unsigned shifts=0,vertices=0,strips=0,outside=0,dlMax=0;
    int minX=0,maxX=0,minY=0,maxY=0;
    bool haveWindow=false,haveVertices=false;
};
extern Geometry geometry;
inline void PlotWindow(double start) {
    if(geometry.haveWindow && start!=geometry.xStart) ++geometry.shifts;
    geometry.xStart=start;geometry.haveWindow=true;
}
inline void PlotInfo(unsigned points,unsigned bytes,bool vector=false) {
    lastPlotSamples=points;lastPlotBytes=bytes;
    lastPlotVector=vector;
}

class Scope {
    Metric metric;
    uint32_t start;
public:
    explicit Scope(Metric value) : metric(value), start(micros()) {}
    ~Scope() { samples[metric].Add(static_cast<uint32_t>(micros() - start)); }
};

inline void Poll() {
    const uint32_t now = micros();
    if (havePoll) {
        const uint32_t gap = now - lastPoll;
        samples[Gap].Add(gap);
        if (gap > 20000) ++gapsOver20ms;
    }
    lastPoll = now;
    havePoll = true;
}

inline void Report() {
    const uint32_t now = millis();
    if (!lastReport) { lastReport = now; return; }
    if (now - lastReport >= 2000 && pendingOffset == pendingSize) {
        const uint32_t window = now - lastReport;
        lastReport = now;
        pendingOffset = pendingSize = 0;
        if (Serial) {
            pendingSize = snprintf(pending, sizeof(pending),
                "UI ms=%lu window=%lu gap>20ms=%lu plot_samples=%u plot_bytes=%u plot_mode=%s x0=%.3f shifts=%u vn=%u vs=%u vx=%d:%d vy=%d:%d outside=%u dlmax=%u; us avg/max/n:",
                (unsigned long)now, (unsigned long)window, (unsigned long)gapsOver20ms,
                lastPlotSamples,lastPlotBytes,lastPlotVector?"vector":"bitmap",
                geometry.xStart,geometry.shifts,geometry.vertices,geometry.strips,
                geometry.minX,geometry.maxX,geometry.minY,geometry.maxY,geometry.outside,geometry.dlMax);
            static const char* const names[Count] = {
                "gap", "system", "input", "dispatch", "begin", "draw", "swap",
                "main_graph", "plot_prepare", "plot_raster", "plot_upload", "plot_vector", "plot_commands"
            };
            for (unsigned i = 0; i < Count; ++i) {
                const Samples& s = samples[i];
                const int written = snprintf(pending + pendingSize, sizeof(pending) - pendingSize,
                    " %s=%lu/%lu/%lu", names[i],
                    (unsigned long)(s.count ? s.sum / s.count : 0),
                    (unsigned long)s.maximum, (unsigned long)s.count);
                if (written < 0) break;
                const size_t remaining = sizeof(pending) - pendingSize;
                if (static_cast<size_t>(written) >= remaining) {
                    pendingSize = sizeof(pending) - 2;
                    break;
                }
                pendingSize += written;
            }
            pending[pendingSize++] = '\n';
        }
        for (auto& s : samples) s = Samples{};
        gapsOver20ms = 0;
        lastPlotSamples=lastPlotBytes=0;
        lastPlotVector=false;
        geometry.shifts=geometry.vertices=geometry.strips=geometry.outside=geometry.dlMax=0;
        geometry.minX=geometry.maxX=geometry.minY=geometry.maxY=0;
        geometry.haveVertices=false;
    }
    // USB serial must never hold up touch waiting for a reader. Drain in small
    // chunks; if the connection disappears, discard the outstanding report.
    if (pendingOffset == pendingSize) return;
    if (!Serial) { pendingOffset = pendingSize = 0; return; }
    if (pendingOffset < pendingSize) {
        size_t bytes = pendingSize - pendingOffset;
        if (bytes > 32) bytes = 32;
        // Core 0 is the only USB writer. Never submit more than the free TX
        // space: Arduino-Pico's write() otherwise waits for the host to drain it.
        const int available = Serial.availableForWrite();
        if (available <= 0) return;
        if (bytes > static_cast<size_t>(available)) bytes = available;
        pendingOffset += Serial.write(
            reinterpret_cast<const uint8_t*>(pending + pendingOffset), bytes);
    }
}
}
#define UI_TIMED(metric, ...) do { UiTiming::Scope timing(UiTiming::metric); __VA_ARGS__; } while (0)
#define UI_TIMING_SCOPE(metric) UiTiming::Scope uiTimingScope##metric(UiTiming::metric)
#define UI_PLOT_INFO(points, bytes) UiTiming::PlotInfo(points, bytes)
#define UI_PLOT_VECTOR_INFO(points, bytes) UiTiming::PlotInfo(points, bytes, true)
#define UI_TOUCH_POLL() UiTiming::Poll()
#define UI_TIMING_REPORT() UiTiming::Report()
#else
#define UI_TIMED(metric, ...) do { __VA_ARGS__; } while (0)
#define UI_TIMING_SCOPE(metric) do {} while (0)
#define UI_PLOT_INFO(points, bytes) do {} while (0)
#define UI_PLOT_VECTOR_INFO(points, bytes) do {} while (0)
#define UI_TOUCH_POLL() do {} while (0)
#define UI_TIMING_REPORT() do {} while (0)
#endif
