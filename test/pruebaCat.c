#include "syscall.h"

#define EOF 0

void cat(OpenFileId id);

int main() {
	OpenFileId id;
	id = Open("./test/recursos/readme");
	cat(id);
	Exit(0);
}

void cat(OpenFileId id) {
	char *c;
	Read(c, 1, id);
	while (*c != EOF) {
		Write(c, 1, ConsoleOutput);
		Read(c, 1, id);
	}
}
