#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

char *fmtname(char *path)
{
    static char buf[DIRSIZ + 1];
    char *p;

    // Find first character after the last slash.
    for (p = path + strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;

    // Return blank-padded name.
    if (strlen(p) >= DIRSIZ)
        return p;
    memmove(buf, p, strlen(p));
    memset(buf + strlen(p), 0, DIRSIZ - strlen(p));
    return buf;
}

// Recursive find function to search for files and directories
void find(char *filename, char *path, char type, int inum, int printi, int includeCurrent)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

 // Open the current directory
    if ((fd = open(path, 0)) < 0)
    {
        printf(2, "find: cannot open %s\n", path);
        return;
    }

// Get information about the current directory
    if (fstat(fd, &st) < 0)
    {
        printf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    // Check if the filename matches and print accordingly
    if ((type == 'f' && st.type == T_FILE && strcmp(fmtname(path), filename) == 0) ||
        (type == 'd' && st.type == T_DIR && strcmp(fmtname(path), filename) == 0) ||
        (type == 'a' && strcmp(fmtname(path), filename) == 0) ||
        (includeCurrent && strcmp(path, ".") == 0))
    {
        if (printi)
            printf(1, "%d ", st.ino);
        printf(1, "%s\n", path);
    }

// If it's a directory, continue searching recursively
    if (st.type == T_DIR)
    {
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
        {
            printf(1, "find: path too long\n");
            close(fd);
            return;
        }
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';
        while (read(fd, &de, sizeof(de)) == sizeof(de))
        {
            if (de.inum == 0)
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if (stat(buf, &st) < 0)
            {
                printf(1, "find: cannot stat %s\n", buf);
                continue;
            }
            if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                continue;

            // Check if the filename matches and print accordingly
            if ((type == 'f' && st.type == T_FILE && strcmp(fmtname(buf), filename) == 0) ||
                (type == 'd' && st.type == T_DIR && strcmp(fmtname(buf), filename) == 0) ||
                (type == 'a' && strcmp(fmtname(buf), filename) == 0) ||
                (includeCurrent && strcmp(buf, ".") == 0))
            {
                if (printi)
                    printf(1, "%d ", st.ino);
                printf(1, "%s\n", buf);
            }

            // Recursively call find only for directories and if they haven't been visited before
            if (st.type == T_DIR)
                find(filename, buf, type, inum, printi, includeCurrent);
        }
    }
    close(fd);
}

int main(int argc, char *argv[])
{
    if (argc < 4 || strcmp(argv[2], "-name") != 0)
    {
        printf(2, "Usage: %s <directory> -name <filename> [-type [f|d|a]] [-inum <inode_number>] [-printi] [-find]\n", argv[0]);
        exit();
    }

    char type = 'a'; // Default is 'a' (all types)
    int inum = -1;   // Default is -1 (no specific inode)
    int printi = 0;  // Default is 0 (do not print inode)
    int includeCurrent = 0; // Default is 0 (do not include current directory)

    // Parse optional flags
    for (int i = 4; i < argc; i += 2)
    {
        if (strcmp(argv[i], "-type") == 0) 
        {
            if (i + 1 < argc)
                type = argv[i + 1][0]; // Set the type flag based on the provided argument
        }
        else if (strcmp(argv[i], "-inum") == 0)
        {
            if (i + 1 < argc)
                inum = atoi(argv[i + 1]); // Set the inode number flag based on the provided argument
        }
        else if (strcmp(argv[i], "-printi") == 0)
        {
            printi = 1; // Set the print inode flag if -printi is present
        }
        else if (strcmp(argv[i], "-find") == 0)
        {
            includeCurrent = 1; //Set the include current directory flag if -find is present
        }
    }
 // Call the find function with the specified parameters
    find(argv[3], argv[1], type, inum, printi, includeCurrent);
    exit();
}
