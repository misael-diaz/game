#ifndef LLGLGD_INPUT_H
#define LLGLGD_INPUT_H

#include <linux/input.h>
#include <termios.h>

#define IN_RELEASE_KEV 0
#define IN_PRESSED_KEV 1
#define IN_BUFSIZE_KEV 16

int in_handle_input(
		int * const kep,
		struct input_event * const iep,
		int const fd
);

int in_open_kb(
		char const * const kbdev
);

void init_term(struct termios * const tp);

#endif

/*

gnu-linux-gaming-dev                                 May 19, 2025

author: @misael-diaz
source: src/input.h

Key-Events KEV MACROS

Copyright (C) 2025 Misael Diaz-Maldonado

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, version 2 only of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.

*/
