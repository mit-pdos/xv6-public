#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"
#include "wstatus.h"

char buf[8192];
char name[3];
char *echoargv[] = { "echo", "ALL", "OBJFSTESTS", "PASSED", 0 };
int stdout = 1;

// another concurrent link/unlink/create test,
// to look for deadlocks.
void
linkunlink()
{
    int pid, i;

    printf(1, "linkunlink test\n");

    unlink("x");
    pid = fork();
    if(pid < 0){
        printf(1, "fork failed\n");
        exit(1);
    }

    unsigned int x = (pid ? 1 : 97);
    for(i = 0; i < 100; i++){
        x = x * 1103515245 + 12345;
        if((x % 3) == 0){
            close(open("x", O_RDWR | O_CREATE));
        } else if((x % 3) == 1){
            link("cat", "x");
        } else {
            unlink("x");
        }
    }

    if(pid)
        wait(0);
    else
        exit(0);

    printf(1, "linkunlink ok\n");
}

// four processes create and delete different files in same directory
void
createdelete(void)
{
    enum { N = 20 };
    int pid, i, fd, pi;
    char name[32];
    int process_count = 4;

    printf(1, "createdelete test\n");

    for(pi = 0; pi < process_count; pi++){
        pid = fork();
        if(pid < 0){
            printf(1, "fork failed\n");
            exit(1);
        }

        if(pid == 0){
            name[0] = 'p' + pi;
            name[2] = '\0';
            for(i = 0; i < N; i++){
                name[1] = '0' + i;
                fd = open(name, O_CREATE | O_RDWR);
                if(fd < 0){
                    printf(1, "create failed\n");
                    exit(1);
                }
                close(fd);
                if(i > 0 && (i % 2 ) == 0){
                    name[1] = '0' + (i / 2);
                    if(unlink(name) < 0){
                        printf(1, "unlink failed\n");
                        exit(1);
                    }
                }
            }
            exit(0);
        }
    }

    for(pi = 0; pi < process_count; pi++){
        wait(0);
    }

    name[0] = name[1] = name[2] = 0;
    for(i = 0; i < N; i++){
        for(pi = 0; pi < process_count; pi++){
            name[0] = 'p' + pi;
            name[1] = '0' + i;
            fd = open(name, 0);
            if((i == 0 || i >= N/2) && fd < 0){
                printf(1, "oops createdelete %s didn't exist\n", name);
                exit(1);
            } else if((i >= 1 && i < N/2) && fd >= 0){
                printf(1, "oops createdelete %s did exist\n", name);
                exit(1);
            }
            if(fd >= 0)
                close(fd);
        }
    }

    for(i = 0; i < N; i++){
        for(pi = 0; pi < process_count; pi++){
            name[0] = 'p' + i;
            name[1] = '0' + i;
            unlink(name);
        }
    }

    printf(1, "createdelete ok\n");
}

// can I unlink a file and still read it?
void
unlinkread(void)
{
    int fd, fd1;

    printf(1, "unlinkread test\n");
    fd = open("unlinkread", O_CREATE | O_RDWR);
    if(fd < 0){
        printf(1, "create unlinkread failed\n");
        exit(1);
    }
    write(fd, "hello", 5);
    close(fd);

    fd = open("unlinkread", O_RDWR);
    if(fd < 0){
        printf(1, "open unlinkread failed\n");
        exit(1);
    }
    if(unlink("unlinkread") != 0){
        printf(1, "unlink unlinkread failed\n");
        exit(1);
    }

    fd1 = open("unlinkread", O_CREATE | O_RDWR);
    write(fd1, "yyy", 3);
    close(fd1);

    if(read(fd, buf, sizeof(buf)) != 5){
        printf(1, "unlinkread read failed");
        exit(1);
    }
    if(buf[0] != 'h'){
        printf(1, "unlinkread wrong data\n");
        exit(1);
    }
    if(write(fd, buf, 10) != 10){
        printf(1, "unlinkread write failed\n");
        exit(1);
    }
    close(fd);
    unlink("unlinkread");
    printf(1, "unlinkread ok\n");
}

// test concurrent create/link/unlink of the same file
#define FILES_COUNT (10)
void
concreate(void)
{
    char file[3];
    int i, pid, n, fd;
    char fa[FILES_COUNT];
    struct {
        ushort inum;
        char name[14];
    } de;

    printf(1, "concreate test\n");
    file[0] = 'C';
    file[2] = '\0';
    for(i = 0; i < FILES_COUNT; i++){
        file[1] = '0' + i;
        unlink(file);
        pid = fork();
        if(pid && (i % 3) == 1){
            link("C0", file);
        } else if(pid == 0 && (i % 5) == 1){
            link("C0", file);
        } else {
            fd = open(file, O_CREATE | O_RDWR);
            if(fd < 0){
                printf(1, "concreate create %s failed\n", file);
                exit(1);
            }
            close(fd);
        }
        if(pid == 0)
            exit(0);
        else
            wait(0);
    }

    memset(fa, 0, sizeof(fa));
    fd = open(".", 0);
    n = 0;
    while(read(fd, &de, sizeof(de)) > 0){
        if(de.inum == 0)
            continue;
        if(de.name[0] == 'C' && de.name[2] == '\0'){
            i = de.name[1] - '0';
            if(i < 0 || i >= sizeof(fa)){
                printf(1, "concreate weird file %s\n", de.name);
                exit(1);
            }
            if(fa[i]){
                printf(1, "concreate duplicate file %s\n", de.name);
                exit(1);
            }
            fa[i] = 1;
            n++;
        }
    }
    close(fd);

    if(n != FILES_COUNT){
        printf(1, "concreate not enough files in directory listing, n = %d\n", n);
        exit(1);
    }

    for(i = 0; i < FILES_COUNT; i++){
        file[1] = '0' + i;
        pid = fork();
        if(pid < 0){
            printf(1, "fork failed\n");
            exit(1);
        }
        if(((i % 3) == 0 && pid == 0) ||
           ((i % 3) == 1 && pid != 0)){
            close(open(file, 0));
            close(open(file, 0));
            close(open(file, 0));
            close(open(file, 0));
        } else {
            unlink(file);
            unlink(file);
            unlink(file);
            unlink(file);
        }
        if(pid == 0)
            exit(0);
        else
            wait(0);
    }

    printf(1, "concreate ok\n");
}

// More file system tests

// two processes write to the same file descriptor
// is the offset shared? does inode locking work?
void
sharedfd(void)
{
    int fd, pid, i, n, nc, np;
    char buf[10];

    printf(1, "sharedfd test\n");

    unlink("sharedfd");
    fd = open("sharedfd", O_CREATE|O_RDWR);
    if(fd < 0){
        printf(1, "fstests: cannot open sharedfd for writing");
        return;
    }
    pid = fork();
    memset(buf, pid==0?'c':'p', sizeof(buf));
    for(i = 0; i < 10; i++){
        if(write(fd, buf, sizeof(buf)) != sizeof(buf)){
            printf(1, "fstests: write sharedfd failed\n");
            break;
        }
    }
    if(pid == 0)
        exit(0);
    else
        wait(0);
    close(fd);
    fd = open("sharedfd", 0);
    if(fd < 0){
        printf(1, "fstests: cannot open sharedfd for reading\n");
        return;
    }
    nc = np = 0;
    while((n = read(fd, buf, sizeof(buf))) > 0){
        for(i = 0; i < sizeof(buf); i++){
            if(buf[i] == 'c')
                nc++;
            if(buf[i] == 'p')
                np++;
        }
    }
    close(fd);
    unlink("sharedfd");
    if(nc == 100 && np == 100){
        printf(1, "sharedfd ok\n");
    } else {
        printf(1, "sharedfd oops %d %d\n", nc, np);
        exit(1);
    }
}

// four processes write different files at the same
// time, to test block allocation.
void
fourfiles(void)
{
    int fd, pid, i, j, n, total, pi;
    char *names[] = { "f0", "f1", "f2", "f3" };
    char *fname;

    printf(1, "fourfiles test\n");

    for(pi = 0; pi < 4; pi++){
        fname = names[pi];
        unlink(fname);

        pid = fork();
        if(pid < 0){
            printf(1, "fork failed\n");
            exit(1);
        }

        if(pid == 0){
            fd = open(fname, O_CREATE | O_RDWR);
            if(fd < 0){
                printf(1, "create failed\n");
                exit(1);
            }

            memset(buf, '0'+pi, 512);
            for(i = 0; i < 12; i++){
                if((n = write(fd, buf, 20)) != 20){
                    printf(1, "write failed %d\n", n);
                    exit(1);
                }
            }
            exit(0);
        }
    }

    for(pi = 0; pi < 4; pi++){
        wait(0);
    }

    for(i = 0; i < 2; i++){
        fname = names[i];
        fd = open(fname, 0);
        total = 0;
        while((n = read(fd, buf, sizeof(buf))) > 0){
            for(j = 0; j < n; j++){
                if(buf[j] != '0'+i){
                    printf(1, "wrong char\n");
                    exit(1);
                }
            }
            total += n;
        }
        close(fd);
        if(total != 12*20){
            printf(1, "wrong length %d\n", total);
            exit(1);
        }
        unlink(fname);
    }

    printf(1, "fourfiles ok\n");
}

// simple file system tests

void
opentest(void)
{
    int fd;

    printf(stdout, "open test\n");
    fd = open("some_file", O_CREATE | O_RDWR);
    if(fd < 0){
        printf(stdout, "create some_file failed!\n");
        exit(1);
    }
    close(fd);
    fd = open("some_file", 0);
    if(fd < 0){
        printf(stdout, "open some_file failed!\n");
        exit(1);
    }
    close(fd);
    fd = open("doesnotexist", 0);
    if(fd >= 0){
        printf(stdout, "open doesnotexist succeeded!\n");
        exit(1);
    }
    printf(stdout, "open test ok\n");
}

void
writetest(void)
{
    int fd;
    int i;

    printf(stdout, "small file test\n");
    fd = open("small", O_CREATE|O_RDWR);
    if(fd >= 0){
        printf(stdout, "create small succeeded; ok\n");
    } else {
        printf(stdout, "error: create small failed!\n");
        exit(1);
    }
    for(i = 0; i < 50; i++){
        if(write(fd, "aaaaaaaaaa", 10) != 10){
            printf(stdout, "error: write aa %d new file failed\n", i);
            exit(1);
        }
        if(write(fd, "bbbbbbbbbb", 10) != 10){
            printf(stdout, "error: write bb %d new file failed\n", i);
            exit(1);
        }
    }
    printf(stdout, "writes ok\n");
    close(fd);
    fd = open("small", O_RDONLY);
    if(fd >= 0){
        printf(stdout, "open small succeeded ok\n");
    } else {
        printf(stdout, "error: open small failed!\n");
        exit(1);
    }
    i = read(fd, buf, 500);
    if(i == 500){
        printf(stdout, "read succeeded ok\n");
    } else {
        printf(stdout, "read failed\n");
        exit(1);
    }
    close(fd);

    if(unlink("small") < 0){
        printf(stdout, "unlink small failed\n");
        exit(1);
    }
    printf(stdout, "small file test ok\n");
}

void
writetest1(void)
{
    int i, fd, n;

    printf(stdout, "big files test\n");

    fd = open("big", O_CREATE|O_RDWR);
    if(fd < 0){
        printf(stdout, "error: creat big failed!\n");
        exit(1);
    }

    for(i = 0; i < OBJECTS_TABLE_SIZE; i++){
        ((int*)buf)[0] = i;
        if(write(fd, buf, 1) != 1){
            printf(stdout, "error: write big file failed\n", i);
            exit(1);
        }
    }

    close(fd);

    fd = open("big", O_RDONLY);
    if(fd < 0){
        printf(stdout, "error: open big failed!\n");
        exit(1);
    }

    n = 0;
    for(;;){
        i = read(fd, buf, 1);
        if(i == 0){
            if(n == OBJECTS_TABLE_SIZE - 1){
                printf(stdout, "read only %d blocks from big", n);
                exit(0);
            }
            break;
        } else if(i != 1){
            printf(stdout, "read failed %d\n", i);
            exit(1);
        }
        if(((int*)buf)[0] != n){
            printf(stdout, "read content of block %d is %d\n",
                   n, ((int*)buf)[0]);
            exit(0);
        }
        n++;
    }
    close(fd);
    if(unlink("big") < 0){
        printf(stdout, "unlink big failed\n");
        exit(1);
    }
    printf(stdout, "big files ok\n");
}

void
createtest(void)
{
    int i, fd;

    printf(stdout, "many creates, followed by unlink test\n");

    name[0] = 'a';
    name[2] = '\0';
    for(i = 0; i < 10; i++){
        name[1] = '0' + i;
        fd = open(name, O_CREATE|O_RDWR);
        close(fd);
    }
    name[0] = 'a';
    name[2] = '\0';
    for(i = 0; i < 10; i++){
        name[1] = '0' + i;
        unlink(name);
    }
    printf(stdout, "many creates, followed by unlink; ok\n");
}

void dirtest(void)
{
    printf(stdout, "mkdir test\n");

    if(mkdir("dir0") < 0){
        printf(stdout, "mkdir failed\n");
        exit(1);
    }

    if(chdir("dir0") < 0){
        printf(stdout, "chdir dir0 failed\n");
        exit(1);
    }

    if(chdir("..") < 0){
        printf(stdout, "chdir .. failed\n");
        exit(1);
    }

    if(unlink("dir0") < 0){
        printf(stdout, "unlink dir0 failed\n");
        exit(1);
    }
    printf(stdout, "mkdir test ok\n");
}

// does chdir() call iput(p->cwd) in a transaction?
void
iputtest(void)
{
    printf(stdout, "iput test\n");

    if(mkdir("iputdir") < 0){
        printf(stdout, "mkdir failed\n");
        exit(1);
    }
    if(chdir("iputdir") < 0){
        printf(stdout, "chdir iputdir failed\n");
        exit(1);
    }
    if(unlink("../iputdir") < 0){
        printf(stdout, "unlink ../iputdir failed\n");
        exit(1);
    }
    if(chdir("/new") < 0){
        printf(stdout, "chdir /new failed\n");
        exit(1);
    }
    printf(stdout, "iput test ok\n");
}

// simple fork and pipe read/write

void
pipe1(void)
{
    int fds[2], pid;
    int seq, i, n, cc, total;

    printf(stdout, "pipe1 test\n");

    if(pipe(fds) != 0){
        printf(stdout, "pipe1 failed test\n");

        printf(1, "pipe() failed\n");
        exit(1);
    }
    pid = fork();
    seq = 0;
    printf(stdout, "pipe1 after fork test\n");

    if(pid == 0){
        printf(stdout, "pipe1 pid == 0 test\n");

        close(fds[0]);
        for(n = 0; n < 5; n++){
            for(i = 0; i < 1033; i++)
                buf[i] = seq++;
            if(write(fds[1], buf, 1033) != 1033){
                printf(1, "pipe1 oops 1\n");
                exit(1);
            }
        }
        printf(1, "pipe1 oops 1.5\n");

        exit(0);
    } else if(pid > 0){
        printf(stdout, "pipe1 pid > 0 test\n");

        close(fds[1]);
        total = 0;
        cc = 1;
        while((n = read(fds[0], buf, cc)) > 0){
            for(i = 0; i < n; i++){
                if((buf[i] & 0xff) != (seq++ & 0xff)){
                    printf(1, "pipe1 oops 2\n");
                    return;
                }
            }
            total += n;
            cc = cc * 2;
            if(cc > sizeof(buf))
                cc = sizeof(buf);
        }
        if(total != 5 * 1033){
            printf(1, "pipe1 oops 3 total %d\n", total);
            exit(1);
        }
        close(fds[0]);
        wait(0);
    } else {
        printf(1, "fork() failed\n");
        exit(1);
    }
    printf(1, "pipe1 ok\n");
}

void
rmdot(void)
{
    printf(1, "rmdot test\n");
    if(mkdir("dots") != 0){
        printf(1, "mkdir dots failed\n");
        exit(1);
    }
    if(chdir("dots") != 0){
        printf(1, "chdir dots failed\n");
        exit(1);
    }
    if(unlink(".") == 0){
        printf(1, "rm . worked!\n");
        exit(1);
    }
    if(unlink("..") == 0){
        printf(1, "rm .. worked!\n");
        exit(1);
    }
    if(chdir("/new") != 0){
        printf(1, "chdir /new failed\n");
        exit(1);
    }
    if(unlink("dots/.") == 0){
        printf(1, "unlink dots/. worked!\n");
        exit(1);
    }
    if(unlink("dots/..") == 0){
        printf(1, "unlink dots/.. worked!\n");
        exit(1);
    }
    if(unlink("dots") != 0){
        printf(1, "unlink dots failed!\n");
        exit(1);
    }
    printf(1, "rmdot ok\n");
}

void
fourteen(void)
{
    int fd;

    // DIRSIZ is 14.
    printf(1, "fourteen test\n");

    if(mkdir("12345678901234") != 0){
        printf(1, "mkdir 12345678901234 failed\n");
        exit(1);
    }
    if(mkdir("12345678901234/123456789012345") != 0){
        printf(1, "mkdir 12345678901234/123456789012345 failed\n");
        exit(1);
    }
    fd = open("123456789012345/123456789012345/123456789012345", O_CREATE);
    if(fd < 0){
        printf(1, "create 123456789012345/123456789012345/123456789012345 failed\n");
        exit(1);
    }
    close(fd);
    fd = open("12345678901234/12345678901234/12345678901234", 0);
    if(fd < 0){
        printf(1, "open 12345678901234/12345678901234/12345678901234 failed\n");
        exit(1);
    }
    close(fd);

    if(mkdir("12345678901234/12345678901234") == 0){
        printf(1, "mkdir 12345678901234/12345678901234 succeeded!\n");
        exit(1);
    }
    if(mkdir("123456789012345/12345678901234") == 0){
        printf(1, "mkdir 12345678901234/123456789012345 succeeded!\n");
        exit(1);
    }

    printf(1, "fourteen ok\n");
}

void
bigfile(void)
{
    int fd, i, total, cc;
    int write_iterations = 20;
    int bytes_per_write = 100;

    printf(1, "bigfile test\n");

    unlink("bigfile");
    fd = open("bigfile", O_CREATE | O_RDWR);
    if(fd < 0){
        printf(1, "cannot create bigfile");
        exit(1);
    }
    //write 
    for(i = 0; i < write_iterations; i++){
        memset(buf, i, bytes_per_write);
        if(write(fd, buf, bytes_per_write) != bytes_per_write){
            printf(1, "write bigfile failed\n");
            exit(1);
        }
    }
    close(fd);

    fd = open("bigfile", 0);
    if(fd < 0){
        printf(1, "cannot open bigfile\n");
        exit(1);
    }
    total = 0;
    for(i = 0; ; i++){
        cc = read(fd, buf, bytes_per_write);
        if(cc < 0){
            printf(1, "read bigfile failed\n");
            exit(1);
        }
        if(cc == 0)
            break;
        if(cc != bytes_per_write){
            printf(1, "short read bigfile\n");
            exit(1);
        }
        if(buf[0] != i || buf[bytes_per_write-1] != i){
            printf(1, "read bigfile wrong data\n");
            exit(1);
        }
        total += cc;
    }
    close(fd);
    if(total != write_iterations * bytes_per_write){
        printf(1, "read bigfile wrong total\n");
        exit(1);
    }
    unlink("bigfile");

    printf(1, "bigfile test ok\n");
}


void
linktest(void)
{
    int fd;

    printf(1, "linktest\n");

    unlink("lf1");
    unlink("lf2");

    fd = open("lf1", O_CREATE|O_RDWR);
    if(fd < 0){
        printf(1, "create lf1 failed\n");
        exit(1);
    }
    if(write(fd, "hello", 5) != 5){
        printf(1, "write lf1 failed\n");
        exit(1);
    }
    close(fd);

    if(link("lf1", "lf2") < 0){
        printf(1, "link lf1 lf2 failed\n");
        exit(1);
    }
    unlink("lf1");

    if(open("lf1", 0) >= 0){
        printf(1, "unlinked lf1 but it is still there!\n");
        exit(1);
    }

    fd = open("lf2", 0);
    if(fd < 0){
        printf(1, "open lf2 failed\n");
        exit(1);
    }
    if(read(fd, buf, sizeof(buf)) != 5){
        printf(1, "read lf2 failed\n");
        exit(1);
    }
    close(fd);

    if(link("lf2", "lf2") >= 0){
        printf(1, "link lf2 lf2 succeeded! oops\n");
        exit(1);
    }

    unlink("lf2");
    if(link("lf2", "lf1") >= 0){
        printf(1, "link non-existant succeeded! oops\n");
        exit(1);
    }

    if(link(".", "lf1") >= 0){
        printf(1, "link . lf1 succeeded! oops\n");
        exit(1);
    }

    printf(1, "linktest ok\n");
}

void
subdir(void)
{
    int fd, cc;

    printf(1, "subdir test\n");

    unlink("ff");
    if(mkdir("dd") != 0){
        printf(1, "subdir mkdir dd failed\n");
        exit(1);
    }

    fd = open("dd/ff", O_CREATE | O_RDWR);
    if(fd < 0){
        printf(1, "create dd/ff failed\n");
        exit(1);
    }
    write(fd, "ff", 2);
    close(fd);

    if(unlink("dd") >= 0){
        printf(1, "unlink dd (non-empty dir) succeeded!\n");
        exit(1);
    }

    if(mkdir("/new/dd/dd") != 0){
        printf(1, "subdir mkdir dd/dd failed\n");
        exit(1);
    }

    fd = open("dd/dd/ff", O_CREATE | O_RDWR);
    if(fd < 0){
        printf(1, "create dd/dd/ff failed\n");
        exit(1);
    }
    write(fd, "FF", 2);
    close(fd);

    fd = open("dd/dd/../ff", 0);
    if(fd < 0){
        printf(1, "open dd/dd/../ff failed\n");
        exit(1);
    }
    cc = read(fd, buf, sizeof(buf));
    if(cc != 2 || buf[0] != 'f'){
        printf(1, "dd/dd/../ff wrong content\n");
        exit(1);
    }
    close(fd);

    if(link("dd/dd/ff", "dd/dd/ffff") != 0){
        printf(1, "link dd/dd/ff dd/dd/ffff failed\n");
        exit(1);
    }

    if(unlink("dd/dd/ff") != 0){
        printf(1, "unlink dd/dd/ff failed\n");
        exit(1);
    }
    if(open("dd/dd/ff", O_RDONLY) >= 0){
        printf(1, "open (unlinked) dd/dd/ff succeeded\n");
        exit(1);
    }

    if(chdir("dd") != 0){
        printf(1, "chdir dd failed\n");
        exit(1);
    }
    if(chdir("dd/../../dd") != 0){
        printf(1, "chdir dd/../../dd failed\n");
        exit(1);
    }
    if(chdir("./..") != 0){
        printf(1, "chdir ./.. failed\n");
        exit(1);
    }

    fd = open("dd/dd/ffff", 0);
    if(fd < 0){
        printf(1, "open dd/dd/ffff failed\n");
        exit(1);
    }
    if(read(fd, buf, sizeof(buf)) != 2){
        printf(1, "read dd/dd/ffff wrong len\n");
        exit(1);
    }
    close(fd);

    if(open("dd/dd/ff", O_RDONLY) >= 0){
        printf(1, "open (unlinked) dd/dd/ff succeeded!\n");
        exit(1);
    }

    if(open("dd/ff/ff", O_CREATE|O_RDWR) >= 0){
        printf(1, "create dd/ff/ff succeeded!\n");
        exit(1);
    }
    if(open("dd/xx/ff", O_CREATE|O_RDWR) >= 0){
        printf(1, "create dd/xx/ff succeeded!\n");
        exit(1);
    }
    if(open("dd", O_CREATE) >= 0){
        printf(1, "create dd succeeded!\n");
        exit(1);
    }
    if(open("dd", O_RDWR) >= 0){
        printf(1, "open dd rdwr succeeded!\n");
        exit(1);
    }
    if(open("dd", O_WRONLY) >= 0){
        printf(1, "open dd wronly succeeded!\n");
        exit(1);
    }
    if(link("dd/ff/ff", "dd/dd/xx") == 0){
        printf(1, "link dd/ff/ff dd/dd/xx succeeded!\n");
        exit(1);
    }
    if(link("dd/xx/ff", "dd/dd/xx") == 0){
        printf(1, "link dd/xx/ff dd/dd/xx succeeded!\n");
        exit(1);
    }
    if(link("dd/ff", "dd/dd/ffff") == 0){
        printf(1, "link dd/ff dd/dd/ffff succeeded!\n");
        exit(1);
    }
    if(mkdir("dd/ff/ff") == 0){
        printf(1, "mkdir dd/ff/ff succeeded!\n");
        exit(1);
    }
    if(mkdir("dd/xx/ff") == 0){
        printf(1, "mkdir dd/xx/ff succeeded!\n");
        exit(1);
    }
    if(mkdir("dd/dd/ffff") == 0){
        printf(1, "mkdir dd/dd/ffff succeeded!\n");
        exit(1);
    }
    if(unlink("dd/xx/ff") == 0){
        printf(1, "unlink dd/xx/ff succeeded!\n");
        exit(1);
    }
    if(unlink("dd/ff/ff") == 0){
        printf(1, "unlink dd/ff/ff succeeded!\n");
        exit(1);
    }
    if(chdir("dd/ff") == 0){
        printf(1, "chdir dd/ff succeeded!\n");
        exit(1);
    }
    if(chdir("dd/xx") == 0){
        printf(1, "chdir dd/xx succeeded!\n");
        exit(1);
    }

    if(unlink("dd/dd/ffff") != 0){
        printf(1, "unlink dd/dd/ff failed\n");
        exit(1);
    }
    if(unlink("dd/ff") != 0){
        printf(1, "unlink dd/ff failed\n");
        exit(1);
    }
    if(unlink("dd") == 0){
        printf(1, "unlink non-empty dd succeeded!\n");
        exit(1);
    }
    if(unlink("dd/dd") < 0){
        printf(1, "unlink dd/dd failed\n");
        exit(1);
    }
    if(unlink("dd") < 0){
        printf(1, "unlink dd failed\n");
        exit(1);
    }

    printf(1, "subdir ok\n");
}

void
dirfile(void)
{
    int fd;

    printf(1, "dir vs file\n");

    fd = open("dirfile", O_CREATE);
    if(fd < 0){
        printf(1, "create dirfile failed\n");
        exit(1);
    }
    close(fd);
    if(chdir("dirfile") == 0){
        printf(1, "chdir dirfile succeeded!\n");
        exit(1);
    }
    fd = open("dirfile/xx", 0);
    if(fd >= 0){
        printf(1, "create dirfile/xx succeeded!\n");
        exit(1);
    }
    fd = open("dirfile/xx", O_CREATE);
    if(fd >= 0){
        printf(1, "create dirfile/xx succeeded!\n");
        exit(1);
    }
    if(mkdir("dirfile/xx") == 0){
        printf(1, "mkdir dirfile/xx succeeded!\n");
        exit(1);
    }
    if(unlink("dirfile/xx") == 0){
        printf(1, "unlink dirfile/xx succeeded!\n");
        exit(1);
    }
    if(link("README", "dirfile/xx") == 0){
        printf(1, "link to dirfile/xx succeeded!\n");
        exit(1);
    }
    if(unlink("dirfile") != 0){
        printf(1, "unlink dirfile failed!\n");
        exit(1);
    }

    fd = open(".", O_RDWR);
    if(fd >= 0){
        printf(1, "open . for writing succeeded!\n");
        exit(1);
    }
    fd = open(".", 0);
    if(write(fd, "x", 1) > 0){
        printf(1, "write . succeeded!\n");
        exit(1);
    }
    close(fd);

    printf(1, "dir vs file OK\n");
}

void
exectest(void)
{
    printf(stdout, "exec test\n");
    if(exec("/echo", echoargv) < 0){
        printf(stdout, "exec echo failed\n");
        exit(1);
    }
}

// test that iput() is called at the end of _namei()
void
iref(void)
{
    int i, fd;

    printf(1, "empty file name\n");

    for(i = 0; i < 20; i++){
        if(mkdir("irefd") != 0){
            printf(1, "mkdir irefd failed\n");
            exit(1);
        }
        if(chdir("irefd") != 0){
            printf(1, "chdir irefd failed\n");
            exit(1);
        }

        mkdir("");
        link("README", "");
        fd = open("", O_CREATE);
        if(fd >= 0)
            close(fd);
        fd = open("xx", O_CREATE);
        if(fd >= 0)
            close(fd);
        unlink("xx");
    }

    chdir("/new");
    printf(1, "empty file name OK\n");
}
int
main(int argc, char *argv[])
{
    printf(1, "objfstests starting\n");
    printmounts();
    printdevices();

    if (mkdir("new") != 0) {
        printf(2, "objtest: failed to create dir new\n");
    }

    if (mount(0, "new", "objfs") != 0) {
        printf(2, "objtest: failed to mount objfs to new\n");
        exit(0);
    }

    if(chdir("/new") < 0) {
        printf(2, "ls: cannot cd new\n");
        exit(0);
    }

    createdelete();
    linkunlink();
    concreate();
    fourfiles();
    sharedfd();
    opentest();
    writetest();
    writetest1();
    createtest();
    iputtest();
    pipe1();
    rmdot();
    fourteen();
    bigfile();
    subdir();
    linktest();
    unlinkread();
    dirfile();
    iref();
    

    exectest(); // Ensure this test to be the last one to run (prints ALL TESTS PASSED)

    exit(0);
}
