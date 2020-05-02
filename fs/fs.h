#ifndef _FS_FS_H_
#define _FS_FS_H_ 1

#ifndef _SYSTEM
#define _SYSTEM
#endif

#ifdef FS_CMAKE

#include "cmake/cmake_util.h"
#define kprintf(...) printf(__VA_ARGS__)
#define KDEBUG(token)   printf("[SYSTEM] "); printf token

#else

#include <winix/kdebug.h>

#endif

#include <curses.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/direct.h>
#include <sys/unistd.h>
#include <fs/type.h>
#include <fs/const.h>
#include <winix/type.h>
#include <fs/inode.h>
#include <fs/filp.h>
#include <kernel/proc.h>
#include "cache.h"
#include <fs/dev.h>
#include "path.h"
#include <fs/super.h>
#include <string.h>
#include <winix/bitmap.h>
#include <winix/page.h>
#include <winix/compiler.h>
#include <stdbool.h>
#include <string.h>

#define VERIFY_READ     1
#define VERIFY_WRITE    2
#define ROOT_DEV    (0x0101)    /* MAKEDEV(1,1) */

#define SIZE (64 * 1024)
extern size_t DISK_SIZE;
extern disk_word_t DISK_RAW[SIZE];

#define SET_CALLER(pcurr)   (curr_user_proc_in_syscall = pcurr)

int sys_open(struct proc *who, char *path,int flags, mode_t mode);
int sys_read(struct proc *who, int fd, void *buf, size_t count);
int sys_write(struct proc *who, int fd, void *buf, size_t count);
int sys_close(struct proc *who, int fd);
int sys_pipe(struct proc* who, int fd[2]);
int sys_chmod(struct proc* who, const char *pathname, mode_t mode);
int sys_chown(struct proc* who, const char *pathname, uid_t owner, gid_t group);
int sys_chdir(struct proc* who, char* pathname);
int sys_dup(struct proc* who, int oldfd);
int sys_umask(struct proc* who, mode_t mask);
int sys_lseek(struct proc* who, int fd, off_t offset, int whence);

bool check_access(struct proc* who, struct inode* ino, mode_t mode);
int get_inode_by_path(struct proc* who, char *path, struct inode** inode);
block_t alloc_block(inode_t *ino, struct device* id);
int makefs( disk_word_t* disk_raw, disk_word_t disk_size_words);
void init_fs(disk_word_t* disk_raw, disk_word_t disk_size_words);
int init_filp_by_inode(struct filp* filp, struct inode* inode);
int init_inode_non_disk(struct inode* ino, ino_t num, struct device* dev, struct superblock* sb);
void init_pipe();
int remove_inode_from_dir(struct inode* dir, struct inode* target);
int get_fd(struct proc *curr, int start, int *open_slot, filp_t **fpt);
int add_inode_to_directory(inode_t* dir, inode_t* ino, char* string);
int register_device(struct device* dev, const char* name, dev_t id, mode_t type);
int release_filp(struct filp* file);
int release_inode(inode_t *inode);
filp_t *get_filp(int fd);
filp_t *find_filp(inode_t *inode);
filp_t *get_free_filp();
void init_filp();
struct inode* get_free_inode_slot();
struct device* get_dev(dev_t dev);
int sys_creat(struct proc* who, char* path, mode_t mode);
size_t get_inode_total_size_word(struct inode* ino);
blkcnt_t get_inode_blocks(struct inode* ino);
struct superblock* get_sb(struct device* id);
void init_inodetable();
inode_t* read_inode(int num, struct device*);
inode_t* get_inode(int num, struct device*);
int put_inode(inode_t *inode, bool is_dirty);
inode_t* alloc_inode(struct proc* who, struct device*);
void init_inode();

void* kmalloc(unsigned int size);
void kfree(void *ptr);
int kprintf(const char *format, ...);


#endif

