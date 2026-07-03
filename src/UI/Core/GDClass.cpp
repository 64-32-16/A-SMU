#include <SPI.h>
#include "GDClass.h"
#include "EVE.h"

// EVE_target_Arduino_RP2040.h
// SPI-Einstellungen T50 Riverdi
// Config der Library über platform.ini

/**
 * RVT50UQFNWC03 V2 
 * 300 mA Stromverbrauch
 * @brief Initialisiert SPI, EVE und interne Basisdaten.
 * @param p Aktuell ungenutzt, bleibt für API-Kompatibilität erhalten.
 */
void GDClass::begin(byte p)
{
    (void)p;

    pinMode(EVE_CS, OUTPUT);
    digitalWrite(EVE_CS, HIGH);

    SPI.begin();

    SPI.beginTransaction(SPISettings(16UL * 1000000UL, MSBFIRST, SPI_MODE0));

    EveStatus = EVE_init();

    




    // Aktuelle Displaygröße der A-SMU GUI
    w = 800;
    h = 480;

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

void GDClass::get_inputs(void ) 
{
	byte *bi = (byte*)&inputs;

    EVE_memRead_sram_buffer( REG_TRACKER, bi, 4);
    EVE_memRead_sram_buffer( REG_TOUCH_RZ, bi + 4,  13);
    EVE_memRead_sram_buffer( REG_TAG,bi + 17,  1);

    inputs.touching = (inputs.x != -32768);
    // inputs.xytouch.set(PIXELS(inputs.x), PIXELS(inputs.y));

}

 /*


 
void GDClass::get_inputs(void)
{
    uint8_t buf[4];

    EVE_memRead_sram_buffer(REG_CTOUCH_TOUCH0_XY, buf, 4);

    int16_t x = (int16_t)((uint16_t)buf[0] | ((uint16_t)buf[1] << 8));
    int16_t y = (int16_t)((uint16_t)buf[2] | ((uint16_t)buf[3] << 8));

    inputs.touching = 0;
    inputs.x = -32768;
    inputs.y = -32768;

    if (!(x == -32768 && y == -32768))
    {
        inputs.touching = 1;
        inputs.x = x;
        inputs.y = y;
    }
}

*/



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