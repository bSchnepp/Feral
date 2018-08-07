(the majority of this doesn't exist yet, it's simply to outline where we're going for now. Tend to work backwards, document how it should work and then make something to make it true.
 We should outline the goals and how it should be built because getting it "almost right" is still __wrong__. It is vastly preferable to fix this than to fix a bunch of bad ring-0 code later 
 on from lazy design, **especially** when something in userspace already relies upon it.)

## Initialization
	The Feral kernel is designed to be flexible, extendable, and versatile. It's designed to derive many of it's concepts from the
	"client-server" model, while maintaining a structure emphasizing performance above all other aspects of the system. In most cases
	involving "client-server", the kernel is almost always the server. As a result of many technologies going foward being designed with
	distributed computing and deeper integration of networking-based designs, the Feral kernel intends to follow this trend. While the performance
	implications of client-server are numerous, Feral is intended to mitigate as much of that as possible.
	
	Interprocess communication, for example, is always a network-based connection. As such, Feral is scalable: one can remotely invoke
	client programs through the network to achieve some meaningful function and return the needed data, given the right permissions.
	This is particularly useful for game developers as it allows a single user to have a small cluster of gaming PCs networked together
	and for additional graphics power to be given to the game (thus increasing frames per second at the cost of some network delay.)
	This is particularly useful for game streaming, as the base primitives for streaming are built right into the kernel, allowing one
	to play a game in a quiet room without lots of loud fans, given network latency isn't a large concern.
	
	Whereas in a *NIX-based system, one could describe the system as "Everything is a file", under Feral Waypoint, one could describe the system as
	"Everything a resource accessible through a port". Resources in Feral are supersets of files: they may be read, written to, appended to, etc.,
	or some subset of those operations, given the recieving server on the other side is capable of translating those calls into meaningful actions. 
	
	The Feral kernel, upon initialization, begins mapping itself to the higher half of available system memory, and
	startup of some other components, roughly in this order:
	
		- ObMgr, which is the Object Manager. It defines and handles management of resources, and is what defines handles. It also defines
		superclasses for objects like windows and pointing devices, which prevent rogue applications from taking over another program's handles
		by setting default settings on the ports they open.
		
		- UN-DEAD, or the "UNiversal Display Enhancing Advanced Devices (manager)", roughtly the same purpose as Linux's DRM.
		(the name comes from that, without it, we fall back to base VGA text-mode, whatever it's in, and hope it's 80x25.) UNDEAD is also
		useful for directly programming a graphics accelerator card, and using it for purposes other than graphics, ie, machine learning.
	
		- LAN-MGR, which manages network interfaces and security. (actual network capability don't start until the scheduler does)
		
		- FsMgr, which implements the FAT filesystem and NX-FILES ("FeralFS") filesystem modules.
	
	The kernel expects to be passed a small structure describing the system, and
	begins it's startup in a procedure called "KiSystemStartup" located in kern/kernel_main.c
	
	The kernel is organized into two parts: an "Executive" and a "Resource Manager".
	The executive is a loose group of "servers" which implement some core functionality,
	such as loading and running executables, performing disk I/O, or managing a graphics
	ASIC. This communicates with the Resource Manager to inform it of what resources
	it is managing and using, as well as to request pages as either kernel or user
	mode memory and so forth.
	
	This function would proceed to bring up the remaining processor cores, and set up a scheduler profile.
	The rules for scheduling are as follows:
	
		- Do not separate a program to multiple CPUs (or CCXes in the case of ZEN) unless we're out of cores.
		
		- Always schedule on ($CURRENT_PROC + $NUM_CMT_THREADS) % $NUMCPUs. If we would overflow the number of CPUs, add 1 to the end.
		(ie, if we had 4 cores and 8 threads, and wanted to schedule a 5th thread, it would go to thread 1, and share CPU time with thread 0.)
		(if we were on a Knight's Mill or something, we would add 4. On mainstream x86 chips, we would add 2. In some lower end models without SMT, we would add 1.)
	
		- Avoid scheduling different priority processes onto the same physical CPU
		
	The kernel would then look in A:/System/Services/ for drivers which implement subsystems to run under.
		(these would be things like c runtimes, compatibility layers, emulation settings, etc, baked into a driver implementing what we need)
		The most common one would be "waypoint.sys", which would implement the Waypoint API for the Feral kernel.
		(though one could also add a libmach_sa.sys or libmach.sys for Mach and *NIX compatibility, or refactor WINE as a subsystem for Feral...)
		
	
	When doing system initialization, the Feral kernel does a few things, all of which implement something akin to 
	"init" in most other operating systems:
	
		- Initialize the Master Control Process (essentially akin to most of "init" in most *NIXoids)
			(mcp.pro, subsystem Native, ELF format)
		
		- Initialize the session manager (In the NTOS, this is akin to both smss.exe and winlogon.exe)
			(sesmgr.pro, subsystem Native, ELF format)
			
		- Daemon Manager, which keeps track of running daemons ("Services" in the NTOS). ("daemgr.pro")
			If a daemon is stopped unexpectedly, the daemon  manager restarts it automatically unless explicitly told not to.

## Ports

	The most important concept for the Feral kernel is the idea of **Ports**. Ports are to Feral what files are to *NIX, in that
	everything is an object accessible via a port. Files may be accessed through opening a port in the filesystem's port space,
	and sending data in packets to the port. Network connections, likewise, can be redirected to files, and be very useful for
	debugging (ie, for developing a game client's network core in which the corresponding server does not exist yet.).
	
	For example, sending a request to the graphics card to draw something is done through a port. Opening a program is done
	by opening a port and mapping the program to it. Identifying the programs running on the system is done by getting a list
	of open ports and requesting the names of each program and other metadata to each one. Mutual Exclusive locks are created
	by assignment to a port and setting a value to listen for, and so on. Everything in Feral is done through ports.
	
	A port is a unit of the system which has several aspects about it:
		
		- It must have an owner. This may be the superuser, "SYSTEM", or a Group, but NOT a Parent User.
		
		- It may have one or more users accessing it. The owner is implied to be using the port. When the owner discards ownership, the port is
		freed. If the port has no users, it is freed. If the port is in use, set to one max user only, and the other side quits, the port is freed.
		Feral ensures that ports are always kept clean, and has safeguards in place to prevent denial-of-service attacks by opening too many ports.
		
		- It must have a defined number of maximum users.
		
		- It must have some form of output, be it a memory buffer, a file, or a buffer in another executable, or to some destination over the network.
		
		- It can be remapped and reassigned: a port to a file can easilly be redirected as a TCP connection to a remote server.
		
		- It must have defined access rights for every user/Group involved.
		
	For example, when a program requests to read a file from the filesystem, a port is opened by the kernel and made available to the program
	requesting it. On both sides are access rights, of which are a superset of the existing *NIX permissions. Of note that are added,
	a flag for APPEND ONLY, DIRECTORY INHERIT, NO INHERIT, and ONLY INHERIT.
	
	Feral introduces the concepts of port spaces, in which there may be an unlimited number of ports available (specific limit is an implementation detail) on the system, 
	but a Task is limited to only using a small subset of the total amount. For example, an IPv4 TCP portspace would be limited to 2^32 possible ports, and of that, 
	the first 1024 are only available to SYSTEM, (ie, a program like Apache targetting port 80 on the system must be run as SYSTEM to get that port).
			
## Users, groups, and Parent Users
	
	Feral has several core units of division: the User, it's Parent User, and Groups.
	A User represents the core functions that a user of the system would be able to do:
	it has Pictures, Games, Movies, and other things, as well as control over files,
	ports, and other aspects of the system. The extent of these features depends on a User's
	permissions.
	
	A Parent User is simply a profile in which Users may be derived from: conceptually, it is
	a "superclass" of which Users are a subset of. When a user's Parent User is modified,
	so too are the permissions of the User if they are not changed from the Parent User's.
	
	Groups are a collection of Users and Parent Users which are used to define a set of users
	which may access a resource or do some other meaningful task. This is not the same as a *NIX
	group, as Groups exist specifically for resource sharing, not to represent access to system features.
	The *NIX style of assigning permissions is handed by Parent Users instead, and on a per-User basis otherwise.
	
	Each User, Parent User, and Group is assigned an ID in the global port space. As with the rest of the design
	of Feral, "Everything is a an object accessible over a port". Permission checks will always go through the path of

	```
		User --> Parent User --> Group
	```
	
	
