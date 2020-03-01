# Compatability
Feral makes several promises about upgrading versions, which
go in effect at version 1.0.0.

The important parts are:
	- Given any usermode program running on version X, it will also run on 
	version X+1, X+2, ... X+n, where n is any positive integer. The only
	exception to this is to fix a serious security bug, or some other
	error in the kernel was meaningful to usermode somehow, such as 
	filesystem structure and specific implementation.
	
	- Given any driver for Feral, it should be binary compatible until the 
	next major version, and source compatible for any future releases. This 
	assumes the driver is using APIs as intended, and does not manipulate 
	kernel data structures, other kernel code, etc. Note that 'should' is
	a recommendation, not a requirement. Driver developers should upstream
	their drivers so as to ensure it can be maintained for future versions
	of the kernel.
	
	- CPU support should remain the same between every major version. 
	Support for older CPUs may be dropped at any major release.
	
	- Release numbering is always consistent: MAJOR.MINOR.PATCH-EXTENSIONS.
	The EXTENSIONS may have anything next to each one, but will not have a 
	'-' nor a space, as a '-' indicates a different extension.
	For example, major version 5, minor version 3, patch 10, with some 
	extension "rc" for a release candidate version 1 would be 
	"Feral 5.3.10-rc1", The release string will never contain spaces.
