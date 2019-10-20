/* Cgroup filesystem functions.*/

#ifndef XV6_CGFS_H
#define XV6_CGFS_H

#include "cgroup.h"
#include "stat.h"

/**
 * This function opens a cgroup filesystem file or directory.
 * Receives cg_file_type parameter "type", string parameter "filename", cgroup struct pointer parameter "cgp", int parameter "omode".
 * "type" is a cg_file_type that denotes whether we are opening a file or a directory.
 * "filename" is a string containing the filename.
 * "cgp" is a pointer cgroup which the file belongs to.
 * "omode" is the opening mode. Same as with regular files.
 * Return values:
 * -1 on failure.
 * file descriptor of the new open file on success.
 * currently supports opening:
 *    1)    "cgroup.procs"
 *    2)    "cgroup.controllers"
 *    3)    "cgroup.subree_control"
 *    4)    "cgroup.events"
 *    5)    "cgroup.max.descendants"
 *    6)    "cgroup.max.depth"
 *    7)    "cgroup.stat"
 *    8)    cgroup directories
 */
int unsafe_cg_open(cg_file_type type, char * filename, struct cgroup * cgp, int omode);


/**
 * This function reads from cgroup filesystem file or directory.
 * Receives file struct pointer parameter "f", string parameter "addr", int parameter "n".
 * "f" is a pointer to the file we read from.
 * "addr" is a pointer to the string we read the contents into.
 * "n" is the amount of characters to read.
 * Return values:
 * -1 on failure.
 * amount of characters read on success.
 * currently supports reading from:
 *    1)    "cgroup.procs"
 *    2)    "cgroup.controllers"
 *    3)    "cgroup.subree_control"
 *    4)    "cgroup.events"
 *    5)    "cgroup.max.descendants"
 *    6)    "cgroup.max.depth"
 *    7)    "cgroup.stat"
 *    8)    cgroup directories
 */
int unsafe_cg_read(cg_file_type type, struct file * f, char * addr, int n);

/**
 * This function writes to cgroup filesystem file.
 * Receives file struct pointer parameter "f", string parameter "addr", int parameter "n".
 * "f" is a pointer to the file we write to.
 * "addr" is a pointer to the string we write the contents of.
 * "n" is the amount of characters to write.
 * Return values:
 * -1 on failure.
 * amount of characters written on success.
 * currently supports writing to:
 *    1)    "cgroup.procs"
 *    2)    "cgroup.subree_control"
 *    3)    "cgroup.max.descendants"
 *    4)    "cgroup.max.depth"
 */
int unsafe_cg_write(struct file * f, char * addr, int n);

/**
 * This function closes a cgroup filesystem file or directory.
 * Executes fileclose() function from "file.c".
 * Receives file struct pointer parameter "f".
 * "f" is a pointer to the file to be closed.
 * Returns 0 on success.
 */
int unsafe_cg_close(struct file * file);

/**
 * This function extracts the file name from a given path.
 * Receives string parameter "path", string parameter "base_name".
 * "path" is the string containing the path.
 * "base_name" is the string into which we write the file name.
 * Return values:
 * -1 on failure.
 * 0 on success.
 */
int get_base_name(char * path, char * base_name);

/**
 * This function extracts the directory path from a given path.
 * Receives string parameter "path", string parameter "dir_name".
 * "path" is the string containing the path.
 * "dir_name" is the string into which we write the directory path.
 * Return values:
 * -1 on failure.
 * 0 on success.
 */
int get_dir_name(char * path, char * dir_name);


/**
 * This function extracts the directory path and the file name from a given path.
 * Receives string parameter "path", string parameter "dir_path", string parameter "file_name".
 * "path" is the string containing the path.
 * "dir_path" is the string into which we write the directory of the file.
 * "file_name" is the string into which we write the file name.
 * Return values:
 * -1 on failure.
 * 0 on success.
 */
int get_cg_file_dir_path_and_file_name(char * path,
                                       char * dir_path,
                                       char * file_name);

/**
 * This function gets stats of cgorup file or directory.
 * Receives file struct pointer parameter "f", stat struct pointer parameter "st".
 * "f" is a pointer to the we get the stats of.
 * "st" is a pointer struct we write the stats to.
 * Return values:
 * -1 on failure.
 * 0 on success.
 */
int unsafe_cg_stat(struct file * f, struct stat * st);

#endif
