# 本コミットについて (osbook_day06b)

## 概要

newlib_support を導入して sprintf を利用できるようにした.(osbook_day05d)  
マウスを動かせるようにするためにまずは PCI デバイスを探索し，その一覧を表示できるようにした．(osbook_day06b)  
本コミットは実装が高度な内容であったため,mikanos からほとんどそのまま持ってきている．printk のみうまく動作しなかったので PutString で文字列の表示を行っている．

## 手順

1.各プログラムをコンパイル  
なぜか`-fno-stack-protector`オプションをつけないと make が通らない

```
g++ -c font.cpp -o font.o
g++ -fno-stack-protector -c main.cpp -o main.o -I../ZeianLoaderPkg/include/
g++ -c -o newlib_support.o newlib_support.c
```

2.makefile で`font.o`,`hankaku.o`,`main.o`,`newlib_support.o`をリンクしてコンパイル

3.edk2 以下に移動し，ビルド後実行

```
cd ~/edk2
build
~/osbook/devenv/run_qemu.sh ~/edk2/Build/ZeianLoaderX64/DEBUG_CLANG38/X64/Loader.efi ~/OpenZeian/kernel/kernel.elf
```
