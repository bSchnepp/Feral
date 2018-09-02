## Properly set up the new build system.

ARCH="$1"
CMD="$2"

if [ -z "$ARCH" ]  
then
	echo "You must supply an arch!"
	echo "Possible options are:"
	echo "x86_64"
	echo "arch64"
	exit 1
fi

if [ -z "$CMD" ]  
then
	echo "You must supply a command!"
	echo "You can run:"
	echo "qemu"
	echo "iso"
	echo "qemu-nokvm"
	echo "clean"
fi

export ARCH
export CMD
bmake $CMD
