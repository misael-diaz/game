#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "system.h"
#include "input.h"
#include "video.h"
#include "entity.h"
#include "game.h"

void g_loop (
		void ** const fb,
		void ** const map,
		struct fb_fix_screeninfo const * const ffsp,
		struct fb_var_screeninfo const * const fvsp,
		struct entity * const entities,
		int const num_entities,
		int const keyboard_fd
)
{
	double etime = 0;
	int num_ticks = 0;
	int count = 0;
	double game_etime = 0;
	double game_period = (1.0 / GAME_FRAMERATE_HZ);
	int const max_count = 256;
	double const max_count_f64 = max_count;
	int key_event = IN_RELEASE_KEV;
	int * const kep = &key_event;
	struct timespec ts = {};
	struct timespec te = {};
	struct timespec *tsp = &ts;
	struct timespec *tep = &te;
	struct timespec time_game_reference = {};
	struct timespec time_game_current = {};
	struct input_event ie = {};
	struct input_event * const iep = &ie;
	struct entity * const gamer = &entities[0];
	clockid_t const clockid = CLOCK_MONOTONIC;
	clockid_t const * const cip = &clockid;
	clock_gettime(
			clockid,
			tsp
	);
	time_game_reference = *tsp;
	while (1) {
		vid_write_fb(
				fb,
				map,
				ffsp,
				fvsp,
				entities,
				num_entities
		);
		if (
			in_handle_input(
					kep,
					iep,
					entities,
					keyboard_fd
			)
		) {
			break;
		}

		en_handle_collisions(
				entities,
				num_entities
		);

		if (!gamer->hp) {
			fprintf(stdout, "%s\n", "game: GAME OVER");
			break;
		}

		clock_gettime(
				clockid,
				&time_game_current
		);
		game_etime += sys_etime(
				&time_game_current,
				&time_game_reference
		);
		num_ticks = ((int) (game_etime * GAME_FRAMERATE_HZ));
		for (int i = 0; i != num_ticks; ++i) {
			en_update(entities, num_entities);
			game_etime -= game_period;
		}
		if (num_ticks) {
			time_game_reference = time_game_current;
		}

		sys_delay(cip);
		++count;
		if (max_count == count) {
			clock_gettime(
					clockid,
					tep
			);
			etime = sys_etime(
					tep,
					tsp
			);
			fprintf(stdout, "main: etime: %lf\n", etime);
			fprintf(stdout, "main: fps: %lf\n", (max_count_f64 / etime));
			count = 0;
			ts = te;
		}
	}
}

/*

gnu-linux-gaming-dev                                          May 17, 2025

author: @misael-diaz
source: game.c

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
