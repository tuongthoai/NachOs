/* sort.c 
 *    Test program to sort a large number of integers.
 *
 *    Intention is to stress virtual memory system.
 *
 *    Ideally, we could read the unsorted array off of the file system,
 *	and store the result back to the file system!
 */

#include "syscall.h"

int a[111];

int
main()
{
    int n, i, j, tmp;
    PrintString("Nhap n: ");
    n = ReadInt();
    if (n <= 0) {
        PrintString("Mang rong");
    }
    else {
        for (i = 0; i < n; ++i) {
            PrintString("Nhap a[");
            PrintInt(i);
            PrintString("]: ");
            a[i] = ReadInt();
        }    
    }

    //bubble sort
    for (i = 0; i < n - 1 ;++ i) {
        for (j = i + 1; j < n; ++j) {
            if (a[i] > a[j]) {
                tmp  = a[i];
                a[i] = a[j];
                a[j] = tmp;
            }
        }
    }

    //print array
    PrintString("The result array: ");
    PrintInt(a[0]);
    for (i = 1; i < n; ++ i) {
        PrintString(", ");
        PrintInt(a[i]);
    }
    Halt();
}
