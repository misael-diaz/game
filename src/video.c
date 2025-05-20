#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "video.h"

int vid_open_fb(
		char const * const fbdev
)
{
	errno = 0;
	int fd = open(fbdev, O_RDWR);
	if (-1 == fd || errno) {
		fprintf(stderr, "%s\n", "vid_open_fbdev: IOERR");
		if (errno) {
			fprintf(stderr, "vid_open_fbdev: reason: %s\n", strerror(errno));
		}
		close(fd);
		_exit(EXIT_FAILURE);
	}
	return fd;
}

void vid_getinfo_fb(
		struct fb_fix_screeninfo * const ffsp,
		struct fb_var_screeninfo * const fvsp,
		int const fd
)
{
	if (ioctl(fd, FBIOGET_FSCREENINFO, ffsp)) {
		fprintf(stderr, "%s\n", "vid_getinfo_fb: IOERR");
		if (errno) {
			fprintf(stderr, "vid_getinfo_fb: reason: %s\n", strerror(errno));
		}
		_exit(EXIT_FAILURE);
	}

	if (ioctl(fd, FBIOGET_VSCREENINFO, fvsp)) {
		fprintf(stderr, "%s\n", "vid_getinfo_fb: IOERR");
		if (errno) {
			fprintf(stderr, "vid_getinfo_fb: reason: %s\n", strerror(errno));
		}
		_exit(EXIT_FAILURE);
	}
}

void vid_loginfo_fb(
		struct fb_fix_screeninfo const * const ffsp,
		struct fb_var_screeninfo const * const fvsp
)
{
	switch (ffsp->visual) {
		case FB_VISUAL_MONO01:
			fprintf(stdout, "%s\n", "vid_loginfo_fb: visual: VisualMono01");
			break;
		case FB_VISUAL_MONO10:
			fprintf(stdout, "%s\n", "vid_loginfo_fb: visual: VisualMono10");
			break;
		case FB_VISUAL_TRUECOLOR:
			fprintf(stdout, "%s\n", "vid_loginfo_fb: visual: VisualTrueColor");
			break;
		case FB_VISUAL_PSEUDOCOLOR:
			fprintf(stdout, "%s\n", "vid_loginfo_fb: visual: VisualPseudoColor");
			break;
		case FB_VISUAL_DIRECTCOLOR:
			fprintf(stdout, "%s\n", "vid_loginfo_fb: visual: VisualDirectColor");
			break;
		case FB_VISUAL_STATIC_PSEUDOCOLOR:
			fprintf(stdout, "%s\n", "vid_loginfo_fb: visual: VisualStaticPseudoColor");
			break;
		case FB_VISUAL_FOURCC:
			fprintf(stdout, "%s\n", "vid_loginfo_fb: visual: VisualFourcc");
			break;
		default:
			fprintf(stdout, "%s\n", "vid_loginfo_fb: visual: UnknownVisual");
			_exit(EXIT_FAILURE);
	}

	switch (ffsp->type) {
		case FB_TYPE_PACKED_PIXELS:
			fprintf(stdout, "%s\n", "vid_loginfo_fb: type: PackedPixelsFrameBuffer");
			break;
		case FB_TYPE_PLANES:
			fprintf(stdout, "%s\n", "vid_loginfo_fb: type: PlanesFrameBuffer");
			break;
		case FB_TYPE_INTERLEAVED_PLANES:
			fprintf(stdout,
				"%s\n",
				"vid_loginfo_fb: type: InverleavedPlanesFrameBuffer");
			break;
		case FB_TYPE_TEXT:
			fprintf(stdout, "%s\n", "vid_loginfo_fb: type: TextFrameBuffer");
			break;
		case FB_TYPE_VGA_PLANES:
			fprintf(stdout, "%s\n", "vid_loginfo_fb: type: VGAPlanesFrameBuffer");
			break;
		case FB_TYPE_FOURCC:
			fprintf(stdout, "%s\n", "vid_loginfo_fb: type: FourccFrameBuffer");
			break;
		default:
			fprintf(stdout, "%s\n", "vid_loginfo_fb: type: UnknownTypeFrameBuffer");
			_exit(EXIT_FAILURE);
	}

	fprintf(stdout, "vid_loginfo_fb: line-length: %d\n", ffsp->line_length);
	fprintf(stdout, "vid_loginfo_fb: bits-per-pixel: %d\n", fvsp->bits_per_pixel);
	fprintf(stdout, "vid_loginfo_fb: height mm: %d\n", fvsp->height);
	fprintf(stdout, "vid_loginfo_fb: width mm: %d\n", fvsp->width);
	fprintf(stdout, "vid_loginfo_fb: xres: %d\n", fvsp->xres);
	fprintf(stdout, "vid_loginfo_fb: yres: %d\n", fvsp->yres);
	fprintf(stdout, "vid_loginfo_fb: xoffset: %d\n", fvsp->xoffset);
	fprintf(stdout, "vid_loginfo_fb: yoffset: %d\n", fvsp->yoffset);
	fprintf(stdout, "vid_loginfo_fb: nonstd: %d\n", fvsp->nonstd);

	fprintf(stdout,
		"vid_loginfo_fb: red: offset: %d length: %d MSB: %d\n",
		fvsp->red.offset,
		fvsp->red.length,
		fvsp->red.msb_right);
	fprintf(stdout,
		"vid_loginfo_fb: green: offset: %d length: %d MSB: %d\n",
		fvsp->green.offset,
		fvsp->green.length,
		fvsp->green.msb_right);
	fprintf(stdout,
		"vid_loginfo_fb: blue: offset: %d length: %d MSB: %d\n",
		fvsp->blue.offset,
		fvsp->blue.length,
		fvsp->blue.msb_right);

	switch (fvsp->grayscale) {
		case 0:
			fprintf(stdout, "%s\n", "vid_loginfo_fb: color: Color");
			break;
		case 1:
			fprintf(stdout, "%s\n", "vid_loginfo_fb: color: Grayscale");
			break;
		default:
			fprintf(stdout, "%s\n", "vid_loginfo_fb: color: Fourcc");
			break;
	}
}

void vid_mmap_fb(
		void ** const map,
		struct fb_fix_screeninfo const * const ffsp,
		int const fd
)
{
	errno = 0;
	*map = mmap(
			NULL,
			ffsp->smem_len,
			PROT_READ | PROT_WRITE,
			MAP_SHARED,
			fd,
			0
		  );
	if (errno) {
		fprintf(stdout, "%s\n", "vid_mmap_fb: ERROR");
		fprintf(stderr, "vid_mmap_fb: reason: %s\n", strerror(errno));
		_exit(EXIT_FAILURE);
	}
}

void vid_munmap_fb(
		void ** const map,
		struct fb_fix_screeninfo const * const ffsp
)
{
	errno = 0;
	if (munmap(*map, ffsp->smem_len)) {
		fprintf(stderr, "%s\n", "vid_munmap_fb: ERROR");
		fprintf(stderr, "vid_munmap_fb: reason: %s\n", strerror(errno));
		_exit(EXIT_FAILURE);
	}
	*map = NULL;
}

void vid_write_fb(
		void ** const map,
		struct fb_fix_screeninfo const * const ffsp,
		struct fb_var_screeninfo const * const fvsp,
		struct entity const * const entities,
		int const num_entities
)
{
	int xbeg = 0;
	int xend = 0;
	int ybeg = 0;
	int yend = 0;
	int const bytes_per_pixel = (fvsp->bits_per_pixel >> 3);
	int const bpp = bytes_per_pixel;
	int const num_pixels = (ffsp->smem_len / bpp);
	int const num_pixels_x = fvsp->xres;
	int const num_pixels_y = fvsp->yres;
	struct rgba * const pix = *map;
	memset(*map, 0, ffsp->smem_len);
	for (int i = 0; i != num_entities; ++i) {
		struct entity const * const ent = &entities[i];
		if (EN_HUD == ent->tag) {
			xbeg = ent->xpos;
			xend = ent->xpos + ent->width;
			ybeg = ent->ypos;
			yend = ent->ypos + ent->height;
		} else {
			xbeg = ent->xpos;
			xend = ent->xpos + ent->len;
			ybeg = ent->ypos;
			yend = ent->ypos + ent->len;
		}
		for (int y = ybeg; y != yend; ++y) {
			for (int x = xbeg; x != xend; ++x) {
				int const of = (num_pixels_x * y + x);
				pix[of].blue = ent->blue;
				pix[of].green = ent->green;
				pix[of].red = ent->red;
				pix[of].alpha = ent->alpha;
			}
		}
	}
}

/*

gnu-linux-gaming-dev                                          May 17, 2025

author: @misael-diaz
source: video.c

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
