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


___

(we'll call it 'BKR' for now: generalized prefix is 'Se', specific is 'Bkr')


## Name?
It's a pun.
Really, that's it.
Nothing obscure, special, anything like that.


## Ideas?
**there is no root user.**

All actions in the kernel must be explicitly allowed
by this security subsystem. By default, *everything is
denied access to everything but a very specific subset of 
possible system calls.*

The kernel enforces this via port rights and hash values.
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


===

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
	- ANYTHING THAT COULD POSSIBLY MAKE YOU LOSE SLEEP AT NIGHT.
	
## Ideology
No level of "good enough" is acceptable: we should always assume there are flaws in the kernel that we need to mitigate.
The security monitor's job is to make it as difficult as possible to hack a given system running Feral with appropriate configuration.
We might not be able to stop *all* hacks (there is inevitably a flaw in some part of the system, or worse yet, a flaw in the hardware itself.),
but we can at least make the effort of trying to break through as difficult as possible.

We should assume *every* user mode process is *potentially* hostile to the system.
This doesn't exclude clang (malicious code could have been injected), logon (again, malicious code), or even the liboses to some extent.


