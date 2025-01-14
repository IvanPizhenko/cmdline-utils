# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) Ivan Pizhenko, 2025. All rights reserved.
# See file LICENSE for more information.

.PHONY: all clean install
.SUFFIXES:

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
