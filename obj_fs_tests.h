/**
 Defines a simple test library.
 To define a new test use `TEST`. For example:
    ```
    TEST(my_test) {
        ASSERT_UINT_EQ(1, 1);
    }
    ```
 To run the tests, use the macro `run_test` in the main and specify the test
 name. For eaxmple:
     ```
     int main() {
        run_test(my_test);
        return 0;
     }
     ```
 */
#include <stdio.h>

void print_error(const char *name, unsigned long int x, unsigned long int y,
                 const char *file, int line) {
    printf("[   FAILED  ] %s - expected %lu but got %lu (%s:%d)\n",
           name, x, y, file, line);
}

#define TEST(test_name) \
    void test_name(const char* name)
#define run_test(test_name) \
    printf("[RUNNING    ] %s\n", #test_name);\
    test_name(#test_name); \
    printf("[       DONE] %s\n", #test_name);
/**
 * NUMERIC VALIDATIONS
 */
#define ASSERT_UINT_EQ(x, y) \
    {\
    uint evaluated_x = (x); \
    uint evaluated_y = (y); \
    if (evaluated_x != evaluated_y) { \
    print_error(name, evaluated_x, evaluated_y, __FILE__, __LINE__);\
    failed = 1; \
    return; \
    }\
    }

#define EXPECT_UINT_EQ(x, y) \
    {\
    uint evaluated_x = (x); \
    uint evaluated_y = (y); \
    if (evaluated_x != evaluated_y) { \
    print_error(name, evaluated_x, evaluated_y, __FILE__, __LINE__);\
    failed = 1; \
    }\
    }
/**
 * BOOLEAN VALIDATIONS
 */
#define ASSERT_TRUE(x) \
    if (!(x)) { \
        printf("[   FAILED  ] %s - expected true for " #x " (%s:%d)\n", \
               name, __FILE__, __LINE__);\
        failed = 1; \
        return; \
    }
#define EXPECT_TRUE(x) \
    if (!(x)) { \
        printf("[   FAILED  ] %s - expected true for " #x " (%s:%d)\n", \
               name, __FILE__, __LINE__);\
        failed = 1; \
    }
#define ASSERT_FALSE(x) \
    if (x) { \
        printf("[   FAILED  ] %s - expected false for " #x "(%s:%d)\n", \
               name, __FILE__, __LINE__);\
        failed = 1; \
        return; \
    }
#define EXPECT_FALSE(x) \
    if (x) { \
        printf("[   FAILED  ] %s - expected false for " #x "(%s:%d)\n", \
               name, __FILE__, __LINE__);\
        failed = 1; \
    }
/**
 * OBJECT FILE SYSTEM SPECIFIC VALIDATIONS
 */
#define ASSERT_NO_ERR(x) \
    ASSERT_UINT_EQ(NO_ERR, (x))
