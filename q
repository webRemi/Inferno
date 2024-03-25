[4mgetgid[24m(2)                  System Calls Manual                  [4mgetgid[24m(2)

[1mNAME[0m
       getgid, getegid - get group identity

[1mLIBRARY[0m
       Standard C library ([4mlibc[24m, [4m-lc[24m)

[1mSYNOPSIS[0m
       [1m#include <unistd.h>[0m

       [1mgid_t getgid(void);[0m
       [1mgid_t getegid(void);[0m

[1mDESCRIPTION[0m
       [1mgetgid[22m() returns the real group ID of the calling process.

       [1mgetegid[22m() returns the effective group ID of the calling process.

[1mERRORS[0m
       These functions are always successful and never modify [4merrno[24m.

[1mVERSIONS[0m
       On  Alpha,  instead  of  a  pair  of [1mgetgid[22m() and [1mgetegid[22m() system
       calls, a single [1mgetxgid[22m() system call is provided, which returns a
       pair of real and effective GIDs.  The glibc [1mgetgid[22m() and [1mgetegid[22m()
       wrapper functions transparently deal with  this.   See  [1msyscall[22m(2)
       for details regarding register mapping.

[1mSTANDARDS[0m
       POSIX.1-2008.

[1mHISTORY[0m
       POSIX.1-2001, 4.3BSD.

       The  original  Linux [1mgetgid[22m() and [1mgetegid[22m() system calls supported
       only 16-bit group IDs.  Subsequently, Linux 2.4  added  [1mgetgid32[22m()
       and  [1mgetegid32[22m(),  supporting  32-bit IDs.  The glibc [1mgetgid[22m() and
       [1mgetegid[22m() wrapper functions transparently deal with the variations
       across kernel versions.

[1mSEE ALSO[0m
       [1mgetresgid[22m(2), [1msetgid[22m(2), [1msetregid[22m(2), [1mcredentials[22m(7)

Linux man-pages 6.05.01         2023-03-30                      [4mgetgid[24m(2)
