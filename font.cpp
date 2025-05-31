//#include <iostream>

#include "font.hpp"

// objcopyで埋め込まれたシンボル（変数）を参照
extern const uint8_t _binary_hankaku_bin_start;//←先頭文字そのものの値
extern const uint8_t _binary_hankaku_bin_end;//←最終文字の次の番地の中身の値
extern const uint8_t _binary_hankaku_bin_size;

const uint8_t* GetFont(int i){
    char c='c';
    auto index = 16 * i;//static_cast<unsigned int>(c);//目的の文字の先頭番地=asciiコードの順番×フォント一文字のバイト数
    const uint8_t* start = &_binary_hankaku_bin_start;//←先頭文字の格納されている番地
    const uint8_t* end   = &_binary_hankaku_bin_end;//最終文字の次の番地

    return &_binary_hankaku_bin_start+index;
}