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

#include "user.h"

#define REMOVE_2_ADDITIONAL_CHARS 2

void print_error(const char* name, unsigned long int x, unsigned long int y,
                 const char* file, int line) {
    for (int i = 0; i < strlen(name) + strlen("[RUNNING] ") + REMOVE_2_ADDITIONAL_CHARS; i++)
        printf(1, "\b");
    printf(1, "[FAILED] %s - expected %d but got %d (%s:%d)\n",
           name, x, y, file, line);
}


#define TEST(test_name) \
    void test_name(const char* name)


#define run_test(test_name) \
    if (failed == 0) {\
        printf(1,"[RUNNING] %s", #test_name);\
        test_name(#test_name);\
    } \
    if (failed == 0) { \
       for(int i=0; i < strlen(#test_name) + strlen("[RUNNING] ") + REMOVE_2_ADDITIONAL_CHARS; i++)\
           printf(1, "\b");\
       printf(1, "[DONE] %s   \n", #test_name);\
    }

// Prints the exectuion message differently.
#define run_test_break_msg(test_name) \
    if (failed == 0) {\
       printf(1, "[RUNNING] %s\n", #test_name);\
       test_name(#test_name); \
    } \
    if (failed == 0)\
        printf(1, "[DONE] %s\n", #test_name);

/**
 * NUMERIC VALIDATIONS
 */

#define ASSERT_UINT_EQ(x,y) \
    if ((x) != (y)) { \
        print_error(name, x, y, __FILE__, __LINE__);\
        failed = 1; \
        return; \
    }

#define EXPECT_UINT_EQ(x,y) \
    if ((x) != (y)) { \
        print_error(name, x, y, __FILE__, __LINE__);\
        failed = 1; \
    }


/**
 * BOOLEAN VALIDATIONS
 */

#define ASSERT_TRUE(x) \
    if (!(x)) { \
        for(int i=0; i < strlen(#x) + strlen("[RUNNING] ") + REMOVE_2_ADDITIONAL_CHARS; i++)\
            printf(1, "\b");\
        printf(1, "[FAILED] %s - expected true for " #x " (%s:%d)\n", \
               name, __FILE__, __LINE__);\
        failed = 1; \
        return; \
    }

#define EXPECT_TRUE(x) \
    if (!(x)) { \
        for(int i=0; i < strlen(#x) + strlen("[RUNNING] ") + REMOVE_2_ADDITIONAL_CHARS; i++)\
            printf(1, "\b");\
        printf(1, "[FAILED] %s - expected true for " #x " (%s:%d)\n", \
               name, __FILE__, __LINE__);\
        failed = 1; \
    }

#define ASSERT_FALSE(x) \
    if (x) { \
        for(int i=0; i < strlen(#x) + strlen("[RUNNING] ") + REMOVE_2_ADDITIONAL_CHARS; i++)\
            printf(1, "\b");\
        printf(1, "[FAILED] %s - expected false for " #x "(%s:%d)\n", \
               name, __FILE__, __LINE__);\
        failed = 1; \
        return; \
    }

#define EXPECT_FALSE(x) \
    if (x) { \
        for(int i=0; i < strlen(#x) + strlen("[RUNNING] ") + REMOVE_2_ADDITIONAL_CHARS; i++)\
            printf(1, "\b");\
        printf(1, "[FAILED] %s - expected false for " #x "(%s:%d)\n", \
               name, __FILE__, __LINE__);\
        failed = 1; \
    }
