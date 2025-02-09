# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) Ivan Pizhenko, 2025. All rights reserved.
# See file LICENSE for more information.

.PHONY: all clean install
.SUFFIXES:

ifeq ("x$(CC)","x")
CC:=gcc
endif

ifeq ("c$(CXX)","x")
CXX:=g++
endif

CFLAGS+=-std=c11 -Wall -Wextra -Werror -Wpedantic -ftrivial-auto-var-init=pattern -DNDEBUG

ifeq ($(OS),Windows_NT)
EXE_EXT:=.exe
endif

ifeq ("x$(MSYSTEM_PREFIX)","x")
INSTALL_PREFIX:=/usr/local
else
INSTALL_PREFIX:=$(MSYSTEM_PREFIX)
endif

SRC:=$(NAME).c $(EXTRA_SRC)
EXE:=$(NAME)$(EXE_EXT)

MAN_SECTION:=1
MAN_SRC:=$(NAME).$(MAN_SECTION)
MAN_PAGE:=$(MAN_SRC).gz
ifeq ("x$(MAN_SHORT_DESC)","x")
MAN_SHORT_DESC:=please add short utility description in the local.mk
endif

all: $(EXE) $(MAN_PAGE)

clean:
	-rm -f $(MAN_PAGE)
	-rm -f $(EXE)

$(EXE): $(SRC) ../mk/common.mk
	$(CC) $(CFLAGS) -O2 $(SRC) -o $(NAME)
	strip $(EXE)

$(MAN_PAGE): $(EXE)
	help2man -N -n "$(MAN_SHORT_DESC)" -o$(MAN_SRC) ./$(EXE)
	gzip -f9 $(MAN_SRC)

install: $(EXE) $(MAN_PAGE)
	install -d "$(INSTALL_PREFIX)/bin" "$(INSTALL_PREFIX)/share/man/man$(MAN_SECTION)"
	install $(EXE) "$(INSTALL_PREFIX)/bin"
	install $(MAN_PAGE) "$(INSTALL_PREFIX)/share/man/man$(MAN_SECTION)"
