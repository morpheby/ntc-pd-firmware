/*
 * list.c
 *
 *  Created on: 12.4.2012
 *      Author: Morphe
 */

#include "system.h"
#include <stdlib.h>

#define LIST_MALLOC   gc_malloc
#define LIST_REALLOC gc_realloc
#define LIST_FREE    gc_free

typedef void *_ValueType;

typedef struct {
	void *next, *prev;
	_ValueType value;
} _ListElement;

typedef struct {
	void *next, *prev;
} _ListHead;

typedef struct {
	_ListHead begin,
			  *end;
    void        (*free_) (_ValueType);
    _ValueType  (*copy_) (const _ValueType);
} _List, *_ListHandle;

typedef struct {
	_ListElement *pos;
	_ListHandle list;
} _ListIter, *_ListIterator;

/* NOTE:
 * _ListIterator in most cases can be created by
 * simply getting pointer to _ListElement *
 * However, there are functions that will not accept
 * this. Those are:
 * list_insert
 * list_remove
 * Use _list_insert and _list_remove, if you need such
 * functionality, but be sure to check begin and end
 * in list handle
 */

void dummy_free(_ValueType v) {
}

_ValueType dummy_copy(const _ValueType v) {
    return v;
}

int _list_validate(_ListHandle list) {
    if(list->end == 0 || list->begin.prev != 0 ||
            (list->begin.next == 0 && list->end != &list->begin)) {
        // list is somehow damaged -> will result in infinite loop
        list->begin.next = 0;
        list->begin.prev = 0;
        list->end = &list->begin;
    }
    return 0;
}

_ListHandle list_create() {
	_ListHandle h = LIST_MALLOC(sizeof(_List));
	h->begin.next = 0;
	h->begin.prev = 0;
	h->end = &h->begin;
    h->free_ = dummy_free;
    h->copy_ = dummy_copy;
	return h;
}

int list_is_empty(_ListHandle list) {
    _list_validate(list);
	return list->end == &list->begin;
}

void list_set_free(_ListHandle list, void (*freeFunc)(_ValueType)) {
    list->free_ = freeFunc;
}

void list_set_copy(_ListHandle list, _ValueType (*copyFunc)(const _ValueType)) {
    list->copy_ = copyFunc;
}

void list_iterator_free(_ListIterator iter) {
	LIST_FREE(iter);
}

// returns 0 if the end is reached, non-zero otherwise
int list_iterate_fwd(_ListIterator iter) {
	if(!iter->pos)
		return 0;
	return iter->pos = iter->pos->next;
}

// returns 0 if the end is reached, non-zero otherwise
int list_iterate_back(_ListIterator iter) {
	if(!iter->pos || !iter->pos->prev)
		return 0;
	iter->pos = iter->pos->prev;
	return iter->pos->prev; // don't let the head to be accessed
}

int list_iterators_equal(_ListIterator iter1, _ListIterator iter2) {
	return iter1->list == iter2->list && iter1->pos == iter2->pos;
}

_ValueType list_iterator_value(_ListIterator iter) {
	if(!iter->pos)
		return 0;
	return iter->pos->value;
}

void list_iterator_set(_ListIterator iter, _ValueType value) {
	if(!iter->pos)
		return;
	iter->pos->value = value;
}

void _list_remove(_ListElement *pos) {
	((_ListElement*)pos->prev)->next = pos->next;
	if(pos->next)
		((_ListElement*)pos->next)->prev = pos->prev;
	LIST_FREE(pos);
}

// returns 0 if the position is not valid
int list_remove(_ListIterator pos) {
	_ListElement *old = pos->pos;
	int res = list_iterate_back(pos);
    pos->list->free_(old->value);
	if(old == pos->list->end)
		list_iterate_back(&pos->list->end);
	_list_remove(old);
	return res;
}

void _list_insert(_ListElement *after, _ValueType value) {
	_ListElement *new = LIST_MALLOC(sizeof(_ListElement));
	if(!after) // the list is empty and we have no information about it
		return;
	new->value = value;
	new->next = after->next;
	new->prev = after;
	after->next = new;
	if(new->next)
		((_ListElement *)new->next)->prev = new;
}

void list_insert(_ListIterator after, _ValueType value) {
	_list_insert(after->pos, value);
	list_iterate_fwd(after);
	if(after->pos == after->list->end)
		list_iterate_fwd(&after->list->end);
}

_ListIterator list_begin(_ListHandle list) {
	_ListIterator iter = LIST_MALLOC(sizeof(_ListIter));
	iter->pos = &list->begin;
	iter->list = list;
	list_iterate_fwd(iter); // no access to the head is allowed
	return iter;
}

_ListIterator list_end(_ListHandle list) {
	_ListIterator iter = LIST_MALLOC(sizeof(_ListIter));
	iter->list = list;
	if(list_is_empty(list))
		iter->pos = 0;
	else
		iter->pos = list->end;
	return iter;
}

void list_push_back(_ListHandle list, _ValueType value) {
	// this way is faster than creating new iterator
	_list_insert(list->end, value);
	list_iterate_fwd(&(list->end));
}

void list_push_front(_ListHandle list, _ValueType value) {
	_list_insert(&list->begin, value);
	if(list_is_empty(list))
		list_iterate_fwd(&list->end);
}

// returns 0 if the list is empty
int list_pop_back(_ListHandle list) {
	// a faster way
	if(list_is_empty(list))
		return 0;
	_ListElement *oldEnd = list->end;
	list_iterate_back(&list->end);
    list->free_(oldEnd->value);
	_list_remove(oldEnd);
	if(list_is_empty(list))
		return 0;
	return 1;
}

// returns 0 if the list is empty
int list_pop_front(_ListHandle list) {
	_ListIterator iter = list_begin(list);
	int res = list_remove(iter);
	list_iterator_free(iter);
	return res;
}

_ValueType list_front(_ListHandle list) {
	if(list_is_empty(list))
		return 0;
	return ((_ListElement*)list->begin.next)->value;
}

_ValueType list_back(_ListHandle list) {
	if(list_is_empty(list))
		return 0;
	return ((_ListElement*)list->end)->value;
}

void list_clear(_ListHandle list) {
	while(list_pop_back(list));
}

_ListHandle list_copy(_ListHandle list) {
	_ListHandle listNew = list_create();
    list_set_copy(listNew, list->copy_);
    list_set_free(listNew, list->free_);
	_ListIterator i = list_begin(list);
	do {
		list_push_back(listNew, list->copy_(list_iterator_value(i)));
	} while (list_iterate_fwd(i));
	list_iterator_free(i);
	return listNew;
}

void list_free(_ListHandle list) {
	list_clear(list);
	LIST_FREE(list);
}
