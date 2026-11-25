#include <SPI.h>
#include "GDClass.h"
#include "EVE.h"
#include "PlotRaster.h"

namespace
{
arduino::MbedSPI displaySpi(EVE_MISO, EVE_MOSI, EVE_SCK);
}

// EVE_target_Arduino_RP2040.h
// SPI-Einstellungen T50 Riverdi
// Config der Library über platformio.ini

/**
 * RVT50UQFNWC03 V2 
 * 300 mA Stromverbrauch
 * @brief Initialisiert SPI, EVE und interne Basisdaten.
 * @param p Aktuell ungenutzt, bleibt für API-Kompatibilität erhalten.
 */
void GDClass::begin(byte p)
{
    (void)p;
    _backlightDuty=255;

    pinMode(EVE_CS, OUTPUT);
    digitalWrite(EVE_CS, HIGH);
    pinMode(EVE_PDN, OUTPUT);
    digitalWrite(EVE_PDN, HIGH);

    // Mbed selects SPI1 from the CN4 pins. EVE uses that same peripheral
    // through EVE_SPI=spi1, configured globally in platformio.ini.
    displaySpi.begin();
    displaySpi.beginTransaction(SPISettings(8000000UL, MSBFIRST, SPI_MODE0));

    EveStatus = EVE_init();
    Serial.print("EVE init status: ");
    Serial.println(EveStatus); // 0 = E_OK

    // Aktuelle Displaygröße der A-SMU GUI
    w = 800;
    h = 480;

    // FT813: extended capacitive mode exposes all five calibrated touch slots.
    // Any future touch calibration must temporarily restore compatibility mode.
    EVE_memWrite8(REG_CTOUCH_EXTENDED, 0);
    rseed = 0x77777777;
}

/**
 * @brief Setzt die Anzahl der Nachkommabits für Vertex2f-Koordinaten.
 */
void GDClass::VertexFormat(byte frac)
{
    EVE_cmd_dl((39UL << 24) | (((frac) & 7) << 0));
}

/**
 * @brief Setzt den Seed des internen Pseudozufallsgenerators.
 */
void GDClass::seed(uint16_t n)
{
    rseed = n ? n : 7;
}

/**
 * @brief Liefert einen 16-Bit-Pseudozufallswert.
 */
uint16_t GDClass::random()
{
    rseed ^= rseed << 2;
    rseed ^= rseed >> 5;
    rseed ^= rseed << 1;
    return rseed;
}

/**
 * @brief Liefert einen Pseudozufallswert im Bereich [0, n).
 */
uint16_t GDClass::random(uint16_t n)
{
    uint16_t p = random();

    if (n == (n & -n))
    {
        return p & (n - 1);
    }

    return (uint32_t(p) * n) >> 16;
}

/**
 * @brief Liefert einen Pseudozufallswert im Bereich [n0, n1).
 */
uint16_t GDClass::random(uint16_t n0, uint16_t n1)
{
    return n0 + random(n1 - n0);
}

/**
 * @brief Speichert den aktuellen Grafik-Kontext.
 */
void GDClass::SaveContext(void)
{
    EVE_cmd_dl(DL_SAVE_CONTEXT);
}

/**
 * @brief Stellt einen zuvor gespeicherten Grafik-Kontext wieder her.
 */
void GDClass::RestoreContext(void)
{
    EVE_cmd_dl(DL_RESTORE_CONTEXT);
}

/**
 * @brief Setzt den Alpha-Wert für nachfolgende Zeichenoperationen.
 */
void GDClass::ColorA(byte alpha)
{
    EVE_cmd_dl(DL_COLOR_A | ((uint32_t)alpha));
}

/**
 * @brief Setzt die Clear-Farbe für einen folgenden Clear-Befehl.
 */
void GDClass::ClearColorRGB(uint32_t rgb)
{
    EVE_cmd_dl(DL_CLEAR_COLOR_RGB | rgb);
}

/**
 * @brief Setzt die aktuelle Zeichenfarbe als 0xRRGGBB.
 */
void GDClass::ColorRGB(uint32_t rgb)
{
    EVE_color_rgb(rgb);
}

/**
 * @brief Setzt die aktuelle Zeichenfarbe über einzelne RGB-Komponenten.
 */
void GDClass::ColorRGB(byte red, byte green, byte blue)
{
    uint32_t rgb = ((red & 255UL) << 16) | ((green & 255UL) << 8) | ((blue & 255UL) << 0);
    EVE_color_rgb(rgb);
}

/**
 * @brief Setzt die Vordergrundfarbe für Widgets wie Buttons.
 */
void GDClass::cmd_fgcolor(uint32_t color)
{
    EVE_cmd_fgcolor(color);
}

/**
 * @brief Löscht Farb-, Stencil- und Tag-Buffer.
 */
void GDClass::Clear(void)
{
    EVE_cmd_dl(DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG);
}

/**
 * @brief Zeichnet Text über den EVE-Co-Prozessor.
 */
void GDClass::cmd_text(int16_t x, int16_t y, byte font, uint16_t options, const char* s)
{
    EVE_cmd_text(x, y, font, options, s);
}

/**
 * @brief Markiert das Ende einer Display-Liste.
 */
void GDClass::Display(void)
{
    EVE_cmd_dl(DL_DISPLAY);
}

/**
 * @brief Aktiviert die aktuelle Display-Liste.
 */
void GDClass::swap(void)
{
    EVE_cmd_dl(DL_DISPLAY);
    EVE_cmd_dl(CMD_SWAP);
}

/**
 * @brief Startet eine neue Display-Liste.
 */
bool GDClass::BeginFrame()
{
    const uint16_t previousBytes = EVE_memRead16(REG_CMD_DL);
    const uint8_t state = EVE_busy();
    if (state == EVE_FAULT_RECOVERED)
    {
        _capturePending=false;_backdropReady=false;
        Serial.print("EVE coprocessor fault recovered; DL bytes: ");
        Serial.println(previousBytes);
        return false;
    }
    // Keep processing input and System.Execute() while the last frame completes.
    // Do not queue another complete frame into a busy command FIFO.
    if (state != E_OK) return false;
    if(_capturePending) {_capturePending=false;_backdropReady=true;}
    if(_captureRequested)
    {
        _captureRequested=false;
        if(w>0 && h>0 && static_cast<uint32_t>(w)*h*2<=0x100000UL-0x40000UL)
        {
            // Queue CMD_SNAPSHOT2 without the driver's blocking EVE_execute_cmd().
            // FIFO order guarantees snapshot completion before the next DLSTART.
            EVE_cmd_dl(CMD_SNAPSHOT2);EVE_cmd_dl(EVE_RGB565);EVE_cmd_dl(0x40000UL);
            EVE_cmd_dl(0);EVE_cmd_dl(static_cast<uint32_t>(w)|(static_cast<uint32_t>(h)<<16));
            _capturePending=true;
            return false; // Main loop continues processing touch and system updates.
        }
    }
    // Read only after the coprocessor has finished the previous display list.
    // Report new peaks instead of logging every frame (serial monitor, bytes).
    static uint16_t peakBytes=0;
    const uint16_t completedBytes=EVE_memRead16(REG_CMD_DL);
    if(completedBytes>peakBytes)
    {
        peakBytes=completedBytes;
        Serial.print("EVE DL peak: ");Serial.print(peakBytes);Serial.println(" / 8192 bytes");
    }
    _plotBank^=1;_plotSlot=0;
    cmd_dlstart();
    return true;
}

void GDClass::cmd_dlstart()
{
    EVE_cmd_dl(CMD_DLSTART);
}

/**
 * @brief Fügt einen Vertex mit 1/16-Pixel-Auflösung ein.
 */
void GDClass::Vertex2f(int16_t x, int16_t y)
{
    EVE_cmd_dl(VERTEX2F(x, y));
}

/**
 * @brief Fügt einen Integer-Vertex ein.
 * @note X und Y dürfen hier nur positive Werte annehmen.
 */
void GDClass::Vertex2ii(uint16_t x, uint16_t y, byte handle, byte cell)
{
    EVE_cmd_dl(VERTEX2II(x, y, handle, cell));
}

/**
 * @brief Startet ein Zeichenprimitive.
 */
void GDClass::Begin(Primitive prim)
{
    EVE_cmd_dl(DL_BEGIN | prim);
}

/**
 * @brief Setzt den Tag-Wert für folgende Grafikobjekte.
 */
void GDClass::Tag(byte s)
{
    EVE_cmd_dl(DL_TAG | s);
}

/**
 * @brief Aktiviert oder deaktiviert das Schreiben in den Tag-Buffer.
 */
void GDClass::TagMask(byte mask)
{
    EVE_cmd_dl(DL_TAG_MASK | ((mask) & 1UL));
}

/**
 * @brief Setzt die Punktgröße für POINTS.
 */
void GDClass::PointSize(uint16_t size)
{
    EVE_cmd_dl(DL_POINT_SIZE | (size & 0x1FFFUL));
}

/**
 * @brief Setzt die Linienbreite für nachfolgende Linienobjekte.
 */
void GDClass::LineWidth(uint16_t width)
{
    EVE_cmd_dl(DL_LINE_WIDTH | (width & 0x0FFFUL));
}

/**
 * @brief Liest Touch-, Tag- und Tracker-Daten in die inputs-Struktur ein.
 */

void GDClass::get_inputs(void)
{
    touches.count=0;
    touches.Add(EVE_memRead32(REG_CTOUCH_TOUCH0_XY),0);
    touches.Add(EVE_memRead32(REG_CTOUCH_TOUCH1_XY),1);
    touches.Add(EVE_memRead32(REG_CTOUCH_TOUCH2_XY),2);
    touches.Add(EVE_memRead32(REG_CTOUCH_TOUCH3_XY),3);
    const uint32_t fifthX=EVE_memRead16(REG_CTOUCH_TOUCH4_X);
    touches.Add((fifthX<<16)|EVE_memRead16(REG_CTOUCH_TOUCH4_Y),4);
    const uint32_t tracker=EVE_memRead32(REG_TRACKER);
    inputs.track_tag=tracker&0xffff;inputs.track_val=tracker>>16;
    inputs.ptag=inputs.tag;
    inputs.tag=EVE_memRead8(REG_TOUCH_TAG);
    inputs.touching=touches.count!=0;
    inputs.x=touches.count?touches.points[0].x:-32768;
    inputs.y=touches.count?touches.points[0].y:-32768;
}

/**
 * @brief Lädt eine ROM-Font in einen Bitmap-Handle.
 */
void GDClass::cmd_romfont(uint32_t font, uint32_t romslot)
{
    EVE_cmd_romfont(font, romslot);
}

/**
 * @brief Zeichnet einen EVE-Button.
 */
void GDClass::cmd_button(int16_t x, int16_t y, uint16_t w, uint16_t h, byte font, uint16_t options, const char* s)
{
    EVE_cmd_button(x, y, w, h, font, options, s);
}

/**
 * @brief Schreibt ein Byte in den EVE-Speicher.
 */
void GDClass::wr(uint32_t addr, uint8_t v)
{
    EVE_memWrite8(addr, v);
}

/**
 * @brief Globale Displayinstanz.
 */
GDClass GD;
void GDClass::DrawRoundedGradientRect(int16_t x, int16_t y, int16_t w, int16_t h,
                                      uint16_t radius, uint32_t topColor, uint32_t bottomColor)
{
    if (w <= 0 || h <= 0) return;
    const uint16_t maxRadius = static_cast<uint16_t>(((w < h ? w : h) - 1) / 2);
    const uint16_t r = radius < maxRadius ? radius : maxRadius;
    SaveContext();
    VertexFormat(0);
    ColorA(255);
    // CMD_GRADIENT must have an explicit scissor rectangle (FT81x guide).
    // The stencil shapes the corners; scissor confines all gradient work and
    // stencil clearing to this button, never to the rest of the screen.
    const int16_t clipX = x < 0 ? 0 : x;
    const int16_t clipY = y < 0 ? 0 : y;
    const int32_t clipW = static_cast<int32_t>(x) + w - clipX;
    const int32_t clipH = static_cast<int32_t>(y) + h - clipY;
    if (clipW <= 0 || clipH <= 0)
    {
        RestoreContext();
        return;
    }
    EVE_scissor_xy(clipX, clipY);
    EVE_scissor_size(clipW, clipH);
    // Reserve one stencil bit; retain the other bits for the caller.
    EVE_stencil_mask(0x80);
    EVE_clear_stencil(0);
    EVE_clear(0, 1, 0);
    EVE_stencil_func(EVE_ALWAYS, 0x80, 0x80);
    EVE_stencil_op(EVE_REPLACE, EVE_REPLACE);
    EVE_color_mask(0, 0, 0, 0);
    LineWidth(r > 0 ? r * 16 : 8);
    Begin(RECTS);
    Vertex2f(x + r, y + r);
    Vertex2f(x + w - 1 - r, y + h - 1 - r);
    EVE_end();
    EVE_color_mask(1, 1, 1, 1);
    EVE_stencil_func(EVE_EQUAL, 0x80, 0x80);
    EVE_stencil_op(EVE_KEEP, EVE_KEEP);
    EVE_cmd_gradient(x, y, topColor, x, y + (h > 1 ? h - 1 : 1), bottomColor);
    RestoreContext();
}

void GDClass::DrawPlotGrid(int16_t x, int16_t y, int16_t w, int16_t h,
                           uint8_t columns, uint8_t rows, uint32_t color)
{
    if(w<=0 || h<=0 || !columns || !rows) return;
    SaveContext(); VertexFormat(0); ColorRGB(color); LineWidth(8); Begin(LINES);
    for(unsigned i=0;i<=columns;++i) { Vertex2f(x+w*i/columns,y);Vertex2f(x+w*i/columns,y+h); }
    for(unsigned i=0;i<=rows;++i) { Vertex2f(x,y+h*i/rows);Vertex2f(x+w,y+h*i/rows); }
    EVE_end(); RestoreContext();
}

void GDClass::DrawPlotSeries(int16_t x, int16_t y, int16_t w, int16_t h,
                             const PlotPoint* points, uint16_t count,
                             const PlotRange& xr, const PlotRange& yr,
                             uint32_t color, bool lines, bool dots, uint8_t xDivisions, uint8_t yDivisions)
{
    if(!points || w<=0 || h<=0 || !xDivisions || !yDivisions || xr.division<=0 || yr.division<=0) return;
    SaveContext(); VertexFormat(0);ColorA(255);ColorRGB(color);
    EVE_scissor_xy(x,y);EVE_scissor_size(w+1,h+1);
    double previousX=0,previousY=0;bool previous=false;
    LineWidth(12);PointSize(3*16);
    for(uint16_t i=0;i<count;++i)
    {
        if(!points[i].valid) {previous=false;continue;}
        const double nx=(points[i].x-xr.start)/xr.division/xDivisions;
        const double ny=(points[i].y-yr.start)/yr.division/yDivisions;
        if(!isfinite(nx) || !isfinite(ny)) {previous=false;continue;}
        if(lines && previous)
        {
            double a=previousX,b=previousY,c=nx,d=ny;
            if(ClipPlotLine(a,b,c,d)) {
                Begin(LINES);Vertex2f(x+a*w,y+(1-b)*h);Vertex2f(x+c*w,y+(1-d)*h);
            }
        }
        if(dots && nx>=0 && nx<=1 && ny>=0 && ny<=1)
        { Begin(POINTS);Vertex2f(x+nx*w,y+(1-ny)*h); }
        previousX=nx;previousY=ny;previous=true;
    }
    EVE_end();RestoreContext();
}

void GDClass::DrawTabShape(int16_t x,int16_t y,int16_t w,int16_t h,uint16_t slope,uint32_t color)
{
    if(w<=0 || h<=0) return;
    const int cut=slope<w?slope:w-1;
    const int round=(h>8 && w-cut>8)?4:0;
    SaveContext();VertexFormat(0);ColorA(255);ColorRGB(color);
    const int left=x<0?0:x, top=y<0?0:y;
    if(x+w<=left || y+h<=top) {RestoreContext();return;}
    EVE_scissor_xy(left,top);EVE_scissor_size(x+w-left,y+h-top);
    Begin(EDGE_STRIP_B);
    Vertex2f(x,y+round);Vertex2f(x+round,y);
    Vertex2f(x+w-cut,y);Vertex2f(x+w,y+h);
    EVE_end();RestoreContext();
}

void GDClass::DrawModalWindow(int16_t x,int16_t y,int16_t w,int16_t h,
                             int16_t wx,int16_t wy,int16_t ww,int16_t wh,
                             uint32_t backdrop,uint32_t border,uint32_t background)
{
    SaveContext();VertexFormat(0);LineWidth(16);
    ColorRGB(backdrop);ColorA(160);Begin(RECTS);Vertex2f(x,y);Vertex2f(x+w,y+h);
    ColorA(255);ColorRGB(border);Begin(RECTS);Vertex2f(wx,wy);Vertex2f(wx+ww,wy+wh);
    ColorRGB(background);Begin(RECTS);Vertex2f(wx+2,wy+2);Vertex2f(wx+ww-2,wy+wh-2);
    EVE_end();RestoreContext();
}

void GDClass::DrawSolidRoundedRect(int16_t x,int16_t y,int16_t w,int16_t h,uint16_t radius,uint32_t color)
{
    if(w<=0 || h<=0) return;
    const int limit=((w<h?w:h)-1)/2;
    const int r=radius<limit?radius:limit;
    SaveContext();VertexFormat(0);ColorA(255);ColorRGB(color);
    LineWidth(r?r*16:8);Begin(RECTS);
    Vertex2f(x+r,y+r);Vertex2f(x+w-1-r,y+h-1-r);
    EVE_end();RestoreContext();
}

void GDClass::RequestBackdropCapture()
{
    _backdropReady=false;
    _captureRequested=true;
}

bool GDClass::DrawCapturedBackdrop()
{
    if(!_backdropReady) return false;
    SaveContext();VertexFormat(0);ColorA(255);ColorRGB(0xFFFFFF);
    EVE_bitmap_handle(0);
    EVE_cmd_setbitmap(0x40000UL,EVE_RGB565,w,h);
    Begin(BITMAPS);Vertex2f(0,0);EVE_end();RestoreContext();
    return true;
}

void GDClass::DrawHistogramBars(int16_t x,int16_t y,int16_t w,int16_t h,
                               const uint16_t* counts,uint16_t count,uint16_t maximum,uint32_t color)
{
    if(!counts || !count || !maximum || w<=0 || h<=0) return;
    SaveContext();VertexFormat(0);ColorA(255);ColorRGB(color);LineWidth(8);
    EVE_scissor_xy(x,y);EVE_scissor_size(w,h);Begin(RECTS);
    for(uint16_t i=0;i<count;++i) {
        if(!counts[i]) continue;
        const int left=x+static_cast<int32_t>(w)*i/count+1;
        const int right=x+static_cast<int32_t>(w)*(i+1)/count-1;
        if(right<left) continue;
        const uint16_t value=counts[i]>maximum?maximum:counts[i];
        int height=static_cast<int32_t>(h)*value/maximum;if(height<1) height=1;
        Vertex2f(left,y+h-height);Vertex2f(right,y+h-1);
    }
    EVE_end();RestoreContext();
}

void GDClass::DrawTableGrid(int16_t x,int16_t y,int16_t rowHeight,uint16_t rows,
                           const int16_t* offsets,uint8_t columns,uint32_t color)
{
    if(!offsets || !columns || !rows || rowHeight<=0) return;
    SaveContext();VertexFormat(0);ColorA(255);ColorRGB(color);LineWidth(8);Begin(LINES);
    const int bottom=y+rows*rowHeight;
    for(unsigned i=0;i<=columns;++i) {Vertex2f(x+offsets[i],y);Vertex2f(x+offsets[i],bottom);}
    for(unsigned i=0;i<=rows;++i) {Vertex2f(x+offsets[0],y+i*rowHeight);Vertex2f(x+offsets[columns],y+i*rowHeight);}
    EVE_end();RestoreContext();
}

// Four trace slots in each of two RAM_G banks, below the modal snapshot at 0x40000.
// Handles 16..19 avoid modal handle 0 and ROM-font handles 1/2.
bool GDClass::DrawPlotTrace(int16_t x,int16_t y,int16_t width,int16_t height,
                           const PlotPointSource& source,const PlotRange& xr,const PlotRange& yr,
                           uint32_t color,bool lines,bool dots) {
    constexpr unsigned slotBytes=25600;
    static uint8_t pixels[slotBytes];
    if(width<=0 || height<=0 || width>=800 || height>=256 || _plotSlot>=4) return false;
    const unsigned stride=(width+1+7)/8,rows=height+1;
    PlotRaster raster(pixels,width+1,rows);
    const bool any=raster.Draw(source,xr,yr,lines,dots);
    const uint32_t address=(_plotBank*4+_plotSlot)*slotBytes;
    EVE_memWrite_sram_buffer(address,pixels,stride*rows);
    SaveContext();VertexFormat(0);ColorA(255);ColorRGB(color);
    EVE_scissor_xy(x,y);EVE_scissor_size(width+1,height+1);
    EVE_bitmap_handle(16+_plotSlot++);
    EVE_cmd_setbitmap(address,EVE_L1,stride*8,rows);
    // L1 stores alpha, not RGB luminance. Honor it so zero bits are transparent.
    EVE_blend_func(EVE_SRC_ALPHA,EVE_ONE_MINUS_SRC_ALPHA);
    Begin(BITMAPS);Vertex2f(x,y);EVE_end();RestoreContext();
    return any;
}

// EVE PWM duty is 0..128; percentages and hardware details stay behind GDClass.
void GDClass::SetBacklightPercent(unsigned percent) {
    if(percent>100) percent=100;
    const uint8_t duty=static_cast<uint8_t>((percent*128+50)/100);
    if(duty!=_backlightDuty) {EVE_memWrite8(REG_PWM_DUTY,duty);_backlightDuty=duty;}
}
void GDClass::DrawSlider(int16_t x,int16_t y,int16_t width,int16_t height,unsigned position,
                         uint32_t track,uint32_t fill,uint32_t thumb) {
    if(width<height || height<4) return;
    if(position>100) position=100;
    const int16_t center=x+height/2+(width-height)*position/100;
    DrawSolidRoundedRect(x,y+height/2-4,width,8,4,track);
    DrawSolidRoundedRect(x,y+height/2-4,center-x,8,4,fill);
    DrawSolidRoundedRect(center-height/2,y,height,height,height/2,thumb);
}
