/**
 * @brief General-purpose list implementations.
 * @author K. Lange
 *
 * @copyright
 * This file is part of ToaruOS and/or MountainOS and is released under the terms
 * of the NCSA / University of Illinois License - see https://github.com/klange/toaruos/blob/master/LICENSE
 * Copyright (C) 2011-2021 K. Lange
 */

#include <util/list.h>
#include <mem.h>
#include <assert.h>

void list_destroy(list_t * list) {
	/* Free all of the contents of a list */
	node_t * n = list->head;
	while (n) {
		kfree(n->value);
		n = n->next;
	}
}

void list_free(list_t * list) {
	/* Free the actual structure of a list */
	node_t * n = list->head;
	while (n) {
		node_t * s = n->next;
		kfree(n);
		n = s;
	}
}

void list_append(list_t * list, node_t * mnode) {
	//assert(!(mnode->next || mnode->prev) && "mnode is already in a list.");
	mnode->next = NULL;
	/* Insert a mnode onto the end of a list */
	mnode->owner = list;
	if (!list->length) {
		list->head = mnode;
		list->tail = mnode;
		mnode->prev = NULL;
		mnode->next = NULL;
		list->length++;
		return;
	}
	list->tail->next = mnode;
	mnode->prev = list->tail;
	list->tail = mnode;
	list->length++;
}

node_t * list_insert(list_t * list, void * item) {
	/* Insert an item into a list */
	node_t * mnode = kmalloc(sizeof(node_t));
	mnode->value = item;
	mnode->next  = NULL;
	mnode->prev  = NULL;
	mnode->owner = NULL;
	list_append(list, mnode);

	return mnode;
}

void list_append_after(list_t * list, node_t * before, node_t * mnode) {
	//assert(!(mnode->next || mnode->prev) && "mnode is already in a list.");
	mnode->owner = list;
	if (!list->length) {
		list_append(list, mnode);
		return;
	}
	if (before == NULL) {
		mnode->next = list->head;
		mnode->prev = NULL;
		list->head->prev = mnode;
		list->head = mnode;
		list->length++;
		return;
	}
	if (before == list->tail) {
		list->tail = mnode;
	} else {
		before->next->prev = mnode;
		mnode->next = before->next;
	}
	mnode->prev = before;
	before->next = mnode;
	list->length++;
}

node_t * list_insert_after(list_t * list, node_t * before, void * item) {
	node_t * mnode = kmalloc(sizeof(node_t));
	mnode->value = item;
	mnode->next  = NULL;
	mnode->prev  = NULL;
	mnode->owner = NULL;
	list_append_after(list, before, mnode);
	return mnode;
}

void list_append_before(list_t * list, node_t * after, node_t * mnode) {
	//assert(!(mnode->next || mnode->prev) && "mnode is already in a list.");
	mnode->owner = list;
	if (!list->length) {
		list_append(list, mnode);
		return;
	}
	if (after == NULL) {
		mnode->next = NULL;
		mnode->prev = list->tail;
		list->tail->next = mnode;
		list->tail = mnode;
		list->length++;
		return;
	}
	if (after == list->head) {
		list->head = mnode;
	} else {
		after->prev->next = mnode;
		mnode->prev = after->prev;
	}
	mnode->next = after;
	after->prev = mnode;
	list->length++;
}

node_t * list_insert_before(list_t * list, node_t * after, void * item) {
	node_t * mnode = kmalloc(sizeof(node_t));
	mnode->value = item;
	mnode->next  = NULL;
	mnode->prev  = NULL;
	mnode->owner = NULL;
	list_append_before(list, after, mnode);
	return mnode;
}

list_t * list_create(const char * name, const void * metadata) {
	/* Create a fresh list */
	list_t * out = kmalloc(sizeof(list_t));
	out->head = NULL;
	out->tail = NULL;
	out->length = 0;
	out->name = name;
	out->metadata = metadata;
	return out;
}

node_t * list_find(list_t * list, void * value) {
	foreach(item, list) {
		if (item->value == value) {
			return item;
		}
	}
	return NULL;
}

int list_index_of(list_t * list, void * value) {
	int i = 0;
	foreach(item, list) {
		if (item->value == value) {
			return i;
		}
		i++;
	}
	return -1; /* not find */
}

void * list_index(list_t * list, int index) {
	int i = 0;
	foreach(item, list) {
		if (i == index) return item->value;
		i++;
	}
	return NULL;
}

void list_remove(list_t * list, size_t index) {
	/* remove index from the list */
	if (index > list->length) return;
	size_t i = 0;
	node_t * n = list->head;
	while (i < index) {
		n = n->next;
		i++;
	}
	list_delete(list, n);
}

void list_delete(list_t * list, node_t * mnode) {
	/* remove mnode from the list */
	//assert(mnode->owner == list && "Tried to remove a list mnode from a list it does not belong to.");
	if (mnode == list->head) {
		list->head = mnode->next;
	}
	if (mnode == list->tail) {
		list->tail = mnode->prev;
	}
	if (mnode->prev) {
		mnode->prev->next = mnode->next;
	}
	if (mnode->next) {
		mnode->next->prev = mnode->prev;
	}
	mnode->prev = NULL;
	mnode->next = NULL;
	mnode->owner = NULL;
	list->length--;
}

node_t * list_pop(list_t * list) {
	/* Remove and return the last value in the list
	 * If you don't need it, you still probably want to free it!
	 * Try free(list_pop(list)); !
	 * */
	if (!list->tail) return NULL;
	node_t * out = list->tail;
	list_delete(list, out);
	return out;
}

node_t * list_dequeue(list_t * list) {
	if (!list->head) return NULL;
	node_t * out = list->head;
	list_delete(list, out);
	return out;
}

list_t * list_copy(list_t * original) {
	/* Create a new copy of original */
	list_t * out = list_create(original->name, original->metadata);
	node_t * mnode = original->head;
	while (mnode) {
		list_insert(out, mnode->value);
	}
	return out;
}

void list_merge(list_t * target, list_t * source) {
	/* Destructively merges source into target */
	foreach(mnode, source) {
		mnode->owner = target;
	}
	if (source->head) {
		source->head->prev = target->tail;
	}
	if (target->tail) {
		target->tail->next = source->head;
	} else {
		target->head = source->head;
	}
	if (source->tail) {
		target->tail = source->tail;
	}
	target->length += source->length;
	kfree(source);
}




