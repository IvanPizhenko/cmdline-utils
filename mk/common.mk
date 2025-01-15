# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) Ivan Pizhenko, 2025. All rights reserved.
# See file LICENSE for more information.

.PHONY: all clean install
.SUFFIXES:

ifeq ($(OS),Windows_NT)
EXE_EXT:=.exe
endif

ifeq ("x$(MSYSTEM_PREFIX)","x")
INSTALL_PREFIX:=/usr/local
else
INSTALL_PREFIX:=$(MYSTEM_PREFIX)
endif

SRC:=$(NAME).c $(EXTRA_SRC)
EXE:=$(NAME)$(EXE_EXT)

MAN_SRC:=$(NAME).1
MAN:=$(addsuffix .gz, $(MAN_SRC))

all: $(EXE)

clean:
	rm $(EXE)

$(EXE): $(SRC)
	cc $(CFLAGS) -O2 $(SRC) -o $(NAME)
	strip $(EXE)

%.1.gz: %.1
	gzip -k9 $<

install: $(EXE) $(MAN)
	install -d "$(INSTALL_PREFIX)/bin" "$(INSTALL_PREFIX)/share/man/man1"
	install $(EXE) "$(INSTALL_PREFIX)/bin"
	install $(MAN) "$(INSTALL_PREFIX)/share/man/man1"
