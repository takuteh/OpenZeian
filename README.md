1.フォントのテキストファイルを作る  
2.テキストをバイナリに変換
```
python3 makefont.py -o test.bin test.txt
```
3.バイナリファイルをELFオブジェクトファイルに変換すると`_start`,`_end`,`_size`シンボルが自動的に作成される
```
objcopy -I binary -O elf64-x86-64 -B i386:x86-64 test.bin test.o
```

4.`nm test.o`でシンボルと値の確認ができる

```
~/OpenZeian$ nm test.o
0000000000000005 D _binary_test_bin_end
0000000000000005 A _binary_test_bin_size
0000000000000000 D _binary_test_bin_start
```
5.各プログラムをコンパイル
```
g++ -c load_bin.cpp -o load_bin.o
g++ -c main.cpp -o main.o -I../ZeianLoaderPkg/include/
```
6.makefileでload_bin.o test.o main.oをリンクしてコンパイル  

7.edk2以下に移動し，ビルド後実行
```
cd ~/edk2
build
~/osbook/devenv/run_qemu.sh ~/edk2/Build/ZeianLoaderX64/DEBUG_CLANG38/X64/Loader.efi ~/OpenZeian/kernel/kernel.elf
```