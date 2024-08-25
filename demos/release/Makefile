# Quick'n'dirty makefile [BC] v2

ifeq ($(strip $(DEVKITPPC)),)
  $(error "Use   export DEVKITPPC=<path to>devkitPPC   and try again")
endif

ifeq ($(strip $(DEVKITPRO)),)
  $(error "Use   export DEVKITPRO=<path to>devkitPRO   and try again")
endif

APP := wiifont_release_demo
ELF := $(APP).elf
DOL := $(APP).dol
MAP := $(ELF).map

LIBS := -lwiifont
LIBS += -lgrrlib -lpngu -lpng -ljpeg -lz -lfat
LIBS += -lwiiuse
#LIBS += -lmodplay -lasnd
LIBS += -lbte -logc
LIBS +=  -lm

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

OGC  := $(DEVKITPRO)/libogc
INCD := $(OGC)/include
LIBD := $(OGC)/lib/wii

MACHDEP := -DGEKKO -mrvl -mcpu=750 -meabi -mhard-float
CFLAGS  := -O2 -Wall $(MACHDEP) -I $(INCD)

LDFLAGS   = $(MACHDEP) -Wl,-Map,$(MAP)
LIBPATHS := -L$(DEVKITPRO)/libogc/lib/wii

SRC    := wii_demo.c hooks.c
SRCOBJ := $(SRC:%.c=%.o)

RES    := $(wildcard *.png *.fnt)
RESOBJ := $(addsuffix .o,$(basename $(RES)))
RESC   := $(addsuffix .c,$(basename $(RES)))
RESH   := $(addsuffix .h,$(basename $(RES)))

OBJ  := $(RESOBJ) $(SRCOBJ)

all : $(DOL)

$(DOL) : $(ELF)
	@echo "Converting to: $@"
	@$(ELF2DOL) $< $@

$(ELF) : $(OBJ) $(RESOBJ)
	@echo "Linking as: $@"
	@$(CC) $^ $(LDFLAGS) $(LIBPATHS) $(LIBS) -o $@

clean :
	rm -f $(OBJ) $(RESC) $(RESH) $(ELF) $(DOL) $(MAP)
	rm -rf $(APP)

scrub : clean
	rm -f *.bak

run : $(DOL)
	$(UPLOAD) $(DOL)

distro : all
	@rm -rf $(APP)
	@mkdir -p $(APP)
	@cp $(DOL) $(APP)/boot.dol
	@cp distro/* $(APP)  &&  rm -f $(APP)/*.psp

%.o : %.c
	@echo "Compiling: $<"
	@$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS : %.c
%.c : %.png
	@echo "Converting resource: $<"
	@./raw2c.exe $< 2>nul

%.c : %.fnt
	@echo "Converting resource: $<"
	@./raw2c.exe $< 2>nul