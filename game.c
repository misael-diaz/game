#include <sys/types.h>
#include <sys/stat.h>
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main ()
{
	errno = 0;
	struct input_event ev;
	struct input_event *evp = &ev;
	char * const kbdev = "/dev/input/by-id/usb-Dell_Dell_QuietKey_Keyboard-event-kbd";
	int fd = open(kbdev, O_RDONLY);
	if (-1 == fd || errno) {
		fprintf(stderr, "%s\n", "main: IOERR");
		if (errno) {
			fprintf(stderr, "main: reason: %s\n", strerror(errno));
		}
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
			exit(EXIT_FAILURE);
		}
		if (EV_KEY == evp->type) {
			if (KEY_UP == evp->code) {
				fprintf(stdout, "%s\n", "main: key-up");
			} else if (KEY_DOWN == evp->code) {
				fprintf(stdout, "%s\n", "main: key-down");
			} else if (KEY_ESC == evp->code) {
				fprintf(stdout, "%s\n", "main: key-esc");
				fprintf(stdout, "%s\n", "main: quitting");
				break;
			}
		}
	}

	close(fd);
	return 0;
}

// references: https://stackoverflow.com/questions/20943322/accessing-keys-from-linux-input-device
