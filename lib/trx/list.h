/* -*- tab-width: 4; indent-tabs-mode: nil -*- */

/*
 * list.h
 *
 * Implements a general purpose doubly linked list and provides a library
 * of useful routines for manipulating this list.
 *
 *
 * Copyright (c) 2008-2011, John D. Blair <jdb@moship.net>
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 *     * Neither the name of John D. Blair nor his lackeys may be used
 *       to endorse or promote products derived from this software
 *       without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * JOHN D. BLAIR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */


#ifndef _LIST_H_
#define _LIST_H_

#define list_head(list) ((list)->head)
#define list_tail(list) ((list)->tail)
#define list_count(list) ((list)->count)

typedef struct list_node {
    void *data;
    struct list_node *next;
    struct list_node *prev;
} list_node_t;

typedef struct list {
    list_node_t *head;          /* first node in the list */
    list_node_t *tail;          /* shortcut to last node in list */
    list_node_t *current;       /* current node - context for iterators */
    int count;
} list_t;

typedef int (*list_node_compare_f)(list_node_t *left,
                                   list_node_t *right);

#define list_node_add(list, new) list_node_insert_after(list, list->tail, new)
#define list_node_add_first(list, new) list_node_insert_before(list, list->head, new)
#define list_push(list, new) list_node_add(list, new)

#define swap(left, right, tmp) {tmp = left; left = right; right = tmp;}

/* macro for iterating over the list
   current must be a pointer to list_node_t */
#define list_for_each(list, current)                                \
    for ((current) = (list)->head; (current) != NULL; (current) = (current)->next)

typedef void (*list_free_data_f)(void *node);

list_t *list_new(void);
void list_dump(list_t *list);
list_node_t *list_node_new(void *data);
void list_free(list_t *list, list_free_data_f free_data);
void list_free_nodes(list_t *list, list_free_data_f free_data);
void list_node_free(list_t *list, list_node_t *node);
int list_node_del(list_t *list, list_node_t *goner);
int list_node_insert_after(list_t *list, list_node_t *where, list_node_t *new);
int list_node_insert_before(list_t *list, list_node_t *where, list_node_t *new);
void list_reverse(list_t *list);
void list_append(list_t *list, list_t *append);
void list_prepend(list_t *list, list_t *prepend);
list_node_t * list_iter(list_t *list,
			int (*iter)(list_node_t *node, void *context),
			void *context);
list_node_t * list_reverse_iter(list_t *list,
				int (*iter)(list_node_t *node, void *context),
				void *context);
list_node_t ** list_map_to_array(list_t *list);
int list_map_from_array(list_t *list, list_node_t **array);
int list_quicksort(list_t *list, list_node_compare_f compare);

#ifdef LIST_SHUFFLE
void list_shuffle(list_t *list, int (*myrand)(void));
#endif /* LIST_SHUFFLE */

list_node_t *list_get_node_number(list_t *list, unsigned int n);
list_t *list_split_new(list_t *list);

#endif /* _LIST_H_ */
