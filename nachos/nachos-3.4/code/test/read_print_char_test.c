#include "syscall.h"
#define MAX_LENGTH 32

int main()
{
	char c = ReadChar();
	PrintChar(c);
	Halt();
}
