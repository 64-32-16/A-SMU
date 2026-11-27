#pragma once
#include "GraphState.h"
#include "GDClass.h"
#include "Theme.h"
#include "../../Core/GraphSettings.h"

// One plot renderer; pages supply position, labels and visible curves.
namespace GraphPlot {
struct Layout {
    int x,y,width,height,yLabelX,xLabelOffset;
    uint32_t xLabelColor;
    bool iv=false,compact=false;
};
inline bool Draw(const GraphState& state,BufferView samples,const BufferClass& buffer,
                 GraphStyle style,const Layout& area,const uint32_t colors[4]) {
    GD.DrawPlotGrid(area.x,area.y,area.width,area.height,
                    GraphState::XDivisions,GraphState::YDivisions,0x303030);
    const auto& xr=state.X();
    const auto& yr=state.y[state.selected];
    static const char* const units[]={"V","A","W","Ohm"};
    char label[40];
    const unsigned yStep=area.compact?2:1;
    for(unsigned i=0;i<=GraphState::YDivisions;i+=yStep) {
        GraphState::FormatAxis(yr,GraphState::YDivisions,i,units[state.selected],label,sizeof(label));
        GD.ColorRGB(colors[state.selected]);
        GD.cmd_text(area.yLabelX,area.y+area.height-i*area.height/GraphState::YDivisions,
                    26,OPT_CENTERY,label);
    }
    for(unsigned i=0;i<=GraphState::XDivisions;i+=2) {
        if(area.iv) GraphState::FormatAxis(xr,GraphState::XDivisions,i,"V",label,sizeof(label));
        else GraphState::FormatTime(xr.start+xr.division*i,xr.division*2,label,sizeof(label));
        GD.ColorRGB(area.xLabelColor);
        const uint16_t option=area.compact && i==GraphState::XDivisions
            ?(OPT_RIGHTX|OPT_CENTERY):area.compact?OPT_CENTERY:OPT_CENTER;
        GD.cmd_text(area.x+area.width*i/GraphState::XDivisions,
                    area.y+area.height+area.xLabelOffset,26,option,label);
    }
    bool any=false;
    const uint16_t count=samples.Count();
    const unsigned firstIndex=buffer.GetTotalSamples()-count;
    for(unsigned q=0;q<4;++q) {
        if(!state.IsCurveVisible(q)) continue;
        const GraphState::Trace trace{&state,samples,static_cast<GraphState::Quantity>(q)};
        const PlotEverySecondSource reduced{trace.Source(count),static_cast<uint8_t>(firstIndex&1)};
        const bool lines=style!=GraphStyle::Points;
        any=GD.DrawPlotTrace(area.x,area.y,area.width,area.height,
            lines?reduced.Source():trace.Source(count),xr,state.y[q],colors[q],
            lines,style!=GraphStyle::Lines,lines) || any;
    }
    return any;
}
}
