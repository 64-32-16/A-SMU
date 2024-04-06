#ifndef _GD2_H_INCLUDED
#define _GD2_H_INCLUDED

#include "Arduino.h"

////////////////////////////////////////////

enum Primitive {
BITMAPS              = 1,
POINTS               = 2,
LINES                = 3,
LINE_STRIP           = 4,
EDGE_STRIP_R         = 5,
EDGE_STRIP_L         = 6,
EDGE_STRIP_A         = 7,
EDGE_STRIP_B         = 8,
RECTS                = 9,
};

////////////////////////////////////////////////////////////////////////





/////////////////////////////////////////


#define OPT_MONO             1
#define OPT_NODL             2
#define OPT_FLAT             256
#define OPT_CENTERX          512
#define OPT_CENTERY          1024
#define OPT_CENTER           (OPT_CENTERX | OPT_CENTERY)

#define REG_ANA_COMP         ((uint32_t) 0x00302184UL) /* only listed in datasheet */
#define REG_BIST_EN          ((uint32_t) 0x00302174UL) /* only listed in datasheet */
#define REG_CLOCK            ((uint32_t) 0x00302008UL)
#define REG_CMDB_SPACE       ((uint32_t) 0x00302574UL)
#define REG_CMDB_WRITE       ((uint32_t) 0x00302578UL)
#define REG_CMD_DL           ((uint32_t) 0x00302100UL)
#define REG_CMD_READ         ((uint32_t) 0x003020f8UL)
#define REG_CMD_WRITE        ((uint32_t) 0x003020fcUL)
#define REG_CPURESET         ((uint32_t) 0x00302020UL)
#define REG_CSPREAD          ((uint32_t) 0x00302068UL)
#define REG_CTOUCH_EXTENDED  ((uint32_t) 0x00302108UL)
#define REG_CTOUCH_TOUCH0_XY ((uint32_t) 0x00302124UL) /* only listed in datasheet */
#define REG_CTOUCH_TOUCH4_X  ((uint32_t) 0x0030216cUL)
#define REG_CTOUCH_TOUCH4_Y  ((uint32_t) 0x00302120UL)
#define REG_CTOUCH_TOUCH1_XY ((uint32_t) 0x0030211cUL)
#define REG_CTOUCH_TOUCH2_XY ((uint32_t) 0x0030218cUL)
#define REG_CTOUCH_TOUCH3_XY ((uint32_t) 0x00302190UL)
#define REG_TOUCH_CONFIG     ((uint32_t) 0x00302168UL)
#define REG_DATESTAMP        ((uint32_t) 0x00302564UL) /* only listed in datasheet */
#define REG_DITHER           ((uint32_t) 0x00302060UL)
#define REG_DLSWAP           ((uint32_t) 0x00302054UL)
#define REG_FRAMES           ((uint32_t) 0x00302004UL)
#define REG_FREQUENCY        ((uint32_t) 0x0030200cUL)
#define REG_GPIO             ((uint32_t) 0x00302094UL)
#define REG_GPIOX            ((uint32_t) 0x0030209cUL)
#define REG_GPIOX_DIR        ((uint32_t) 0x00302098UL)
#define REG_GPIO_DIR         ((uint32_t) 0x00302090UL)
#define REG_HCYCLE           ((uint32_t) 0x0030202cUL)
#define REG_HOFFSET          ((uint32_t) 0x00302030UL)
#define REG_HSIZE            ((uint32_t) 0x00302034UL)
#define REG_HSYNC0           ((uint32_t) 0x00302038UL)
#define REG_HSYNC1           ((uint32_t) 0x0030203cUL)
#define REG_ID               ((uint32_t) 0x00302000UL)
#define REG_INT_EN           ((uint32_t) 0x003020acUL)
#define REG_INT_FLAGS        ((uint32_t) 0x003020a8UL)
#define REG_INT_MASK         ((uint32_t) 0x003020b0UL)
#define REG_MACRO_0          ((uint32_t) 0x003020d8UL)
#define REG_MACRO_1          ((uint32_t) 0x003020dcUL)
#define REG_MEDIAFIFO_READ   ((uint32_t) 0x00309014UL) /* only listed in programmers guide */
#define REG_MEDIAFIFO_WRITE  ((uint32_t) 0x00309018UL) /* only listed in programmers guide */
#define REG_OUTBITS          ((uint32_t) 0x0030205cUL)
#define REG_PCLK             ((uint32_t) 0x00302070UL)
#define REG_PCLK_POL         ((uint32_t) 0x0030206cUL)
#define REG_PLAY             ((uint32_t) 0x0030208cUL)
#define REG_PLAYBACK_FORMAT  ((uint32_t) 0x003020c4UL)
#define REG_PLAYBACK_FREQ    ((uint32_t) 0x003020c0UL)
#define REG_PLAYBACK_LENGTH  ((uint32_t) 0x003020b8UL)
#define REG_PLAYBACK_LOOP    ((uint32_t) 0x003020c8UL)
#define REG_PLAYBACK_PLAY    ((uint32_t) 0x003020ccUL)
#define REG_PLAYBACK_READPTR ((uint32_t) 0x003020bcUL)
#define REG_PLAYBACK_START   ((uint32_t) 0x003020b4UL)
#define REG_PWM_DUTY         ((uint32_t) 0x003020d4UL)
#define REG_PWM_HZ           ((uint32_t) 0x003020d0UL)
#define REG_RENDERMODE       ((uint32_t) 0x00302010UL) /* only listed in datasheet */
#define REG_ROTATE           ((uint32_t) 0x00302058UL)
#define REG_SNAPFORMAT       ((uint32_t) 0x0030201cUL) /* only listed in datasheet */
#define REG_SNAPSHOT         ((uint32_t) 0x00302018UL) /* only listed in datasheet */
#define REG_SNAPY            ((uint32_t) 0x00302014UL) /* only listed in datasheet */
#define REG_SOUND            ((uint32_t) 0x00302088UL)
#define REG_SPI_WIDTH        ((uint32_t) 0x00302188UL) /* listed with false offset in programmers guide V1.1 */
#define REG_SWIZZLE          ((uint32_t) 0x00302064UL)
#define REG_TAG              ((uint32_t) 0x0030207cUL)
#define REG_TAG_X            ((uint32_t) 0x00302074UL)
#define REG_TAG_Y            ((uint32_t) 0x00302078UL)
#define REG_TAP_CRC          ((uint32_t) 0x00302024UL) /* only listed in datasheet */
#define REG_TAP_MASK         ((uint32_t) 0x00302028UL) /* only listed in datasheet */
#define REG_TOUCH_ADC_MODE   ((uint32_t) 0x00302108UL)
#define REG_TOUCH_CHARGE     ((uint32_t) 0x0030210cUL)
#define REG_TOUCH_DIRECT_XY  ((uint32_t) 0x0030218cUL)
#define REG_TOUCH_DIRECT_Z1Z2 ((uint32_t) 0x00302190UL)
#define REG_TOUCH_MODE       ((uint32_t) 0x00302104UL)
#define REG_TOUCH_OVERSAMPLE ((uint32_t) 0x00302114UL)
#define REG_TOUCH_RAW_XY     ((uint32_t) 0x0030211cUL)
#define REG_TOUCH_RZ         ((uint32_t) 0x00302120UL)
#define REG_TOUCH_RZTHRESH   ((uint32_t) 0x00302118UL)
#define REG_TOUCH_SCREEN_XY  ((uint32_t) 0x00302124UL)
#define REG_TOUCH_SETTLE     ((uint32_t) 0x00302110UL)
#define REG_TOUCH_TAG        ((uint32_t) 0x0030212cUL)
#define REG_TOUCH_TAG1       ((uint32_t) 0x00302134UL) /* only listed in datasheet */
#define REG_TOUCH_TAG1_XY    ((uint32_t) 0x00302130UL) /* only listed in datasheet */
#define REG_TOUCH_TAG2       ((uint32_t) 0x0030213cUL) /* only listed in datasheet */
#define REG_TOUCH_TAG2_XY    ((uint32_t) 0x00302138UL) /* only listed in datasheet */
#define REG_TOUCH_TAG3       ((uint32_t) 0x00302144UL) /* only listed in datasheet */
#define REG_TOUCH_TAG3_XY    ((uint32_t) 0x00302140UL) /* only listed in datasheet */
#define REG_TOUCH_TAG4       ((uint32_t) 0x0030214cUL)/* only listed in datasheet */
#define REG_TOUCH_TAG4_XY    ((uint32_t) 0x00302148UL) /* only listed in datasheet */
#define REG_TOUCH_TAG_XY     ((uint32_t) 0x00302128UL)
#define REG_TOUCH_TRANSFORM_A ((uint32_t) 0x00302150UL)
#define REG_TOUCH_TRANSFORM_B ((uint32_t) 0x00302154UL)
#define REG_TOUCH_TRANSFORM_C ((uint32_t) 0x00302158UL)
#define REG_TOUCH_TRANSFORM_D ((uint32_t) 0x0030215cUL)
#define REG_TOUCH_TRANSFORM_E ((uint32_t) 0x00302160UL)
#define REG_TOUCH_TRANSFORM_F ((uint32_t) 0x00302164UL)
#define REG_TRACKER          ((uint32_t) 0x00309000UL) /* only listed in programmers guide */
#define REG_TRACKER_1        ((uint32_t) 0x00309004UL) /* only listed in programmers guide */
#define REG_TRACKER_2        ((uint32_t) 0x00309008UL) /* only listed in programmers guide */
#define REG_TRACKER_3        ((uint32_t) 0x0030900cUL) /* only listed in programmers guide */
#define REG_TRACKER_4        ((uint32_t) 0x00309010UL) /* only listed in programmers guide */
#define REG_TRIM             ((uint32_t) 0x00302180UL)
#define REG_VCYCLE           ((uint32_t) 0x00302040UL)
#define REG_VOFFSET          ((uint32_t) 0x00302044UL)
#define REG_VOL_PB           ((uint32_t) 0x00302080UL)
#define REG_VOL_SOUND        ((uint32_t) 0x00302084UL)
#define REG_VSIZE            ((uint32_t) 0x00302048UL)
#define REG_VSYNC0           ((uint32_t) 0x0030204cUL)
#define REG_VSYNC1           ((uint32_t) 0x00302050UL)


////

class GDClass {
public:
  uint32_t rseed;


  int w, h;
  uint8_t EveStatus;

  void begin(byte p); 

  void SaveContext(void);
  void RestoreContext(void);
  void Display(void);
  void Clear(void); 
  void cmd_text(int16_t x, int16_t y, byte font, uint16_t options, const char *s);
  void cmd_dlstart();
  void ColorA(byte alpha);
  void ColorRGB(uint32_t rgb); 
  void ClearColorRGB(uint32_t rgb);
  void ColorRGB(byte red, byte green, byte blue);
  void cmd_fgcolor( uint32_t color);
  void swap(void);
  void Vertex2f(int16_t x, int16_t y);
  void VertexFormat(byte frac);
  void Vertex2ii(uint16_t x, uint16_t y, byte handle = 0, byte cell = 0);
  void Begin(Primitive prim); 
  void PointSize(uint16_t size);
  void LineWidth(uint16_t width);
  void TagMask(byte mask);
  void Tag(byte s);

  void cmd_button(int16_t x, int16_t y, uint16_t w, uint16_t h, byte font, uint16_t options, const char *s);
  void wr(uint32_t addr, uint8_t v);
  void cmd_romfont(uint32_t font, uint32_t romslot) ;

  void get_inputs(void);


  uint16_t random();
  uint16_t random(uint16_t n);
  uint16_t random(uint16_t n0, uint16_t n1);
  
  void seed(uint16_t n);


    struct {
    uint16_t track_tag;
    uint16_t track_val;
    uint16_t rz;
    uint16_t __dummy_1;
    int16_t y;
    int16_t x;
    int16_t tag_y;
    int16_t tag_x;
    uint8_t tag;
    uint8_t ptag;
    uint8_t touching;
    // xy xytouch;
    // struct _wii wii[2];
  } inputs;

};

extern GDClass GD;




#endif