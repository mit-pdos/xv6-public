#ifndef CGROUPS_TEST_FUNCTIONS_H
#define CGROUPS_TEST_FUNCTIONS_H

int is_valid_controller_type(int controller_type);

char *get_controller_name(int controller_type);

void set_suppress(char _suppress);

int test_open(const char* file);

int test_close(int fd);

int test_open_close(const char* file);

char* test_read(const char* file, int print);

int test_write(const char* file, char* text);

int test_enable_controller(int type);

int test_disable_controller(int type);

int test_verify_controller_enabled(int type);

int test_verify_controller_disabled(int type);

int test_move_proc(const char* file, int pid);

int test_pid_in_group(const char* file, int pid);

int test_temp_write(int num);

int test_temp_read(int print);

int test_temp_delete();

#define MAX_CONTROLLER_NAME_LENGTH      16
#define CONTROLLER_COUNT                4

enum controller_types {CPU_CNT, PID_CNT, SET_CNT, MEM_CNT};

#endif
