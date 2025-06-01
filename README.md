# 本コミットについて (osbook_day05d)

## 概要
入力された文字を行と列の2次元要素の配列にバッファとして格納できるようにした．また，テストで0行3列目に表示する文字をバッファに格納し，そのバッファの内容を10行10列目に表示している．

## 手順
1.各プログラムをコンパイル  
なぜか-fno-stack-protectorオプションをつけないとmakeが通らない
```
g++ -c font.cpp -o font.o
g++ -fno-stack-protector -c main.cpp -o main.o -I../ZeianLoaderPkg/include/
```
2.makefileでfont.o hankaku.o main.oをリンクしてコンパイル  

3.edk2以下に移動し，ビルド後実行
```
cd ~/edk2
build
~/osbook/devenv/run_qemu.sh ~/edk2/Build/ZeianLoaderX64/DEBUG_CLANG38/X64/Loader.efi ~/OpenZeian/kernel/kernel.elf
```