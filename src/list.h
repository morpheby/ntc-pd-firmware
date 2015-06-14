/* 
 * File:   list.h
 * Author: Morphe
 *
 * Created on 12 April 2012
 */

#ifndef LIST_H_
#define LIST_H_

typedef void *_ListHandle;
typedef void *_ListIterator;

typedef void *_ValueType;

/*
 * list_create()
 * Allocates a new list and returns the handle to it
 */
_ListHandle list_create();

/*
 * list_free(_ListHandle)
 * Frees the list returned by the list_create,
 * clearing it contents
 */
void list_free(_ListHandle list);

/*
 * list_is_empty
 * Returns 0 if the list contains at least one
 * value and non-zero otherwise
 */
int list_is_empty(_ListHandle list);

/*
 * list_begin(_ListHandle)
 * Creates and returns an iterator, initialized to the
 * beginning of the list. If the list is empty, the resulting
 * iterator's position will be invalid.
 * The resulting iterator shall be freed by the call to list_iterator_free
 */
_ListIterator list_begin(_ListHandle list);


/*
 * list_end(_ListHandle)
 * Creates and returns an iterator, initialized to the
 * end of the list. If the list is empty, the resulting
 * iterator's position will be invalid.
 * The resulting iterator shall be freed by the call to list_iterator_free
 */
_ListIterator list_end(_ListHandle list);

/*
 * list_iterator_free(_ListIterator)
 * Frees the iterator, previously created by the list_begin
 * or list_end
 */
void list_iterator_free(_ListIterator iter);

/*
 * list_iterate_fwd(_ListIterator)
 * Advances iterator forward
 * Returns 0 if the end is reached, non-zero otherwise
 */
int list_iterate_fwd(_ListIterator iter);

/*
 * list_iterate_back(_ListIterator)
 * Advances iterator backwards
 * Returns 0 if the start is reached, non-zero otherwise
 */
int list_iterate_back(_ListIterator iter);

/*
 * list_iterators_equal
 * Returns non-zero if the two given iterators describe
 * the same position and zero otherwise
 */
int list_iterators_equal(_ListIterator iter1, _ListIterator iter2);

/*
 * list_iterator_value
 * Returns value of the element at the position the iterator
 * is pointing to
 */
_ValueType list_iterator_value(_ListIterator iter);

void list_iterator_set(_ListIterator iter, _ValueType value);

/*
 * list_remove
 * Removes the element at the position of the iterator
 * and decreases the iterator
 * Returns 0 if the list is empty
 */
int list_remove(_ListIterator pos);

/*
 * list_insert
 * Inserts the value after the position of the given
 * iterator and increases this iterator
 */
void list_insert(_ListIterator after, _ValueType value);

void list_push_back(_ListHandle list, _ValueType value);

void list_push_front(_ListHandle list, _ValueType value);

/*
 * list_pop_back
 * ....
 * Returns 0 if the list is empty
 */
int list_pop_back(_ListHandle list);

/*
 * list_pop_front
 * ....
 * Returns 0 if the list is empty
 */
int list_pop_front(_ListHandle list);

/*
 * list_front
 * Returns the first value in the list.
 */
_ValueType list_front(_ListHandle list);

/*
 * list_back
 * Returns the last value in the list.
 */
_ValueType list_back(_ListHandle list);

void list_clear(_ListHandle list);

_ListHandle list_copy(_ListHandle list);


void list_set_free(_ListHandle list, void (*freeFunc)(_ValueType));

void list_set_copy(_ListHandle list, _ValueType (*copyFunc)(const _ValueType));

#endif /* LIST_H_ */
