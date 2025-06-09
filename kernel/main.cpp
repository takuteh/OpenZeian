/** 
 * @file main.cpp
 *
 * カーネル本体のプログラムを書いたファイル．
 */

#include <cstdint>
#include <cstddef>
#include <cstdio>
#include "frame_buffer_config.hpp"
#include "../font.hpp"
#include "pci.hpp"

static char buf[31][31]={};

// #@@range_begin(write_pixel)
struct PixelColor {
  uint8_t r, g, b;
};

// #@@range_begin(mosue_cursor_shape)
const int kMouseCursorWidth = 15;
const int kMouseCursorHeight = 24;
const char mouse_cursor_shape[kMouseCursorHeight][kMouseCursorWidth + 1] = {
  "@              ",
  "@@             ",
  "@.@            ",
  "@..@           ",
  "@...@          ",
  "@....@         ",
  "@.....@        ",
  "@......@       ",
  "@.......@      ",
  "@........@     ",
  "@.........@    ",
  "@..........@   ",
  "@...........@  ",
  "@............@ ",
  "@......@@@@@@@@",
  "@......@       ",
  "@....@@.@      ",
  "@...@ @.@      ",
  "@..@   @.@     ",
  "@.@    @.@     ",
  "@@      @.@    ",
  "@       @.@    ",
  "         @.@   ",
  "         @@@   ",
};
// #@@range_end(mosue_cursor_shape)

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
  // if(c != 'A'){
  //   return;
  // }
  for(int dy=0;dy<16;++dy){
    for(int dx=0;dx<8;++dx){
      //dyをdxの位置までビットシフトして先頭ビットが1ならピクセルを描画
      const uint8_t* font=GetFont(c);
      if((font[dy]<<dx)&0b10000000){
        WritePixel(config,x+dx,y+dy,color);
      }
    }
  }
}

void WriteString(const FrameBufferConfig& config,int x, int y,const char* s,const PixelColor& color){
  int i=0;
  while(s[i]!=0){
    WriteAscii(config,8*(x+i),16*y,s[i],{0,0,0});
    i++;
  }
}

void PutString(const char* s,const FrameBufferConfig& frame_buffer_conf){
  int cursor_column=0;
  int cursor_row=0;
  int maxColumns=30;//最大列(x)
  int maxRows=30;//最大行(y)

  //char buf[maxRows][maxColumns+1];
  while(*s!='\0'){
    if(*s=='\n'||cursor_column==maxColumns){//改行文字か端まで行ったら      
        buf[cursor_row][cursor_column]='\0';
         cursor_column=0;
         cursor_row++;
    }else{
    //端までいかなければカーソルを進める
    WriteAscii(frame_buffer_conf,8*cursor_column,16*cursor_row,*s,{0,0,0});
    buf[cursor_row][cursor_column]=*s;
    cursor_column++;
    }

    if(cursor_row > maxRows){//最大列までいった場合スクロール
    for (int y = 0; y < 16 * maxRows; ++y) {//一度画面をフラッシュ
      for (int x = 0; x < 8 * maxColumns; ++x) {
        WritePixel(frame_buffer_conf, x, y, {0, 0, 255});
      }
    }
    for(int i=0;i<=maxRows;i++){//再描画
      WriteString(frame_buffer_conf,0,i,buf[i+1],{0,0,0});
    }
  }
    s++;
}
}

void WriteMouse(const FrameBufferConfig& config,int x, int y){
  for(int dy=0;dy<=kMouseCursorHeight;dy++){
    for(int dx=0;dx<=kMouseCursorWidth;dx++){
      if(mouse_cursor_shape[dy][dx]=='@'){
        WritePixel(config, x+dx, y+dy, {0, 0, 0});
      }else if(mouse_cursor_shape[dy][dx]=='.'){
        WritePixel(config, x+dx, y+dy, {255, 255, 255});
      }
    }
  }
}
void ShowPciDeviceInfo(const FrameBufferConfig& config, int x, int y, const pci::Device& dev) {
  uint16_t vendor_id = pci::ReadVendorId(dev.bus, dev.device, dev.function);
  uint32_t class_code = pci::ReadClassCode(dev.bus, dev.device, dev.function);
  char buf[128];

  sprintf(buf, "%02x.%02x.%x: vend %04x, class %06x, head %02x\n",
          dev.bus, dev.device, dev.function,
          vendor_id, class_code >> 8, dev.header_type);

  WriteString(config, x, y, buf, {255, 255, 255});  // 白色で出力（適宜調整）
}

// int printk(const char* format, const FrameBufferConfig& frame_buffer_config,...) {
//   va_list ap;
//   int result;
//   char s[1024];

//   va_start(ap, format);
//   result = vsprintf(s, format, ap);
//   va_end(ap);

//   PutString(s,config);
//   return result;
// }
// #@@range_begin(call_write_pixel)
extern "C" void KernelMain(const FrameBufferConfig& frame_buffer_config) {
  for (int x = 0; x < frame_buffer_config.horizontal_resolution; ++x) {
    for (int y = 0; y < frame_buffer_config.vertical_resolution; ++y) {
      WritePixel(frame_buffer_config, x, y, {0, 0, 255});
    }
  }
  for (int x = 0; x < 200; ++x) {
    for (int y = 0; y < 100; ++y) {
      WritePixel(frame_buffer_config, 100 + x, 100 + y, {0, 255, 0});
    }
  }
  int i=0;
  //PutString("1aasdd\n 2afga\n 3asdtga\n 4gfhjhk\n 5fdhj\n 6dfgs\n 7adfg\n 8adfha\n 9adfha\n 10sdagag\n 11dfgaf\n 12dfga",frame_buffer_config);
  WriteMouse(frame_buffer_config,200,100);

  auto err = pci::ScanAllBus();
  //printk("ScanAllBus: %s\n",frame_buffer_config, err.Name());

  for (int i = 0; i < pci::num_device; ++i) {
    const auto& dev = pci::devices[i];
    auto vendor_id = pci::ReadVendorId(dev.bus, dev.device, dev.function);
    auto class_code = pci::ReadClassCode(dev.bus, dev.device, dev.function);
  char buf[128];
  sprintf(buf, "%02x.%02x.%x: vend %04x, class %08x, head %02x",
          dev.bus, dev.device, dev.function,
          vendor_id, class_code, dev.header_type);

  WriteString(frame_buffer_config, 0, i + 2, buf, {255, 255, 255});
  }
  // #@@range_end(show_devices)
  while (1) __asm__("hlt");
}
// #@@range_end(call_write_pixel)
