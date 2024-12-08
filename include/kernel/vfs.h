#ifndef __VFS_H_
#define __VFS_H_

#include <errno.h>

#include <types.h>
#include <time.h>

#include <util/tree.h>

#define PATH_SEPARATOR '/'
#define PATH_SEPARATOR_STRING "/"
#define PATH_UP  ".."
#define PATH_DOT "."

#define O_ACCMODE	   0003
#define O_RDONLY	     00
#define O_WRONLY	     01
#define O_RDWR		     02
#ifndef O_CREAT
# define O_CREAT	   0100	/* Not fcntl.  */
#endif
#ifndef O_EXCL
# define O_EXCL		   0200	/* Not fcntl.  */
#endif
#ifndef O_NOCTTY
# define O_NOCTTY	   0400	/* Not fcntl.  */
#endif
#ifndef O_TRUNC
# define O_TRUNC	  01000	/* Not fcntl.  */
#endif
#ifndef O_APPEND
# define O_APPEND	  02000
#endif
#ifndef O_NONBLOCK
# define O_NONBLOCK	  04000
#endif
#ifndef O_NDELAY
# define O_NDELAY	O_NONBLOCK
#endif
#ifndef O_SYNC
# define O_SYNC	       04010000
#endif
#define O_FSYNC		O_SYNC
#ifndef O_ASYNC
# define O_ASYNC	 020000
#endif
#ifndef O_LARGEFILE
# define O_LARGEFILE	0100000
#endif

#ifndef O_DIRECTORY
# define O_DIRECTORY	0200000
#endif
#ifndef O_NOFOLLOW
# define O_NOFOLLOW	0400000
#endif
#ifndef O_CLOEXEC
# define O_CLOEXEC   02000000
#endif
#ifndef O_DIRECT
# define O_DIRECT	 040000
#endif
#ifndef O_NOATIME
# define O_NOATIME   01000000
#endif
#ifndef O_PATH
# define O_PATH     010000000
#endif
#ifndef O_DSYNC
# define O_DSYNC	 010000
#endif
#ifndef O_TMPFILE
# define O_TMPFILE   (020000000 | O_DIRECTORY)
#endif

/* Standard file descriptors.  */
#define	STDIN_FILENO	0	/* Standard input.  */
#define	STDOUT_FILENO	1	/* Standard output.  */
#define	STDERR_FILENO	2	/* Standard error output.  */

#define SEEK_SET	0	/* Seek from beginning of file.  */
#define SEEK_CUR	1	/* Seek from current position.  */
#define SEEK_END	2	/* Seek from end of file.  */

#define FS_FILE        0x01
#define FS_DIRECTORY   0x02
#define FS_CHARDEVICE  0x04
#define FS_BLOCKDEVICE 0x08
#define FS_PIPE        0x10
#define FS_SYMLINK     0x20
#define FS_MOUNTPOINT  0x40
#define FS_SOCKET      0x80

#define _IFMT       0170000 /* type of file */
#define     _IFDIR  0040000 /* directory */
#define     _IFCHR  0020000 /* character special */
#define     _IFBLK  0060000 /* block special */
#define     _IFREG  0100000 /* regular */
#define     _IFLNK  0120000 /* symbolic link */
#define     _IFSOCK 0140000 /* socket */
#define     _IFIFO  0010000 /* fifo */

typedef struct dirent {
	uint32_t d_ino;
	char d_name[256];
} dirent;

struct fs_node;

typedef ssize_t (*read_type_t) (struct fs_node *,  off_t, size_t, uint8_t *);
typedef ssize_t (*write_type_t) (struct fs_node *, off_t, size_t, uint8_t *);
typedef void (*open_type_t) (struct fs_node *, unsigned int flags);
typedef void (*close_type_t) (struct fs_node *);
typedef struct dirent *(*readdir_type_t) (struct fs_node *, unsigned long);
typedef struct fs_node *(*finddir_type_t) (struct fs_node *, char *name);
typedef int (*create_type_t) (struct fs_node *, char *name, mode_t permission);
typedef int (*unlink_type_t) (struct fs_node *, char *name);
typedef int (*mkdir_type_t) (struct fs_node *, char *name, mode_t permission);
typedef int (*ioctl_type_t) (struct fs_node *, unsigned long request, void * argp);
typedef int (*get_size_type_t) (struct fs_node *);
typedef int (*chmod_type_t) (struct fs_node *, mode_t mode);
typedef int (*symlink_type_t) (struct fs_node *, char * name, char * value);
typedef ssize_t (*readlink_type_t) (struct fs_node *, char * buf, size_t size);
typedef int (*selectcheck_type_t) (struct fs_node *);
typedef int (*selectwait_type_t) (struct fs_node *, void * process);
typedef int (*chown_type_t) (struct fs_node *, uid_t, gid_t);
typedef int (*truncate_type_t) (struct fs_node *);

typedef struct fs_node {
	char name[256];         /* The filename. */
	void * device;          /* Device object (optional) */
	mode_t mask;            /* The permissions mask. */
	uid_t uid;              /* The owning user. */
	uid_t gid;              /* The owning group. */
	uint64_t flags;         /* Flags (node type, etc). */
	uint64_t inode;         /* Inode number. */
	uint64_t length;        /* Size of the file, in byte. */
	uint64_t impl;          /* Used to keep track which fs it belongs to. */
	uint64_t open_flags;    /* Flags passed to open (read/write/append, etc.) */

	/* times */
	time_t atime;         /* Accessed */
	time_t mtime;         /* Modified */
	time_t ctime;         /* Created  */

	/* File operations */
	read_type_t read;
	write_type_t write;
	open_type_t open;
	close_type_t close;
	readdir_type_t readdir;
	finddir_type_t finddir;
	create_type_t create;
	mkdir_type_t mkdir;
	ioctl_type_t ioctl;
	get_size_type_t get_size;
	chmod_type_t chmod;
	unlink_type_t unlink;
	symlink_type_t symlink;
	readlink_type_t readlink;
	truncate_type_t truncate;

	struct fs_node *ptr;   /* Alias pointer, for symlinks. */
	int64_t refcount;
	uint64_t nlink;

	selectcheck_type_t selectcheck;
	selectwait_type_t selectwait;

	chown_type_t chown;

	size_t metadata[4];
} fs_node_t;

struct vfs_entry {
	char * name;
	fs_node_t * file;
	char * device;
	char * fs_type;
};

typedef fs_node_t * (*vfs_mount_callback)(const char * arg, const char * mount_point);

void vfs_init(void);
tree_node_t *vfs_mount(const char* mountpoint, fs_node_t *local_root);

int vfs_register_fs(const char * name, vfs_mount_callback callback);

#endif //__VFS_H_