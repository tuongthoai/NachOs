#include "syscall.h"

int main() {
	int i;
	PrintString("The complete ascii table:\n");
	for (i = 0; i < 255; ++i) {
		PrintString("Character ");
		PrintInt(i);
		PrintString(" = ");
		PrintChar((char)(i));
		PrintChar('\n');
	}
	Halt();
}