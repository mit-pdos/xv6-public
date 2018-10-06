/* Note the structure of active_mounts. New mounts are always added to the
 * front, so the mount "parent" referencesare always pointing to entries
 * farther in the back.
 */

struct mount_ns {
  int ref;
  struct spinlock lock; // protects active_mounts
  struct mount* root;
  struct mount_list *active_mounts;
};