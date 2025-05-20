#include <linux/fb.h>
#include <unistd.h>
#include <termios.h>
#include "input.h"
#include "video.h"
#include "game.h"

int main ()
{
	void *map = NULL;
	struct termios term = {};
	struct termios * const tp = &term;
	struct fb_fix_screeninfo ffs = {};
	struct fb_fix_screeninfo * const ffsp = &ffs;
	struct fb_var_screeninfo fvs = {};
	struct fb_var_screeninfo * const fvsp = &fvs;
	char const * const fbdev = "/dev/fb0";
	char const * const kbdev = (
			"/dev/input/by-id/usb-Dell_Dell_QuietKey_Keyboard-event-kbd"
	);
	int framebuffer_fd = vid_open_fb(fbdev);
	int keyboard_fd = in_open_kb(kbdev);
	vid_getinfo_fb(
			ffsp,
			fvsp,
			framebuffer_fd
	);
	vid_loginfo_fb(
			ffsp,
			fvsp
	);
	vid_mmap_fb(
			&map,
			ffsp,
			framebuffer_fd
	);
	init_term(tp);
	g_loop(
			&map,
			ffsp,
			keyboard_fd
	);
	close(keyboard_fd);
	close(framebuffer_fd);
	vid_munmap_fb(
			&map,
			ffsp
	);
	tcsetattr(
			STDIN_FILENO,
			TCSAFLUSH,
			tp
	);
	return 0;
}

// References:
//
//
// game-loop:
// https://github.com/misael-diaz/arena-Quake-III/blob/main/src/game/game.c
//
//
// processing keyboard input:
// references: https://stackoverflow.com/questions/20943322/accessing-keys-from-linux-input-device
// https://www.kernel.org/doc/Documentation/input/input.txt
// https://www.kernel.org/doc/html/v4.16/input/event-codes.html#ev-key
//
//
// disabling terminal input echoing:
// https://www.gnu.org/software/libc/manual/html_node/Low_002dLevel-Terminal-Interface.html
// https://www.gnu.org/software/libc/manual/html_node/getpass.html
// also see `man tcsetattr` for info on `TCSAFLUSH`
// https://stackoverflow.com/questions/59922972/how-to-stop-echo-in-terminal-using-c/59923166#59923166
//
//
// framebuffer:
// https://www.kernel.org/doc/html/v5.4/fb/api.html
// https://kevinboone.me/linuxfbc.html

/*

Game                                          May 17, 2025

author: @misael-diaz
source: game.c

Attempts to implement a basic game in GNU/Linux without frameworks.

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
