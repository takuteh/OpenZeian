TARGET = kernel.elf
OBJS = main.o ../hankaku.o ../font.o

CXXFLAGS += -O2 -Wall -g --target=x86_64-elf -ffreestanding -mno-red-zone \
            -fno-exceptions -fno-rtti -std=c++17
LDFLAGS  += --entry KernelMain -z norelro --image-base 0x100000 --static

INCDIR = -I ../ZeianLoaderPkg/include

.PHONY: all
all: $(TARGET)

.PHONY: clean
clean:
	rm -rf *.o

kernel.elf: $(OBJS) Makefile
	ld.lld $(LDFLAGS) -o kernel.elf $(OBJS) 

%.o: %.cpp Makefile
	clang++ $(INCDIR) $(CPPFLAGS) $(CXXFLAGS) -c $< 
