#include "fcntl.h"
#include "types.h"
#include "user.h"
#include "cgroupstests.h"
#include "cgroups_test_functions.h"

char controller_names[CONTROLLER_COUNT][MAX_CONTROLLER_NAME_LENGTH] =
    {"cpu", "pid"};

char suppress = 0;

int is_valid_controller_type(int controller_type) {
    return controller_type >= CPU_CNT &&
        controller_type <= CPU_CNT + CONTROLLER_COUNT - 1;
}

char *get_controller_name(int controller_type) {
    if (!is_valid_controller_type(controller_type)) {
        return 0;
    }

    return controller_names[controller_type];
}

void set_suppress(char _suppress) {
    suppress = _suppress;
}

// Set the given string length to empty.
static void empty_string(char * string, int length)
{
    for (int i = 0; i < length; i++)
        string[i] = 0;
}

int test_open(const char* file)
{
    int fd;
    if ((fd = open(file, O_RDWR)) < 1) {
        if (suppress == 0)
            printf(1, "\nFailed to open file: %s\n", file);
        return 0;
    }

    return fd;
}

int test_close(int fd)
{
    if (close(fd) != 0) {
        if (suppress == 0)
            printf(1, "\nFailed to close file\n");
        return 0;
    }

    return 1;
}

int test_open_close(const char* file) {
    int fd = test_open(file);
    if (!fd) {
        return 0;
    }
    return test_close(fd);
}

// Reading from a file. if fails, return 0. Upon success returns the contents of the file.
// If print is set to 1, also prints the returned string.
char* test_read(const char* file, int print)
{
    static char buf[256];
    empty_string(buf, 256);

    int fd = test_open(file);
    if (!fd) {
        return 0;
    }

    if (read(fd, buf, 256) < 0) {
        if (suppress == 0)
            printf(1, "\nFailed to read file: %s\n", file);
        return 0;
    }

    if (print) {
        printf(1, "Contents of %s: \n%s\n", file, buf);
    }

    if (!test_close(fd)) {
        return 0;
    }

    return buf;
}

int test_write(const char* file, char* text) {
    int fd = test_open(file);
    char buf[256];

    empty_string(buf, 256);
    strcpy(buf, text);

    if (write(fd, buf, sizeof(buf)) < 0) {
        if (suppress == 0)
            printf(1, "\nFailed to write into file %s\n", file);
        return 0;
    }

    return test_close(fd);
}

// Test enabling controller according to given type.
int test_enable_controller(int type) {
    char buf[5] = {'+', 0};
    if (!is_valid_controller_type(type)) {
        return 0;
    }

    strcpy(buf + 1, controller_names[type]);

    return test_write(TEST_1_CGROUP_SUBTREE_CONTROL, buf);
}

// Test disabling controller according to given type.
int test_disable_controller(int type) {
    char buf[5] = {'-', 0};
    if (!is_valid_controller_type(type)) {
        return 0;
    }

    strcpy(buf + 1, controller_names[type]);

    return test_write(TEST_1_CGROUP_SUBTREE_CONTROL, buf);
}

// Test verrifying a controller is active according to given type.
int test_verify_controller_enabled(int type) {
    char buf[4] = {0};
    if (!is_valid_controller_type(type)) {
        return 0;
    }

    strcpy(buf, controller_names[type]);

    char* contents = test_read(TEST_1_CGROUP_SUBTREE_CONTROL, 0);

    int i;
    for (i = 0; contents[i] != 0 && i < sizeof(contents)-2; i++) {
        if (contents[i] == buf[0] && contents[i + 1] == buf[1] && contents[i + 2] == buf[2]) {
            return 1;
        }
    }

    return 0;
}

// Test verifying a controller is disabled according to given type.
int test_verify_controller_disabled(int type) {
    char buf[4] = {0};
    if (!is_valid_controller_type(type)) {
        return 0;
    }

    strcpy(buf, controller_names[type]);

    char* contents = test_read(TEST_1_CGROUP_SUBTREE_CONTROL, 0);
    int i;

    for (i = 0; contents[i+2] != 0; i++) {
        if (contents[i] == buf[0] && contents[i + 1] == buf[1] && contents[i + 2] == buf[2]) {
            printf(1, "\nController %s is still enabled\n", buf);
            return 0;
        }
    }

    return 1;
}

// Test moving a process to given cgroup.
int test_move_proc(const char* file, int pid) {
    char pid_buf[3];
    empty_string(pid_buf, 3);
    itoa(pid_buf, pid);
    return test_write(file, pid_buf);
}

// Test a given pid in string format belongs to a given cgroup.
int test_pid_in_group(const char* file, int pid) {
    char* contents = test_read(file, 0);
    char pid_buf[3];

    empty_string(pid_buf, 3);
    itoa(pid_buf, pid);

    if (pid_buf[1] == 0)
        pid_buf[1] = '\n';

    int i;
    for (i = 0; contents[i+1] != 0; i++) {
        if (pid_buf[0] == contents[i] && pid_buf[1] == contents[i + 1]) {
            return 1;
        }
    }

    if (suppress == 0) {
        printf(1, "\nFailed to find pid %d in group %s\n", atoi(pid_buf), file);
    }

    return 0;
}

// Write an integer into a temporary file. Notice only one such file currently supported.
int test_temp_write(int num) {
    int fd;
    if ((fd = open(TEMP_FILE, O_CREATE | O_RDWR)) < 1) {
        if (suppress == 0)
            printf(1, "\nFailed to open a temporary file\n");
        return 0;
    }

    char buf[256];
    itoa(buf, num);

    if (!test_write(TEMP_FILE, buf)) {
        return 0;
    }

    return test_close(fd);
}

// Read an integer from the temporary file.
int test_temp_read(int print) {
    return atoi(test_read(TEMP_FILE, print));
}

// Delete the temporary file.
int test_temp_delete() {
    if (unlink(TEMP_FILE)) {
        printf(1, "\nFailed to delete temporary file\n");
        return 0;
    }

    return 1;
}
