1.適当なバイナリファイルを作る
```
echo -n "hello" > test.bin
```
2.バイナリファイルをELFオブジェクトファイルに変換すると`_start`,`_end`,`_size`シンボルが自動的に作成される
```
objcopy -I binary -O elf64-x86-64 -B i386 test.bin test.o
```

3.`nm test.o`でシンボルと値の確認ができる

```
~/OpenZeian$ nm test.o
0000000000000005 D _binary_test_bin_end
0000000000000005 A _binary_test_bin_size
0000000000000000 D _binary_test_bin_start
```
4.オブジェクトファイルをリンクしてコンパイル
```
g++ load_bin.cpp test.o -o load_bin
```