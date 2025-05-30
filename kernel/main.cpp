/** 
 * @file main.cpp
 *
 * カーネル本体のプログラムを書いたファイル．
 */

#include <cstdint>
#include <cstddef>

#include "frame_buffer_config.hpp"
#include "../font.hpp"

// #@@range_begin(write_pixel)
struct PixelColor {
  uint8_t r, g, b;
};

const uint8_t kFontA[16]{
  0b00000000,
  0b00011000,
  0b00011000,
  0b00011000,
  0b00011000,
  0b00100100,
  0b00100100,
  0b00100100,
  0b01111110,
  0b01000010,
  0b01000010,
  0b01000010,
  0b11100111,
  0b00000000,
  0b00000000,
};

/** WritePixelは1つの点を描画します．
 * @retval 0   成功
 * @retval 非0 失敗
 */
int WritePixel(const FrameBufferConfig& config,
               int x, int y, const PixelColor& c) {
                //config.pixels_per_scan_line 1行当たりのピクセル数
  const int pixel_position = config.pixels_per_scan_line * y + x;
  if (config.pixel_format == kPixelRGBResv8BitPerColor) {
    uint8_t* p = &config.frame_buffer[4 * pixel_position];
    p[0] = c.r;
    p[1] = c.g;
    p[2] = c.b;
  } else if (config.pixel_format == kPixelBGRResv8BitPerColor) {
    uint8_t* p = &config.frame_buffer[4 * pixel_position];
    p[0] = c.b;
    p[1] = c.g;
    p[2] = c.r;
  } else {
    return -1;
  }
  return 0;
}
// #@@range_end(write_pixel)
void WriteAscii(const FrameBufferConfig& config,int x, int y,char c,const PixelColor& color){
  if(c != 'A'){
    return;
  }
  for(int dy=0;dy<16;++dy){
    for(int dx=0;dx<8;++dx){
      //dyをdxの位置までビットシフトして先頭ビットが1ならピクセルを描画
      const uint8_t* font=GetFont();
      if((font[dy]<<dx)&0b10000000){
        WritePixel(config,x+dx,y+dy,color);
      }
    }
  }
}
// #@@range_begin(call_write_pixel)
extern "C" void KernelMain(const FrameBufferConfig& frame_buffer_config) {
  for (int x = 0; x < frame_buffer_config.horizontal_resolution; ++x) {
    for (int y = 0; y < frame_buffer_config.vertical_resolution; ++y) {
      WritePixel(frame_buffer_config, x, y, {255, 0, 0});
    }
  }
  for (int x = 0; x < 200; ++x) {
    for (int y = 0; y < 100; ++y) {
      WritePixel(frame_buffer_config, 100 + x, 100 + y, {0, 255, 0});
    }
  }
  WriteAscii(frame_buffer_config,50,50,'A',{0,0,0});
  while (1) __asm__("hlt");
}
// #@@range_end(call_write_pixel)
