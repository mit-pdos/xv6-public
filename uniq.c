#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

// Custom function to compare first N characters of two strings
int strncmp_custom(const char *s1, const char *s2, int n) {
    for (int i = 0; i < n; i++) {
        if (s1[i] != s2[i] || s1[i] == '\0' || s2[i] == '\0') {
            return (unsigned char)s1[i] - (unsigned char)s2[i];
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    int fd = 0; // Default to standard input
    int countFlag = 0, uniqueFlag = 0, numChars = -1;
    char *filename = 0;

    // Parse command line arguments
    for(int i = 1; i < argc; i++) {
        if(argv[i][0] == '-') {
            switch(argv[i][1]) {
                case 'c':
                    countFlag = 1;
                    break;
                case 'u':
                    uniqueFlag = 1;
                    break;
                case 'w': // Handle -w flag
                    if (i + 1 < argc) {
                        numChars = atoi(argv[++i]);
                    } else {
                        printf(1, "uniq: option requires an argument -- 'w'\n");
                        exit();
                    }
                    break;
                default:
                    printf(1, "uniq: invalid option -- '%c'\n", argv[i][1]);
                    exit();
            }
        } else if (!filename) {
            filename = argv[i];
        } else {
            printf(1, "uniq: extra operand '%s'\n", argv[i]);
            exit();
        }
    }

    // If a filename is provided, try to open it
    if(filename && (fd = open(filename, O_RDONLY)) < 0){
        printf(1, "uniq: cannot open %s\n", filename);
        exit();
    }

    printf(1, "Uniq command is getting executed in user mode.\n");

    char prevLine[512] = "", currLine[512];
    int n, i = 0, count = 0;
    while((n = read(fd, currLine+i, 1)) > 0 || i > 0) {
        if(n == 0 || currLine[i] == '\n') {
            currLine[i] = 0; // Terminate the string
            if(i > 0) {
                int cmp;
                if (numChars > 0 && numChars <= i) {
                    cmp = strncmp_custom(prevLine, currLine, numChars);
                } else {
                    cmp = strcmp(prevLine, currLine);
                }

                if(cmp != 0) {
                    if(uniqueFlag && count == 1) {
                        if(countFlag)
                            printf(1, "1 %s\n", prevLine);
                        else
                            printf(1, "%s\n", prevLine);
                    } else if (!uniqueFlag) {
                        if(countFlag)
                            printf(1, "%d %s\n", count, prevLine);
                        else
                            printf(1, "%s\n", prevLine);
                    }
                    strcpy(prevLine, currLine);
                    count = 1;
                } else {
                    count++;
                }
            }
            i = 0;
            if(n == 0)
                break;
        } else {
            i++;
        }
    }

    // Handle the last line
    if((uniqueFlag && count == 1) || (!uniqueFlag)) {
        if(countFlag)
            printf(1, "%d %s\n", count, prevLine);
        else
            printf(1, "%s\n", prevLine);
    }

    if(fd != 0)
        close(fd);
    exit();
}
