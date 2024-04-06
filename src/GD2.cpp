#include <SPI.h>
#include <GD2.h>

#include "EVE.h"


// EVE_target_Arduino_RP2040.h 
// SPI-EinstellungenT50 Riverdi
// ** Config der Library über platform.ini ** 
// lib_deps = https://github.com/RudolphRiedel/FT800-FT813/archive/refs/heads/5.x.zip
// framework = arduino
// build_flags = -D EVE_RVT50H
// -D EVE_CS=10
// -D EVE_PDN=8

//----------------------------------
// [env:pico]
// platform=raspberrypi
// board=pico
// build_unflags=-Os
// build_flags=
// ${env.build_flags}
// -DEVE_CS=5
// -DEVE_PDN=6
// -DEVE_SCK=2
// -DEVE_MOSI=3
// -DEVE_MISO=4
// -DEVE_SPI=spi0
// -DEVE_DMA
// -DEVE_ROTATE=0
// -DEVE_BACKLIGHT_PWM=30
// -O2

/**
Historie:

* add lineWidth
* add void GDClass::wr(uint32_t addr, uint8_t v) 
* add void cmd_romfont(uint32_t font, uint32_t romslot)
* 

*/
////////////////////////////////////////////////////////////////////////


void GDClass::begin(byte p )
{
    pinMode(EVE_CS, OUTPUT);
    digitalWrite(EVE_CS, HIGH);

    //pinMode(EVE_PDN, OUTPUT);
    //digitalWrite(EVE_PDN, LOW);

  
    SPI.begin(); /* sets up the SPI to run in Mode 0 and 1 MHz */
    SPI.beginTransaction(SPISettings(16UL * 1000000UL, MSBFIRST, SPI_MODE0));

    // Initializes EVE according to the selected configuration from EVE_config.h.
    EveStatus = EVE_init();

  

    w = EVE_HSIZE;
    h = EVE_VSIZE;   

    rseed = 0x77777777; // random init

};


void GDClass::VertexFormat(byte frac) {
  EVE_cmd_dl((39UL << 24) | (((frac) & 7) << 0));
}


void GDClass::seed(uint16_t n) {
  rseed = n ? n : 7;
}

uint16_t GDClass::random() {
  rseed ^= rseed << 2;
  rseed ^= rseed >> 5;
  rseed ^= rseed << 1;
  return rseed;
}

uint16_t GDClass::random(uint16_t n) {
  uint16_t p = random();
  if (n == (n & -n))
    return p & (n - 1);
  return (uint32_t(p) * n) >> 16;
}

uint16_t GDClass::random(uint16_t n0, uint16_t n1) {
  return n0 + random(n1 - n0);
}


void GDClass::SaveContext(void)
{
  EVE_cmd_dl(DL_SAVE_CONTEXT);
} 

void GDClass::RestoreContext(void)
{
  EVE_cmd_dl(DL_RESTORE_CONTEXT);
}


/**
 * @brief Set the current color alpha (transparency).
 */
void GDClass::ColorA(byte alpha) {
    EVE_cmd_dl(DL_COLOR_A | ((uint32_t) alpha));
}


/* 
 set the default clear color 
 Sets the color values used by a following CLEAR.
*/
void GDClass::ClearColorRGB(uint32_t rgb) 
{
    EVE_cmd_dl(DL_CLEAR_COLOR_RGB | rgb);    
}


/**
*  @brief Set the Pen Color
*/
void GDClass::ColorRGB(uint32_t rgb) 
{
    EVE_color_rgb(rgb); 
}


void GDClass::ColorRGB(byte red, byte green, byte blue) {
  uint32_t rgb = ((red & 255L) << 16) | ((green & 255L) << 8) | ((blue & 255L) << 0);
  EVE_color_rgb (  rgb );
}

void GDClass::cmd_fgcolor( uint32_t color) {
  EVE_cmd_fgcolor(color);
}

/* 
 clear the screen - this and the previous prevent artifacts between lists, 
 attributes are the color, stencil and tag buffers 
*/
void GDClass::Clear(void) 
{
    EVE_cmd_dl(DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG);    
}


void GDClass::cmd_text(int16_t x, int16_t y, byte font, uint16_t options, const char *s) {
    EVE_cmd_text(x, y, font, options, s);
}


/*
The co-processor list finishes with a DISPLAY command which, when actioned by the co-processor
and added to the display list, tells the FT8XX that this is the end of the set of display items.
*/
void GDClass::Display(void)
{
    EVE_cmd_dl(DL_DISPLAY);
}

/*
The SWAP command performs the same task as writing to the swap register; once the display list has
been written to the RAM_DL, this command will swap the foreground and background display list
memory so that the newly written display list is now active on the screen.
*/
void GDClass::swap(void ) 
{
    EVE_cmd_dl(DL_DISPLAY); /* mark the end of the display-list */
    EVE_cmd_dl(CMD_SWAP);   /* make this list active */
}

/* 
instruct the co-processor to start a new display list 
*/
void GDClass::cmd_dlstart()
{
    EVE_cmd_dl(CMD_DLSTART); 
}



/**
* @brief The VERTEX2F command allows negative
 coordinates. It also allows fractional coordinates, because it specifies screen (x,y) in
 units of 1/16 of a pixel.

Es erlaubt auch gebrochene Koordinaten, da es den Bildschirm (x,y) in angibt
Einheiten von 1/16 eines Pixels.
*/
void GDClass::Vertex2f(int16_t x, int16_t y) 
{
    EVE_cmd_dl(VERTEX2F(x,y));
}



/**
* @brief The VERTEX2II command used above only allows positive screen coordinates
*/
void GDClass::Vertex2ii(uint16_t x, uint16_t y, byte handle, byte cell)
{
    // ACHTUNG x und y max 511 pixel


    EVE_cmd_dl(VERTEX2II(x, y, handle, cell));    
}

void GDClass::Begin(Primitive prim) 
{
    EVE_cmd_dl( DL_BEGIN | prim );
}

//#########################
// 17.03.2024 




/**
 * @brief Attach the tag value for the following graphics objects drawn on the screen.
 */
void GDClass::Tag(byte s) {
    //GD2-Lib: cI((3UL << 24) | ((s & 255L) << 0));
    EVE_cmd_dl( (DL_TAG | s) );
    
}
/**
* @brief Control the writing of the tag buffer.
*/
void GDClass::TagMask(byte mask) {
    
    //GD2-Lib: cI((20UL << 24) | ((mask & 1L) << 0));
    EVE_cmd_dl(  (DL_TAG_MASK | ((mask) & 1UL)) );
}

/**
 * @brief Set the radius of points to be drawn with primitive POINTS in 1/16 pixel precision.
 */
void GDClass::PointSize(uint16_t size)
{       
    EVE_cmd_dl( (DL_POINT_SIZE | (size & 0x1FFFUL))  );
 
}

void GDClass::LineWidth(uint16_t width)
{    
    EVE_cmd_dl(  (DL_LINE_WIDTH | (width & 0xFFFUL)) );
}


void GDClass::get_inputs(void ) 
{
	byte *bi = (byte*)&inputs;

    EVE_memRead_sram_buffer( REG_TRACKER, bi, 4);
    EVE_memRead_sram_buffer( REG_TOUCH_RZ, bi + 4,  13);
    EVE_memRead_sram_buffer( REG_TAG,bi + 17,  1);

    inputs.touching = (inputs.x != -32768);
    // inputs.xytouch.set(PIXELS(inputs.x), PIXELS(inputs.y));

}


/**
 * @brief Load a ROM font into bitmap handle.
 * @note - generates display list commands, so it needs to be put in a display list
 * 
 * @example cmd_romfont( 1, 34 ) font-size 34 to font 1
 */
void GDClass::cmd_romfont(uint32_t font, uint32_t romslot) {
    EVE_cmd_romfont( font, romslot);
}

void GDClass::cmd_button(int16_t x, int16_t y, uint16_t w, uint16_t h, byte font, uint16_t options, const char *s) 
{
    EVE_cmd_button(x, y, w, h, font, options, s);
}



void GDClass::wr(uint32_t addr, uint8_t v) {
   EVE_memWrite8( addr, v);
}


////////////////////////////////////////////////////////////////////////

GDClass GD;

////////////////////////////////////////////////////////////////////////







