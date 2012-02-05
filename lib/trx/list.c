/* -*- tab-width: 4; indent-tabs-mode: nil -*- */

/*
 * list.c
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

#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "list.h"
#include "debug.h"

static void _quicksort(list_node_t **array, int p, int r,
                            int (*compare)(list_node_t *left,
                                           list_node_t *right));
static int _quicksort_partition(list_node_t **array, int p, int r,
                                     int (*compare)(list_node_t *left,
                                                    list_node_t *right));

void list_dump(list_t *list)
{
    printf("list_t:  %p\n", list);
    printf("head:    %p\n", list->head);
    printf("tail:    %p\n", list->tail);
    printf("current: %p\n", list->current);
    printf("count:   %d\n", list->count);
}
    

/* create a new list */
list_t *list_new(void)
{
    list_t *new = malloc(sizeof(list_t));
    new->head = NULL;
    new->tail = NULL;
    new->current = NULL;
    new->count = 0;

    return new;
}


/* create a new node */
list_node_t *list_node_new(void *data)
{
    assert(data != NULL);

    list_node_t *new = malloc(sizeof(list_node_t));
    if (new == NULL) {
        return NULL;
    }
        
    new->data = data;
    new->next = NULL;
    new->prev = NULL;

    return new;
}


/* free a list node.  be sure to also free the data pointed to by the node */
void list_node_free(list_t *list, list_node_t *node)
{
    assert(list != NULL);
    assert(node != NULL);

    free(node);
}


void list_free(list_t *list, list_free_data_f free_data) {
    list_free_nodes(list, free_data);
    free(list);
}


/* free all list nodes, but leave the list structure intact (but empty)
   use free_data() to free each node's data  */
void list_free_nodes(list_t *list, list_free_data_f free_data)
{
    list_node_t *current = list->head;

    if (! free_data) {
        free_data = free;
    }
    
    while (current) {
        list_node_t *next = current->next;
        free_data(current->data);
        free(current);
        current = next;
    }

    list->head = NULL;
    list->tail = NULL;
    list->current = NULL;
    list->count = 0;
}


/* remove the specified node from the list
   returns the number of nodes in the list */
int list_node_del(list_t *list, list_node_t *goner)
{
    assert(list != NULL);
    assert(goner != NULL);

    if (list->head == goner) {
        list->head = goner->next;
    } else {
        goner->prev->next = goner->next;
    }
    if (list->tail == goner) {
        list->tail = goner->prev;
    } else {
          goner->next->prev = goner->prev;
    }
    list_node_free(list, goner);
    list->count--;

    return list->count;
}

/* insert a node after the specified node
   correctly deals with inserting into an empty list
   returns the number of nodes in the list */
int list_node_insert_after(list_t *list, list_node_t *where, list_node_t *new)
{
    assert(list != NULL);
    assert(where != NULL);

    list_node_t *next;

    if (new == NULL) {
        return -1;
    }

    if (list->tail == where) {
        /* new node is the last node */
        list->tail = new;
        new->next = NULL;
    } else {
        /* there is a node after the new node */
        next = where->next;
        new->next = next;
        next->prev = new;
    }

    if (where) {
        where->next = new;
    } else {
        /* only item in the list */
        list->head = new;
    }

    new->prev = where;
    list->count++;

    return list->count;
}


int list_node_insert_before(list_t *list, list_node_t *where, list_node_t *new)
{
    assert(list != NULL);
    assert(new != NULL);

    list_node_t *prev;

    if (list->head == where) {
        /* new node is the first node */
        list->head = new;
        new->prev = NULL;
    } else {
        /* there is a node before the new node */
        prev = where->prev;
        prev->next = new;
        new->prev = prev;
    }

    if (where) {
        where->prev = new;
    } else {
        /* only item in the list */
        list->tail = new;
    }

    new->next = where;
    list->count++;

    return list->count;
}


list_node_t * list_pop(list_t *list)
{
    list_node_t *head = list->head;
    list_node_del(list, list->head);
    return head;
}


void list_reverse(list_t *list)
{
    list_node_t *current, *tmp;

    current = list->head;
    while (current) {
        tmp = current->prev;
        current->prev = current->next;
        current->next = tmp;
        current = current->prev;  /* b/c now we've swapped the next & prev pointers */
    }

    tmp = list->head;
    list->head = list->tail;
    list->tail = tmp;
}


/* call iter() for each node in a list
   stops if iter() returns non-zero
   returns the last node it called iter() for */
list_node_t * list_iter(list_t *list,
                        int (*iter)(list_node_t *node, void *context),
                        void *context)
{
    assert(list != NULL);
    assert(iter != NULL);
    
    list->current = list->head;

    while (list->current != NULL) {
        if (iter(list->current, context)) {
            return list->current;
        }
        list->current = list->current->next;
    }
    
    return NULL;
}


/* call iter() for each node in a list, starting with the tail
   stops if iter() returns non-zero
   returns the last node it called iter() for */
list_node_t * list_reverse_iter(list_t *list,
                                int (*iter)(list_node_t *node, void *context),
                                void *context)
{
    list->current = list->tail;

    while (list->current != NULL) {
        if (iter(list->current, context)) {
            break;
        }
        list->current = list->current->prev;
    }

    return list->current;
}


void list_append(list_t *list, list_t *append)
{
    list->tail->next = append->head;
    list->tail = append->tail;
    list->count += append->count;
}


void list_prepend(list_t *list, list_t *prepend)
{
    list->head->prev = prepend->tail;
    list->head = prepend->head;
}   


list_node_t ** list_map_to_array(list_t *list)
{
    list_node_t *current = list->head;
    list_node_t **array = malloc(sizeof(list_node_t*) * (list->count + 1));
    int i = 0;

    while (current) {
        array[i] = current;
        current = current->next;
        i++;
    }
    array[i] = NULL;

    return array;
}


int list_map_from_array(list_t *list, list_node_t **array)
{
    int i;
    list_node_t *prev = NULL;
    list->head = array[0];
    for (i = 0; array[i] != NULL; i++) {
        array[i]->prev = prev;
        array[i]->next = array[i+1];
    }
    list->tail = array[i-1];
    return i;
}


/* Implements Hoare's Quicksort algorithm.  The list is mapped to an array of
   pointers to allow random access.  The list is sorted in-place using this
   array.

   compare() must compare two list_node_t and return -1, 0 or 1 in the same
   manner as strcmp().
*/
int list_quicksort(list_t *list,
                   list_node_compare_f compare)
{
    list_node_t **array = list_map_to_array(list);
    _quicksort(array, 0, list->count - 1, compare);
    list_map_from_array(list, array);
    free(array);

    return list->count;
}


/* taken from "Introduction to Algorithms" p. 154 */
static void _quicksort(list_node_t **array, int p, int r,
                       list_node_compare_f compare)
{
    int q;

    if (p < r) {
        q = _quicksort_partition(array, p, r, compare);
        _quicksort(array, p, q, compare);
        _quicksort(array, q + 1, r, compare);
    }
}


/* taken from "Introduction to Algorithms" p. 154 */
static int _quicksort_partition(list_node_t **array, int p, int r,
                                list_node_compare_f compare)

{
    list_node_t *tmp;
    int i, j;  

    i = p - 1;
    j = r + 1;

    while (1) {
        do {
            j--;
        } while (compare(array[j], array[p]) > 0);
        do {
            i++;
        } while (compare(array[i], array[p]) < 0);
        if (i < j) {
            swap(array[i], array[j], tmp);
        } else {
            return j;
        }
    }
}


/* implement the Fisher-Yates shuffling algorithm, using Durstenfield's
   refinement to createa a O(n) in-place shuffle algorithm.
   this code is based on java code found in wikipedia.

   myrand is a random number generator, which should return an integer between
   0 and RAND_MAX.
*/
#ifdef LIST_SHUFFLE
void list_shuffle(list_t *list, int (*myrand)(void))
{
    list_node_t **array = list_map_to_array(list);
    int n = list->count;

    int (*_rand)(void);
    if (myrand) {
        _rand = myrand;
    } else {
        _rand = rand;
    }

    while (--n >= 0) {
        /* use round() instead of trunc() so we don't bias against the
           last element */
        int k = (int) round( (double) (RAND_MAX / _rand()));
        list_node_t *tmp;
        swap (array[n], array[k], tmp);
    }

    list_map_from_array(list, array);
    free(array);
}
#endif
                 

/* returns the nth node in the list.  returns NULL of n is out-of-bounds */
list_node_t *list_get_node_number(list_t *list, unsigned int n)
{
    unsigned int i;

    if (n > list->count)
        return NULL;
    
    list_node_t *node = list->head;
    for (i = 0; i < n; i++) {
        node = node->next;
    }
    
    return node;
}


/* splits a list in half
   returns a pointer to the new list containing the 2nd half of *list */
list_t *list_split_new(list_t *list)
{
    list_t *new = list_new();
    int list_count = list->count / 2;
    list_node_t *new_tail_node = list_get_node_number(list, list_count);

    new->count = list->count - list_count;

    new->head = new_tail_node->next;
    new->head->prev = NULL;
    new->tail = list->tail;
    list->tail = new_tail_node;
    new_tail_node->next = NULL;

    return new;
}


#if 0
    
list_t *list_merge_new(list_t *left, list_t *right,
                       list_node_compare_f compare)
{
    list_t *merged = list_new();
    if (! merged)
        return NULL;

    merged->count = left->count + right->count;
    
    while (left->head || right->head) {
        if (left->head) {
            /* optimization: throw the rest of right onto merged all at once */
            merged->tail->next = left->head;
            left->head->prev = merged->tail;
            merged->tail = left->tail;
            merged->count += left->count;
            
            /* zero out the left list */
            left->head = NULL;
            left->tail = NULL;
            left->count = 0;
        } else if (right->head) {
            /* optimization: throw the rest of left onto merged all at once */
            merged->tail->next = right->head;
            right->head->prev = merged->tail;
            merged->tail = right->tail;
            merged->count += right->count;
            
            /* zero out the right list */
            right->head = NULL;
            right->tail = NULL;
            right->count = 0;
        } else {
            /* there is data in both lists */
            /* compare first nodes in left and right, adding them to merged */
            if (compare(left->head, right->head) < 0) {
                list_node_add(merged, left->head);
                left->head = left->head->next;
                left->count--;
            } else {
                list_node_add(merged, right->head);
                right->head = right->head->next;
                right->count--;
            }
        }
    }
        
    return merged;
}


list_t *list_mergesort(list_t *list, list_node_compare_f *compare)
{
    list_t *left, *right, *result;

    if (list->count <= 1) {
        return list;
    }

    left = list;
    right = list_split_new(list);
    
    return result;
}

#endif
