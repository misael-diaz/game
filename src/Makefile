#!/usr/bin/make
#
# gnu-linux-game-dev                           May 19, 2025
#
# source: src/Makefile
# author: @misael-diaz
#
# Synopsis:
# Defines the Makefile for building the program with GNU make.
#
# Copyright (C) 2025 Misael Diaz-Maldonado
#
# This file is released under the GNU General Public License version 2 only
# as published by the Free Software Foundation.
#

include make-inc

all: $(BIN)

$(MAIN_OBJ): \
	$(MAIN_SRC)
	$(CC) $(CCOPT) -c $(MAIN_SRC) -o $(MAIN_OBJ)
$(BIN): $(MAIN_OBJ)
	$(CC) $(CCOPT) \
		$(MAIN_OBJ) \
		-o \
		$(BIN)
clean:
	rm -f ../obj/*.o
	rm -f $(BIN)
