#include "types.h"
#include "user.h"
#include "fcntl.h"

void runCommand(char *command, char **argv) {
    int pid = fork();
    if (pid < 0) {
        printf(1, "Fork failed\n");
        return;
    }
    if (pid == 0) {
        exec(command, argv);
        printf(1, "exec %s failed\n", command);
        exit();
    } else {
        wait();
    }
}

int main(void) {
    int start_ticks, end_ticks;

    // Record start time
    start_ticks = uptime();

    // Run stressfs
    char *stressfs_argv[] = {"stressfs", 0};
    runCommand("stressfs", stressfs_argv);

    // Run find
    char *find_argv[] = {"find", "/", 0};
    runCommand("find", find_argv);

    // Run cat README | uniq
    char *cat_argv[] = {"cat", "README", 0};
    runCommand("cat", cat_argv); // Assuming the 'uniq' part is handled in shell

    // Run a non-trivial command of your choosing
    // Example: char *custom_argv[] = {"your_command", "arg1", "arg2", 0};
    // runCommand("your_command", custom_argv);

    // Record end time
    end_ticks = uptime();

    // Calculate and print the total time taken
    printf(1, "Total ticks elapsed: %d\n", end_ticks - start_ticks);

    exit();
}
