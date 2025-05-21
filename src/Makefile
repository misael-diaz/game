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

$(ENTITY_OBJ): \
	$(GMDEFS_H) \
	$(SYSTEM_H) \
	$(ENTITY_H) \
	$(ENTITY_SRC)
	$(CC) $(CCOPT) -c $(ENTITY_SRC) -o $(ENTITY_OBJ)
$(SYSTEM_OBJ): \
	$(GMDEFS_H) \
	$(SYSTEM_H) \
	$(SYSTEM_SRC)
	$(CC) $(CCOPT) -c $(SYSTEM_SRC) -o $(SYSTEM_OBJ)
$(INPUT_OBJ): \
	$(ENTITY_H) \
	$(INPUT_H) \
	$(INPUT_SRC)
	$(CC) $(CCOPT) -c $(INPUT_SRC) -o $(INPUT_OBJ)
$(VIDEO_OBJ): \
	$(ENTITY_H) \
	$(VIDEO_H) \
	$(VIDEO_SRC)
	$(CC) $(CCOPT) -c $(VIDEO_SRC) -o $(VIDEO_OBJ)
$(GAME_OBJ): \
	$(SYSTEM_H) \
	$(INPUT_H) \
	$(VIDEO_H) \
	$(GAME_H) \
	$(GAME_SRC)
	$(CC) $(CCOPT) -c $(GAME_SRC) -o $(GAME_OBJ)
$(MAIN_OBJ): \
	$(INPUT_H) \
	$(VIDEO_H) \
	$(GAME_H) \
	$(ENTITY_OBJ) \
	$(SYSTEM_OBJ) \
	$(INPUT_OBJ) \
	$(VIDEO_OBJ) \
	$(GAME_OBJ) \
	$(MAIN_SRC)
	$(CC) $(CCOPT) -c $(MAIN_SRC) -o $(MAIN_OBJ)
$(BIN): $(MAIN_OBJ)
	$(CC) $(CCOPT) \
		$(ENTITY_OBJ) \
		$(SYSTEM_OBJ) \
		$(INPUT_OBJ) \
		$(VIDEO_OBJ) \
		$(GAME_OBJ) \
		$(MAIN_OBJ) \
		-o \
		$(BIN)
clean:
	rm -f ../obj/*.o
	rm -f $(BIN)
