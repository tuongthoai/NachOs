#include "syscall.h"
#define MAX_LENGTH 32

int main()
{
	int num = ReadInt();
	PrintInt(num);
	Halt();
}
