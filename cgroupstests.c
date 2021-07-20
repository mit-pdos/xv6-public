#include "fcntl.h"
#include "types.h"
#include "user.h"
#include "test.h"
#include "param.h"
#include "cgroupstests.h"

char controller_names[CONTROLLER_COUNT][MAX_CONTROLLER_NAME_LENGTH] =
  { "cpu", "pid", "set", "mem" };

char suppress = 0;

int failed = 0;

// Return if controller type is valid.
int is_valid_controller_type(int controller_type) {
  return controller_type >= CPU_CNT &&
    controller_type <= CPU_CNT + CONTROLLER_COUNT - 1;
}

// Return controller name.
char* get_controller_name(int controller_type) {
  if (!is_valid_controller_type(controller_type)) {
    return 0;
  }

  return controller_names[controller_type];
}

// Turn on debug messeges or turn them off.
void set_suppress(char _suppress) {
  suppress = _suppress;
}

// Set the given string length to empty.
static void empty_string(char* string, int length)
{
     memset(string, 0, length);
}

// Open given file.
int open_file(const char* file)
{
  int fd;
  if ((fd = open(file, O_RDWR)) < 1) {
    if (suppress == 0)
      printf(1, "\nFailed to open file: %s\n", file);
    return 0;
  }

  return fd;
}

// Close given file.
int close_file(int fd)
{
  if (close(fd) != 0) {
    if (suppress == 0)
      printf(1, "\nFailed to close file\n");
    return 0;
  }

  return 1;
}

// Open and close given file.
int open_close_file(const char* file) {
  int fd = open_file(file);
  if (!fd) {
    return 0;
  }
  return close_file(fd);
}

// Read from a given file. if fails, return 0. Upon success returns the contents of the file.
// If print is set to 1, also prints the returned string.
char* read_file(const char* file, int print)
{
  static char buf[256];
  empty_string(buf, 256);

  int fd = open_file(file);
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

  if (!close_file(fd)) {
    return 0;
  }

  return buf;
}

// Write into a file. If succesful returns 1, otherwise 0.
int write_file(const char* file, char* text) {
  int fd = open_file(file);
  char buf[256];

  empty_string(buf, 256);
  strcpy(buf, text);

  if (write(fd, buf, sizeof(buf)) < 0) {
    if (suppress == 0)
      printf(1, "\nFailed to write into file %s\n", file);
    return 0;
  }

  return close_file(fd);
}

int write_new_file(const char* file, char* text) {
   int fd;
   if ((fd = open(file, O_CREATE | O_RDWR)) < 1) {
     if (suppress == 0)
        printf(1, "\nFailed to open a new file \n");
     return 0;
   }

   if (!write_file(file, text)) {
     return 0;
   }

  return fd;
}

// Test enabling controller according to given type.
int enable_controller(int type) {
  char buf[5] = { '+', 0 };
  if (!is_valid_controller_type(type)) {
    return 0;
  }

  strcpy(buf + 1, controller_names[type]);

  return write_file(TEST_1_CGROUP_SUBTREE_CONTROL, buf);
}

// Test disabling controller according to given type.
int disable_controller(int type) {
  char buf[5] = { '-', 0 };
  if (!is_valid_controller_type(type)) {
    return 0;
  }

  strcpy(buf + 1, controller_names[type]);

  return write_file(TEST_1_CGROUP_SUBTREE_CONTROL, buf);
}

// Test verrifying a controller is active according to given type.
int verify_controller_enabled(int type) {
  char buf[4] = { 0 };
  if (!is_valid_controller_type(type)) {
    return 0;
  }

  strcpy(buf, controller_names[type]);

  char* contents = read_file(TEST_1_CGROUP_SUBTREE_CONTROL, 0);

  int i;
  for (i = 0; contents[i] != 0 && i < sizeof(contents) - 2; i++) {
    if (contents[i] == buf[0] && contents[i + 1] == buf[1] && contents[i + 2] == buf[2]) {
      return 1;
    }
  }

  return 0;
}

// Test verifying a controller is disabled according to given type.
int verify_controller_disabled(int type) {
  char buf[4] = { 0 };
  if (!is_valid_controller_type(type)) {
    return 0;
  }

  strcpy(buf, controller_names[type]);

  char* contents = read_file(TEST_1_CGROUP_SUBTREE_CONTROL, 0);
  int i;

  for (i = 0; contents[i + 2] != 0; i++) {
    if (contents[i] == buf[0] && contents[i + 1] == buf[1] && contents[i + 2] == buf[2]) {
      printf(1, "\nController %s is still enabled\n", buf);
      return 0;
    }
  }

  return 1;
}

// Test moving a process to given cgroup.
int move_proc(const char* file, int pid) {
  char pid_buf[3];
  empty_string(pid_buf, 3);
  itoa(pid_buf, pid);
  return write_file(file, pid_buf);
}

// Test a given pid in string format belongs to a given cgroup.
int is_pid_in_group(const char* file, int pid) {
  char* contents = read_file(file, 0);
  char pid_buf[3];

  empty_string(pid_buf, 3);
  itoa(pid_buf, pid);

  if (pid_buf[1] == 0)
    pid_buf[1] = '\n';

  int i;
  for (i = 0; contents[i + 1] != 0; i++) {
    if (pid_buf[0] == contents[i] && pid_buf[1] == contents[i + 1]) {
      return 1;
    }
  }

  if (suppress == 0) {
    printf(1, "Failed to find pid %d in group %s\n", atoi(pid_buf), file);
  }

  return 0;
}

// Write an integer into a temporary file. Notice only one such file currently supported.
int temp_write(int num) {
  int fd;
  if ((fd = open(TEMP_FILE, O_CREATE | O_RDWR)) < 1) {
    if (suppress == 0)
      printf(1, "\nFailed to open a temporary file\n");
    return 0;
  }

  char buf[256];
  itoa(buf, num);

  if (!write_file(TEMP_FILE, buf)) {
    return 0;
  }

  return close_file(fd);
}

// Read an integer from the temporary file.
int temp_read(int print) {
  return atoi(read_file(TEMP_FILE, print));
}

// Delete the temporary file.
int temp_delete() {
  if (unlink(TEMP_FILE)) {
    printf(1, "Failed to delete temporary file\n");
    return 0;
  }

  return 1;
}

//return the value for a given entry from the bufer
//entry mast contains all characters before the value include white-spase
int get_val(char *buf, char *entry){

  do{
     if(strncmp(buf, entry, strlen(entry))==0){
       buf+=strlen(entry);
       return atoi(buf);
       }
     else
       while (*buf++!='\n')//go to next line
            ;
      }while(*buf!='\0');
  return -1;//Assuming all values are supposed to be non-negative
}

// Write into buffer the sequence of activating, disabling then activating a given controller.
// Returns the buffer written.
char* build_activate_disable_activate(int controller_type)
{
  if (!is_valid_controller_type(controller_type)) {
    return 0;
  }

  int max_size = (MAX_CONTROLLER_NAME_LENGTH + 2) * 3;
  char* buf = malloc(max_size);
  memset(buf, 0, max_size);

  // "+controller "
  strcat(buf, "+");
  strcat(buf, get_controller_name(controller_type));
  strcat(buf, " ");

  // "-controller "
  strcat(buf, "-");
  strcat(buf, get_controller_name(controller_type));
  strcat(buf, " ");

  // "+controller"
  strcat(buf, "+");
  strcat(buf, get_controller_name(controller_type));

  return buf;
}

// Write into buffer the sequence of activating then disabling a given controller.
// Returns the buffer written.
char* build_activate_disable(int controller_type)
{
  if (!is_valid_controller_type(controller_type)) {
    return 0;
  }

  int max_size = (MAX_CONTROLLER_NAME_LENGTH + 2) * 2;
  char* buf = malloc(max_size);
  memset(buf, 0, max_size);

  // "+controller "
  strcat(buf, "+");
  strcat(buf, get_controller_name(controller_type));
  strcat(buf, " ");

  // "-controller"
  strcat(buf, "-");
  strcat(buf, get_controller_name(controller_type));

  return buf;
}

TEST(test_mount_cgroup_fs)
{
    ASSERT_FALSE(mkdir(ROOT_CGROUP));
    ASSERT_FALSE(mount(0, ROOT_CGROUP, CGROUP));
}

TEST(test_umount_cgroup_fs)
{
    ASSERT_FALSE(umount(ROOT_CGROUP));
    ASSERT_FALSE(unlink(ROOT_CGROUP));
}

TEST(test_creating_cgroups)
{
    ASSERT_FALSE(mkdir(TEST_1));
    ASSERT_FALSE(mkdir(TEST_2));
    ASSERT_FALSE(mkdir(TEST_1_1));
    ASSERT_FALSE(mkdir(TEST_1_2));
}

TEST(test_deleting_cgroups)
{
    ASSERT_FALSE(unlink(TEST_1_2));
    ASSERT_FALSE(unlink(TEST_1_1));
    ASSERT_FALSE(unlink(TEST_2));
    ASSERT_FALSE(unlink(TEST_1));
}

TEST(test_opening_and_closing_cgroup_files)
{
    ASSERT_TRUE(open_close_file(TEST_1_CGROUP_PROCS));
    ASSERT_TRUE(open_close_file(TEST_1_CGROUP_CONTROLLERS));
    ASSERT_TRUE(open_close_file(TEST_1_CGROUP_SUBTREE_CONTROL));
    ASSERT_TRUE(open_close_file(TEST_1_CGROUP_EVENTS));
    ASSERT_TRUE(open_close_file(TEST_1_CGROUP_DESCENDANTS));
    ASSERT_TRUE(open_close_file(TEST_1_CGROUP_MAX_DEPTH));
    ASSERT_TRUE(open_close_file(TEST_1_CGROUP_STAT));
    ASSERT_TRUE(open_close_file(TEST_1_CPU_MAX));
    ASSERT_TRUE(open_close_file(TEST_1_CPU_WEIGHT));
    ASSERT_TRUE(open_close_file(TEST_1_CPU_STAT));
    ASSERT_TRUE(open_close_file(TEST_1_PID_MAX));
    ASSERT_TRUE(open_close_file(TEST_1_PID_CURRENT));
    ASSERT_TRUE(open_close_file(TEST_1_SET_CPU));
    ASSERT_TRUE(open_close_file(TEST_1_SET_FRZ));
    ASSERT_TRUE(open_close_file(TEST_1_MEM_CURRENT));
    ASSERT_TRUE(open_close_file(TEST_1_MEM_MAX));
    ASSERT_TRUE(open_close_file(TEST_1_MEM_STAT));
}

TEST(test_reading_cgroup_files)
{
    ASSERT_TRUE(read_file(TEST_1_CGROUP_PROCS, 1));
    ASSERT_TRUE(read_file(TEST_1_CGROUP_CONTROLLERS, 1));
    ASSERT_TRUE(read_file(TEST_1_CGROUP_SUBTREE_CONTROL, 1));
    ASSERT_TRUE(read_file(TEST_1_CGROUP_EVENTS, 1));
    ASSERT_TRUE(read_file(TEST_1_CGROUP_DESCENDANTS, 1));
    ASSERT_TRUE(read_file(TEST_1_CGROUP_MAX_DEPTH, 1));
    ASSERT_TRUE(read_file(TEST_1_CGROUP_STAT, 1));
    ASSERT_TRUE(read_file(TEST_1_CPU_MAX, 1));
    ASSERT_TRUE(read_file(TEST_1_CPU_WEIGHT, 1));
    ASSERT_TRUE(read_file(TEST_1_CPU_STAT, 1));
    ASSERT_TRUE(read_file(TEST_1_PID_MAX, 1));
    ASSERT_TRUE(read_file(TEST_1_PID_CURRENT, 1));
    ASSERT_TRUE(read_file(TEST_1_SET_CPU, 1));
    ASSERT_TRUE(read_file(TEST_1_SET_FRZ, 1));
    ASSERT_TRUE(read_file(TEST_1_MEM_CURRENT, 1));
    ASSERT_TRUE(read_file(TEST_1_MEM_MAX, 1));
    ASSERT_TRUE(read_file(TEST_1_MEM_STAT, 1));
}

int test_enable_and_disable_controller(int controller_type)
{
    char *buf;
    int result = 1;

    // Enable given controller.
    buf = build_activate_disable_activate(controller_type);
    if (!buf) {
        return 0;
    }
    result &= write_file(TEST_1_CGROUP_SUBTREE_CONTROL, buf);
    free(buf);


    // Check that the given controller is really enabled.
    result &= verify_controller_enabled(controller_type);

    // Disable the given controller.
    buf = build_activate_disable(controller_type);
    if (!buf) {
        return 0;
    }
    result &= write_file(TEST_1_CGROUP_SUBTREE_CONTROL, buf);
    free(buf);

    // Check that the given controller is really disabled.
    result &= verify_controller_disabled(controller_type);

    return result;
}

TEST(test_enable_and_disable_all_controllers)
{
    for (int i = CPU_CNT; i < CPU_CNT + CONTROLLER_COUNT; i++) {
        ASSERT_TRUE(test_enable_and_disable_controller(i));
    }
}

TEST(test_limiting_cpu_max_and_period)
{
    // Enable cpu controller
    ASSERT_TRUE(enable_controller(CPU_CNT));

    // Update only max
    ASSERT_TRUE(write_file(TEST_1_CPU_MAX, "5000"));

    // Check changes
    ASSERT_FALSE(strcmp(read_file(TEST_1_CPU_MAX, 0), "max - 5000\nperiod - 100000\n"));

    // Update max & period
    ASSERT_TRUE(write_file(TEST_1_CPU_MAX, "1000,20000"));

    // Check changes
    ASSERT_FALSE(strcmp(read_file(TEST_1_CPU_MAX, 0), "max - 1000\nperiod - 20000\n"));

    // Disable cpu controller
    ASSERT_TRUE(disable_controller(CPU_CNT));
}

TEST(test_limiting_pids)
{
    // Enable pid controller
    ASSERT_TRUE(enable_controller(PID_CNT));

    // Update pid limit
    ASSERT_TRUE(write_file(TEST_1_PID_MAX, "10"));

    // Check changes
    ASSERT_FALSE(strcmp(read_file(TEST_1_PID_MAX, 0), "max - 10\n"));

    // Restore to 64
    ASSERT_TRUE(write_file(TEST_1_PID_MAX, "64"));

    // Check changes
    ASSERT_FALSE(strcmp(read_file(TEST_1_PID_MAX, 0), "max - 64\n"));
}

TEST(test_move_failure)
{
    // Enable pid controller
    ASSERT_TRUE(enable_controller(PID_CNT));

    // Update pid limit
    ASSERT_TRUE(write_file(TEST_1_PID_MAX, "0"));

    // Attempt to move the current process to "/cgroup/test1" cgroup.
    // Notice write here should fail, and so we suppress error outputs from this point.
    ASSERT_FALSE(move_proc(TEST_1_CGROUP_PROCS, getpid()));

    // Check that the process we moved is not in "/cgroup/test1" cgroup.
    // Notice this should fail.
    ASSERT_FALSE(is_pid_in_group(TEST_1_CGROUP_PROCS, getpid()));

    // Check that the process is still in root cgroup.
    ASSERT_TRUE(is_pid_in_group(ROOT_CGROUP_PROCS, getpid()));

    // Disable PID controller
    ASSERT_TRUE(disable_controller(PID_CNT));
}

TEST(test_fork_failure)
{
    // Enable pid controller
    ASSERT_TRUE(enable_controller(PID_CNT));

    // Update pid limit
    ASSERT_TRUE(write_file(TEST_1_PID_MAX, "1"));

    // Move the current process to "/cgroup/test1" cgroup.
    ASSERT_TRUE(move_proc(TEST_1_CGROUP_PROCS, getpid()));

    // Check that the process we moved is really in "/cgroup/test1" cgroup.
    ASSERT_TRUE(is_pid_in_group(TEST_1_CGROUP_PROCS, getpid()));

    // Attempt to fork, notice this operation should fail and return -1.
    ASSERT_UINT_EQ(fork(), -1);

    // Return the process to root cgroup.
    ASSERT_TRUE(move_proc(ROOT_CGROUP_PROCS, getpid()));

    // Check that the process we returned is really in root cgroup.
    ASSERT_TRUE(is_pid_in_group(ROOT_CGROUP_PROCS, getpid()));

    // Disable PID controller
    ASSERT_TRUE(disable_controller(PID_CNT));
}

TEST(test_pid_current)
{
    // Move the current process to "/cgroup/test1" cgroup.
    ASSERT_TRUE(move_proc(TEST_1_CGROUP_PROCS, getpid()));

    // Check that the process we moved is really in "/cgroup/test1" cgroup.
    ASSERT_TRUE(is_pid_in_group(TEST_1_CGROUP_PROCS, getpid()));

    // Check pid.current changed to display 1
    ASSERT_FALSE(strcmp(read_file(TEST_1_PID_CURRENT, 0), "num_of_procs - 1\n"));

    // Return the process to root cgroup.
    ASSERT_TRUE(move_proc(ROOT_CGROUP_PROCS, getpid()));

    // Check that the process we returned is really in root cgroup.
    ASSERT_TRUE(is_pid_in_group(ROOT_CGROUP_PROCS, getpid()));
}

TEST(test_moving_process)
{
    // Move the current process to "/cgroup/test1" cgroup.
    ASSERT_TRUE(move_proc(TEST_1_CGROUP_PROCS, getpid()));

    // Check that the process we moved is really in "/cgroup/test1" cgroup.
    ASSERT_TRUE(is_pid_in_group(TEST_1_CGROUP_PROCS, getpid()));

    // Check that events recorded it correctly.
    ASSERT_FALSE(strcmp(read_file(TEST_1_CGROUP_EVENTS, 0), "populated - 1\nfrozen - 0\n"));

    // Return the process to root cgroup.
    ASSERT_TRUE(move_proc(ROOT_CGROUP_PROCS, getpid()));

    // Check that the process we returned is really in root cgroup.
    ASSERT_TRUE(is_pid_in_group(ROOT_CGROUP_PROCS, getpid()));
}

TEST(test_setting_max_descendants_and_max_depth)
{
    // Set new values for max descendants allowed and max depth allowed
    ASSERT_TRUE(write_file(TEST_1_CGROUP_DESCENDANTS, "30"));
    ASSERT_TRUE(write_file(TEST_1_CGROUP_MAX_DEPTH, "20"));

    // Check that the values have really been set
    ASSERT_FALSE(strcmp(read_file(TEST_1_CGROUP_DESCENDANTS, 0), "30\n"));
    ASSERT_FALSE(strcmp(read_file(TEST_1_CGROUP_MAX_DEPTH, 0), "20\n"));
}


TEST(test_enable_and_disable_set_controller)
{
    // Enable cpu set controller.
    ASSERT_TRUE(enable_controller(SET_CNT));

    // Check that cpu set controller is really enabled.
    ASSERT_TRUE(verify_controller_enabled(SET_CNT));

    // Disable cpu set controller.
    ASSERT_TRUE(disable_controller(SET_CNT));

    // Check that cpu set controller is really disabled.
    ASSERT_TRUE(verify_controller_disabled(SET_CNT));
}

TEST(test_setting_cpu_id)
{
    // Enable cpu set controller.
    ASSERT_TRUE(enable_controller(SET_CNT));

    // Update cpu id.
    ASSERT_TRUE(write_file(TEST_1_SET_CPU, "1"));

    // Check changes.
    ASSERT_FALSE(strcmp(read_file(TEST_1_SET_CPU, 0), "use_cpu - 1\n"));

    // Restore default cpu id.
    ASSERT_TRUE(write_file(TEST_1_SET_CPU, "0"));

    // Check changes.
    ASSERT_FALSE(strcmp(read_file(TEST_1_SET_CPU, 0), "use_cpu - 0\n"));

    // Disable cpu set controller.
    ASSERT_TRUE(disable_controller(SET_CNT));
}

TEST(test_correct_cpu_running)
{
    // Enable cpu set controller.
    ASSERT_TRUE(enable_controller(SET_CNT));

    // Update cpu id.
    ASSERT_TRUE(write_file(TEST_1_SET_CPU, "1"));

    // Check changes.
    ASSERT_FALSE(strcmp(read_file(TEST_1_SET_CPU, 0), "use_cpu - 1\n"));

    // Move the current process to "/cgroup/test1" cgroup.
    ASSERT_TRUE(move_proc(TEST_1_CGROUP_PROCS, getpid()));

    // Check that the process we moved is really in "/cgroup/test1" cgroup.
    ASSERT_TRUE(is_pid_in_group(TEST_1_CGROUP_PROCS, getpid()));

    // Go to sleep in order to make the process is rescheduled.
    sleep(5);

    // Verify that the process is scheduled on the set cpu.
    ASSERT_UINT_EQ(getcpu(), 1);

    // Return the process to root cgroup.
    ASSERT_TRUE(move_proc(ROOT_CGROUP_PROCS, getpid()));

    // Check that the process we returned is really in root cgroup.
    ASSERT_TRUE(is_pid_in_group(ROOT_CGROUP_PROCS, getpid()));

    // Restore default cpu id.
    ASSERT_TRUE(write_file(TEST_1_SET_CPU, "0"));

    // Check changes.
    ASSERT_FALSE(strcmp(read_file(TEST_1_SET_CPU, 0), "use_cpu - 0\n"));

    // Disable cpu set controller.
    ASSERT_TRUE(disable_controller(SET_CNT));
}

TEST(test_no_run)
{
    // Enable cpu set controller.
    ASSERT_TRUE(enable_controller(SET_CNT));

    // Update cpu id.
    ASSERT_TRUE(write_file(TEST_1_SET_CPU, "2"));

    // Fork here since the process should not be running after we move it inside the cgroup.
    int pid = fork();
    int pidToMove = 0;
    int sum = 0;
    int wstatus;

    // Child
    if (pid == 0) {
        pidToMove = getpid();

        // Save the pid of child in temp file.
        temp_write(pidToMove);

        // Go to sleep for long period of time.
        sleep(20);

        // At this point, the child process should already be inside the cgroup.
        // Therefore, the following operations should not be executed right away.
        for (int i = 0; i < 10; i++) {
            sum += 1;
        }

        // Save sum into temp file.
        temp_write(sum);
        exit(0);
    }
    // Father
    else {
        sleep(5);
        // Read the child pid from temp file.
        pidToMove = temp_read(0);
        // Update the temp file for further reading, since next sum will be read from it.
        ASSERT_TRUE(temp_write(0));

        // Move the child process to "/cgroup/test1" cgroup.
        ASSERT_TRUE(move_proc(TEST_1_CGROUP_PROCS, pidToMove));

        // Check that the process we moved is really in "/cgroup/test1" cgroup.
        ASSERT_TRUE(is_pid_in_group(TEST_1_CGROUP_PROCS, pidToMove));

        // Go to sleep to ensure the child process had a chance to be scheduled.
        sleep(10);

        // Verify that the child process have not ran
        sum = temp_read(0);
        ASSERT_UINT_EQ(sum, 0);

        // Return the child to root cgroup.
        ASSERT_TRUE(move_proc(ROOT_CGROUP_PROCS, pidToMove));

        // Check that the child we returned is really in root cgroup.
        ASSERT_TRUE(is_pid_in_group(ROOT_CGROUP_PROCS, pidToMove));

        // Wait for child to exit.
        wait(&wstatus);
        ASSERT_TRUE(wstatus);

        // Verify that child did execute the procudure.
        sum = temp_read(0);
        ASSERT_UINT_EQ(sum, 10);

        // Remove the temp file.
        ASSERT_TRUE(temp_delete());

        // Disable cpu set controller.
        ASSERT_TRUE(disable_controller(SET_CNT));
    }
}

TEST(test_setting_freeze)
{
    // Verify frozen start as 0.
    ASSERT_FALSE(strcmp(read_file(TEST_1_CGROUP_EVENTS, 0), "populated - 0\nfrozen - 0\n"));

    // Update frozen.
    ASSERT_TRUE(write_file(TEST_1_SET_FRZ, "1"));

    // Check changes.
    ASSERT_FALSE(strcmp(read_file(TEST_1_SET_FRZ, 0), "1\n"));

    // Check Evenets correctly recorded.
    ASSERT_FALSE(strcmp(read_file(TEST_1_CGROUP_EVENTS, 0), "populated - 0\nfrozen - 1\n"));

    // Restore frozen.
    ASSERT_TRUE(write_file(TEST_1_SET_FRZ, "0"));

    // Verify frozen is 0 again.
    ASSERT_FALSE(strcmp(read_file(TEST_1_CGROUP_EVENTS, 0), "populated - 0\nfrozen - 0\n"));

    // Check changes.
    ASSERT_FALSE(strcmp(read_file(TEST_1_SET_FRZ, 0), "0\n"));
}

TEST(test_frozen_not_running)
{
    // Update frozen.
    ASSERT_TRUE(write_file(TEST_1_SET_FRZ, "1"));

    // Fork here since the process should not be running after we move it inside the cgroup.
    int pid = fork();
    int pidToMove = 0;
    int sum = 0;
    int wstatus;

    // Child
    if (pid == 0) {
        pidToMove = getpid();
        // Save the pid of child in temp file.
        ASSERT_TRUE(temp_write(pidToMove));
        // Go to sleep for long period of time.
        sleep(20);
        // At this point, the child process should already be inside the cgroup, therefore, the following operations
        // should not be executed.
        for (int i = 0; i < 10; i++) {
            sum += 1;
        }
        // Save sum into temp file.
        ASSERT_TRUE(temp_write(sum));
        exit(0);
    }
    // Father
    else {
        sleep(5);
        // Read the child pid from temp file.
        pidToMove = temp_read(0);
        // Update the temp file for further reading, since next sum will be read from it.
        ASSERT_TRUE(temp_write(0));

        // Move the child process to "/cgroup/test1" cgroup.
        ASSERT_TRUE(move_proc(TEST_1_CGROUP_PROCS, pidToMove));

        // Check that the process we moved is really in "/cgroup/test1" cgroup.
        ASSERT_TRUE(is_pid_in_group(TEST_1_CGROUP_PROCS, pidToMove));

        // Go to sleep to ensure the child process had a chance to be scheduled.
        sleep(10);

        // Verify that the child process have not ran
        sum = temp_read(0);
        ASSERT_UINT_EQ(sum, 0);

        // Return the child to root cgroup.
        ASSERT_TRUE(move_proc(ROOT_CGROUP_PROCS, pidToMove));

        // Check that the child we returned is really in root cgroup.
        ASSERT_TRUE(is_pid_in_group(ROOT_CGROUP_PROCS, pidToMove));

        // Wait for child to exit.
        wait(&wstatus);
        ASSERT_TRUE(wstatus);

        // Verify that child did execute the procudure.
        sum = temp_read(0);
        ASSERT_UINT_EQ(sum, 10);

        // Remove the temp file.
        ASSERT_TRUE(temp_delete());

        // Update frozen.
        ASSERT_TRUE(write_file(TEST_1_SET_FRZ, "0"));
    }
}

//In this following memory accounting tests we move only single process to "/cgroup/test1" in order to simplify the testing.
TEST(test_mem_current) {
  // Save current process memory size.
  char proc_mem[10];
  itoa(proc_mem, getmem());
  strcat(proc_mem, "\n");
  // Buffer to read contents from memory file.
  char saved_mem[10];

  // Move the current process to "/cgroup/test1" cgroup.
  ASSERT_TRUE(move_proc(TEST_1_CGROUP_PROCS, getpid()));

  // Check that the process we moved is really in "/cgroup/test1" cgroup.
  ASSERT_TRUE(is_pid_in_group(TEST_1_CGROUP_PROCS, getpid()));

  // Read the contents of current memory file and convert it for comparison.
  strcpy(saved_mem, read_file(TEST_1_MEM_CURRENT, 0));

  // Check memory usaged updated correctly.
  ASSERT_FALSE(strcmp(saved_mem, proc_mem));

  // Return the process to root cgroup.
  ASSERT_TRUE(move_proc(ROOT_CGROUP_PROCS, getpid()));

  // Check that the process we returned is really in root cgroup.
  ASSERT_TRUE(is_pid_in_group(ROOT_CGROUP_PROCS, getpid()));
}

TEST(test_correct_mem_account_of_growth_and_shrink) {
  // Save current process memory size.
  char proc_mem[10];
  // Buffer to read contents from memory file.
  char saved_mem[10];

  // Move the current process to "/cgroup/test1" cgroup.
  ASSERT_TRUE(move_proc(TEST_1_CGROUP_PROCS, getpid()));

  // Check that the process we moved is really in "/cgroup/test1" cgroup.
  ASSERT_TRUE(is_pid_in_group(TEST_1_CGROUP_PROCS, getpid()));

  // Grow the current process by 100 bytes.
  sbrk(100);

  // Read the contents of current memory file and convert it for comparison.
  strcpy(saved_mem, read_file(TEST_1_MEM_CURRENT, 0));

  // Convert process memory to a string.
  itoa(proc_mem, getmem());
  strcat(proc_mem, "\n");

  // Read the contents of current memory file and convert it for comparison.
  strcpy(saved_mem, read_file(TEST_1_MEM_CURRENT, 0));

  // Check that the memory accounting correctly updated after memory growth.
  ASSERT_FALSE(strcmp(saved_mem, proc_mem));

  // Decrease current proc by 100 bytes.
  sbrk(-100);

  // Read the contents of current memory file and convert it for comparison.
  strcpy(saved_mem, read_file(TEST_1_MEM_CURRENT, 0));

  // Convert process memory to a string.
  itoa(proc_mem, getmem());
  strcat(proc_mem, "\n");

  // Read the contents of current memory file and convert it for comparison.
  strcpy(saved_mem, read_file(TEST_1_MEM_CURRENT, 0));

  // Check that the memory accounting correctly updated after memory growth.
  ASSERT_FALSE(strcmp(saved_mem, proc_mem));

  // Return the process to root cgroup.
  ASSERT_TRUE(move_proc(ROOT_CGROUP_PROCS, getpid()));

  // Check that the process we returned is really in root cgroup.
  ASSERT_TRUE(is_pid_in_group(ROOT_CGROUP_PROCS, getpid()));
}

TEST(test_limiting_mem)
{
  // Buffer for saving current memory written in limit
  char saved_mem[12];

  // Enable memory controller
  ASSERT_TRUE(enable_controller(MEM_CNT));

  // Copy the current saved memory and remove newline at the end
  strcpy(saved_mem, read_file(TEST_1_MEM_MAX, 0));
  saved_mem[strlen(saved_mem) - 1] = '\0';

  // Update memory limit
  ASSERT_TRUE(write_file(TEST_1_MEM_MAX, "100"));

  // Check changes
  ASSERT_FALSE(strcmp(read_file(TEST_1_MEM_MAX, 0), "100\n"));

  // Restore memory limit to original
  ASSERT_TRUE(write_file(TEST_1_MEM_MAX, saved_mem));

  // Check changes
  ASSERT_FALSE(strncmp(read_file(TEST_1_MEM_MAX, 0), saved_mem, strlen(saved_mem)));

  // Disable memory controller
  ASSERT_TRUE(disable_controller(MEM_CNT));
}

TEST(test_cant_move_over_mem_limit)
{
  // Buffer for saving current memory written in limit
  char saved_mem[12];

  // Enable memory controller
  ASSERT_TRUE(enable_controller(MEM_CNT));

  // Copy the current saved memory and remove newline at the end
  strcpy(saved_mem, read_file(TEST_1_MEM_MAX, 0));
  saved_mem[strlen(saved_mem) - 1] = '\0';

  // Update memory limit
  ASSERT_TRUE(write_file(TEST_1_MEM_MAX, "0"));

  // Check changes
  ASSERT_FALSE(strcmp(read_file(TEST_1_MEM_MAX, 0), "0\n"));

  // Attemp to move the current process to "/cgroup/test1" cgroup.
  ASSERT_FALSE(move_proc(TEST_1_CGROUP_PROCS, getpid()));

  // Check that the current process is not in "/cgroup/test1" cgroup.
  ASSERT_FALSE(is_pid_in_group(TEST_1_CGROUP_PROCS, getpid()));

  // Check that the current process is still in root group.
  ASSERT_TRUE(is_pid_in_group(ROOT_CGROUP_PROCS, getpid()));

  // Restore memory limit to original
  ASSERT_TRUE(write_file(TEST_1_MEM_MAX, saved_mem));

  // Check changes
  ASSERT_FALSE(strncmp(read_file(TEST_1_MEM_MAX, 0), saved_mem, strlen(saved_mem)));

  // Disable memory controller
  ASSERT_TRUE(disable_controller(MEM_CNT));
}

TEST(test_cant_fork_over_mem_limit)
{
  // Save current process memory size.
  char proc_mem[10];
  itoa(proc_mem, getmem());
  // Buffer to read contents from memory file.
  char saved_mem[10];

  // Enable memory controller
  ASSERT_TRUE(enable_controller(MEM_CNT));

  // Update memory limit
  ASSERT_TRUE(write_file(TEST_1_MEM_MAX, proc_mem));

  // Read the contents of limit file and convert it for comparison.
  strcpy(saved_mem, read_file(TEST_1_MEM_MAX, 0));

  strcat(proc_mem, "\n");

  // Check changes
  ASSERT_FALSE(strcmp(saved_mem, proc_mem));

  // Move the current process to "/cgroup/test1" cgroup.
  ASSERT_TRUE(move_proc(TEST_1_CGROUP_PROCS, getpid()));

  // Attempt to fork, notice this operation should fail and return -1.
  ASSERT_UINT_EQ(fork(), -1);

  // Return the process to root cgroup.
  ASSERT_TRUE(move_proc(ROOT_CGROUP_PROCS, getpid()));

  // Check that the process we returned is really in root cgroup.
  ASSERT_TRUE(is_pid_in_group(ROOT_CGROUP_PROCS, getpid()));

  // Disable memory controller
  ASSERT_TRUE(disable_controller(MEM_CNT));
}

TEST(test_cant_grow_over_mem_limit)
{
  // Save current process memory size.
  char proc_mem[10];
  itoa(proc_mem, getmem());
  // Buffer to read contents from memory file.
  char saved_mem[10];

  // Enable memory controller
  ASSERT_TRUE(enable_controller(MEM_CNT));

  // Update memory limit
  ASSERT_TRUE(write_file(TEST_1_MEM_MAX, proc_mem));

  strcat(proc_mem, "\n");

  // Read the contents of limit file and convert it for comparison.
  strcpy(saved_mem, read_file(TEST_1_MEM_MAX, 0));

  // Check changes
  ASSERT_FALSE(strcmp(saved_mem, proc_mem));

  // Move the current process to "/cgroup/test1" cgroup.
  ASSERT_TRUE(move_proc(TEST_1_CGROUP_PROCS, getpid()));

  // Attempt to grow process memory, notice this operation should fail and return -1.
  ASSERT_UINT_EQ((int)sbrk(10), -1);

  // Return the process to root cgroup.
  ASSERT_TRUE(move_proc(ROOT_CGROUP_PROCS, getpid()));

  // Check that the process we returned is really in root cgroup.
  ASSERT_TRUE(is_pid_in_group(ROOT_CGROUP_PROCS, getpid()));

  // Disable memory controller
  ASSERT_TRUE(disable_controller(MEM_CNT));
}

TEST(test_memory_stat_content_valid)
{
    char buf[265];
    strcpy(buf, read_file(TEST_1_MEM_STAT, 0));
    int file_dirty = get_val(buf, "file_dirty - ");
    int file_dirty_aggregated = get_val(buf, "file_dirty_aggregated - ");
    int pgfault = get_val(buf, "pgfault - ");
    int pgmajfault = get_val(buf, "file_dirty - ");
    ASSERT_UINT_EQ(file_dirty, 0);
    ASSERT_UINT_EQ(file_dirty_aggregated, 0);
    ASSERT_UINT_EQ(pgfault, 0);
    ASSERT_UINT_EQ(pgmajfault, 0);
}

TEST(test_kernel_freem_mem)
{
  ASSERT_FALSE(kmemtest());
}

TEST(test_cpu_stat_content_valid)
{
    char buf[265];
    strcpy(buf, read_file(TEST_1_CPU_STAT, 0));
    int usage_usec = get_val(buf, "usage_usec - ");
    int user_usec = get_val(buf, "user_usec - ");
    int system_usec = get_val(buf, "system_usec - ");
    ASSERT_UINT_EQ(usage_usec, 0);
    ASSERT_UINT_EQ(user_usec, 0);
    ASSERT_UINT_EQ(system_usec, 0);
}

TEST(test_cpu_stat)
{

    char buf1[265];
    char buf2[265];
    char buf3[265];

    // read cpu.stat into a buffer
    strcpy(buf1, read_file(TEST_1_CPU_STAT,0));

    // Fork here since the process should not be running after we move it inside the cgroup.
    int pid = fork();
    int pidToMove = 0;
    int sum = 0;
    int wstatus;

    // Child
    if (pid == 0) {
        pidToMove = getpid();

        // Save the pid of child in temp file.
        temp_write(pidToMove);

        // Go to sleep for long period of time.
        sleep(10);

        // At this point, the child process should already be inside the cgroup.
        //By running the loop we ensure CPU usage which should be reflected in cpu.stst
        for (int i = 0; i < 10; i++) {
            sum += 1;
        }

        // Save sum into temp file.
        temp_write(sum);

        // Go to sleep to ensure we cen return the child to root cgroup
        sleep(25);
        exit(0);
    }
    // Father
    else {
        sleep(5);

        // Read the child pid from temp file.
        pidToMove = temp_read(0);

        // Update the temp file for further reading, since next sum will be read from it.
        ASSERT_TRUE(temp_write(0));

        // Move the child process to "/cgroup/test1" cgroup.
        ASSERT_TRUE(move_proc(TEST_1_CGROUP_PROCS, pidToMove));

        // Check that the process we moved is really in "/cgroup/test1" cgroup.
        ASSERT_TRUE(is_pid_in_group(TEST_1_CGROUP_PROCS, pidToMove));

        // Go to sleep to ensure the child process had a chance to be scheduled.
        sleep(15);

        // Verify that the child process have ran
        sum = temp_read(0);
        ASSERT_UINT_EQ(sum, 10);

        // Return the child to root cgroup.
        ASSERT_TRUE(move_proc(ROOT_CGROUP_PROCS, pidToMove));

        // Check that the child we returned is really in root cgroup.
        ASSERT_TRUE(is_pid_in_group(ROOT_CGROUP_PROCS, pidToMove));

        // read cpu.stat into a seconde buffer
        strcpy(buf2,read_file(TEST_1_CPU_STAT,0));

        // Verify that the cpu time has changed because of the child's runing
        ASSERT_TRUE(strcmp(buf1, buf2));

        sleep(10);

        // read cpu.stat into a third buffer
        strcpy(buf3,read_file(TEST_1_CPU_STAT,0));

        // Verify that the cpu time has no changed since the child removed
        ASSERT_FALSE(strcmp(buf2,buf3));

        // Wait for child to exit.
        wait(&wstatus);
        ASSERT_TRUE(wstatus);

        // Remove the temp file.
        ASSERT_TRUE(temp_delete());

    }
}

TEST (test_mem_stat) {
    int wstatus;
    char befor_all[265];
    char effect_write_first_file[265];
    char effect_write_second_file[265];

    strcpy(befor_all, read_file(TEST_1_MEM_STAT,0));
    // Fork a process because reading the memory values from inside the cgroup may affect the values.
    int pid = fork();
    int pidToMove = 0;
    // Child
    if (pid == 0) {
        pidToMove = getpid();
        // Save the pid of child in temp file.
        ASSERT_TRUE(temp_write(pidToMove));

        // Go to sleep for long period of time alowe move the prosses into cgroup.
        sleep(10);
        char str [256];
        memset(str, 'a', 256);

        // Write to a new file 2 times.
        int fd;
        ASSERT_TRUE(fd=write_new_file("c", str));
        ASSERT_TRUE(write_new_file("c", str));
        ASSERT_TRUE(close_file(fd));
        sleep(20);

        // Write times to another file with the file closed in the middle.
        ASSERT_TRUE(fd=write_new_file("d", str));
        ASSERT_TRUE(close_file(fd));
        ASSERT_TRUE(write_new_file("d", str));
        ASSERT_TRUE(close_file(fd));

        exit(0);

    } else { // Father

        sleep(5);
        // Read the child pid from temp file.
        pidToMove = temp_read(0);
        // Move the child process to "/cgroup/test1" cgroup.
        ASSERT_TRUE(move_proc(TEST_1_CGROUP_PROCS, pidToMove));
        // Check that the process we moved is really in "/cgroup/test1" cgroup.
        ASSERT_TRUE(is_pid_in_group(TEST_1_CGROUP_PROCS, pidToMove));
        // Go to sleep to ensure the child process had a chance to be scheduled.
        // Allows the child to write a page twice for a new file
        sleep(20);
        strcpy(effect_write_first_file, read_file(TEST_1_MEM_STAT,0));

        //Allows the child to write to a new file close and write again
        sleep(20);
        strcpy(effect_write_second_file, read_file(TEST_1_MEM_STAT,0));

        // check the effect of pgmajfault
        int pgmajfault_befor = get_val(befor_all, "pgmajfault - ");
        int pgmajfault_after = get_val(effect_write_second_file, "pgmajfault - ");
        ASSERT_TRUE(pgmajfault_after - pgmajfault_befor >= 2);

        // check the effect of pgfaul
        // The second write to file c was before closing and file d was after closing,
        // so we need more pgfaults besides what the writing itself causes
        int grow_pgfoult_after_first = get_val(effect_write_first_file, "pgfault - ") - get_val(befor_all , "pgfault - ");
        int grow_pgfoult_after_seconde = get_val(effect_write_second_file, "pgfault - ") - get_val(effect_write_first_file, "pgfault - ");
        ASSERT_TRUE(grow_pgfoult_after_first);
        ASSERT_TRUE(grow_pgfoult_after_seconde > grow_pgfoult_after_first);

        // check the effect of file dirty
        // we calculate the dirte and aggregated together as it is impossible to know if there is a delay in writing to disk
        int dirty_befor = get_val(befor_all, "file_dirty - ") + get_val(befor_all, "file_dirty_aggregated - ");
        int dirty_after = get_val(effect_write_second_file, "file_dirty - ") + get_val(effect_write_second_file, "file_dirty_aggregated - ");
        ASSERT_TRUE(dirty_after - dirty_befor >= 2);
        // Wait for child to exit.
        wait(&wstatus);
        ASSERT_TRUE(wstatus);
        // Remove the temp file.
        ASSERT_TRUE(temp_delete());
    }
}

int main(int argc, char * argv[])
{
    // comment out for debug messages
    set_suppress(1);

    run_test(test_mount_cgroup_fs);
    run_test(test_creating_cgroups);
    run_test(test_opening_and_closing_cgroup_files);
    run_test_break_msg(test_reading_cgroup_files);
    run_test(test_memory_stat_content_valid);
    run_test(test_cpu_stat_content_valid);
    run_test(test_moving_process);
    run_test(test_enable_and_disable_all_controllers);
    run_test(test_limiting_pids);
    run_test(test_move_failure);
    run_test(test_fork_failure);
    run_test(test_cpu_stat);
    run_test(test_pid_current);
    run_test(test_setting_cpu_id);
    run_test(test_correct_cpu_running);
    run_test(test_no_run);
    run_test(test_mem_stat);
    run_test(test_setting_freeze);
    run_test(test_frozen_not_running);
    run_test(test_mem_current);
    run_test(test_correct_mem_account_of_growth_and_shrink);
    run_test(test_limiting_mem);
    run_test(test_cant_move_over_mem_limit);
    run_test(test_cant_fork_over_mem_limit);
    run_test(test_cant_grow_over_mem_limit);
    run_test(test_limiting_cpu_max_and_period);
    run_test(test_setting_max_descendants_and_max_depth);
    run_test(test_deleting_cgroups);
    run_test(test_umount_cgroup_fs);
    run_test_break_msg(test_kernel_freem_mem);

    if (failed) {
        printf(1, "[    CGROUPTESTS FAILED    ]\n");
        exit(1);
    } else {
        printf(1, "[    CGROUPTESTS PASSED    ]\n");
        exit(0);
    }
}