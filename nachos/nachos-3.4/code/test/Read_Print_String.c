#include "syscall.h"

int main() {
	char inputString[256];
	ReadString(inputString, 255);
	PrintString(inputString);
	Halt();
}