
INCLUDE_PATH= "$(HPGCC)\include"
LIBS_PATH= "$(HPGCC)\lib"

export CC= arm-elf-gcc
export AR= arm-elf-ar
export ELF2HP= elf2hp

export C_FLAGS= -mtune=arm920t -mcpu=arm920t \
	-mlittle-endian -fomit-frame-pointer -Wall \
	-Os -I$(INCLUDE_PATH) -L$(LIBS_PATH)
#add this for Thumb interwork mode:
#export C_FLAGS += -mthumb-interwork -mthumb

export LD= arm-elf-ld
export LD_FLAGS= -L$(LIBS_PATH) -T MMUld.script $(LIBS_PATH)/crt0.o 
export LIBS= -lwin -lggl -lhpg -lhplib -lgcc

SRC = $(shell echo *.c)

OBJ = $(SRC:%.c=%.o)

EXE = $(SRC:%.c=%.exe)

HP = $(SRC:%.c=%.hp)

all: $(HP)

install: all

clean:
	rm -rf *.o *.hp *.exe


%.o: %.c
	$(CC) $(C_FLAGS) -c $<

%.exe: %.o
	$(LD) $(LD_FLAGS) $< $(LIBS) -o $@

%.hp: %.exe
	$(ELF2HP) $< $@



