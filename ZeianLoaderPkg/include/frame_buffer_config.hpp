#ifndef FRAME_BUFFER_CONFIG_HPP
#define FRAME_BUFFER_CONFIG_HPP

#include <stdint.h>

//UEFIで定義されているピクセルフォーマット
enum PixelFormat{
    //ハードによってサポートされている物が違うが，大半は以下の２つのどちらかで対応可
    kPixelRGBResv8BitPerColor,
    kPixelBGRResv8BitPerColor,
};

struct FrameBufferConfig{
    uint8_t* frame_buffer; //frame buffer領域へのポインタ
    uint32_t pixels_per_scan_line; //バッファの余白を含めた横方向のピクセル数
    uint32_t horizontal_resolution; //水平の解像度
    uint32_t vertical_resolution; //垂直の解像度
    enum PixelFormat pixel_format; //ピクセルのデータ形式
};

#endif