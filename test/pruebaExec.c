#include "syscall.h"

#define NULL_32 '0'
#define NULL_64 0

void cat(OpenFileId id);

int main() {
	OpenFileId id;
	id = Open("pep");
	cat(id);
	Exit(0);
}

void cat(OpenFileId id) {
	char *c;
	Read(c, 1, id);
	while (*c != NULL_32 && *c != NULL_64) {
		Write(c, 1, ConsoleOutput);
		Read(c, 1, id);
	}
}
