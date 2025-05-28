#include <iostream>
#include <cstdint>

// objcopyで埋め込まれたシンボル（変数）を参照
extern const uint8_t _binary_test_bin_start;//←先頭文字そのものの値
extern const uint8_t _binary_test_bin_end;//←最終文字の次の番地の中身の値
extern const uint8_t _binary_test_bin_size;

int main(){
    const uint8_t* start = &_binary_test_bin_start;//←先頭文字の格納されている番地
    const uint8_t* end   = &_binary_test_bin_end;//最終文字の次の番地

    std::cout<<_binary_test_bin_start<<std::endl;
    std::cout<<*(&_binary_test_bin_end - 3)<<std::endl;

    return 0;
}