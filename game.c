#include <sys/types.h>
#include <sys/stat.h>
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define KEY_RELEASED 0
#define KEY_PRESSED  1

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

	while (1) {
		errno = 0;
		if (sizeof(*evp) != read(fd, evp, sizeof(*evp))) {
			fprintf(stderr, "%s\n", "main: IOERR");
			if (errno) {
				fprintf(stderr, "main: reason: %s\n", strerror(errno));
			}
			close(fd);
			tcsetattr(STDIN_FILENO, TCSAFLUSH, terp);
			exit(EXIT_FAILURE);
		}
		if (EV_KEY == evp->type) {
			if (
				(KEY_UP == evp->code) &&
				(KEY_RELEASED == evp->value)
			   ) {
				fprintf(stdout, "%s\n", "main: key-up");
			} else if (
				(KEY_DOWN == evp->code) &&
				(KEY_RELEASED == evp->value)
				) {
				fprintf(stdout, "%s\n", "main: key-down");
			} else if (
				(KEY_LEFT == evp->code) &&
				(KEY_RELEASED == evp->value)
				) {
				fprintf(stdout, "%s\n", "main: key-left");
			} else if (
				(KEY_RIGHT == evp->code) &&
				(KEY_RELEASED == evp->value)
				) {
				fprintf(stdout, "%s\n", "main: key-right");
			} else if (KEY_ESC == evp->code) {
				fprintf(stdout, "%s\n", "main: key-esc");
				fprintf(stdout, "%s\n", "main: quitting");
				break;
			}
		}
	}

	close(fd);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, terp);
	return 0;
}

// References:
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
