#ifndef LLGLGD_GAME_H
#define LLGLGD_GAME_H

#include "gmdefs.h"
#include "entity.h"

void g_loop (
		void ** const map,
		struct fb_fix_screeninfo const * const ffsp,
		struct fb_var_screeninfo const * const fvsp,
		struct entity * const entities,
		int const num_entities,
		int const keyboard_fd
);


#endif

/*

gnu-linux-gaming-dev                                 May 19, 2025

author: @misael-diaz
source: src/game.h

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
