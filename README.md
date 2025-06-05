# 本コミットについて (osbook_day05e)

## 概要

最大列まで行った場合は，最大列までの描画した部分を一度塗りつぶし，1 行先のバッファの内容を再描画することでスクロールできるようにした．
最大列以降も文字列が描画されてしまうのが問題

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
