#include <stdio.h>
#include <stdlib.h>
#include <linux/fb.h>
#include "entity.h"

void en_init(
		struct entity * const entities,
		struct fb_var_screeninfo const * const fvsp,
		int num_entities
)
{
	if ((0 >= num_entities) || (EN_NUM_ENTITY_MAX < num_entities)) {
		fprintf(stderr, "%s\n", "en_init: InvalidEntityCount");
		_Exit(EXIT_FAILURE);
	}
	if (!entities) {
		fprintf(stderr, "%s\n", "en_init: NullEntityArrayError");
		_Exit(EXIT_FAILURE);
	}
	int const xres = fvsp->xres;
	int const yres = fvsp->yres;
	int const gamer_len = EN_GAMER_LEN;
	int const enemy_len = EN_ENEMY_LEN;
	for (int i = 0; i != num_entities; ++i) {
		struct entity * const ent = &entities[i];
		if (0 == i) {
			ent->tag = EN_GAMER;
			ent->xpos = ((xres >> 1) - (gamer_len >> 1));
			ent->ypos = ((yres >> 1) - (gamer_len >> 1));
			ent->xvel = 0;
			ent->yvel = 0;
			ent->xmin = 0;
			ent->ymin = 0;
			ent->xmax = (xres - gamer_len);
			ent->ymax = (yres - gamer_len);
			ent->blue  = EN_GAMER_BLUE;
			ent->green = EN_GAMER_GREEN;
			ent->red   = EN_GAMER_RED;
			ent->alpha = EN_GAMER_ALPHA;
			ent->len = gamer_len;
		} else {
			ent->tag = EN_ENEMY;
			ent->xpos = random() - (RAND_MAX >> 1);
			ent->ypos = random() - (RAND_MAX >> 1);
			ent->xvel = ((i & 1)? (-EN_ENEMY_VEL): EN_ENEMY_VEL);
			ent->yvel = ((i & 1)? (-EN_ENEMY_VEL): EN_ENEMY_VEL);
			ent->xmin = 0;
			ent->ymin = 0;
			ent->xmax = (xres - enemy_len);
			ent->ymax = (yres - enemy_len);
			ent->blue  = EN_ENEMY_BLUE;
			ent->green = EN_ENEMY_GREEN;
			ent->red   = EN_ENEMY_RED;
			ent->alpha = EN_ENEMY_ALPHA;
			ent->len = enemy_len;
		}
		ent->xpos = ((ent->xmin > ent->xpos)? ent->xmin : ent->xpos);
		ent->xpos = ((ent->xmax < ent->xpos)? ent->xmax : ent->xpos);
		ent->ypos = ((ent->ymin > ent->ypos)? ent->ymin : ent->ypos);
		ent->ypos = ((ent->ymax < ent->ypos)? ent->ymax : ent->ypos);
	}
}

void en_update(
		struct entity * const entities,
		int num_entities
)
{
	for (int i = 0; i != num_entities; ++i) {
		struct entity * const ent = &entities[i];
		ent->xpos += ent->xvel;
		ent->ypos += ent->yvel;
		ent->xpos = ((ent->xmin > ent->xpos)? ent->xmin : ent->xpos);
		ent->xpos = ((ent->xmax < ent->xpos)? ent->xmax : ent->xpos);
		ent->ypos = ((ent->ymin > ent->ypos)? ent->ymin : ent->ypos);
		ent->ypos = ((ent->ymax < ent->ypos)? ent->ymax : ent->ypos);
		if (EN_ENEMY == ent->tag) {
			if ((ent->xmin == ent->xpos) || (ent->xmin == ent->xpos)) {
				ent->xvel = -ent->xvel;
			}
			if ((ent->ymin == ent->ypos) || (ent->ymin == ent->ypos)) {
				ent->yvel = -ent->yvel;
			}
		}
	}
}

/*

gnu-linux-gaming-dev                                          May 17, 2025

author: @misael-diaz
source: src/entity.c

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
