/*
 * HashTable_Lib.h
 *
 *  Created on: Jul 1, 2010
 *      Author: Wincenty
 */

#ifndef HASHTABLE_LIB_H_
#define HASHTABLE_LIB_H_

//#include <wincompat.h>
#include  "../headers/wintypes.h"
#include  "../headers/Image.h"

#define H_MAXIMUM_DICTIONARY_DISPOSAL_WAIT_LOOP_COUNT   4000
#define H_DEFAULT_HASHTABLE_SIZE 1572869
/*#define H_DEFAULT_HASHTABLE_SIZE 3145739 -If we need to go bigger here is a good prime */
#define H_DEFAULT_HASHTABLE_WIN32_SIZE 737279
#define H_DEFAULT_HASH_FUNCTION NULL
#define H_MAXIMUM_KEY_LENGTH   128
#define H_SIGNIFICANT_CHARS   32

typedef /* h_status */ enum {
  H_SUCCESS = 0,
  H_NO_SUCH_KEY = 1,
  H_INVALID_KEY = 2,
  H_INVALID_OBJECT = 3,
  H_INVALID_HASH = 4,
  H_INVALID_SYMBOL = 5,
  H_INVALID_DICTIONARY = 6,
  H_INVALID_SYMBOL_TABLE = 7,
  H_INVALID_HASH_FUNCTION = 8,
  H_INVALID_COPY_FUNCTION = 9,
  H_INVALID_EVENT_CODE = 10,
  H_INVALID_EVENT_HANDLER = 11,
  H_INVALID_ENUMERATOR = 12,
  H_MEMORY_ALLOCATION_FAILED = 13,
  H_UNABLE_TO_OBTAIN_RWLOCK = 14,
  H_OBJECT_REPLACEMENT_NOT_PERMITTED = 15,
  H_OPERATION_NOT_PERMITTED_ON_NON_EMPTY_DICT = 16,
  H_OPERATION_NOT_PERMITTED_ON_NON_EMPTY_SYMTABLE = 17,
  H_NO_FURTHER_ITEMS_AVAILABLE_IN_ENUMERATION = 18,
  H_DICTIONARY_DISPOSAL_PENDING = 19,
  H_DICTIONARY_DISPOSAL_IN_PROGRESS = 20
} h_status;
#define H_SUCCESS  0
#define H_NO_SUCH_KEY  1
#define H_INVALID_KEY  2
#define H_INVALID_OBJECT  3
#define H_INVALID_HASH  4
#define H_INVALID_SYMBOL  5
#define H_INVALID_DICTIONARY  6
#define H_INVALID_SYMBOL_TABLE  7
#define H_INVALID_HASH_FUNCTION  8
#define H_INVALID_COPY_FUNCTION  9
#define H_INVALID_EVENT_CODE  10
#define H_INVALID_EVENT_HANDLER  11
#define H_INVALID_ENUMERATOR  12
#define H_MEMORY_ALLOCATION_FAILED  13
#define H_UNABLE_TO_OBTAIN_RWLOCK  14
#define H_OBJECT_REPLACEMENT_NOT_PERMITTED  15
#define H_OPERATION_NOT_PERMITTED_ON_NON_EMPTY_DICT  16
#define H_OPERATION_NOT_PERMITTED_ON_NON_EMPTY_SYMTABLE  17
#define H_NO_FURTHER_ITEMS_AVAILABLE_IN_ENUMERATION  18
#define H_DICTIONARY_DISPOSAL_PENDING  19
#define H_DICTIONARY_DISPOSAL_IN_PROGRESS  20


typedef /* h_event */ enum {   /* triggered when ... */
  H_WILL_INSERT_OBJECT,      /*  ... an object is about to be added */
  H_DID_INSERT_OBJECT,       /*  ... an object has just been added */
  H_WILL_REMOVE_OBJECT,      /*  ... an object is about to be removed */
  H_DID_REMOVE_OBJECT,       /*  ... an object has just been removed */
                  /* to or from a dictionary, */
    H_WILL_DISPOSE_DICT,       /*  ... dictionary is about to be free'd */
    H_DID_DISPOSE_DICT         /*  ... dictionary has just been free'd */
} h_event; // end h_event
#define H_WILL_INSERT_OBJECT 0      /*  ... an object is about to be added */
#define H_DID_INSERT_OBJECT 1       /*  ... an object has just been added */
#define H_WILL_REMOVE_OBJECT 2      /*  ... an object is about to be removed */
#define H_DID_REMOVE_OBJECT 3       /*  ... an object has just been removed */
                /* to or from a dictionary, */
#define   H_WILL_DISPOSE_DICT 4       /*  ... dictionary is about to be free'd */
#define   H_DID_DISPOSE_DICT  5        /*  ... dictionary has just been free'd */

typedef unsigned int (*h_hash_string_function) (const char *);
typedef unsigned int (*h_hash_int_function) (unsigned int);
typedef unsigned int (*h_hash_pixel_function) (PIXELW);

typedef void *h_dictionary;
typedef void (*h_object_copy_function) (void *, void *);
typedef void (*h_object_event_handler) (void *);

// _bucket
//  +--------------------+
//  | insert_lock        |  lock to prevent concurrent inserts and removals
//  +--------------------+
//  | first              |  -----> pointer to first entry of this bucket
//  +--------------------+

typedef struct _bucket {
  int insert_thread;
  int insert_lock;
  struct _dict_entry *first;
} _bucket;

#define DICT_TYPE_STRING                1
#define DICT_TYPE_INT                   2
#define DICT_TYPE_PIXEL                 3
#define DICT_DEFAULT_STRING_KEY_LENGTH  16
#define DICT_DEFAULT_INT_KEY_LENGTH     sizeof(int)
#define DICT_DEFAULT_PIXEL_KEY_LENGTH   sizeof(PIXELW)

// --------------------------------------------------------------------------
// private datatype:  _dictionary
// --------------------------------------------------------------------------
//
// _dictionary header, holds parameters and counters
//  +------------------------+
//  | marked_for_disposal    |  whether dictionary is scheduled for deletion
//  +------------------------+
//  | disposal_in_progress   |  whether dictionary is being deleted
//  +------------------------+
//  | hashtable_size         |  number of buckets in hashtable
//  +------------------------+
//  | buckets_in_use         |  number of hashtable buckets in use
//  +------------------------+
//  | pending_lookups        |  number of lookups in progress
//  +------------------------+
//  | pending_inserts        |  number of inserts in progress
//  +------------------------+
//  | retained_entries       |  number of entries being retained
//  +------------------------+
//  | entry_count            |  number of objects stored in this dictionary
//  +------------------------+
//  | hash_string            |  hash function to calculate hash values
//  +------------------------+
//  | copy_object            |  object content copy function
//  +------------------------+
//  | will_insert_object     |  event handler for DD_WILL_INSERT_OBJECT
//  +------------------------+
//  | did_insert_object      |  event handler for DD_DID_INSERT_OBJECT
//  +------------------------+
//  | will_remove_object     |  event handler for DD_WILL_REMOVE_OBJECT
//  +------------------------+
//  | did_remove_object      |  event handler for DD_DID_REMOVE_OBJECT
//  +------------------------+
//  | will_dispose_dict      |  event handler for DD_WILL_DISPOSE_DICT
//  +------------------------+
//  | did_dispose_dict       |  event handler for DD_DID_DISPOSE_DICT
//  +------------------------+
//
// _dictionary body, holds linked list and hashtable
//  +------------------------+
//  | first_entry            |  -----> pointer to first entry added
//  +------------------------+
//  | last entry             |  -----> pointer to last entry added
//  +------------------------+
//  | bucket[0]              |  first bucket in hashtable
//  +------------------------+
//  | bucket[1]              |  second bucket in hashtable
//  +------------------------+
//  | bucket[2]              |  third bucket in hashtable
//  +------------------------+
//  .                        .
//  .                        .
//  +------------------------+
//  | bucket[n]              |  last bucket in hashtable
//  +------------------------+

typedef struct _dictionary {
    /* dictionary header */
  int  nType;
  BOOL marked_for_disposal, disposal_in_progress;
  int  disposal_lock; // _MUTEX(disposal_lock);
  int  align2, // word_t _ATTR(align2)
       hashtable_size,
       buckets_in_use,
       pending_lookups,
       pending_inserts,
       retained_entries,
       entry_count;

  h_hash_string_function hash_string;
  h_hash_int_function    hash_integer;
  h_hash_pixel_function  hash_pixel;

  h_object_copy_function copy_object;
  h_object_event_handler will_insert_object,
                         did_insert_object,
                         will_remove_object,
                         did_remove_object,
                         will_dispose_dict,
                         did_dispose_dict;

  int                  fixed_capacity;
  int                  fixed_used;
  struct _dict_entry * fixed_entries;
  size_t               fixed_size;

    /* linked list */
  struct _dict_entry *first_entry, *last_entry;
    /* hashtable */
  _bucket bucket[0];
} _dictionary;


// --------------------------------------------------------------------------
// private datatype: _dict_entry
// --------------------------------------------------------------------------
//
// _dict_entry
//  +--------------------+
//  | marked_for_removal |  whether entry is scheduled for deletion
//  +--------------------+
//  | lock               |  mutex lock, only if DD_USE_PTHREADS is true
//  +--------------------+
//  | hash               |  hash value of this entry's key
//  +--------------------+
//  | retain_count       |  number of times this entry has been retained
//  +--------------------+
//  | object             |  -----> pointer to stored object
//  +--------------------+
//  | next               |  -----> pointer to next entry on collision
//  +--------------------+
//  | previously_added   |  -----> pointer to previously added entry
//  +--------------------+
//  | subsequently_added |  -----> pointer to subsequently added entry
//  +--------------------+
//  | key[0]             |  key, first character
//  +--------------------+
//  | key[1]             |  key, second character
//  +--------------------+
//  .                    .
//  .                    .
//  +--------------------+
//  | key[n]             |  key, terminating character
//  +--------------------+

typedef struct _dict_entry {
  BOOL marked_for_removal;
  BOOL removal_in_progress;
  BYTE byte1; //__fillbyte(1);
  BYTE byte2; //__fillbyte(2);
  int  lock; //_MUTEX(lock);
  unsigned int hash, retain_count;
  void *object;

  struct _dict_entry * next;
  struct _dict_entry * previously_added;   // iterative predecessor
  struct _dict_entry * subsequently_added; // iterative successor

  BYTE key[0];
} _dict_entry;


// --------------------------------------------------------------------------
// private datatype:  entry_ptr
// --------------------------------------------------------------------------
//
// Pointer to _dict_entry

typedef _dict_entry * entry_ptr;


// --------------------------------------------------------------------------
// private datatype:  _enumerator
// --------------------------------------------------------------------------
//
// _enumerator
//  +--------------------+
//  | dict               |  -----> pointer to dictionary
//  +--------------------+
//  | next_item          |  -----> pointer to next item in enumeration
//  +--------------------+
//  | reverse_order      |  flag indicating the enumeration order of items
//  +--------------------+

typedef /* _enumerator */ struct {
  _dictionary *dict;
  entry_ptr next_item;
  BOOL reverse_order;
} _enumerator;


unsigned int  h_SDBM_hash_string(const char *string);
unsigned int  h_hash_integer( unsigned int key );
unsigned int  h_hash_pixel( PIXELW key );
h_dictionary  h_new_dictionary( int nType, int nCapacity);
h_status      h_clean_dictionary(h_dictionary dict);
h_status 	  h_free_Fixeddictionary(h_dictionary dict);
h_status      h_set_hash_string_function( h_dictionary dict, h_hash_string_function hashf );
h_status      h_set_hash_int_function( h_dictionary dict, h_hash_int_function hashf );
h_status      h_set_hash_pixel_function( h_dictionary dict, h_hash_pixel_function hashf );
h_status      h_set_object_copy_function(h_dictionary dict, h_object_copy_function copyf);

h_status      h_add_object_for_key   (h_dictionary dict, const void *key, void *object, int *lastIdx);
void *        h_get_object_for_key   (h_dictionary dict, const void *key, BOOL retain );
h_status      h_remove_object_for_key(h_dictionary dict, const void *key);
void *        h_update_object_for_key(h_dictionary dict, const void *key, BOOL retain, void *object);

const char *  h_key_for_hash(h_dictionary dict, unsigned int hash, BOOL retain);
void          _free_object(void *object);
void          copyDocument (void *docSrc, void *docTarget);
void *        get_object_for_key(h_dictionary dict, const void *key, BOOL retain);
void          _copy_n_bytes_from_key(const void * key, void * target, unsigned int n );
_dictionary * _alloc_init_dict_with(int nType, unsigned int size, int nCapacity);
entry_ptr     _alloc_init_entry_with(
                   _dictionary *          this_dict,
                   const void *           key,
                   unsigned int           keylen,
                   void *                 object,
                   unsigned int           hash,
                   int                  * lastIdx);

#endif /* HASHTABLE_LIB_H_ */


