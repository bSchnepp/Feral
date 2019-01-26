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


=======
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

Each user needs to be specifically identified:
we do not allow anonymous users.
**THERE IS NO ROOT USER**

Logs should be kept on objects by default.

There must be several levels behind every specific type of
role: levels 1 - 7, for example, with graphics permissions,
with 0 being no access, 1 being a limited, safe subset of the
UI library, and 7 being full control of UI subsystem.

