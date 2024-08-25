# Quick'n'dirty makefile [BC]

#//!
#//! THIS REALLY NEEDS TO HAVE DEPENDANCIES ADDED TO IT
#//! CURRENTLY IF YOU EDIT A .C FILE, THIS MAKEFILE WILL *NOT* NOTICE
#//! SO  *ALWAYS*  USE  `make clean all`  NOT  `make all`
#//!

ifeq ($(strip $(DEVKITPPC)),)
  $(error "Use   export DEVKITPPC=<path to>devkitPPC   and try again")
endif

ifeq ($(strip $(DEVKITPRO)),)
  $(error "Use   export DEVKITPRO=<path to>devkitPRO   and try again")
endif

APP := wiifont
ARC := lib$(APP).a

MAP := $(APP).map

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
OGCINCD := $(OGC)/include
OGCLIBD := $(OGC)/lib/wii

BMFCORE := core

MACHDEP := -DGEKKO -mrvl -mcpu=750 -meabi -mhard-float
CFLAGS  := -O2 -Wall $(MACHDEP) -I$(OGCINCD) -I$(BMFCORE)

LDFLAGS := $(MACHDEP) -Wl,-Map,$(MAP)

BMFHDR  := $(wildcard core/*.h)
DRAWHDR := $(wildcard drawers/*.h)
FXHDR   := $(wildcard effects/*/*.h)

SRC     := $(wildcard core/*.c)
SRC     += $(wildcard drawers/*.c)
SRC     += $(wildcard effects/*/*.c)

BAK     := $(wildcard *.bak)
BAK     += $(wildcard core/*.bak)
BAK     += $(wildcard drawers/*.bak)
BAK     += $(wildcard effects/*.bak)
BAK     += $(wildcard effects/*/*.bak)

OBJ     := $(SRC:%.c=%.o)

all : $(ARC)


$(ARC) : $(OBJ)
	@echo "Linking as: $@"
	@$(AR) -r $(ARC) $(OBJ)

clean :
	rm -f $(OBJ) $(ARC) $(MAP)

scrub : clean
	rm -f $(BAK)

install :
	rm -rf $(OGCINCD)/bmf
	mkdir -p  $(OGCLIBD)  $(OGCINCD)  $(OGCINCD)/bmf
	cp -f $(ARC) $(OGCLIBD)/
	cp -f $(BMFHDR) $(OGCINCD)/
	cp -f $(DRAWHDR) $(FXHDR) $(OGCINCD)/bmf

%.o : %.c
	@echo "Compiling: $<"
	@$(CC) $(CFLAGS) -c $< -o $@
