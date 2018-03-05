// LICENSE HEADER SCRIPT IS BROKEN ON LAPTOP, FIX THIS


#include <feral/stdtypes.h>
#include <feral/feralstatus.h>

typedef enum MountOptions
{
	FS_READ,
	FS_WRITE,
}MountOptions;

FERALSTATUS FsMountFilesystem(STRING DriveLetter, 
