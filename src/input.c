#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "input.h"

int in_handle_input(
		int * const kep,
		struct input_event * const iep,
		struct entity * const entities,
		int const fd
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
	char buf[IN_BUFSIZE_KEV];
	char key_left_str[4]  = { 0x1b, 0x5b, 0x44, 0x00 };
	char key_right_str[4] = { 0x1b, 0x5b, 0x43, 0x00 };
	char key_up_str[4]    = { 0x1b, 0x5b, 0x41, 0x00 };
	char key_down_str[4]  = { 0x1b, 0x5b, 0x42, 0x00 };
	char key_esc_str[2]   = { 0x1b, 0x00 };
	struct entity * const gamer = &entities[EN_GAMER_ID];

	memset(buf, 0, IN_BUFSIZE_KEV * sizeof(*buf));
	if (IN_PRESSED_KEV == *kep) {
		code = iep->code;
	} else {
		bytes = read(STDIN_FILENO, buf, IN_BUFSIZE_KEV * sizeof(*buf));
		if (0 == bytes) {
			return rc;
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
	} while ((EV_KEY != iep->type) || (code != iep->code));

	if ((key_up == iep->code)) {
		if ((IN_RELEASE_KEV == iep->value)) {
			*kep = IN_RELEASE_KEV;
			gamer->yvel = 0;
		} else {
			*kep = IN_PRESSED_KEV;
			gamer->yvel = -EN_GAMER_VEL;
		}
	} else if (key_down == iep->code) {
		if ((IN_RELEASE_KEV == iep->value)) {
			*kep = IN_RELEASE_KEV;
			gamer->yvel = 0;
		} else {
			*kep = IN_PRESSED_KEV;
			gamer->yvel = EN_GAMER_VEL;
		}
	} else if (key_left == iep->code) {
		if ((IN_RELEASE_KEV == iep->value)) {
			*kep = IN_RELEASE_KEV;
			gamer->xvel = 0;
		} else {
			*kep = IN_PRESSED_KEV;
			gamer->xvel = -EN_GAMER_VEL;
		}
	} else if (key_right == iep->code) {
		if ((IN_RELEASE_KEV == iep->value)) {
			*kep = IN_RELEASE_KEV;
			gamer->xvel = 0;
		} else {
			*kep = IN_PRESSED_KEV;
			gamer->xvel = EN_GAMER_VEL;
		}
	} else if (KEY_ESC == iep->code) {
		if ((IN_RELEASE_KEV == iep->value)) {
			*kep = IN_RELEASE_KEV;
		} else {
			*kep = IN_PRESSED_KEV;
		}
		rc = 1;
	}
	return rc;
}

// opens keyboard device
int in_open_kb(
		char const * const kbdev
)
{
	errno = 0;
	int fd = open(kbdev, O_RDONLY);
	if (-1 == fd || errno) {
		fprintf(stderr, "%s\n", "open_kbdev: IOERR");
		if (errno) {
			fprintf(stderr, "open_kbdev: reason: %s\n", strerror(errno));
		}
		_exit(EXIT_FAILURE);
	}
	return fd;
}

// enables non-blocking stdin reads and disables terminal input echoing and canonical mode
// DEV:
// We want to call this just before starting the game loop because if an error happens
// that we cannot handle we are going to mess up the terminal for the shell because
// the shell expects a canonical terminal
void init_term (struct termios * const tp)
{
	int flags = fcntl(STDIN_FILENO, F_GETFD);
	flags |= O_NONBLOCK;
	fcntl(STDIN_FILENO, F_SETFD, flags);
	tcgetattr(STDIN_FILENO, tp);
	tp->c_lflag &= (~(ECHO | ICANON));
	tp->c_cc[VMIN] = 0;
	tp->c_cc[VTIME] = 0;
	tcsetattr(STDIN_FILENO, TCSAFLUSH, tp);
	tp->c_lflag |= (ECHO | ICANON);
}

/*

gnu-linux-gaming-dev                                 May 17, 2025

author: @misael-diaz
source: src/input.c

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
