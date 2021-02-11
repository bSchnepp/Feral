# Syscall design
System calls should generally be diverted to userspace
so that a userspace daemon can catch them.

How this can be achieved is by setting up a shell which handles
the current personality for processes running under it. The initial process
to do this remains in a special, Feral-only system call ABI, which has
functions to change the current system call ABI, ie, to a personality managed
by some other daemon (or the current process).

When a process calls ExFork() or some other system call like that, the current
personality is inherited from the parent process. Once outside the Feral-specific
system call ABI, it cannot be returned to. Alternatively, these personalities can
be brought into the kernel, and whenever an equivalent of CreateProcesss is called,
then a personality is specified...