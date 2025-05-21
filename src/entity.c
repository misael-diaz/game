#include <stdio.h>
#include <stdlib.h>
#include <linux/fb.h>
#include <time.h>
#include <math.h>
#include "system.h"
#include "entity.h"

static double en_clamp(
		double const val,
		double const min,
		double const max
)
{
	double _min = min;
	double _max = max;
	if (max < min) {
		_max = min;
		_min = max;
	}
	double ret = val;
	if (_min > val) {
		ret = _min;
	} else if (_max < val) {
		ret = _max;
	}
	return ret;
}

static void en_fix_overlap(
		struct entity * const entities,
		int num_entities
)
{
	for (int i = 0; i != num_entities; ++i) {
		struct entity const * const entity = &entities[i];
		if ((EN_GAMER != entity->tag) && (EN_ENEMY != entity->tag)) {
			continue;
		}
		double const x1 = (entity->xpos + entity->width);
		double const y1 = (entity->ypos + entity->height);
		for (int j = 0; j != num_entities; ++j) {
			struct entity * const other = &entities[j];
			if (EN_ENEMY != other->tag) {
				continue;
			}
			if (other == entity) {
				continue;
			}
			double const x2 = (other->xpos + other->width);
			double const y2 = (other->ypos + other->height);
			double const r2 = (
					(x2 - x1) * (x2 - x1) +
					(y2 - y1) * (y2 - y1)
			);
			double const contact = (entity->len - other->len);
			double const contact2 = (contact * contact);
			if (contact2 > r2) {
				double const r = sqrt(r2);
				double const r_inv = (1.0 / r);
				double const correction = (2.0 * r);
				double const c = correction;
				double const x2_new = x1 + c * (x2 - x1) * r_inv;
				double const y2_new = y1 + c * (y2 - y1) * r_inv;
				double const xpos_new = (x2_new - entity->width);
				double const ypos_new = (y2_new - entity->height);
				other->xpos = xpos_new;
				other->ypos = ypos_new;
			}
		}
	}
}

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
	int const hud_len = EN_HUD_LEN;
	int const hud_width = EN_HUD_WIDTH;
	int const hud_height = EN_HUD_HEIGHT;
	int const hud_xpos = EN_HUD_XPOS;
	int const hud_ypos = EN_HUD_YPOS;
	for (int i = 0; i != num_entities; ++i) {
		struct entity * const ent = &entities[i];
		if (EN_HUD_ID == i) {
			ent->tag = EN_HUD;
			ent->invisibility = 0;
			ent->ticks = 0;
			ent->xpos = (hud_xpos - (hud_width >> 1));
			ent->ypos = (hud_ypos - (hud_height >> 1));
			ent->xvel = 0;
			ent->yvel = 0;
			ent->xmin = 0;
			ent->ymin = 0;
			ent->xmax = (xres - hud_width);
			ent->ymax = (yres - hud_height);
			ent->blue  = EN_HUD_BLUE;
			ent->green = EN_HUD_GREEN;
			ent->red   = EN_HUD_RED;
			ent->alpha = EN_HUD_ALPHA;
			ent->len = hud_len;
			ent->width = hud_width;
			ent->height = hud_height;
			ent->hp = EN_HUD_HP;
		} else if (EN_GAMER_ID == i) {
			ent->tag = EN_GAMER;
			ent->invisibility = 0;
			ent->ticks = 0;
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
			ent->width = gamer_len;
			ent->height = gamer_len;
			ent->hp = EN_GAMER_HP;
		} else {
			ent->tag = EN_ENEMY;
			ent->invisibility = 0;
			ent->ticks = 0;
			ent->xpos = sys_random(0, xres);
			ent->ypos = sys_random(0, yres);
			ent->xvel = sys_random(-EN_ENEMY_VEL, EN_ENEMY_VEL);
			ent->yvel = sys_random(-EN_ENEMY_VEL, EN_ENEMY_VEL);
			ent->xmin = 0;
			ent->ymin = 0;
			ent->xmax = (xres - enemy_len);
			ent->ymax = (yres - enemy_len);
			ent->blue  = EN_ENEMY_BLUE;
			ent->green = EN_ENEMY_GREEN;
			ent->red   = EN_ENEMY_RED;
			ent->alpha = EN_ENEMY_ALPHA;
			ent->len = enemy_len;
			ent->width = enemy_len;
			ent->height = enemy_len;
			ent->hp = EN_ENEMY_HP;
		}
		ent->xpos = en_clamp(ent->xpos, ent->xmin, ent->xmax);
		ent->ypos = en_clamp(ent->ypos, ent->ymin, ent->ymax);
	}
	en_fix_overlap(entities, num_entities);
	for (int i = 0; i != num_entities; ++i) {
		struct entity * const ent = &entities[i];
		ent->xpos = en_clamp(ent->xpos, 0, xres);
		ent->ypos = en_clamp(ent->ypos, 0, yres);
	}
}

// returns the squared contact-distance between a pair of entities
static double en_contact2 (
		struct entity const * const entity,
		struct entity const * const other
)
{
	double const contact = (0.5 * (entity->len + other->len));
	return (contact * contact);
}

// returns the squared contact distance
static double en_sqdist (
		struct entity const * const entity,
		struct entity const * const other
)
{
	double const x1 = entity->xpos + entity->width;
	double const y1 = entity->ypos + entity->height;
	double const x2 = other->xpos + other->width;
	double const y2 = other->ypos + other->height;
	double const r2 = (
			(x2 - x1) * (x2 - x1) +
			(y2 - y1) * (y2 - y1)
	);
	return r2;
}

// returns one if the entities overlap, zero otherwise
static int en_check_collision(
		struct entity const * const entity,
		struct entity const * const other
)
{
	if (entity == other) {
		return 0;
	}
	if (en_contact2(entity, other) >= en_sqdist(entity, other)) {
		return 1;
	} else {
		return 0;
	}
}

void en_handle_collisions(
		struct entity * const entities,
		int const num_entities
)
{
	struct entity * const hud = &entities[EN_HUD_ID];
	struct entity * const gamer = &entities[EN_GAMER_ID];
	for (int i = 1; i != num_entities; ++i) {
		struct entity const * const other = &entities[i];
		if (EN_ENEMY == other->tag) {
			struct entity const * const enemy = other;
			if (en_check_collision(gamer, enemy)) {
				if (!gamer->invisibility) {
					hud->hp -= EN_COLLISION_DAMAGE;
					hud->len -= EN_HUD_DAMAGE;
					hud->width -= EN_HUD_DAMAGE;
					gamer->hp -= EN_COLLISION_DAMAGE;
					gamer->blue  = EN_GAMER_DAMAGED_BLUE;
					gamer->green = EN_GAMER_DAMAGED_GREEN;
					gamer->red   = EN_GAMER_DAMAGED_RED;
					gamer->alpha = EN_GAMER_DAMAGED_ALPHA;
					gamer->invisibility = 1;
					gamer->ticks = 0;
				}
			}
		}
	}
	if (0 > gamer->hp) {
		gamer->hp = 0;
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
			if ((ent->xmin == ent->xpos) || (ent->xmax == ent->xpos)) {
				ent->xvel = -ent->xvel;
			}
			if ((ent->ymin == ent->ypos) || (ent->ymax == ent->ypos)) {
				ent->yvel = -ent->yvel;
			}
		}
		if (EN_GAMER == ent->tag) {
			if (ent->invisibility) {
				if (EN_COLLISION_IGNORE == ent->ticks) {
					ent->blue  = EN_GAMER_BLUE;
					ent->green = EN_GAMER_GREEN;
					ent->red   = EN_GAMER_RED;
					ent->alpha = EN_GAMER_ALPHA;
					ent->invisibility = 0;
					ent->ticks = 0;
				} else {
					ent->ticks++;
				}
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
