// exception.cc 
//  Entry point into the Nachos kernel from user programs.
//  There are two kinds of things that can cause control to
//  transfer back to here from user code:
//
//  syscall -- The user code explicitly requests to call a procedure
//  in the Nachos kernel.  Right now, the only function we support is
//  "Halt".
//
//  exceptions -- The user code does something that the CPU can't handle.
//  For instance, accessing memory that doesn't exist, arithmetic errors,
//  etc.  
//
//  Interrupts (which can also cause control to transfer from user
//  code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#include "system.h"

#include "syscall.h"

//----------------------------------------------------------------------
// ExceptionHandler
//  Entry point into the Nachos kernel.  Called when a user program
//  is executing, and either does a syscall, or generates an addressing
//  or arithmetic exception.
//
//  For system calls, the following is the calling convention:
//
//  system call code -- r2
//      arg1 -- r4
//      arg2 -- r5
//      arg3 -- r6
//      arg4 -- r7
//
//  The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//  "which" is the kind of exception.  The list of possible exceptions 
//  are in machine.h.
//----------------------------------------------------------------------
// Doi thanh ghi Program counter cua he thong ve sau 4 byte de tiep tuc nap lenh
void 
IncreasePC() {
    int counter = machine -> ReadRegister(PCReg);
    machine -> WriteRegister(PrevPCReg, counter);
    counter = machine -> ReadRegister(NextPCReg);
    machine -> WriteRegister(PCReg, counter);
    machine -> WriteRegister(NextPCReg, counter + 4);
}

#define MaxFileLength 32

// Input: Khong gian dia chi User(int) - gioi han cua buffer(int)
// Output: Bo nho dem Buffer(char*)
// Chuc nang: Sao chep vung nho User sang vung nho System
char * 
User2System(int virtAddr, int limit) {
    int i; //chi so index
    int oneChar;
    char * kernelBuf = NULL;
    kernelBuf = new char[limit + 1]; //can cho chuoi terminal
    if (kernelBuf == NULL)
        return kernelBuf;

    memset(kernelBuf, 0, limit + 1);

    for (i = 0; i < limit; i++) {
        machine -> ReadMem(virtAddr + i, 1, & oneChar);
        kernelBuf[i] = (char) oneChar;
        if (oneChar == 0)
            break;
    }
    return kernelBuf;
}

// Input: Khong gian vung nho User(int) - gioi han cua buffer(int) - bo nho dem buffer(char*)
// Output: So byte da sao chep(int)
// Chuc nang: Sao chep vung nho System sang vung nho User
int 
System2User(int virtAddr, int len, char * buffer) {
    if (len < 0) return -1;
    if (len == 0) return len;
    int i = 0;
    int oneChar = 0;
    do {
        oneChar = (int) buffer[i];
        machine -> WriteMem(virtAddr + i, 1, oneChar);
        i++;
    } while (i < len && oneChar != 0);
    return i;
}


void
ExceptionHandler(ExceptionType which) {
    int type = machine -> ReadRegister(2);

    switch (which) {
    case NoException:
        return;

    case PageFaultException:
        DEBUG('a', "\n No valid translation found");
        printf("\n\n No valid translation found");
        interrupt -> Halt();
        break;

    case ReadOnlyException:
        DEBUG('a', "\n Write attempted to page marked read-only");
        printf("\n\n Write attempted to page marked read-only");
        interrupt -> Halt();
        break;

    case BusErrorException:
        DEBUG('a', "\n Translation resulted invalid physical address");
        printf("\n\n Translation resulted invalid physical address");
        interrupt -> Halt();
        break;

    case AddressErrorException:
        DEBUG('a', "\n Unaligned reference or one that was beyond the end of the address space");
        printf("\n\n Unaligned reference or one that was beyond the end of the address space");
        interrupt -> Halt();
        break;

    case OverflowException:
        DEBUG('a', "\nInteger overflow in add or sub.");
        printf("\n\n Integer overflow in add or sub.");
        interrupt -> Halt();
        break;

    case IllegalInstrException:
        DEBUG('a', "\n Unimplemented or reserved instr.");
        printf("\n\n Unimplemented or reserved instr.");
        interrupt -> Halt();
        break;

    case NumExceptionTypes:
        DEBUG('a', "\n Number exception types");
        printf("\n\n Number exception types");
        interrupt -> Halt();
        break;

    case SyscallException:
        switch (type) {

        case SC_Halt:
            // Input: Khong co
            // Output: Thong bao tat may
            // Chuc nang: Tat HDH
            DEBUG('a', "\nShutdown, initiated by user program. ");
            printf("\nShutdown, initiated by user program. ");
            interrupt -> Halt();
            return;

        case SC_ReadInt:
            {
                // Input: K co
                // Output: Tra ve so nguyen doc duoc tu man hinh console.
                // Chuc nang: Doc so nguyen tu man hinh console.
                char* buff = new char[255 + 1]; // 255 max buffer size
                int bytesRead = gSynchConsole->Read(buff, 255);
                int res = 0;

                bool isNegative = false;
                int left = 0;
                int right = 0;
                bool allow = 1;

                if (buff[0] == '-') 
                {
                    isNegative = true;
                    left = 1;
                    right = 1;
                }
                
                // valid integer
                for (int i = left; i < bytesRead; ++i) {
                    if (buff[i] == '.') {
                        for (int j = i + 1; j < bytesRead; ++j) {
                            if (buff[j] != '0') {
                                allow = 0;
                            }
                        }
                        right = i - 1;
                        break;
                    }
                    else {
                        if ('0' <= buff[i] && buff[i] <= '9') {
                            right = i;
                            continue;
                        }
                        else {
                            allow = 0;
                        }
                    }
                }

                if (allow) {
                    for (int i  = left; i <= right; ++i) {
                        res = res * 10 + (int)(buff[i] - '0');
                    }

                    if (isNegative) res *= -1;

                    machine->WriteRegister(2, res);
                    IncreasePC();
                    delete buff;
                    return;
                }

                // printf("\n\n The integer number is not valid");
                // DEBUG('a', "\n The integer number is not valid");
                machine->WriteRegister(2, 0);
                IncreasePC();
                delete buff;
                return;
            }

        case SC_PrintInt:
            {
                /*
                INPUT:  INTEGER
                OUTPUT: void
                THIS SYSCALL PRINT A NUMBER ONTO CONSOLE
                */

                int n = machine->ReadRegister(4);
                
                // Incase n == 0
                if (n == 0) {
                    gSynchConsole->Write("0", 1);
                    IncreasePC();
                    return;
                }

                int numberOfNum = 0; // Bien de luu so chu so cua number
                int firstNumIndex = 0; 
        
                if(n < 0)
                {
                    n *= -1;
                    gSynchConsole->Write("-", 1);
                }   
                
                int t_number = n; // bien tam cho number
                while(t_number)
                {
                    numberOfNum++;
                    t_number /= 10;
                }

                // Tao buffer chuoi de in ra man hinh
                char* buffer;
                buffer = new char[255 + 1];
                for(int i = numberOfNum - 1; i >= 0; i--)
                {
                    buffer[i] = (char)((n % 10) + 48);
                    n /= 10;
                }
                buffer[numberOfNum] = 0;    
                gSynchConsole->Write(buffer, numberOfNum);
                delete buffer;
                IncreasePC();
                return;  
            }

        case SC_ReadChar:
            {
                char* buff = new char[255];
                int bytesRead = gSynchConsole->Read(buff, 255);

                if (bytesRead >= 1) {
                    machine->WriteRegister(2, buff[0]);
                }
                else {
                    printf("Ky tu rong!");
                    DEBUG('a', "\nERROR: Ky tu rong!");
                    machine->WriteRegister(2, 0);
                }
                delete buff;
                break;
            }

        case SC_PrintChar:
            {
                char c = (char)machine->ReadRegister(4);
                gSynchConsole->Write(&c, 1);
                IncreasePC();
                return;
            }

        case SC_PrintString:
            {
                int vitualAddress;
                char* buffer;

                vitualAddress = machine->ReadRegister(4);
                buffer = User2System(vitualAddress, 255);
                int len = 0;
                while (buffer[len] != 0 && len < 255) len++;

                if (len == 255) {
                    //this case is the buffer has no stop character
                    buffer[len-1] = 0;
                }

                gSynchConsole->Write(buffer, len + 1);

                delete buffer;
                IncreasePC();
                return;
            }

        case SC_ReadString:
            {
                /*
                INPUT : Buffer (char*), BufferSize
                OUTPUT: VOID
                THIS Receice a pointer to a buffer then read date from console and store into it.
                */
                int vitualAddress, bufferSize;
                char * buffer;

                vitualAddress = machine->ReadRegister(4);
                bufferSize = machine->ReadRegister(5);
                buffer = User2System(vitualAddress, bufferSize);
                gSynchConsole->Read(buffer, bufferSize);
                System2User(vitualAddress, bufferSize, buffer);

                delete buffer;
                IncreasePC();
                return;
            }

        case SC_Create: 
            {
            // Input: Dia chi tu vung nho user cua ten file
            // Output: -1 = Loi, 0 = Thanh cong
            // Chuc nang: Tao ra file voi tham so la ten file
            int virtAddr;
            char * filename;
            DEBUG('a', "\n SC_CreateFile call ...");
            DEBUG('a', "\n Reading virtual address of filename");

            virtAddr = machine -> ReadRegister(4); //Doc dia chi cua file tu thanh ghi R4
            DEBUG('a', "\n Reading filename.");

            //Sao chep khong gian bo nho User sang System, voi do dang toi da la (32 + 1) bytes
            filename = User2System(virtAddr, MaxFileLength + 1);
            if (strlen(filename) == 0) {
                printf("\n File name is not valid");
                DEBUG('a', "\n File name is not valid");
                machine -> WriteRegister(2, -1); //Return -1 vao thanh ghi R2
                //IncreasePC();
                //return;
                break;
            }

            if (filename == NULL) //Neu khong doc duoc
            {
                printf("\n Not enough memory in system");
                DEBUG('a', "\n Not enough memory in system");
                machine -> WriteRegister(2, -1); //Return -1 vao thanh ghi R2
                delete filename;
                //IncreasePC();
                //return;
                break;
            }
            DEBUG('a', "\n Finish reading filename.");

            if (!fileSystem -> Create(filename, 0)) //Tao file bang ham Create cua fileSystem, tra ve ket qua
            {
                //Tao file that bai
                printf("\n Error create file '%s'", filename);
                machine -> WriteRegister(2, -1);
                delete filename;
                //IncreasePC();
                //return;
                break;
            }

            //Tao file thanh cong
            machine -> WriteRegister(2, 0);
            delete filename;
            //IncreasePC(); //Day thanh ghi lui ve sau de tiep tuc ghi
            //return;
            break;
        }

        default:
            break;
        }
        IncreasePC();
    }
}