# Security subsystem
This folder contains security-related code for the Feral kernel.
It is intended to suit the needs for security in the kernel by
use of various means, including error correction via duplication 
(at least 3 copies of kernel objects, if modified, check state of other copies: 
if they differ, reset the object to a known good state.),
mandatory access control, directory-based access control, role-based
authentification, hashing, encryption, file-based access control,
full disk encryption, directory-based encryption, implementationing various ciphers 
for encryption of all kernel objects and other means suitable to help build as close 
to "unbreakable" as we can get. We expect only a few modules to be enabled on
real systems, and of which, some may be mutually exclusive (directory vs file permissions).
However, we want to force certain ones to be enabled by default, and potentially not allowed
to be disabled.

As always, anything less than perfect is totally unacceptable (especially in the security module).

It is mostly intended to fill the same role as SELinux or AppArmor do to the Linux kernel.
(Fittingly, I'm considering the name "Beaker", as in a laboratory beaker.)


Beaker specific code is prefixed with "Bkr", and the general function prefix is "Se".


## Name?
It's a pun.
Really, that's it.
Nothing obscure, special, anything like that.


## What are we doing?
We'll aim for what *could* be considered B3 in the Orange Book, (or better)
but we're not bothering with any formal compliance or anything like that.
(again, if you *really* want security, use OpenBSD or Linux with AppArmor/SELinux.)

Thus, we'll break rules when it's convenient to implementation, or when it doesn't
make sense in the context of Feral, if those cases exist.

## Ideas?
**there is no root user.**

All actions in the kernel must be explicitly allowed
by this security subsystem. By default, *everything is
denied access to everything but a very specific subset of 
possible system calls.*

The kernel enforces this via port rights, this security system, and hash values.
Depending on kernel configuration, we go up to
3 layers of in-memory encryption for kernel objects,
and up to 3 copies of each item, with checksums taken
on each item to ensure correctness and prevent buffer
overflow attacks in case they *somehow* go through
kernel protection.

We use several layers of encryption because unless there's
mathematical proof showing it's correct and unbreakable
*with the current hardware and software implementation*,
we should assume it's possible to break, or possible to
make easier to break into via something like a side-channel
attack. We can't use one-time pads because we can't
guarantee *true randomness*. (Nor could we really use
one time pads for filesystems anyway without requiring enormous
encryption keys.)

We should also provide each object with 3 different kinds of
checksums to be provided with it, depending on how
important the object in question is: this makes duplicating
a given object and replacing it's contents much harder, as it
now has to go through 3 different algorithms and find a common
flaw in all 3 to ensure it isn't detected by the kernel and thwarted.

Since we're monolithic, our job is *lots* harder since we can't jettison code
into user mode (performance!!! even L4 variants still have signifigant issues
with IPC performance, microkernels are too slow to be practical anywhere
except with very special hardware (ie, the CTR-001 handheld console) or for 
very specific systems (ie, some scientific robots or some sort of embedded control 
system))

Each user needs to be specifically identified:
we do not allow anonymous users.
**THERE IS NO ROOT USER**

Logs should be kept on objects by default.

There must be several levels behind every specific type of
role: levels 1 - 7, for example, with graphics permissions,
with 0 being no access, 1 being a limited, safe subset of a library, 
and 7 being full control of a given subsystem. Level 7 can only be
granted to a collocated kernel-level server, and can only be assigned
to a single one. (we be more trusting of code in the kernel except driver
code, since once you're in the kernel, you have better things to worry
about than some linked list somewhere being abused for some vulnerability.)

Possible threats:

In particular, we're looking to mitigate/thwart threats like
	- Permission escalation
	- Exfiltrating protected files from a system
	- Denial of Service attacks
	- Data loss caused by external attackers
	- Protecting the integrity of the system, even under physical compromise (ie, stolen hard drives, we want full-disk encryption)
	- Malicious code with the intent to damage hardware (ie, subvert control/safety and overclock a part by 1000000000%)
	- Attempting to inject code into the kernel (given our buffer system, __this is very important__.)
	- IPC abused to perform remote code execution
	- Information leakage to supply an attacker with information on the operations of a particular system
	- Port leakage, and claiming to be a particular service but is actually a malicious actor (ie, malware)
	- Overwriting log files, and covering tracks of any potential breaches.
	- Overwriting system configuration
	- Theft of encryption keys
	- User account theft
	- Hijacking another process on the same machine (ie, subverting Chromium and sniffing outbound traffic)
	- Attacking random pages in memory, marking them as free to cause crashes on random programs.
	- Leaking pages, such that it's possible to see another program's memory space.

## Specific details?
Essentially re-implementing SELinux (and by extension, building yet another Flask implementation).
However, this is just one part of Beaker (we do other things here on top of the same stuff SELinux does)
The kernel provides a security server in which call objects are assigned an ID.
This is cached by the object manager.
We do stuff to check a given object has permission to do certain tasks.
The above process must be atomic: we can't let an object exists without an accompanying
security token.

Don't let objects without specific permissions do thing they're not allowed to do.
etc. etc.

Even if an administrative account is compromised, it shouldn't bring down a whole system.
	

We can even extend this to be used for the filesystem, such that the tokens are unique to the system
and simply taking a hard drive and attempting to read it without taking over the host system becomes
much more difficult.

We're the operating system, we have to think of everything that could possibly go wrong
and lead to compromised systems which is bad for everyone.

## Ideology
No level of "good enough" is acceptable: we should always assume there are flaws in the kernel that we need to mitigate.
The security monitor's job is to make it as difficult as possible to hack a given system running Feral with appropriate configuration.
We might not be able to stop *all* hacks (there is inevitably a flaw in some part of the system, or worse yet, a flaw in the hardware itself.),
but we can at least make the effort of trying to break through as difficult as possible.

Since at the end of the day we're just software, we have to assume the hardware below is
correct in at least basic functionality. For better compatibility, we should
avoid rdrand (One of my test machines doesn't support it).

We should assume *every* user mode process is *potentially* hostile to the system.
This doesn't exclude clang (malicious code could have been injected), logon (again, malicious code), or even the liboses to some extent.


## Why do you care so much?
Simply because security should always be a high priority.
(also mostly because it'd be really fun to seriously try and be better than OpenBSD--not going to happen,
but we can pretend like it can!)

It's perhaps elucidated by providing an example: a device like a laptop or phone
connects to many networks, of which many other people could also be on. Some of these
might be malicious, and of those that might be malicious, they may specifically
be targetting you for one reason or another: they want to take payment information,
private keys, browser cookies, simply take your hardware, or do something "for the lulz". 
No matter the case, security should always be a high priority, especially when you don't have
complete control of every possible interaction a given machine might make.



## Additional things?
All "root"s (we have different permission 'pools' for every given task) have special
IDs based upon specific hardware configurations, which changes at every boot and upon
every hardware change. A function must be called to get these user IDs.

This is to make it difficult to target a server running Feral over the Internet
(or some RENEGADE network or something), since the attacker must either use this
function (difficult, requires remote code execution), or know exactly
the system they're targetting, what time it was booted, and details of all
the hardware components.... (or somehow otherwise access a cache page holding
the IDs, but this is basically the same as the first case.)

This is done at boot time, and does not change for removable media (hard disks
are ignored, including CD drives and USB sticks and all that.) We'll have to
deal with systems where RAM can be hot-swapped, but that's for some other
time to worry about.

We don't use random number generators because
	- We can't validate how good they are (except maybe software ones, but....)
	- We have a contradiction anyway: we need one that's both perfectly random (cryptographically secure) and psuedo-random (possible to guess outcomes.)
	- Even if we avoided above by caching perfectly random values, we'd then have a page where things are just open, and if the memory system is compromised, there everything is.
	- We can check the integrity of results by just performing the check again: if an attacker replaced a value with a different one, we would know.



## Goals
	If it's more efficient for an attacker to physically compomise a piece of hardware than
	it is to attack it through software, we've achieved our goal. We should then also do everything
	in our power to make it as difficult as possible to break into data even with physical compromise.
	(we can't halt attackers **forever**, but we can make it take many many many years to break encryption, make it difficult
	to perform arbitrary/remote code execution, and hard to otherwise compromise security.)




