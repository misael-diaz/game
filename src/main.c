#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <linux/input.h>
#include <linux/fb.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define KEY_RELEASED 0
#define KEY_PRESSED  1

#define GAME_FRAMERATE_HZ 50.0
#define GAME_PERIOD_NS ((long)((1.0e9 * (1.0 / GAME_FRAMERATE_HZ))))

#define GAME_BUFFER_SZ 256

void draw(void * const map, struct fb_fix_screeninfo const * const ffsp)
{
	memset(map, 0, ffsp->smem_len);
}

double etime (
	struct timespec const * const tep,
	struct timespec const * const tsp
)
{
	double const delta_sec = (tep->tv_sec - tsp->tv_sec);
	double const delta_nsec = (tep->tv_nsec - tsp->tv_nsec);
	double elapsed_time = (delta_sec + (1.0e-9 * delta_nsec));
	return elapsed_time;
}

int delay(clockid_t const clockid)
{
	struct timespec ts;
	struct timespec * const tsp = &ts;
	clock_gettime(clockid, tsp);
	time_t const sec = tsp->tv_sec;
	long const nsec = (tsp->tv_nsec + GAME_PERIOD_NS);
	struct timespec ts_walltime;
	struct timespec *twp = &ts_walltime;
	if (nsec > 999999999L) {
		twp->tv_sec = (1 + sec);
		twp->tv_nsec = (nsec % 999999999L);
	} else {
		twp->tv_sec = sec;
		twp->tv_nsec = nsec;
	}
	int const flags = TIMER_ABSTIME;
	return clock_nanosleep(clockid, flags, twp, NULL);
}

int handle_input(
	int const fd,
	struct termios * const terp,
	struct input_event * const iep,
	int * const kep
)
{
	errno = 0;
	int rc = 0;
	int code = 0;
	ssize_t bytes = 0;
	int const key_up = KEY_UP;
	int const key_down = KEY_DOWN;
	int const key_left = KEY_LEFT;
	int const key_right = KEY_RIGHT;
	char buf[GAME_BUFFER_SZ];
	char key_left_str[4]  = { 0x1b, 0x5b, 0x44, 0x00 };
	char key_right_str[4] = { 0x1b, 0x5b, 0x43, 0x00 };
	char key_up_str[4]    = { 0x1b, 0x5b, 0x41, 0x00 };
	char key_down_str[4]  = { 0x1b, 0x5b, 0x42, 0x00 };
	char key_esc_str[2]   = { 0x1b, 0x00 };

	memset(buf, 0, GAME_BUFFER_SZ * sizeof(*buf));
	if (KEY_PRESSED == *kep) {
		code = iep->code;
	} else {
		bytes = read(STDIN_FILENO, buf, GAME_BUFFER_SZ * sizeof(*buf));
		if (0 == bytes) {
			return rc;
		}
		if (-1 == bytes) {
			if (EAGAIN == errno) {
				return rc;
			}
			fprintf(stderr, "%s\n", "main: IOERR");
			if (errno) {
				fprintf(stderr, "main: reason: %s\n", strerror(errno));
			}
			close(fd);
			tcsetattr(STDIN_FILENO, TCSAFLUSH, terp);
			exit(EXIT_FAILURE);
		}

		if (1 == bytes) {
			buf[1] = 0;
			if (!strncmp(key_esc_str, buf, sizeof(key_esc_str))) {
				code = KEY_ESC;
			} else {
				return rc;
			}
		} else if (3 == bytes) {
			buf[3] = 0;
			if (!strncmp(key_left_str, buf, sizeof(key_left_str))) {
				code = KEY_LEFT;
			} else if (!strncmp(key_right_str, buf, sizeof(key_right_str))) {
				code = KEY_RIGHT;
			} else if (!strncmp(key_down_str, buf, sizeof(key_down_str))) {
				code = KEY_DOWN;
			} else if (!strncmp(key_up_str, buf, sizeof(key_up_str))) {
				code = KEY_UP;
			} else {
				return rc;
			}
		} else {
			return rc;
		}
	}

	memset(iep, 0, sizeof(*iep));
	do {
		bytes = read(fd, iep, sizeof(*iep));
		if (0 == bytes) {
			return rc;
		}
		if (-1 == bytes) {
			fprintf(stderr, "%s\n", "main: IOERR");
			if (errno) {
				fprintf(stderr, "main: reason: %s\n", strerror(errno));
			}
			close(fd);
			tcsetattr(STDIN_FILENO, TCSAFLUSH, terp);
			exit(EXIT_FAILURE);
		} else if (sizeof(*iep) != bytes) {
			fprintf(stderr, "%s\n", "main: IOERR");
			if (errno) {
				fprintf(stderr, "main: reason: %s\n", strerror(errno));
			}
			close(fd);
			tcsetattr(STDIN_FILENO, TCSAFLUSH, terp);
			exit(EXIT_FAILURE);
		}
	} while ((EV_KEY != iep->type) || (code != iep->code));

	if ((key_up == iep->code)) {
		if ((KEY_RELEASED == iep->value)) {
			fprintf(stdout, "%s\n", "main: key-up released");
			*kep = KEY_RELEASED;
		} else {
			fprintf(stdout, "%s\n", "main: key-up pressed");
			*kep = KEY_PRESSED;
		}
	} else if (key_down == iep->code) {
		if ((KEY_RELEASED == iep->value)) {
			fprintf(stdout, "%s\n", "main: key-down released");
			*kep = KEY_RELEASED;
		} else {
			fprintf(stdout, "%s\n", "main: key-down pressed");
			*kep = KEY_PRESSED;
		}
	} else if (key_left == iep->code) {
		if ((KEY_RELEASED == iep->value)) {
			fprintf(stdout, "%s\n", "main: key-left released");
			*kep = KEY_RELEASED;
		} else {
			fprintf(stdout, "%s\n", "main: key-left pressed");
			*kep = KEY_PRESSED;
		}
	} else if (key_right == iep->code) {
		if ((KEY_RELEASED == iep->value)) {
			fprintf(stdout, "%s\n", "main: key-right released");
			*kep = KEY_RELEASED;
		} else {
			fprintf(stdout, "%s\n", "main: key-right pressed");
			*kep = KEY_PRESSED;
		}
	} else if (KEY_ESC == iep->code) {
		if ((KEY_RELEASED == iep->value)) {
			fprintf(stdout, "%s\n", "main: key-esc released");
			fprintf(stdout, "%s\n", "main: quitting");
			*kep = KEY_RELEASED;
		} else {
			fprintf(stdout, "%s\n", "main: key-esc pressed");
			fprintf(stdout, "%s\n", "main: quitting");
			*kep = KEY_PRESSED;
		}
		rc = 1;
	}
	return rc;
}

int main ()
{
	// non-block stdin reads
	int flags = fcntl(STDIN_FILENO, F_GETFD);
	flags |= O_NONBLOCK;
	fcntl(STDIN_FILENO, F_SETFD, flags);
	// disables terminal input echoing and canonical mode
	struct termios term = {};
	struct termios *terp = &term;
	tcgetattr(STDIN_FILENO, terp);
	terp->c_lflag &= (~(ECHO | ICANON));
	terp->c_cc[VMIN] = 0;
	terp->c_cc[VTIME] = 0;
	tcsetattr(STDIN_FILENO, TCSAFLUSH, terp);
	terp->c_lflag |= (ECHO | ICANON);


	errno = 0;
	int count = 0;
	int const max_count = 64;
	struct input_event ev = {};
	struct input_event *evp = &ev;
	char * const kbdev = "/dev/input/by-id/usb-Dell_Dell_QuietKey_Keyboard-event-kbd";
	int fd = open(kbdev, O_RDONLY);
	if (-1 == fd || errno) {
		fprintf(stderr, "%s\n", "main: IOERR");
		if (errno) {
			fprintf(stderr, "main: reason: %s\n", strerror(errno));
		}
		tcsetattr(STDIN_FILENO, TCSAFLUSH, terp);
		exit(EXIT_FAILURE);
	}

	errno = 0;
	struct fb_fix_screeninfo ffs = {};
	struct fb_var_screeninfo fvs = {};
	struct fb_fix_screeninfo * const ffsp = &ffs;
	struct fb_var_screeninfo * const fvsp = &fvs;
	char const * const fbdev = "/dev/fb0";
	int framebuffer_fd = open(fbdev, O_RDWR);
	if (-1 == framebuffer_fd || errno) {
		fprintf(stderr, "%s\n", "main: IOERR");
		if (errno) {
			fprintf(stderr, "main: reason: %s\n", strerror(errno));
		}
		close(fd);
		tcsetattr(STDIN_FILENO, TCSAFLUSH, terp);
		exit(EXIT_FAILURE);
	}

	if (ioctl(framebuffer_fd, FBIOGET_FSCREENINFO, ffsp)) {
		fprintf(stderr, "%s\n", "main: IOERR");
		if (errno) {
			fprintf(stderr, "main: reason: %s\n", strerror(errno));
		}
		close(fd);
		close(framebuffer_fd);
		tcsetattr(STDIN_FILENO, TCSAFLUSH, terp);
		exit(EXIT_FAILURE);
	}

	if (ioctl(framebuffer_fd, FBIOGET_VSCREENINFO, fvsp)) {
		fprintf(stderr, "%s\n", "main: IOERR");
		if (errno) {
			fprintf(stderr, "main: reason: %s\n", strerror(errno));
		}
		close(fd);
		close(framebuffer_fd);
		tcsetattr(STDIN_FILENO, TCSAFLUSH, terp);
		exit(EXIT_FAILURE);
	}

	switch (ffsp->visual) {
		case FB_VISUAL_MONO01:
			fprintf(stdout, "%s\n", "main: visual: VisualMono01");
			break;
		case FB_VISUAL_MONO10:
			fprintf(stdout, "%s\n", "main: visual: VisualMono10");
			break;
		case FB_VISUAL_TRUECOLOR:
			fprintf(stdout, "%s\n", "main: visual: VisualTrueColor");
			break;
		case FB_VISUAL_PSEUDOCOLOR:
			fprintf(stdout, "%s\n", "main: visual: VisualPseudoColor");
			break;
		case FB_VISUAL_DIRECTCOLOR:
			fprintf(stdout, "%s\n", "main: visual: VisualDirectColor");
			break;
		case FB_VISUAL_STATIC_PSEUDOCOLOR:
			fprintf(stdout, "%s\n", "main: visual: VisualStaticPseudoColor");
			break;
		case FB_VISUAL_FOURCC:
			fprintf(stdout, "%s\n", "main: visual: VisualFourcc");
			break;
		default:
			fprintf(stdout, "%s\n", "main: visual: UnknownVisual");
			close(fd);
			close(framebuffer_fd);
			tcsetattr(STDIN_FILENO, TCSAFLUSH, terp);
			exit(EXIT_FAILURE);
	}

	switch (ffsp->type) {
		case FB_TYPE_PACKED_PIXELS:
			fprintf(stdout, "%s\n", "main: type: PackedPixelsFrameBuffer");
			break;
		case FB_TYPE_PLANES:
			fprintf(stdout, "%s\n", "main: type: PlanesFrameBuffer");
			break;
		case FB_TYPE_INTERLEAVED_PLANES:
			fprintf(stdout,
				"%s\n",
				"main: type: InverleavedPlanesFrameBuffer");
			break;
		case FB_TYPE_TEXT:
			fprintf(stdout, "%s\n", "main: type: TextFrameBuffer");
			break;
		case FB_TYPE_VGA_PLANES:
			fprintf(stdout, "%s\n", "main: type: VGAPlanesFrameBuffer");
			break;
		case FB_TYPE_FOURCC:
			fprintf(stdout, "%s\n", "main: type: FourccFrameBuffer");
			break;
		default:
			fprintf(stdout, "%s\n", "main: type: UnknownTypeFrameBuffer");
			close(fd);
			close(framebuffer_fd);
			tcsetattr(STDIN_FILENO, TCSAFLUSH, terp);
			exit(EXIT_FAILURE);
	}

	fprintf(stdout, "main: line-length: %d\n", ffsp->line_length);
	fprintf(stdout, "main: bits-per-pixel: %d\n", fvsp->bits_per_pixel);
	fprintf(stdout, "main: height mm: %d\n", fvsp->height);
	fprintf(stdout, "main: width mm: %d\n", fvsp->width);
	fprintf(stdout, "main: xres: %d\n", fvsp->xres);
	fprintf(stdout, "main: yres: %d\n", fvsp->yres);
	fprintf(stdout, "main: xoffset: %d\n", fvsp->xoffset);
	fprintf(stdout, "main: yoffset: %d\n", fvsp->yoffset);
	fprintf(stdout, "main: nonstd: %d\n", fvsp->nonstd);

	fprintf(stdout,
		"main: red: offset: %d length: %d MSB: %d\n",
		fvsp->red.offset,
		fvsp->red.length,
		fvsp->red.msb_right);
	fprintf(stdout,
		"main: green: offset: %d length: %d MSB: %d\n",
		fvsp->green.offset,
		fvsp->green.length,
		fvsp->green.msb_right);
	fprintf(stdout,
		"main: blue: offset: %d length: %d MSB: %d\n",
		fvsp->blue.offset,
		fvsp->blue.length,
		fvsp->blue.msb_right);

	switch (fvsp->grayscale) {
		case 0:
			fprintf(stdout, "%s\n", "main: color: Color");
			break;
		case 1:
			fprintf(stdout, "%s\n", "main: color: Grayscale");
			break;
		default:
			fprintf(stdout, "%s\n", "main: color: Fourcc");
			break;
	}

	errno = 0;
	void *map = mmap(NULL,
			ffsp->smem_len,
			PROT_READ | PROT_WRITE,
			MAP_SHARED,
			framebuffer_fd,
			0);
	if (errno) {
		fprintf(stdout, "%s\n", "main: ERROR");
		fprintf(stderr, "main: reason: %s\n", strerror(errno));
		close(fd);
		close(framebuffer_fd);
		tcsetattr(STDIN_FILENO, TCSAFLUSH, terp);
		exit(EXIT_FAILURE);
	}

	count = 0;
	int key_event = KEY_RELEASED;
	int * const kep = &key_event;
	struct timespec ts = {};
	struct timespec te = {};
	struct timespec *tsp = &ts;
	struct timespec *tep = &te;
	clockid_t clockid = CLOCK_MONOTONIC;
	if (clock_gettime(clockid, tsp)) {
		goto clock_err;
	}
	while (1) {
		if (handle_input(fd, terp, evp, kep)) {
			break;
		}
		draw(map, ffsp);
		errno = 0;
		if (delay(clockid)) {
			goto clock_err;
		}
		++count;
		if (max_count == count) {
			if (clock_gettime(clockid, tep)) {
				goto clock_err;
			}
			fprintf(stdout, "main: etime: %lf\n", etime(tep, tsp));
			fprintf(stdout, "main: fps: %lf\n", max_count / etime(tep, tsp));
			count = 0;
			ts = te;
		}
	}

	close(fd);
	close(framebuffer_fd);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, terp);
	errno = 0;
	if (munmap(map, ffsp->smem_len)) {
		fprintf(stderr, "%s\n", "main: ERROR");
		fprintf(stderr, "main: reason: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	return 0;
clock_err:
	{
		fprintf(stderr, "%s\n", "main: SystemClockError");
		if (errno) {
			fprintf(stderr, "main: reason: %s\n", strerror(errno));
		}
		close(fd);
		tcsetattr(STDIN_FILENO, TCSAFLUSH, terp);
		exit(EXIT_FAILURE);
	}
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
