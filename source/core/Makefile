ifeq ($(strip $(DEVKITPPC)),)
  $(error "Use   export DEVKITPPC=<path to>devkitPPC   and try again")
endif

ifeq ($(strip $(DEVKITPRO)),)
  $(error "Use   export DEVKITPRO=<path to>devkitPRO   and try again")
endif

BINDIR  := $(DEVKITPPC)/bin
PREFIX  := $(BINDIR)/powerpc-gekko-
CC      := $(PREFIX)gcc
CXX     := $(PREFIX)g++
AR      := $(PREFIX)ar
AS      := $(PREFIX)as
LD      := $(CC)
OBJCOPY := $(PREFIX)objcopy
ELF2DOL := $(BINDIR)/elf2dol
UPLOAD  := $(BINDIR)/wiiload

OGC     := $(DEVKITPRO)/libogc
OGCINC  := $(OGC)/include
OGCLIB  := $(OGC)/lib/wii

BMFCORE := .

MACHDEP := -DGEKKO -mrvl -mcpu=750 -meabi -mhard-float
CFLAGS  := -O2 -Wall $(MACHDEP) -I$(OGCINC) -I$(BMFCORE)

SRC     := $(wildcard *.c)
BLD     := .
OBJ     := $(addprefix $(BLD)/,$(SRC:.c=.o))

all : $(OBJ)

clean :
	rm -f $(OBJ)

%.o : %.c
	$(CC) $(CFLAGS) $< -c -o $@
