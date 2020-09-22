#include "fcntl.h"
#include "types.h"
#include "user.h"
#include "test.h"
#include "cgroupstests.h"
#include "cgroups_test_functions.h"

int failed = 0;

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
    ASSERT_TRUE(test_open_close(TEST_1_CGROUP_PROCS));
    ASSERT_TRUE(test_open_close(TEST_1_CGROUP_CONTROLLERS));
    ASSERT_TRUE(test_open_close(TEST_1_CGROUP_SUBTREE_CONTROL));
    ASSERT_TRUE(test_open_close(TEST_1_CGROUP_EVENTS));
    ASSERT_TRUE(test_open_close(TEST_1_CGROUP_DESCENDANTS));
    ASSERT_TRUE(test_open_close(TEST_1_CGROUP_MAX_DEPTH));
    ASSERT_TRUE(test_open_close(TEST_1_CGROUP_STAT));
    ASSERT_TRUE(test_open_close(TEST_1_CPU_MAX));
    ASSERT_TRUE(test_open_close(TEST_1_CPU_WEIGHT));
    ASSERT_TRUE(test_open_close(TEST_1_CPU_STAT));
    ASSERT_TRUE(test_open_close(TEST_1_PID_MAX));
    ASSERT_TRUE(test_open_close(TEST_1_PID_CURRENT));
    ASSERT_TRUE(test_open_close(TEST_1_SET_CPU));
    ASSERT_TRUE(test_open_close(TEST_1_SET_FRZ));
    ASSERT_TRUE(test_open_close(TEST_1_MEM_CURRENT));
    ASSERT_TRUE(test_open_close(TEST_1_MEM_MAX));
}

TEST(test_reading_cgroup_files)
{
    ASSERT_TRUE(test_read(TEST_1_CGROUP_PROCS, 1));
    ASSERT_TRUE(test_read(TEST_1_CGROUP_CONTROLLERS, 1));
    ASSERT_TRUE(test_read(TEST_1_CGROUP_SUBTREE_CONTROL, 1));
    ASSERT_TRUE(test_read(TEST_1_CGROUP_EVENTS, 1));
    ASSERT_TRUE(test_read(TEST_1_CGROUP_DESCENDANTS, 1));
    ASSERT_TRUE(test_read(TEST_1_CGROUP_MAX_DEPTH, 1));
    ASSERT_TRUE(test_read(TEST_1_CGROUP_STAT, 1));
    ASSERT_TRUE(test_read(TEST_1_CPU_MAX, 1));
    ASSERT_TRUE(test_read(TEST_1_CPU_WEIGHT, 1));
    ASSERT_TRUE(test_read(TEST_1_CPU_STAT, 1));
    ASSERT_TRUE(test_read(TEST_1_PID_MAX, 1));
    ASSERT_TRUE(test_read(TEST_1_PID_CURRENT, 1));
    ASSERT_TRUE(test_read(TEST_1_SET_CPU, 1));
    ASSERT_TRUE(test_read(TEST_1_SET_FRZ, 1));
    ASSERT_TRUE(test_read(TEST_1_MEM_CURRENT, 1));
    ASSERT_TRUE(test_read(TEST_1_MEM_MAX, 1));
}

char *build_activate_disable_activate(int controller_type)
{
    if (!is_valid_controller_type(controller_type)) {
        return 0;
    }

    int max_size = (MAX_CONTROLLER_NAME_LENGTH + 2) * 3;
    char *buf = malloc(max_size);
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

char *build_activate_disable(int controller_type)
{
    if (!is_valid_controller_type(controller_type)) {
        return 0;
    }

    int max_size = (MAX_CONTROLLER_NAME_LENGTH + 2) * 2;
    char *buf = malloc(max_size);
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

int test_enable_and_disable_controller(int controller_type)
{
    char *buf;
    int result = 1;

    // Enable pid controller
    buf = build_activate_disable_activate(controller_type);
    if (!buf) {
        return 0;
    }
    result &= test_write(TEST_1_CGROUP_SUBTREE_CONTROL, buf);
    free(buf);


    // Check that pid controller is really enabled
    result &= test_verify_controller_enabled(controller_type);

    // Disable pid controller
    buf = build_activate_disable(controller_type);
    if (!buf) {
        return 0;
    }
    result &= test_write(TEST_1_CGROUP_SUBTREE_CONTROL, buf);
    free(buf);

    // Check that pid controller is really disabled
    result &= test_verify_controller_disabled(controller_type);

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
    ASSERT_TRUE(test_enable_controller(CPU_CNT));

    // Update only max
    ASSERT_TRUE(test_write(TEST_1_CPU_MAX, "5000"));

    // Check changes
    ASSERT_FALSE(strcmp(test_read(TEST_1_CPU_MAX, 0), "max - 5000\nperiod - 100000\n"));

    // Update max & period
    ASSERT_TRUE(test_write(TEST_1_CPU_MAX, "1000,20000"));

    // Check changes
    ASSERT_FALSE(strcmp(test_read(TEST_1_CPU_MAX, 0), "max - 1000\nperiod - 20000\n"));

    // Disable cpu controller
    ASSERT_TRUE(test_disable_controller(CPU_CNT));
}

TEST(test_limiting_pids)
{
    // Enable pid controller
    ASSERT_TRUE(test_enable_controller(PID_CNT));

    // Update pid limit
    ASSERT_TRUE(test_write(TEST_1_PID_MAX, "10"));

    // Check changes
    ASSERT_FALSE(strcmp(test_read(TEST_1_PID_MAX, 0), "max - 10\n"));

    // Restore to 64
    ASSERT_TRUE(test_write(TEST_1_PID_MAX, "64"));

    // Check changes
    ASSERT_FALSE(strcmp(test_read(TEST_1_PID_MAX, 0), "max - 64\n"));
}

TEST(test_move_failure)
{
    // Enable pid controller
    ASSERT_TRUE(test_enable_controller(PID_CNT));

    // Update pid limit
    ASSERT_TRUE(test_write(TEST_1_PID_MAX, "0"));

    // Attempt to move the current process to "/cgroup/test1" cgroup.
    // Notice write here should fail, and so we suppress error outputs from this point.
    ASSERT_FALSE(test_move_proc(TEST_1_CGROUP_PROCS, getpid()));

    // Check that the process we moved is not in "/cgroup/test1" cgroup.
    // Notice this should fail.
    ASSERT_FALSE(test_pid_in_group(TEST_1_CGROUP_PROCS, getpid()));

    // Check that the process is still in root cgroup.
    ASSERT_TRUE(test_pid_in_group(ROOT_CGROUP_PROCS, getpid()));

    // Disable PID controller
    ASSERT_TRUE(test_disable_controller(PID_CNT));
}

TEST(test_fork_failure)
{
    // Enable pid controller
    ASSERT_TRUE(test_enable_controller(PID_CNT));

    // Update pid limit
    ASSERT_TRUE(test_write(TEST_1_PID_MAX, "1"));

    // Move the current process to "/cgroup/test1" cgroup.
    ASSERT_TRUE(test_move_proc(TEST_1_CGROUP_PROCS, getpid()));

    // Check that the process we moved is really in "/cgroup/test1" cgroup.
    ASSERT_TRUE(test_pid_in_group(TEST_1_CGROUP_PROCS, getpid()));

    // Attempt to fork, notice this operation should fail and return -1.
    ASSERT_UINT_EQ(fork(), -1);

    // Return the process to root cgroup.
    ASSERT_TRUE(test_move_proc(ROOT_CGROUP_PROCS, getpid()));

    // Check that the process we returned is really in root cgroup.
    ASSERT_TRUE(test_pid_in_group(ROOT_CGROUP_PROCS, getpid()));

    // Disable PID controller
    ASSERT_TRUE(test_disable_controller(PID_CNT));
}

TEST(test_pid_current)
{
    // Move the current process to "/cgroup/test1" cgroup.
    ASSERT_TRUE(test_move_proc(TEST_1_CGROUP_PROCS, getpid()));

    // Check that the process we moved is really in "/cgroup/test1" cgroup.
    ASSERT_TRUE(test_pid_in_group(TEST_1_CGROUP_PROCS, getpid()));

    // Check pid.current changed to display 1
    ASSERT_FALSE(strcmp(test_read(TEST_1_PID_CURRENT, 0), "num_of_procs - 1\n"));

    // Return the process to root cgroup.
    ASSERT_TRUE(test_move_proc(ROOT_CGROUP_PROCS, getpid()));

    // Check that the process we returned is really in root cgroup.
    ASSERT_TRUE(test_pid_in_group(ROOT_CGROUP_PROCS, getpid()));
}

TEST(test_moving_process)
{
    // Move the current process to "/cgroup/test1" cgroup.
    ASSERT_TRUE(test_move_proc(TEST_1_CGROUP_PROCS, getpid()));

    // Check that the process we moved is really in "/cgroup/test1" cgroup.
    ASSERT_TRUE(test_pid_in_group(TEST_1_CGROUP_PROCS, getpid()));

    // Check that events recorded it correctly.
    ASSERT_FALSE(strcmp(test_read(TEST_1_CGROUP_EVENTS, 0), "populated - 1\nfrozen - 0\n"));

    // Return the process to root cgroup.
    ASSERT_TRUE(test_move_proc(ROOT_CGROUP_PROCS, getpid()));

    // Check that the process we returned is really in root cgroup.
    ASSERT_TRUE(test_pid_in_group(ROOT_CGROUP_PROCS, getpid()));
}

TEST(test_setting_max_descendants_and_max_depth)
{
    // Set new values for max descendants allowed and max depth allowed
    ASSERT_TRUE(test_write(TEST_1_CGROUP_DESCENDANTS, "30"));
    ASSERT_TRUE(test_write(TEST_1_CGROUP_MAX_DEPTH, "20"));

    // Check that the values have really been set
    ASSERT_FALSE(strcmp(test_read(TEST_1_CGROUP_DESCENDANTS, 0), "30\n"));
    ASSERT_FALSE(strcmp(test_read(TEST_1_CGROUP_MAX_DEPTH, 0), "20\n"));
}


TEST(test_enable_and_disable_set_controller)
{
    // Enable cpu set controller.
    ASSERT_TRUE(test_enable_controller(SET_CNT));

    // Check that cpu set controller is really enabled.
    ASSERT_TRUE(test_verify_controller_enabled(SET_CNT));

    // Disable cpu set controller.
    ASSERT_TRUE(test_disable_controller(SET_CNT));

    // Check that cpu set controller is really disabled.
    ASSERT_TRUE(test_verify_controller_disabled(SET_CNT));
}

TEST(test_setting_cpu_id)
{
    // Enable cpu set controller.
    ASSERT_TRUE(test_enable_controller(SET_CNT));

    // Update cpu id.
    ASSERT_TRUE(test_write(TEST_1_SET_CPU, "1"));

    // Check changes.
    ASSERT_FALSE(strcmp(test_read(TEST_1_SET_CPU, 0), "use_cpu - 1\n"));

    // Restore default cpu id.
    ASSERT_TRUE(test_write(TEST_1_SET_CPU, "0"));

    // Check changes.
    ASSERT_FALSE(strcmp(test_read(TEST_1_SET_CPU, 0), "use_cpu - 0\n"));

    // Disable cpu set controller.
    ASSERT_TRUE(test_disable_controller(SET_CNT));
}

TEST(test_correct_cpu_running)
{
    // Enable cpu set controller.
    ASSERT_TRUE(test_enable_controller(SET_CNT));

    // Update cpu id.
    ASSERT_TRUE(test_write(TEST_1_SET_CPU, "1"));

    // Check changes.
    ASSERT_FALSE(strcmp(test_read(TEST_1_SET_CPU, 0), "use_cpu - 1\n"));

    // Move the current process to "/cgroup/test1" cgroup.
    ASSERT_TRUE(test_move_proc(TEST_1_CGROUP_PROCS, getpid()));

    // Check that the process we moved is really in "/cgroup/test1" cgroup.
    ASSERT_TRUE(test_pid_in_group(TEST_1_CGROUP_PROCS, getpid()));

    // Go to sleep in order to make the process is rescheduled.
    sleep(5);

    // Verify that the process is scheduled on the set cpu.
    ASSERT_UINT_EQ(getcpu(), 1);

    // Return the process to root cgroup.
    ASSERT_TRUE(test_move_proc(ROOT_CGROUP_PROCS, getpid()));

    // Check that the process we returned is really in root cgroup.
    ASSERT_TRUE(test_pid_in_group(ROOT_CGROUP_PROCS, getpid()));

    // Restore default cpu id.
    ASSERT_TRUE(test_write(TEST_1_SET_CPU, "0"));

    // Check changes.
    ASSERT_FALSE(strcmp(test_read(TEST_1_SET_CPU, 0), "use_cpu - 0\n"));

    // Disable cpu set controller.
    ASSERT_TRUE(test_disable_controller(SET_CNT));
}

TEST(test_no_run)
{
    // Enable cpu set controller.
    ASSERT_TRUE(test_enable_controller(SET_CNT));

    // Update cpu id.
    ASSERT_TRUE(test_write(TEST_1_SET_CPU, "2"));

    // Fork here since the process should not be running after we move it inside the cgroup.
    int pid = fork();
    int pidToMove = 0;
    int sum = 0;
    int wstatus;

    // Child
    if (pid == 0) {
        pidToMove = getpid();

        // Save the pid of child in temp file.
        test_temp_write(pidToMove);

        // Go to sleep for long period of time.
        sleep(20);

        // At this point, the child process should already be inside the cgroup.
        // Therefore, the following operations should not be executed right away.
        for (int i = 0; i < 10; i++) {
            sum += 1;
        }

        // Save sum into temp file.
        test_temp_write(sum);
        exit(0);
    }
    // Father
    else {
        sleep(5);
        // Read the child pid from temp file.
        pidToMove = test_temp_read(0);
        // Update the temp file for further reading, since next sum will be read from it.
        ASSERT_TRUE(test_temp_write(0));

        // Move the child process to "/cgroup/test1" cgroup.
        ASSERT_TRUE(test_move_proc(TEST_1_CGROUP_PROCS, pidToMove));

        // Check that the process we moved is really in "/cgroup/test1" cgroup.
        ASSERT_TRUE(test_pid_in_group(TEST_1_CGROUP_PROCS, pidToMove));

        // Go to sleep to ensure the child process had a chance to be scheduled.
        sleep(10);

        // Verify that the child process have not ran
        sum = test_temp_read(0);
        ASSERT_UINT_EQ(sum, 0);

        // Return the child to root cgroup.
        ASSERT_TRUE(test_move_proc(ROOT_CGROUP_PROCS, pidToMove));

        // Check that the child we returned is really in root cgroup.
        ASSERT_TRUE(test_pid_in_group(ROOT_CGROUP_PROCS, pidToMove));

        // Wait for child to exit.
        wait(&wstatus);
        ASSERT_TRUE(wstatus);

        // Verify that child did execute the procudure.
        sum = test_temp_read(0);
        ASSERT_UINT_EQ(sum, 10);

        // Remove the temp file.
        ASSERT_TRUE(test_temp_delete());

        // Disable cpu set controller.
        ASSERT_TRUE(test_disable_controller(SET_CNT));
    }
}

TEST(test_setting_freeze)
{
    // Verify frozen start as 0.
    ASSERT_FALSE(strcmp(test_read(TEST_1_CGROUP_EVENTS, 0), "populated - 0\nfrozen - 0\n"));

    // Update frozen.
    ASSERT_TRUE(test_write(TEST_1_SET_FRZ, "1"));

    // Check changes.
    ASSERT_FALSE(strcmp(test_read(TEST_1_SET_FRZ, 0), "1\n"));

    // Check Evenets correctly recorded.
    ASSERT_FALSE(strcmp(test_read(TEST_1_CGROUP_EVENTS, 0), "populated - 0\nfrozen - 1\n"));

    // Restore frozen.
    ASSERT_TRUE(test_write(TEST_1_SET_FRZ, "0"));

    // Verify frozen is 0 again.
    ASSERT_FALSE(strcmp(test_read(TEST_1_CGROUP_EVENTS, 0), "populated - 0\nfrozen - 0\n"));

    // Check changes.
    ASSERT_FALSE(strcmp(test_read(TEST_1_SET_FRZ, 0), "0\n"));
}

TEST(test_frozen_not_running)
{
    // Update frozen.
    ASSERT_TRUE(test_write(TEST_1_SET_FRZ, "1"));

    // Fork here since the process should not be running after we move it inside the cgroup.
    int pid = fork();
    int pidToMove = 0;
    int sum = 0;
    int wstatus;

    // Child
    if (pid == 0) {
        pidToMove = getpid();
        // Save the pid of child in temp file.
        ASSERT_TRUE(test_temp_write(pidToMove));
        // Go to sleep for long period of time.
        sleep(20);
        // At this point, the child process should already be inside the cgroup, therefore, the following operations
        // should not be executed.
        for (int i = 0; i < 10; i++) {
            sum += 1;
        }
        // Save sum into temp file.
        ASSERT_TRUE(test_temp_write(sum));
        exit(0);
    }
    // Father
    else {
        sleep(5);
        // Read the child pid from temp file.
        pidToMove = test_temp_read(0);
        // Update the temp file for further reading, since next sum will be read from it.
        ASSERT_TRUE(test_temp_write(0));

        // Move the child process to "/cgroup/test1" cgroup.
        ASSERT_TRUE(test_move_proc(TEST_1_CGROUP_PROCS, pidToMove));

        // Check that the process we moved is really in "/cgroup/test1" cgroup.
        ASSERT_TRUE(test_pid_in_group(TEST_1_CGROUP_PROCS, pidToMove));

        // Go to sleep to ensure the child process had a chance to be scheduled.
        sleep(10);

        // Verify that the child process have not ran
        sum = test_temp_read(0);
        ASSERT_UINT_EQ(sum, 0);

        // Return the child to root cgroup.
        ASSERT_TRUE(test_move_proc(ROOT_CGROUP_PROCS, pidToMove));

        // Check that the child we returned is really in root cgroup.
        ASSERT_TRUE(test_pid_in_group(ROOT_CGROUP_PROCS, pidToMove));

        // Wait for child to exit.
        wait(&wstatus);
        ASSERT_TRUE(wstatus);

        // Verify that child did execute the procudure.
        sum = test_temp_read(0);
        ASSERT_UINT_EQ(sum, 10);

        // Remove the temp file.
        ASSERT_TRUE(test_temp_delete());

        // Update frozen.
        ASSERT_TRUE(test_write(TEST_1_SET_FRZ, "0"));
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

  // Move the current process to "/cgroup/test1" cgroup so we can
  // compare it's memory usage to the controller's accounting.
  ASSERT_TRUE(test_move_proc(TEST_1_CGROUP_PROCS, getpid()));

  // Check that the process we moved is really in "/cgroup/test1" cgroup.
  ASSERT_TRUE(test_pid_in_group(TEST_1_CGROUP_PROCS, getpid()));

  // Read the contents of current memory file and convert it for comparison.
  strcpy(saved_mem, test_read(TEST_1_MEM_CURRENT, 0));

  // Check memory usaged updated correctly.
  ASSERT_FALSE(strcmp(saved_mem, proc_mem));

  // Return the process to root cgroup.
  ASSERT_TRUE(test_move_proc(ROOT_CGROUP_PROCS, getpid()));

  // Check that the process we returned is really in root cgroup.
  ASSERT_TRUE(test_pid_in_group(ROOT_CGROUP_PROCS, getpid()));
}

TEST(test_correct_mem_account_of_growth_and_shrink) {
  // Save current process memory size.
  char proc_mem[10];
  // Buffer to read contents from memory file.
  char saved_mem[10];

  // Move the current process to "/cgroup/test1" cgroup.
  ASSERT_TRUE(test_move_proc(TEST_1_CGROUP_PROCS, getpid()));

  // Check that the process we moved is really in "/cgroup/test1" cgroup.
  ASSERT_TRUE(test_pid_in_group(TEST_1_CGROUP_PROCS, getpid()));

  // Grow the current process by 100 bytes.
  sbrk(100);

  // Read the contents of current memory file and convert it for comparison.
  strcpy(saved_mem, test_read(TEST_1_MEM_CURRENT, 0));

  // Convert process memory to a string.
  itoa(proc_mem, getmem());
  strcat(proc_mem, "\n");

  // Read the contents of current memory file and convert it for comparison.
  strcpy(saved_mem, test_read(TEST_1_MEM_CURRENT, 0));

  // Check that the memory accounting correctly updated after memory growth.
  ASSERT_FALSE(strcmp(saved_mem, proc_mem));

  // Decrease current proc by 100 bytes.
  sbrk(-100);

  // Read the contents of current memory file and convert it for comparison.
  strcpy(saved_mem, test_read(TEST_1_MEM_CURRENT, 0));

  // Convert process memory to a string.
  itoa(proc_mem, getmem());
  strcat(proc_mem, "\n");

  // Read the contents of current memory file and convert it for comparison.
  strcpy(saved_mem, test_read(TEST_1_MEM_CURRENT, 0));

  // Check that the memory accounting correctly updated after memory growth.
  ASSERT_FALSE(strcmp(saved_mem, proc_mem));

  // Return the process to root cgroup.
  ASSERT_TRUE(test_move_proc(ROOT_CGROUP_PROCS, getpid()));

  // Check that the process we returned is really in root cgroup.
  ASSERT_TRUE(test_pid_in_group(ROOT_CGROUP_PROCS, getpid()));
}

TEST(test_limiting_mem)
{
  // Buffer for saving current memory written in limit
  char saved_mem[12];

  // Enable memory controller
  ASSERT_TRUE(test_enable_controller(MEM_CNT));

  // Copy the current saved memory and remove newline at the end
  strcpy(saved_mem, test_read(TEST_1_MEM_MAX, 0));
  saved_mem[strlen(saved_mem) - 1] = '\0';

  // Update memory limit
  ASSERT_TRUE(test_write(TEST_1_MEM_MAX, "100"));

  // Check changes
  ASSERT_FALSE(strcmp(test_read(TEST_1_MEM_MAX, 0), "100\n"));

  // Restore memory limit to original
  ASSERT_TRUE(test_write(TEST_1_MEM_MAX, saved_mem));

  // Check changes
  ASSERT_FALSE(strncmp(test_read(TEST_1_MEM_MAX, 0), saved_mem, strlen(saved_mem)));

  // Disable memory controller
  ASSERT_TRUE(test_disable_controller(MEM_CNT));
}

TEST(test_cant_move_over_mem_limit)
{
  // Buffer for saving current memory written in limit
  char saved_mem[12];

  // Enable memory controller
  ASSERT_TRUE(test_enable_controller(MEM_CNT));

  // Copy the current saved memory and remove newline at the end
  strcpy(saved_mem, test_read(TEST_1_MEM_MAX, 0));
  saved_mem[strlen(saved_mem) - 1] = '\0';

  // Update memory limit
  ASSERT_TRUE(test_write(TEST_1_MEM_MAX, "0"));

  // Check changes
  ASSERT_FALSE(strcmp(test_read(TEST_1_MEM_MAX, 0), "0\n"));

  // Attemp to move the current process to "/cgroup/test1" cgroup.
  ASSERT_FALSE(test_move_proc(TEST_1_CGROUP_PROCS, getpid()));

  // Check that the current process is not in "/cgroup/test1" cgroup.
  ASSERT_FALSE(test_pid_in_group(TEST_1_CGROUP_PROCS, getpid()));

  // Check that the current process is still in root group.
  ASSERT_TRUE(test_pid_in_group(ROOT_CGROUP_PROCS, getpid()));

  // Restore memory limit to original
  ASSERT_TRUE(test_write(TEST_1_MEM_MAX, saved_mem));

  // Check changes
  ASSERT_FALSE(strncmp(test_read(TEST_1_MEM_MAX, 0), saved_mem, strlen(saved_mem)));

  // Disable memory controller
  ASSERT_TRUE(test_disable_controller(MEM_CNT));
}

TEST(test_cant_fork_over_mem_limit)
{
  // Save current process memory size.
  char proc_mem[10];
  itoa(proc_mem, getmem());
  // Buffer to read contents from memory file.
  char saved_mem[10];

  // Enable memory controller
  ASSERT_TRUE(test_enable_controller(MEM_CNT));

  // Update memory limit
  ASSERT_TRUE(test_write(TEST_1_MEM_MAX, proc_mem));

  // Read the contents of limit file and convert it for comparison.
  strcpy(saved_mem, test_read(TEST_1_MEM_MAX, 0));

  strcat(proc_mem, "\n");

  // Check changes
  ASSERT_FALSE(strcmp(saved_mem, proc_mem));

  // Move the current process to "/cgroup/test1" cgroup.
  ASSERT_TRUE(test_move_proc(TEST_1_CGROUP_PROCS, getpid()));

  // Attempt to fork, notice this operation should fail and return -1.
  ASSERT_UINT_EQ(fork(), -1);

  // Return the process to root cgroup.
  ASSERT_TRUE(test_move_proc(ROOT_CGROUP_PROCS, getpid()));

  // Check that the process we returned is really in root cgroup.
  ASSERT_TRUE(test_pid_in_group(ROOT_CGROUP_PROCS, getpid()));

  // Disable memory controller
  ASSERT_TRUE(test_disable_controller(MEM_CNT));
}

TEST(test_cant_grow_over_mem_limit)
{
  // Save current process memory size.
  char proc_mem[10];
  itoa(proc_mem, getmem());
  // Buffer to read contents from memory file.
  char saved_mem[10];

  // Enable memory controller
  ASSERT_TRUE(test_enable_controller(MEM_CNT));

  // Update memory limit
  ASSERT_TRUE(test_write(TEST_1_MEM_MAX, proc_mem));

  strcat(proc_mem, "\n");

  // Read the contents of limit file and convert it for comparison.
  strcpy(saved_mem, test_read(TEST_1_MEM_MAX, 0));

  // Check changes
  ASSERT_FALSE(strcmp(saved_mem, proc_mem));

  // Move the current process to "/cgroup/test1" cgroup.
  ASSERT_TRUE(test_move_proc(TEST_1_CGROUP_PROCS, getpid()));

  // Attempt to grow process memory, notice this operation should fail and return -1.
  ASSERT_UINT_EQ((int)sbrk(10), -1);

  // Return the process to root cgroup.
  ASSERT_TRUE(test_move_proc(ROOT_CGROUP_PROCS, getpid()));

  // Check that the process we returned is really in root cgroup.
  ASSERT_TRUE(test_pid_in_group(ROOT_CGROUP_PROCS, getpid()));

  // Disable memory controller
  ASSERT_TRUE(test_disable_controller(MEM_CNT));
}

int main(int argc, char * argv[])
{
    // comment out for debug messages
    set_suppress(1);

    run_test(test_mount_cgroup_fs);
    run_test(test_creating_cgroups);
    run_test(test_opening_and_closing_cgroup_files);
    run_test_break_msg(test_reading_cgroup_files);
    run_test(test_moving_process);
    run_test(test_enable_and_disable_all_controllers);
    run_test(test_limiting_pids);
    run_test(test_move_failure);
    run_test(test_fork_failure);
    run_test(test_pid_current);
    run_test(test_setting_cpu_id);
    run_test(test_correct_cpu_running);
    run_test(test_no_run);
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

    printf(1, "[===========]\n");
    if (failed) {
        printf(1, "[  FAILED   ]\n");
    } else {
        printf(1, "[    PASS   ]\n");
    }
    printf(1, "[===========]\n");

    exit(0);
}
