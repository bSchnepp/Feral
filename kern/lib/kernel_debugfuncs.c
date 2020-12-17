#include <feral/stdtypes.h>
#include <feral/kern/frlos.h>

#ifndef NDEBUG
#include <../../drivers/serial/serial.h>
#define SERIAL_PUTCHAR(x) SerialSend(COM1_PORT, x, 1)
#else
#define SERIAL_PUTCHAR(x) /* Nothing! */
#endif


VOID SerialPrint(STRING X)
{
#ifndef _NO_SERIAL_DEBUG_PRINT_
	UINT64 Len = 0;
	KiGetStringLength(X, &Len);
	SerialSend(COM1_PORT, X, Len);
#endif
}

FERALSTATUS KiDebugPrint(STRING string)
{
	KiPrintWarnLine(string);
	SerialPrint(string);
	SerialPrint("\n");
	return STATUS_SUCCESS;
}