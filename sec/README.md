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



