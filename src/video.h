#ifndef LLGLGD_VIDEO_H
#define LLGLGD_VIDEO_H

#include <linux/fb.h>

struct rgba {
	unsigned char blue;
	unsigned char green;
	unsigned char red;
	unsigned char alpha;
};

int vid_open_fb(
		char const * const fbdev
);

void vid_getinfo_fb(
		struct fb_fix_screeninfo * const ffsp,
		struct fb_var_screeninfo * const fvsp,
		int const fd
);

void vid_loginfo_fb(
		struct fb_fix_screeninfo const * const ffsp,
		struct fb_var_screeninfo const * const fvsp
);

void vid_mmap_fb(
		void ** const map,
		struct fb_fix_screeninfo const * const ffsp,
		int const fd
);

void vid_munmap_fb(
                void ** const map,
                struct fb_fix_screeninfo const * const ffsp
);

void vid_write_fb(
		void ** const map,
		struct fb_fix_screeninfo const * const ffsp,
		struct fb_var_screeninfo const * const fvsp
);

#endif

/*

gnu-linux-gaming-dev                                 May 19, 2025

author: @misael-diaz
source: src/system.h

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
