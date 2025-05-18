#include <sys/types.h>
#include <sys/stat.h>
#include <linux/input.h>
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

void draw(void)
{
	return;
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
	struct input_event * const iep
)
{
	errno = 0;
	int rc = 0;
	if (sizeof(*iep) != read(fd, iep, sizeof(*iep))) {
		fprintf(stderr, "%s\n", "main: IOERR");
		if (errno) {
			fprintf(stderr, "main: reason: %s\n", strerror(errno));
		}
		close(fd);
		tcsetattr(STDIN_FILENO, TCSAFLUSH, terp);
		exit(EXIT_FAILURE);
	}
	if (EV_KEY == iep->type) {
		if (
				(KEY_UP == iep->code) &&
				(KEY_RELEASED == iep->value)
		   ) {
			fprintf(stdout, "%s\n", "main: key-up");
		} else if (
				(KEY_DOWN == iep->code) &&
				(KEY_RELEASED == iep->value)
			  ) {
			fprintf(stdout, "%s\n", "main: key-down");
		} else if (
				(KEY_LEFT == iep->code) &&
				(KEY_RELEASED == iep->value)
			  ) {
			fprintf(stdout, "%s\n", "main: key-left");
		} else if (
				(KEY_RIGHT == iep->code) &&
				(KEY_RELEASED == iep->value)
			  ) {
			fprintf(stdout, "%s\n", "main: key-right");
		} else if (KEY_ESC == iep->code) {
			fprintf(stdout, "%s\n", "main: key-esc");
			fprintf(stdout, "%s\n", "main: quitting");
			rc = 1;
		}
	}
	return rc;
}

int main ()
{
	// disables terminal input echoing
	struct termios term = {};
	struct termios *terp = &term;
	tcgetattr(STDIN_FILENO, terp);
	terp->c_lflag &= (~ECHO);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, terp);
	terp->c_lflag |= ECHO;

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

	count = 0;
	struct timespec ts = {};
	struct timespec te = {};
	struct timespec *tsp = &ts;
	struct timespec *tep = &te;
	clockid_t clockid = CLOCK_MONOTONIC;
	if (clock_gettime(clockid, tsp)) {
		goto clock_err;
	}
	while (1) {
		if (handle_input(fd, terp, evp)) {
			break;
		}
		draw();
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
	tcsetattr(STDIN_FILENO, TCSAFLUSH, terp);
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
