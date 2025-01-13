.PHONY: all clean install
.SUFFIXES:

include local.mk

ifeq ($(OS),Windows_NT)
EXE_EXT:=.exe
endif

SRC:=$(NAME).c $(EXTRA_SRC)
EXE:=$(NAME)$(EXE_EXT)

all: $(EXE)

clean:
	rm $(EXE)

$(EXE): $(SRC)
	cc $(CFLAGS) -O2 $(SRC) -o $(NAME)
	strip $(EXE)

install: $(EXE)
	cp -f $(EXE) /usr/local/bin
