# FERAL KERNEL

## What is Feral?
Feral is an experimental monolithic kernel aimed at being used for x86-64-based computers that are intended to be used as a gaming system.
It's primary purpose is having a strong graphics stack built specifically for maximizing performance out of GCN5+ GPUs, minimizing OS overhead of video games, allowing for careful control of the underlying hardware, and incorporate other features which might be useful for video games.

## Is there any naming convention?
Folders should be named in a way to avoid unnecessary characters (ie, 'inc' vs 'include'.) This is just because I tend to have too many tabs of Nautilus open and each one very small. Not important.

Functions should *always* be prefixed with 'internal' if they are temporary. (ie, to test something.) These should be wrapped in an
```
#ifdef DEBUG

VOID internalDoSomething()
{
	return;
}


internalDoSomething();
#endif DEBUG
```

Functions intended to be stable should use:
	
	- Minimal characters prefixed to define what it does ('Hal', 'Vd', 'Ke', etc.)
	- Always return a FERALSTATUS



