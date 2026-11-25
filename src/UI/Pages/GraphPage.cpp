#include "GraphPage.h"
#include "../Core/GDClass.h"
#include "../Core/Theme.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

namespace
{
constexpr int plotX=136,plotY=108,plotW=636,plotH=224;
const char* names[]={"Voltage","Current","Power","Resistance"};
const char* units[]={"V","A","W","Ohm"};
const uint32_t colors[]={0x40FF60,0x40D8FF,0xFFE050,0xFF80DC};
const char* keyNames[]={"0","1","2","3","4","5","6","7","8","9",".","-","E","DEL","CLR","OK","CANCEL"};
void Text(int x,int y,const char* text,uint32_t color=0xF2F2F2,uint8_t font=28,uint16_t options=0)
{ GD.ColorRGB(color);GD.cmd_text(x,y,font,options,text); }
void Style(Button& button)
{
    button.SetFont(28);button.SetBackgroundColor(Theme::MainButtonBackground);
    button.SetTextColor(Theme::MainButtonText);button.SetCornerRadius(0);
    button.SetDisabledBackgroundColor(0x303030);button.SetDisabledTextColor(0xAAAAAA);
}
}

GraphPage::GraphPage(int16_t x,int16_t y,int16_t w,int16_t h):Panel(x,y,w,h),_system(&System)
{
    SetShowBackground(false);SetShowBorder(false);SetShowTitle(false);
    for(auto& button:_buttons) {Style(button);AddChild(&button);}
    _editor.SetShowBackground(false);_editor.SetShowBorder(false);_editor.SetShowTitle(false);
    for(unsigned i=0;i<17;++i) {
        _keys[i].SetNativeFlat(false);
        _keys[i].SetFont(Theme::FontPopupButton);
        _keys[i].SetBackgroundColor(Theme::ToolbarButtonBackground);
        _keys[i].SetTextColor(Theme::ToolbarButtonText);
        _keys[i].SetText(keyNames[i]);_editor.AddChild(&_keys[i]);
    }
    const int wx=(800-Theme::NumericPopupWidth)/2,wy=(480-Theme::NumericPopupHeight)/2;
    for(unsigned i=1;i<=9;++i) _keys[i].SetBounds(wx+30+((i-1)%3)*94,wy+124+((i-1)/3)*66,86,58);
    _keys[0].SetBounds(wx+30,wy+322,180,58);
    _keys[10].SetBounds(wx+312,wy+322,86,58); // Decimal point
    _keys[11].SetBounds(wx+218,wy+322,86,58); // Minus, including negative exponents
    _keys[12].SetBounds(wx+312,wy+124,86,58); // Scientific notation
    _keys[13].SetBounds(wx+430,wy+124,140,58);_keys[13].SetText("Back");
    _keys[14].SetBounds(wx+430,wy+190,140,58);_keys[14].SetText("Clear");
    _keys[15].SetBounds(wx+430,wy+322,140,58);
    _keys[16].SetBounds(wx+430,wy+256,140,58);_keys[16].SetText("Cancel");
    _editor.SetBounds(0,0,800,480);_editor.SetVisible(false);AddChild(&_editor);
    _tabs.AddTab("Graph");_tabs.AddTab("Data");_tabs.AddTab("Scale");_tabs.AddTab("Trigger");
    _tabs.SetBounds(8,4,480,44);AddChild(&_tabs);
    SetVisible(false);Layout();
}
void GraphPage::SetSystem(SystemClass* system) {_system=system?system:&System;}
void GraphPage::SetOnBack(ButtonClickHandler handler) {_buttons[5].SetOnClick(handler);}
void GraphPage::SetOnHome(ButtonClickHandler handler) {_buttons[6].SetOnClick(handler);}
void GraphPage::Open()
{
    Panel::TouchEnd(-1,-1);_drag=false;_editing=-1;_editor.SetVisible(false);
    if(!_initialized)
    {
        switch(_system->GetMeasureMode()) {
        case MeasureMode::Current:_state.selected=GraphState::Current;break;
        case MeasureMode::Power:_state.selected=GraphState::Power;break;
        case MeasureMode::Resistance:_state.selected=GraphState::Resistance;break;
        default:break;
        }
        for(unsigned q=0;q<4;++q) _state.enabled[q]=q==_state.selected;
        _firstCurve=_state.selected;
        _initialized=true;
    }
    SetVisible(true);ReadBuffer();Layout();
}
void GraphPage::ReadBuffer()
{
    _samples=_system->Buffer.View();
    _count=_samples.Count();
    _state.Resolve(_system->Buffer,_samples,MeasurementTiming::PairTimeUs(_system->GetNplcProfile()));
}
void GraphPage::Configure(unsigned id,const char* text,int x,int y,int w,int h)
{
    snprintf(_captions[id],sizeof(_captions[id]),"%s",text);
    _buttons[id].SetText(_captions[id]);_buttons[id].SetBounds(x,y,w,h);_buttons[id].SetVisible(true);
}
void GraphPage::Layout()
{
    for(auto& b:_buttons) {b.SetVisible(false);b.SetEnabled(true);b.SetBackgroundColor(Theme::MainButtonBackground);}
    _tabs.SetVisible(true);
    Configure(4,"AUTO SCALE",650,8,142,36);
    Configure(5,"BACK",8,52,80,34);Configure(6,"HOME",96,52,80,34);
    if(_tabs.GetSelectedIndex()!=3) for(unsigned q=0;q<4;++q)
    {
        Configure(7+q,names[q],192+q*146,52,138,34);
        if(q==_state.selected) _buttons[7+q].SetBackgroundColor(CurveColor(q));
        if(_state.iv && q!=GraphState::Current) _buttons[7+q].SetEnabled(false);
    }
    char text[56];
    if(_tabs.GetSelectedIndex()==0)
    {
        Configure(24,"SCALE",8,366,94,32);Configure(25,"STATS",108,366,94,32);
        Configure(26,"CURSOR",208,366,94,32);
        _buttons[24+_info].SetBackgroundColor(Theme::SourceBlue);
        if(_info==2)
        {
            const char* modes[]={"Off","Vert","Horiz","Both"};
            snprintf(text,sizeof(text),"Mode: %s",modes[_state.cursorMode]);Configure(11,text,310,366,154,32);
            Configure(12,_state.cursor==0?"Move A":"Move B",472,366,102,32);
        }
        // Zoom direction is inferred from the pinch gesture; no mode button.
    }
    else if(_tabs.GetSelectedIndex()==1)
    {
        Configure(11,_state.IsCurveVisible(_state.selected)?"Remove curve":"Add curve",24,170,196,44);
        Configure(12,"Line",24,248,130,44);Configure(13,"Points",166,248,130,44);Configure(14,"Both",308,248,130,44);
        _buttons[12+static_cast<unsigned>(_system->GetGraphSettings().style)].SetBackgroundColor(Theme::SourceBlue);
        Configure(15,"Value / Time",24,326,196,44);Configure(16,"I / V",232,326,170,44);
        _buttons[_state.iv?16:15].SetBackgroundColor(Theme::SourceBlue);
        Configure(17,"Clear measurement buffer",448,326,322,44);
    }
    else if(_tabs.GetSelectedIndex()==2)
    {
        char number[32];
        Format(_state.X().start,_state.iv?"V":"s",number,sizeof(number));snprintf(text,sizeof(text),"X start: %s",number);Configure(11,text,24,146,350,48);
        Format(_state.X().division,_state.iv?"V":"s",number,sizeof(number));snprintf(text,sizeof(text),"X / div: %s",number);Configure(12,text,410,146,366,48);
        Format(_state.y[_state.selected].start,units[_state.selected],number,sizeof(number));snprintf(text,sizeof(text),"Y start: %s",number);Configure(13,text,24,218,350,48);
        Format(_state.y[_state.selected].division,units[_state.selected],number,sizeof(number));snprintf(text,sizeof(text),"Y / div: %s",number);Configure(14,text,410,218,366,48);
        Configure(15,"Step scrolling",24,304,216,44);Configure(16,"Full buffer",252,304,216,44);
        _buttons[_state.scrolling?15:16].SetBackgroundColor(Theme::SourceBlue);
        Format(_state.windowSeconds,"s",number,sizeof(number));snprintf(text,sizeof(text),"%s: %s",_state.scrolling?(_state.automaticWindow?"Auto window":"Window"):"Min span",number);Configure(17,text,480,304,296,44);
        if(_state.iv) {_buttons[15].SetEnabled(false);_buttons[16].SetEnabled(false);_buttons[17].SetEnabled(false);}
    }
    else
    {
        Configure(11,"Configure: not implemented",24,226,354,48);_buttons[11].SetEnabled(false);
        Configure(12,"Single: not implemented",410,226,366,48);_buttons[12].SetEnabled(false);
        Configure(13,"Templates: not implemented",24,294,354,48);_buttons[13].SetEnabled(false);
    }
}
void GraphPage::Format(double value,const char* unit,char* text,size_t size)
{
    GraphState::FormatValue(value,unit,text,size);
}
void GraphPage::Action(unsigned id)
{
    if(id<4) return;
    else if(id==4) {_state.AutoScale();ReadBuffer();}
    else if(id>=7 && id<=10) {_state.selected=static_cast<GraphState::Quantity>(id-7);}
    else if(id>=24 && id<=26) {_info=id-24;}
    else if(_tabs.GetSelectedIndex()==0 && id==11) {_state.cursorMode=static_cast<GraphState::CursorMode>((_state.cursorMode+1)%4);}
    else if(_tabs.GetSelectedIndex()==0 && id==12) {_state.cursor=1-_state.cursor;}
    else if(_tabs.GetSelectedIndex()==1)
    {
        if(id==11)
        {
            if(_state.iv) _state.ivEnabled=!_state.ivEnabled;
            else _state.enabled[_state.selected]=!_state.enabled[_state.selected];
        }
        else if(id>=12 && id<=14) _system->SetGraphStyle(static_cast<GraphStyle>(id-12));
        else if(id==15 || id==16) _state.SetIV(id==16);
        else if(id==17) {_system->ClearMeasurementBuffer();ReadBuffer();}
    }
    else if(_tabs.GetSelectedIndex()==2)
    {
        if(id>=11 && id<=14) Edit(id-11);
        else if(id==15 || id==16) {_state.scrolling=id==15;_state.X().automatic=true;}
        else if(id==17) Edit(4);
    }
    Layout();
}
bool GraphPage::InPlot(int x,int y) const {return x>=plotX && x<=plotX+plotW && y>=plotY && y<=plotY+plotH;}
void GraphPage::MoveCursor(int x,int y)
{
    const double nx=fmax(0,fmin(1,(x-plotX)/static_cast<double>(plotW)));
    const double ny=fmax(0,fmin(1,1-(y-plotY)/static_cast<double>(plotH)));
    if(_state.cursorMode==GraphState::Vertical || _state.cursorMode==GraphState::Cross) _state.cursorX[_state.cursor]=nx;
    if(_state.cursorMode==GraphState::Horizontal || _state.cursorMode==GraphState::Cross) _state.cursorY[_state.cursor]=ny;
}
double GraphPage::PinchDistance(const TouchFrame& frame) const
{
    const double dx=frame.points[0].x-frame.points[1].x,dy=frame.points[0].y-frame.points[1].y;
    return _zoomAxis==GraphState::ZoomX?fabs(dx):_zoomAxis==GraphState::ZoomY?fabs(dy):hypot(dx,dy);
}
bool GraphPage::HandleMultiTouch(const TouchFrame& frame,bool allowStart)
{
    if(!_multiBlocked && (!allowStart || !IsVisible() || frame.count<2)) return false;
    if(!_multiBlocked)
    {
        _multiBlocked=true;_pinching=false;_drag=false;
        Panel::TouchEnd(-1,-1); // Cancel button presses, never activate on release.
        if(IsVisible() && IsEnabled() && _editing<0 && _tabs.GetSelectedIndex()==0 && frame.count==2 &&
           InPlot(frame.points[0].x,frame.points[0].y) && InPlot(frame.points[1].x,frame.points[1].y))
        {
            const auto& a=frame.points[0];const auto& b=frame.points[1];
            _zoomAxis=GraphState::ZoomXY;_pinchAxisChosen=false;
            _pinchStartDx=fabs(double(a.x-b.x));_pinchStartDy=fabs(double(a.y-b.y));
            _pinchDistance=hypot(_pinchStartDx,_pinchStartDy);
            _pinching=_pinchDistance>=16;
            _pinchIds[0]=a.id;_pinchIds[1]=b.id;
            _pinchX=((a.x+b.x)*0.5-plotX)/plotW;
            _pinchY=1-((a.y+b.y)*0.5-plotY)/plotH;
        }
    }
    else if(frame.count!=2 || frame.points[0].id!=_pinchIds[0] || frame.points[1].id!=_pinchIds[1])
        _pinching=false;
    else if(_pinching && IsVisible() && IsEnabled())
    {
        if(!_pinchAxisChosen) {
            const double dx=fabs(double(frame.points[0].x-frame.points[1].x));
            const double dy=fabs(double(frame.points[0].y-frame.points[1].y));
            const double motionX=fabs(dx-_pinchStartDx),motionY=fabs(dy-_pinchStartDy);
            if(fmax(motionX,motionY)<4) return true; // ignore initial touch jitter
            _zoomAxis=motionX>2*motionY?GraphState::ZoomX:
                      motionY>2*motionX?GraphState::ZoomY:GraphState::ZoomXY;
            _pinchAxisChosen=true; // lock the direction until both fingers are released
            _pinchDistance=_zoomAxis==GraphState::ZoomX?_pinchStartDx:
                           _zoomAxis==GraphState::ZoomY?_pinchStartDy:hypot(_pinchStartDx,_pinchStartDy);
            if(_pinchDistance<16) {_pinchDistance=PinchDistance(frame);return true;}
        }
        const double distance=PinchDistance(frame);
        if(distance>=16 && fabs(distance-_pinchDistance)>=2)
        {
            _state.Zoom(_pinchDistance/distance,_pinchX,_pinchY,_zoomAxis);
            _pinchDistance=distance;
        }
    }
    if(frame.count==0) {_multiBlocked=false;_pinching=false;}
    return true;
}
void GraphPage::TouchStart(int16_t x,int16_t y)
{
    if(!IsVisible() || !IsEnabled()) return;
    if(_editing>=0) {_editor.TouchStart(x,y);return;}
    _drag=_tabs.GetSelectedIndex()==0 && InPlot(x,y);_lastX=x;_lastY=y;
    if(_drag) {if(_info==2 && _state.cursorMode!=GraphState::Off) MoveCursor(x,y);}
    else Panel::TouchStart(x,y);
}
void GraphPage::TouchMove(int16_t x,int16_t y)
{
    if(_editing>=0) {_editor.TouchMove(x,y);return;}
    if(_drag)
    {
        if(_info==2 && _state.cursorMode!=GraphState::Off) MoveCursor(x,y);
        else if(x!=_lastX || y!=_lastY) _state.Pan((x-_lastX)/static_cast<double>(plotW),(y-_lastY)/static_cast<double>(plotH));
        _lastX=x;_lastY=y;
    }
    else Panel::TouchMove(x,y);
}
void GraphPage::TouchEnd(int16_t x,int16_t y)
{
    if(!IsVisible() || !IsEnabled()) return;
    if(_editing>=0)
    {
        int key=-1;for(unsigned i=0;i<17;++i) if(_keys[i].IsPressed() && _keys[i].Contains(x,y)) key=i;
        _editor.TouchEnd(x,y);if(key>=0) EditorKey(key);return;
    }
    if(_drag) {TouchMove(x,y);_drag=false;return;}
    int id=-1;
    for(unsigned i=0;i<29;++i) if(_buttons[i].IsVisible() && _buttons[i].IsEnabled() && _buttons[i].IsPressed() && _buttons[i].Contains(x,y)) id=i;
    Panel::TouchEnd(x,y);
    if(id>=0 && id!=5 && id!=6) Action(id);
}
void GraphPage::Edit(unsigned target)
{
    _editing=target;_error="";_replaceInput=true;_drag=false;ClearFocus();
    GD.RequestBackdropCapture();
    const double value=target==4?_state.windowSeconds:target==0?_state.X().start:target==1?_state.X().division:target==2?_state.y[_state.selected].start:_state.y[_state.selected].division;
    snprintf(_input,sizeof(_input),"%.8g",value);_editor.SetVisible(true);
}
void GraphPage::EditorKey(unsigned key)
{
    if(key==16) {_editing=-1;_editor.SetVisible(false);Layout();return;}
    if(key==15)
    {
        char* end=nullptr;const double value=strtod(_input,&end);
        bool valid=end!=_input && *end=='\0' && isfinite(value);
        if(valid && _editing==4) {valid=value>=0.001 && value<=1e8;if(valid) {_state.SetWindowSeconds(value);_state.X().automatic=true;}}
        else if(valid) valid=_state.SetManual(_editing<2,(_editing%2)==1,value);
        if(!valid) {_error="Invalid value: divisions must be positive.";return;}
        _editing=-1;_editor.SetVisible(false);Layout();return;
    }
    if(key==14) {_input[0]='\0';_replaceInput=false;}
    else if(key==13) {const size_t length=strlen(_input);if(length) _input[length-1]='\0';_replaceInput=false;}
    else
    {
        if(_replaceInput) {_input[0]='\0';_replaceInput=false;}
        const size_t length=strlen(_input);
        if(length+1<sizeof(_input)) {_input[length]=keyNames[key][0];_input[length+1]='\0';}
    }
    _error="";
}
void GraphPage::Draw()
{
    if(!IsVisible()) return;
    if(_editing>=0)
    {
        // Keep the original page visible as one bitmap, not a second set of widgets.
        GD.DrawCapturedBackdrop();
        DrawEditor();return;
    }
    ReadBuffer();Layout();
    char status[64];snprintf(status,sizeof(status),"%s %u",_system->GetBackendText(),_count);
    Text(500,17,status,Theme::ToolbarText,28);
    GD.SaveContext();GD.VertexFormat(0);GD.ColorRGB(Theme::TabActive);GD.LineWidth(32);
    GD.Begin(LINES);GD.Vertex2f(8,46);GD.Vertex2f(792,46);GD.RestoreContext();
    if(_tabs.GetSelectedIndex()==0) {DrawGraph();DrawInfo();} else DrawSettings();
    _editor.SetVisible(false);Panel::Draw();
}
void GraphPage::DrawEditor()
{
    const int wx=(800-Theme::NumericPopupWidth)/2,wy=(480-Theme::NumericPopupHeight)/2;
    GD.DrawModalWindow(0,0,800,480,wx,wy,Theme::NumericPopupWidth,Theme::NumericPopupHeight,
                       Theme::PopupBackdrop,Theme::PopupBorder,Theme::PopupBackground);
    const char* titles[]={"X START","X PER DIVISION","Y START","Y PER DIVISION","TIME WINDOW"};
    const char* unit=_editing==4?"s":_editing<2?(_state.iv?"V":"s"):units[_state.selected];
    char title[80];snprintf(title,sizeof(title),"SET %s [%s]",titles[_editing],unit);
    Text(400,wy+28,title,Theme::HeaderText,Theme::FontPopupTitle,OPT_CENTERX);
    GD.SaveContext();GD.VertexFormat(0);GD.LineWidth(16);
    GD.ColorRGB(0xF0F0E0);GD.Begin(RECTS);GD.Vertex2f(wx+28,wy+60);GD.Vertex2f(wx+572,wy+116);
    GD.ColorRGB(0x0A0A0A);GD.Begin(RECTS);GD.Vertex2f(wx+34,wy+66);GD.Vertex2f(wx+566,wy+110);
    GD.RestoreContext();
    Text(wx+46,wy+88,_input,Theme::ValueGreen,strlen(_input)>22?26:strlen(_input)>16?28:Theme::FontPopupInput,OPT_CENTERY);
    Text(400,wy+392,*_error?_error:"Base units; E = exponent (e.g. 1E-6)",
         *_error?0xFF6060:Theme::ToolbarText,26,OPT_CENTERX);
    _editor.Draw();
}
uint32_t GraphPage::CurveColor(unsigned quantity) const
{
    return _state.iv ? colors[0] : colors[(quantity+4-_firstCurve)%4];
}
void GraphPage::DrawGraph()
{
    GD.DrawPlotGrid(plotX,plotY,plotW,plotH,10,6,0x303030);
    char label[40];const auto& xr=_state.X();const auto& yr=_state.y[_state.selected];
    for(unsigned i=0;i<=6;++i)
    {
        GraphState::FormatAxis(yr,6,i,units[_state.selected],label,sizeof(label));
        Text(4,plotY+plotH-i*plotH/6,label,CurveColor(_state.selected),26,OPT_CENTERY);
    }
    for(unsigned i=0;i<=10;i+=2)
    {
        if(_state.iv) GraphState::FormatAxis(xr,10,i,"V",label,sizeof(label));
        else GraphState::FormatTime(xr.start+xr.division*i,xr.division*2,label,sizeof(label));
        Text(plotX+plotW*i/10,plotY+plotH+14,label,Theme::ToolbarText,26,OPT_CENTER);
    }
    bool any=false;
    for(unsigned q=0;q<4;++q)
    {
        if(!_state.IsCurveVisible(q)) continue;
        const GraphState::Trace trace{&_state,_samples,static_cast<GraphState::Quantity>(q)};
        any=GD.DrawPlotTrace(plotX,plotY,plotW,plotH,trace.Source(_count),xr,_state.y[q],
                          CurveColor(q),_system->GetGraphSettings().style!=GraphStyle::Points,
                          _system->GetGraphSettings().style!=GraphStyle::Lines) || any;
    }
    if(!any) Text(360,204,_count?"No visible valid curve":"Buffer empty",0xAAAAAA,26);
    if(_state.cursorMode!=GraphState::Off)
    {
        GD.SaveContext();GD.VertexFormat(0);GD.LineWidth(12);GD.Begin(LINES);
        for(unsigned c=0;c<2;++c)
        {
            GD.ColorRGB(c==0?0xFFFFFF:0xFFAA40);
            const int x=plotX+_state.cursorX[c]*plotW,y=plotY+(1-_state.cursorY[c])*plotH;
            if(_state.cursorMode==GraphState::Vertical || _state.cursorMode==GraphState::Cross) {GD.Vertex2f(x,plotY);GD.Vertex2f(x,plotY+plotH);}
            if(_state.cursorMode==GraphState::Horizontal || _state.cursorMode==GraphState::Cross) {GD.Vertex2f(plotX,y);GD.Vertex2f(plotX+plotW,y);}
        }
        GD.RestoreContext();
    }
}
void GraphPage::DrawInfo()
{
    char text[160],a[32],b[32],c[32],d[32];const unsigned q=_state.selected;
    if(_info==0)
    {
        Format(_state.X().division,_state.iv?"V":"s",a,sizeof(a));Format(_state.y[q].division,units[q],b,sizeof(b));
        snprintf(text,sizeof(text),"%s%s | X: %s / div | Y: %s / div",names[q],_state.IsCurveVisible(q)?"":" (hidden)",a,b);Text(12,410,text);
        snprintf(text,sizeof(text),"X: %s | Y: %s | %s",_state.X().automatic?"AUTO":"MANUAL",_state.y[q].automatic?"AUTO":"MANUAL",_state.iv?"I(V)":_state.scrolling?"Step scrolling":"Full retained buffer");Text(12,440,text);
    }
    else if(_info==1)
    {
        const auto stats=_system->Buffer.GetStatistics(static_cast<BufferQuantity>(q));
        if(!stats.Count) {Text(12,412,"Buffer statistics: no valid samples",Theme::SourceBlue);return;}
        Format(stats.Minimum,units[q],a,sizeof(a));Format(stats.Maximum,units[q],b,sizeof(b));
        Format(stats.Average,units[q],c,sizeof(c));Format(stats.StandardDeviation,units[q],d,sizeof(d));
        Text(12,410,names[q],Theme::SourceBlue);
        Text(12,440,"N:",Theme::SourceBlue);
        snprintf(text,sizeof(text),"%u",stats.Count);Text(44,440,text,Theme::MeasureLabel);
        Text(170,410,"Max:",Theme::SourceBlue);Text(240,410,b,Theme::MeasureLabel);
        Text(170,440,"Min:",Theme::SourceBlue);Text(240,440,a,Theme::MeasureLabel);
        Text(490,410,"Avg:",Theme::SourceBlue);Text(560,410,c,Theme::MeasureLabel);
        Text(490,440,"Std:",Theme::SourceBlue);Text(560,440,d,Theme::MeasureLabel);
    }
    else if(_state.cursorMode==GraphState::Off) Text(12,414,"Cursor off. Select a mode, then move A or B in the plot.");
    else
    {
        if(_state.cursorMode==GraphState::Vertical || _state.cursorMode==GraphState::Cross)
        {
            const double x1=_state.X().start+_state.cursorX[0]*_state.X().division*10;
            const double x2=_state.X().start+_state.cursorX[1]*_state.X().division*10;
            Format(x1,_state.iv?"V":"s",a,sizeof(a));Format(x2,_state.iv?"V":"s",b,sizeof(b));Format(x2-x1,_state.iv?"V":"s",c,sizeof(c));
            snprintf(text,sizeof(text),"X A: %s | X B: %s | dX: %s",a,b,c);Text(12,410,text);
        }
        if(_state.cursorMode==GraphState::Horizontal || _state.cursorMode==GraphState::Cross)
        {
            const double y1=_state.y[q].start+_state.cursorY[0]*_state.y[q].division*6;
            const double y2=_state.y[q].start+_state.cursorY[1]*_state.y[q].division*6;
            Format(y1,units[q],a,sizeof(a));Format(y2,units[q],b,sizeof(b));Format(y2-y1,units[q],c,sizeof(c));
            snprintf(text,sizeof(text),"Y A: %s | Y B: %s | dY: %s",a,b,c);Text(12,440,text);
        }
    }
}
void GraphPage::DrawSettings()
{
    if(_tabs.GetSelectedIndex()==1)
    {
        Text(24,112,"Data source: Active Buffer (timestamped measurements)",Theme::SourceBlue,27);
        Text(24,142,"Top buttons select a curve; Add / Remove controls visibility.");
        char visible[100];
        if(_state.iv) snprintf(visible,sizeof(visible),"Visible: %s",_state.ivEnabled?"I(V)":"none");
        else snprintf(visible,sizeof(visible),"Visible: %s %s %s %s",_state.enabled[0]?"V":"",_state.enabled[1]?"I":"",_state.enabled[2]?"P":"",_state.enabled[3]?"R":"");
        Text(252,182,visible);
        Text(24,222,"Display style:");Text(24,300,"Axes:");
        Text(24,402,"Independent Y scales; axis labels follow the selected curve.");
        Text(24,438,"Remove keeps data. Clear empties the measurement buffer.");
    }
    else if(_tabs.GetSelectedIndex()==2)
    {
        Text(24,108,"Manual scale: tap a value to enter start / units per division.",Theme::SourceBlue,27);
        Text(24,282,"Time axis:");
        Text(24,386,"One finger pans; two fingers zoom. AUTO SCALE resets.");
        Text(24,420,"AUTO SCALE restores automatic axes. Settings survive tab changes.");
        char capacityText[80];
        snprintf(capacityText,sizeof(capacityText),"Full buffer = retained measurements (maximum %u readings).",
                 static_cast<unsigned>(_system->Buffer.GetCapacity()));
        Text(24,452,capacityText,0xAAAAAA);
    }
    else
    {
        Text(24,112,"Trigger control",Theme::SourceBlue,29);
        Text(24,160,_system->GetSimulationEnabled()?"Current acquisition: continuous simulation":"Current acquisition: stored buffer");
        Text(24,192,"No hardware trigger controller is implemented yet.");
        Text(24,382,"The controls above are placeholders and do not change acquisition.");
        Text(24,418,_system->GetOutputOn()?"Output: ON (unchanged by graph navigation)":"Output: OFF (unchanged by graph navigation)");
    }
}
