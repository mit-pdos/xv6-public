#include <stdlib.h>
#include <string.h>

#include "obj_disk.h"
#include "obj_cache.h"
#include "obj_log.h"
#include "obj_fs_tests.h"


/**
 * Tests library variables
 */
int failed = 0;


/**
 * Disk layer tests
 */


const uint initial_objects_table_bytes =
    INITIAL_OBJECT_TABLE_SIZE * sizeof(ObjectsTableEntry);


/**
 * Tests that the metadata values inside the super block are intiialized
 * correctly by calling the get functions.
 */
TEST(initialization) {
    EXPECT_UINT_EQ(2, occupied_objects());
    EXPECT_UINT_EQ(STORAGE_DEVICE_SIZE, device_size());
    EXPECT_UINT_EQ(sizeof(struct objsuperblock) + initial_objects_table_bytes,
                   occupied_bytes());
}


/**
 * Validates the correctness of getting the super block object.
 */
TEST(super_block_object) {
    uint size;
    ASSERT_NO_ERR(object_size(SUPER_BLOCK_ID, &size));
    ASSERT_UINT_EQ(sizeof(struct objsuperblock), size);

    struct objsuperblock sb;
    ASSERT_NO_ERR(get_object(SUPER_BLOCK_ID, &sb));

    EXPECT_UINT_EQ(STORAGE_DEVICE_SIZE, sb.storage_device_size);
    EXPECT_UINT_EQ(sizeof(struct objsuperblock), sb.objects_table_offset);
    EXPECT_UINT_EQ(2, sb.occupied_objects);
    EXPECT_UINT_EQ(sizeof(struct objsuperblock) + initial_objects_table_bytes,
                  sb.bytes_occupied);
}


/**
 * Validates the correctness of the objects table initial state by getting the
 * objects table object. We don't check the entries themsels because they are
 * checked by calling `get_object`. We only left the check that the rest of
 * the table is empty.
 */
TEST(table_object) {
    uint size;
    ASSERT_NO_ERR(object_size(OBJECT_TABLE_ID, &size));
    ASSERT_UINT_EQ(initial_objects_table_bytes, size)

    ObjectsTableEntry table[OBJECTS_TABLE_SIZE];
    ASSERT_NO_ERR(get_object(OBJECT_TABLE_ID, &table));
    for (size_t i = 3; i < OBJECTS_TABLE_SIZE; ++i) {
        EXPECT_FALSE(table[i].occupied);
    }

    EXPECT_TRUE(table[0].occupied)
    EXPECT_UINT_EQ(sizeof(struct objsuperblock), table[0].size)
    EXPECT_TRUE(table[1].occupied)
    EXPECT_UINT_EQ(initial_objects_table_bytes, table[1].size)
}


TEST(get_object_with_name_too_long) {
    uint size;
    EXPECT_UINT_EQ(OBJECT_NAME_TOO_LONG,
                   object_size("012345678901234567890123456789", &size));
}


TEST(get_non_existing_object) {
    uint size;
    EXPECT_UINT_EQ(OBJECT_NOT_EXISTS,
                   object_size("non_existing", &size));
}


TEST(add_single_object) {
    char my_string[] = "my super amazing string";
    ASSERT_NO_ERR(add_object(
        my_string, strlen(my_string) + 1, "simple_string"
    ));
    uint size;
    ASSERT_NO_ERR(object_size("simple_string", &size));
    ASSERT_UINT_EQ(strlen(my_string) + 1, size);
    char actual[strlen(my_string) + 1];
    ASSERT_NO_ERR(get_object("simple_string", actual));
    ASSERT_UINT_EQ(0, strcmp(actual, my_string));
}

TEST(add_object_already_exist) {
    uint placeholder = 0;
    ASSERT_NO_ERR(
        add_object(&placeholder, sizeof(placeholder), "already_exist_0")
    );
    ASSERT_UINT_EQ(
        OBJECT_EXISTS,
        add_object(&placeholder, sizeof(placeholder), "already_exist_0")
    );
}


TEST(delete_existing_object) {
    uint placeholder = 0;
    ASSERT_NO_ERR(
        add_object(&placeholder, sizeof(placeholder), "delete_0")
    );
    uint size;
    ASSERT_NO_ERR(object_size("delete_0", &size));
    ASSERT_NO_ERR(delete_object("delete_0"));
    ASSERT_UINT_EQ(OBJECT_NOT_EXISTS, object_size("delete_0", &size));
}


TEST(delete_no_existing_object) {
    ASSERT_UINT_EQ(OBJECT_NOT_EXISTS, delete_object("not_existing"));
}


TEST(rewrite_existing_object_with_shorter_data) {
    char first_string[] = "0123456789";
    char second_string[] = "abcdef";
    ASSERT_NO_ERR(add_object(
        first_string, strlen(first_string) + 1, "rewrite_shorter"
    ));

    //sanity check
    uint size;
    ASSERT_NO_ERR(object_size("rewrite_shorter", &size));
    ASSERT_UINT_EQ(strlen(first_string) + 1, size);

    //rewrite
    ASSERT_NO_ERR(rewrite_object(
        second_string, strlen(second_string) + 1, "rewrite_shorter"
    ));

    //validate the new size and data
    ASSERT_NO_ERR(object_size("rewrite_shorter", &size));
    ASSERT_UINT_EQ(strlen(second_string) + 1, size);
    char actual[strlen(second_string) + 1];
    ASSERT_NO_ERR(get_object("rewrite_shorter", actual));
    ASSERT_UINT_EQ(0, strcmp(actual, second_string));
}


TEST(rewrite_existing_object_with_longer_data) {
    char first_string[] = "0123456789";
    char second_string[] = "01234567890123456789";
    ASSERT_NO_ERR(add_object(
        first_string, strlen(first_string) + 1, "rewrite_longer"
    ));

    //sanity check
    uint size;
    ASSERT_NO_ERR(object_size("rewrite_longer", &size));
    ASSERT_UINT_EQ(strlen(first_string) + 1, size);

    //rewrite
    ASSERT_NO_ERR(rewrite_object(
        second_string, strlen(second_string) + 1, "rewrite_longer"
    ));

    //validate the new size and data
    ASSERT_NO_ERR(object_size("rewrite_longer", &size));
    ASSERT_UINT_EQ(strlen(second_string) + 1, size);
    char actual[strlen(second_string) + 1];
    ASSERT_NO_ERR(get_object("rewrite_longer", actual));
    ASSERT_UINT_EQ(0, strcmp(actual, second_string));
}


TEST(writing_multiple_objects) {
    const char* objects_data[3] = {
        "first data",
        "second data",
        "third data"
    };
    const char* objects_name[3] = {
        "writing multiple 1",
        "writing multiple 2",
        "writing multiple 3",
    };
    for (uint i = 0; i < 3; ++i) {
        ASSERT_NO_ERR(add_object(
            objects_data[i], strlen(objects_data[i]) + 1, objects_name[i]
        ));
    }
    for (uint i = 0; i < 3; ++i) {
        uint size;
        ASSERT_NO_ERR(object_size(objects_name[i], &size));
        ASSERT_UINT_EQ(strlen(objects_data[i]) + 1, size);
        char actual_data[size];
        ASSERT_NO_ERR(get_object(objects_name[i], actual_data));
        ASSERT_TRUE(strcmp(objects_data[i], actual_data) == 0);
    }
}

TEST(add_to_full_table) {
    ObjectsTableEntry original[OBJECTS_TABLE_SIZE];
    ObjectsTableEntry table[OBJECTS_TABLE_SIZE];
    ASSERT_NO_ERR(get_object(OBJECT_TABLE_ID, &table));
    memcpy(original, table, sizeof(table));
    for (uint i = 0; i < OBJECTS_TABLE_SIZE; ++i) {
        table[i].occupied = 1;
    }
    ASSERT_NO_ERR(rewrite_object(table, sizeof(table), OBJECT_TABLE_ID));

    uint data = 0;
    ASSERT_UINT_EQ(
        OBJECTS_TABLE_FULL,
        add_object(&data, sizeof(data), "non existing object")
    );
    ASSERT_NO_ERR(rewrite_object(original, sizeof(original), OBJECT_TABLE_ID));
}


uint find_object_offset(const char* object_name) {
    ObjectsTableEntry table[OBJECTS_TABLE_SIZE];
    get_object(OBJECT_TABLE_ID, &table);
    uint address = -1;
    for (uint i = 0; i < OBJECTS_TABLE_SIZE; ++i) {
        if (strcmp(table[i].object_id, object_name) == 0) {
            address = table[i].disk_offset;
            break;
        }
    }
    return address;
}


TEST(reusing_freed_space) {
    uint data = 0;
    ASSERT_NO_ERR(add_object(&data, sizeof(data), "reusing object 1"));
    uint obj_1_offset = find_object_offset("reusing object 1");
    ASSERT_TRUE(obj_1_offset != -1);

    ASSERT_NO_ERR(delete_object("reusing object 1"));
    ASSERT_NO_ERR(add_object(&data, sizeof(data), "reusing object 2"));
    uint obj_2_offset = find_object_offset("reusing object 2");
    ASSERT_UINT_EQ(obj_1_offset, obj_2_offset);
}


TEST(add_when_there_is_no_more_disk_left) {
    ASSERT_UINT_EQ(
        NO_DISK_SPACE_FOUND,
        add_object(NULL, STORAGE_DEVICE_SIZE + 1, "object too large")
    );
}


/**
 * The following tests validate the correctness of the cache layer.
 * The tests use the `objects_cache_hits` and `objects_cache_misses` methods
 * to check the cache behavior vs the expected flow.
 */


TEST(get_object_in_cache) {
    char my_string[] = "my super amazing string";
    const char* obj_name = "get_object_in_cache";
    // inserting the object through the cache keeps it inside it
    cache_add_object(my_string, strlen(my_string) + 1, obj_name);

    uint misses_at_start = objects_cache_misses();
    uint hits_at_start = objects_cache_hits();

    // validate correctness
    char actual[strlen(my_string) + 1];
    ASSERT_NO_ERR(cache_get_object(name, actual));
    ASSERT_UINT_EQ(0, strcmp(actual, my_string));

    // valiadte hits and misses
    EXPECT_UINT_EQ(0, objects_cache_misses() - misses_at_start);
    EXPECT_UINT_EQ(1, objects_cache_hits() - hits_at_start);
}


TEST(get_object_not_in_cache) {
    char my_string[] = "my super amazing string";
    const char* obj_name = "object_no_cache_00";
    // inserting the object WITHOUT going through the cache
    add_object(my_string, strlen(my_string) + 1, obj_name);

    uint misses_at_start = objects_cache_misses();
    uint hits_at_start = objects_cache_hits();

    // validate correctness
    char actual[strlen(my_string) + 1];
    ASSERT_NO_ERR(cache_get_object(obj_name, actual));
    ASSERT_UINT_EQ(0, strcmp(actual, my_string));

    // valiadte hits and misses
    EXPECT_UINT_EQ(1, objects_cache_misses() - misses_at_start);
    ASSERT_UINT_EQ(0, objects_cache_hits() - hits_at_start);

    // re-accessing the object now set "hit" because it's in the cache from
    // previous attempt.
    ASSERT_NO_ERR(cache_get_object(obj_name, actual));
    EXPECT_UINT_EQ(1, objects_cache_misses() - misses_at_start);
    ASSERT_UINT_EQ(1, objects_cache_hits() - hits_at_start);
}


TEST(get_object_size_in_cache) {
    char my_string[] = "my super amazing string";
    const char* obj_name = "object_in_cache_01";
    // inserting the object WITH going through the cache
    cache_add_object(my_string, strlen(my_string) + 1, obj_name);

    uint misses_at_start = objects_cache_misses();
    uint hits_at_start = objects_cache_hits();

    ASSERT_NO_ERR(cache_get_object(obj_name, my_string));

    // valiadte hits and misses
    EXPECT_UINT_EQ(0, objects_cache_misses() - misses_at_start);
    ASSERT_UINT_EQ(1, objects_cache_hits() - hits_at_start);
}


TEST(get_object_size_not_in_cache_and_doesnt_add_to_cache) {
    char my_string[] = "my super amazing string";
    const char* obj_name = "object_no_cache_02";
    // inserting the object WITHOUT going through the cache
    add_object(my_string, strlen(my_string) + 1, obj_name);

    uint misses_at_start = objects_cache_misses();
    uint hits_at_start = objects_cache_hits();

    // validate correctness
    uint size;
    ASSERT_NO_ERR(cache_object_size(obj_name, &size));
    ASSERT_UINT_EQ(strlen(my_string) + 1, size);

    // valiadte hits and misses
    EXPECT_UINT_EQ(1, objects_cache_misses() - misses_at_start);
    ASSERT_UINT_EQ(0, objects_cache_hits() - hits_at_start);

    ASSERT_NO_ERR(cache_object_size(obj_name, &size));
    EXPECT_UINT_EQ(2, objects_cache_misses() - misses_at_start);
    ASSERT_UINT_EQ(0, objects_cache_hits() - hits_at_start);
}


TEST(object_too_large_not_inserted_to_cache) {
    char large_data[cache_max_object_size() * 2];
    const char* obj_name = "object_no_cache_03";

    for (uint i = 0; i < sizeof(large_data); ++i) {
        large_data[i] = i % 256;
    }

    uint misses_at_start = objects_cache_misses();
    uint hits_at_start = objects_cache_hits();

    cache_add_object(large_data, sizeof(large_data), obj_name);

    // validate correctness
    char actual[sizeof(large_data)];
    ASSERT_NO_ERR(cache_get_object(obj_name, actual));
    for (uint i = 0; i < sizeof(large_data); ++i) {
        ASSERT_UINT_EQ(large_data[i], actual[i]);
    }

    // valiadte hits and misses
    EXPECT_UINT_EQ(1, objects_cache_misses() - misses_at_start);
    ASSERT_UINT_EQ(0, objects_cache_hits() - hits_at_start);
}


/**
 * Logbook layer tests
 */


TEST(logbook_add_object_regular_flow) {
    const char* obj_name = "log_add_test_obj";
    char my_string[] = "my super amazing string";
    ASSERT_NO_ERR(log_add_object(
        my_string, strlen(my_string) + 1, obj_name
    ));
    uint size;
    ASSERT_NO_ERR(cache_object_size(obj_name, &size));
    ASSERT_UINT_EQ(strlen(my_string) + 1, size);
    char actual[strlen(my_string) + 1];
    ASSERT_NO_ERR(cache_get_object(obj_name, actual));
    ASSERT_UINT_EQ(0, strcmp(actual, my_string));
}


TEST(logbook_rewrite_object_regular_flow) {
    const char* obj_name = "log_rw_test_obj";
    char first_string[] = "0123456789";
    char second_string[] = "abcdef";
    ASSERT_NO_ERR(log_add_object(
        first_string, strlen(first_string) + 1, obj_name
    ));

    uint before, after;
    get_objects_table_index(obj_name, &before);
    //rewrite
    ASSERT_NO_ERR(log_rewrite_object(
        second_string, strlen(second_string) + 1, obj_name
    ));
    get_objects_table_index(obj_name, &after);

    //validate the new size and data
    uint size;
    ASSERT_NO_ERR(cache_object_size(obj_name, &size));
    ASSERT_UINT_EQ(strlen(second_string) + 1, size);
    char actual[strlen(second_string) + 1];
    ASSERT_NO_ERR(cache_get_object(obj_name, actual));
}


TEST(logbook_delete_object_regular_flow) {
    const char* object_name = "log delete obj";
    uint placeholder = 0;
    ASSERT_NO_ERR(
        log_add_object(&placeholder, sizeof(placeholder), object_name)
    );
    ASSERT_NO_ERR(log_delete_object(object_name));
    uint size;
    ASSERT_UINT_EQ(OBJECT_NOT_EXISTS, cache_object_size(object_name, &size));
}


int main() {
    printf("[===========]\n");
    init_obj_fs();
    init_objects_cache();

    // Driver layer
    run_test(initialization);
    run_test(super_block_object);
    run_test(table_object);
    run_test(add_single_object);
    run_test(add_object_already_exist);
    run_test(delete_existing_object);
    run_test(rewrite_existing_object_with_shorter_data);
    run_test(rewrite_existing_object_with_longer_data);
    run_test(writing_multiple_objects);
    run_test(get_object_with_name_too_long);
    run_test(get_non_existing_object);
    run_test(add_to_full_table);
    run_test(reusing_freed_space);
    run_test(add_when_there_is_no_more_disk_left);

    // Cache layer
    run_test(get_object_in_cache);
    run_test(get_object_not_in_cache);
    run_test(get_object_size_in_cache);
    run_test(get_object_size_not_in_cache_and_doesnt_add_to_cache);
    run_test(object_too_large_not_inserted_to_cache);

    // Logbook layer
    run_test(logbook_add_object_regular_flow);
    run_test(logbook_rewrite_object_regular_flow);
    run_test(logbook_delete_object_regular_flow);

    // Summary
    printf("[===========]\n");
    if (failed) {
        printf("[  FAILED   ]\n");
    } else {
        printf("[    PASS   ]\n");
    }
    printf("[===========]\n");
    return failed;
}
