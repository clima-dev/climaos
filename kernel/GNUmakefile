# Name of the final executable
override KERNEL := kernel.elf

# The compiler we are using
CC := cc
AS := nasm

# Compiler flags
CFLAGS :=                             \
	-Wall -Wextra -g -Og -I stivale/  \
	-I kernel/klibc/liballoc/include/ \
	-I kernel/acpi/lai/include/       \
	-pipe -DKVERSION=\"git-$(shell git log -1 --pretty=format:%h)\" \
	-g
	

# Assembler flags
ASFLAGS := -gdwarf -MD -MP -g

# Internal flags that shouldn't be changed
override INTERNALCFLAGS :=   \
	-I.                  \
	-std=gnu11           \
	-ffreestanding       \
	-fno-stack-protector \
	-fno-pic             \
	-mabi=sysv           \
	-mno-80387           \
	-mno-mmx             \
	-mno-3dnow           \
	-mno-sse             \
	-mno-sse2            \
	-mno-red-zone        \
	-mcmodel=kernel      \
	-MMD

# Internal linker flags that should not be changed by the user.
override INTERNALLDFLAGS :=    \
	-Tlinker.ld            \
	-nostdlib              \
	-zmax-page-size=0x1000 \
	-static

override CFILES := $(shell find ./ -type f -name '*.c')
override ASMFILES := $(shell find ./ -type f -name '*.asm')
override OBJ := $(CFILES:.c=.o) $(ASMFILES:.asm=.o)
override DEPENDS := $(CFILES:.c=.d) $(ASMFILES:.asm=.o.d)

# Default target.
.PHONY: all
all: $(KERNEL)

stivale2.h:
	curl https://raw.githubusercontent.com/stivale/stivale/master/stivale2.h -o $@

# Link rules for the final kernel executable.
$(KERNEL): $(OBJ)
	$(LD) $(OBJ) $(LDFLAGS) $(INTERNALLDFLAGS) -o $@

# Compilation rules for *.c files.
-include $(DEPENDS)
%.o: %.c stivale2.h
	$(CC) $(CFLAGS) $(INTERNALCFLAGS) -c $< -o $@

%.o: %.asm GNUmakefile
	$(AS) $(ASFLAGS) -f elf64 $< -o $@

# Remove object files and the final executable.
.PHONY: clean
clean:
	rm -rf $(KERNEL) $(OBJ) $(DEPENDS)

.PHONY: distclean
distclean: clean
	rm -f stivale2.h
