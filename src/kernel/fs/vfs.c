#include <vfs.h>

#include <mem.h>

#include <tools.h>

#include <util/list.h>
#include <util/hashmap.h>

tree_t    * fs_tree = NULL; /* File system mountpoint tree */
fs_node_t * fs_root = NULL; /* Pointer to the root mount fs_node */

hashmap_t * fs_types = NULL;

void vfs_init(void) {
    fs_tree = tree_create();
    fs_types = hashmap_create(8);

	struct vfs_entry * root = kmalloc(sizeof(struct vfs_entry));

	root->name = strdup("$root");
	root->file = NULL;
	root->fs_type = NULL;
	root->device = NULL;

	tree_set_root(fs_tree, root);
}

int vfs_register_fs(const char * name, vfs_mount_callback callback) {
	if (hashmap_get(fs_types, name)) return 1;
	hashmap_set(fs_types, name, (void *)(uintptr_t)callback);
	return 0;
}

tree_node_t *vfs_mount(const char* mountpoint, fs_node_t *local_root) {
    if(fs_tree == NULL) {
        debug_log(ERROR, "VFS hasn't been initialized. You can't mount this now.");
        return NULL;
    }

    if(mountpoint == NULL || mountpoint[0] != '/') {
        debug_log(ERROR, "Invalid mountpoint.");
        return NULL;
    }

    tree_node_t* ret_val;

    if(!strcmp(mountpoint, "/")) {
        struct vfs_entry * root = (struct vfs_entry *)fs_tree->root->value;
        if(root->file != NULL) {
            debug_log(WARNING, "The root is already mounted, it is recommended to unmount it.");
        }
        root->file = local_root;
		fs_root = local_root;
		ret_val = fs_tree->root;
    } else {
        tree_node_t * node = fs_tree->root;
        char* point = strdup(mountpoint+1);
        char *saveptr, *token;

        token = strtok_r(point+1, "/", &saveptr);
        debug_log(NOTICE, "Searching for %s", mountpoint);
        while(token != NULL) {
            bool found = false;

            foreach(child, node->children) {
                tree_node_t * ch = (tree_node_t *)child->value;
                struct vfs_entry * ent = (struct vfs_entry *)ch->value;
                if(!strcmp(ent->name, token)) {
                    found = true;
                    node = ch;
                    ret_val = node;
                    break;
                }
            }

            if(!found) {
                debug_log(NOTICE, "Did not find %s, making it.", token);
				struct vfs_entry * ent = kmalloc(sizeof(struct vfs_entry));
				ent->name = strdup(token);
				ent->file = NULL;
				ent->device = NULL;
				ent->fs_type = NULL;
				node = tree_node_insert_child(fs_tree, node, ent);
            }

            token = strtok_r(NULL, "/", &saveptr);
        }

        kfree(point);
    }

    return ret_val;
}

int vfs_mount_fs(const char* type, const char* dev, const char* mountpoint) {
    vfs_mount_callback mount = hashmap_get(fs_types, type);

    if(mount == NULL) {
        debug_log(ERROR, "Unknown filesystem (%s).", type);
        return ENODEV;
    }

    fs_node_t *local_root = mount(dev, mountpoint);

    if(local_root == NULL) {
        return EINVAL;
    }
}