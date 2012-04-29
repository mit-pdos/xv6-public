#if defined(LINUX)

#define O_CREATE O_CREAT
#define xfork() fork()
#define xexit() exit(EXIT_SUCCESS)
static inline void xwait()
{
  int status;
  if (wait(&status) < 0)
    edie("wait");
  if (!WIFEXITED(status))
    die("bad status %u", status);
}
#define xmkdir(pathname) mkdir((pathname), S_IWUSR|S_IRUSR);
#define xcreat(name) open((name), O_CREATE, S_IRUSR|S_IWUSR)
#define mtenable(x) do { } while(0)
#define mtdisable(x) do { } while(0)

#else // Must be xv6

#define xfork() fork(0)
#define xexit() exit()
#define xmkdir(pathname) mkdir((pathname))
#define xcreat(name) open((name), O_CREATE|O_RDWR)
#define xwait() wait()

#endif
