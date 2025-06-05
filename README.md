# 本コミットについて (osbook_day06a)

## 概要

マウスの描画をした．バイナリにしようと考えたが，矢印のフォントを作るのにスペースが必要なため断念して main.cpp に直接組み込むことにした．

## 手順

1.各プログラムをコンパイル  
なぜか`-fno-stack-protector`オプションをつけないと make が通らない

```
g++ -c font.cpp -o font.o
g++ -fno-stack-protector -c main.cpp -o main.o -I../ZeianLoaderPkg/include/
```

2.makefile で`font.o`,`hankaku.o`,`main.o`をリンクしてコンパイル

3.edk2 以下に移動し，ビルド後実行

```
cd ~/edk2
build
~/osbook/devenv/run_qemu.sh ~/edk2/Build/ZeianLoaderX64/DEBUG_CLANG38/X64/Loader.efi ~/OpenZeian/kernel/kernel.elf
```
