TARGET = kernel.elf
OBJS = main.o ../hankaku.o ../font.o pci.o asmfunc.o newlib_support.o

CFLAGS   += -O2 -Wall -g --target=x86_64-elf -ffreestanding -mno-red-zone
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
	ld.lld $(LDFLAGS) -o kernel.elf $(OBJS) -lc 

%.o: %.cpp Makefile
	clang++ $(INCDIR) $(CPPFLAGS) $(CXXFLAGS) -c $< 

%.o: %.c Makefile
	clang $(CPPFLAGS) $(CFLAGS) -c $<

.%.d: %.c
	clang $(CPPFLAGS) $(CFLAGS) -MM $< > $@
	$(eval OBJ = $(<:.c=.o))
	sed --in-place 's|$(notdir $(OBJ))|$(OBJ)|' $@