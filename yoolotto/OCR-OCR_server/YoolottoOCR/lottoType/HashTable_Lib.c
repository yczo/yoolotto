/*
 * HashTable_Lib.c
 *
 *  Created on: Jul 1, 2010
 *      Author: Wincenty
 */


//#include "JavaUtils.h"

//#include "common.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include  "../headers/Image.h"
#include "../headers/util.h"
#include "../headers/memmgr.h"
#include  "../headers/Functions.h"


#include "HashTable_Lib.h"


void debugFree( void *ptr){
	yl_free(ptr);
}
void *debugMalloc( int size){
    return yl_calloc("SWT1", size, sizeof(BYTE));
}
////////////////////////////////////////////////////////////////////////////////////////////
//  _LOCK -
////////////////////////////////////////////////////////////////////////////////////////////
void _LOCK(int disposal_lock)
{
    disposal_lock = 1;
}

////////////////////////////////////////////////////////////////////////////////////////////
//  _UNLOCK -
////////////////////////////////////////////////////////////////////////////////////////////
void _UNLOCK(int disposal_lock)
{
    disposal_lock = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////
//  _WRUNLOCK -
////////////////////////////////////////////////////////////////////////////////////////////
void _WRUNLOCK(_bucket *bucket)
{
//  _bucket->insert_thread = 0;
    bucket->insert_lock = 1;
}

////////////////////////////////////////////////////////////////////////////////////////////
//  _INIT_RWLOCK -
////////////////////////////////////////////////////////////////////////////////////////////
void _INIT_RWLOCK(_bucket *bucket)
{
  int my_thread = 1; //????

  bucket->insert_thread = my_thread;
  bucket->insert_lock = 1;
}

////////////////////////////////////////////////////////////////////////////////////////////
//  _WRLOCK
////////////////////////////////////////////////////////////////////////////////////////////
void _WRLOCK(_bucket *bucket)
{
  int my_thread = 1; //????

  if ( bucket->insert_thread == my_thread || bucket->insert_thread == 0 )
    bucket->insert_lock = 0;
  else
    bucket->insert_lock = 1;
}

////////////////////////////////////////////////////////////////////////////////////////////
//  _DEALLOCATE_entry -
////////////////////////////////////////////////////////////////////////////////////////////
void _DEALLOCATE_entry(_dict_entry *this_entry)
{
//  printf( "_DEALLOCATE_entry %p\n", this_entry );
//  debugFree( this_entry, debugMem );
  yl_free(this_entry);
}

////////////////////////////////////////////////////////////////////////////////////////////
//  _DEALLOCATE_dictionary -
////////////////////////////////////////////////////////////////////////////////////////////
void _DEALLOCATE_dictionary(_dictionary *dict)
{
  if ( dict != NULL )
  {
    if ( dict->fixed_entries != NULL )
    {
//      printf( "Deallocating fixed entries %p \n", dict->fixed_entries );
      debugFree( dict->fixed_entries);
      dict->fixed_entries = NULL;
    }

//    printf( "Deallocating dictionary %p \n", dict );
    debugFree( dict);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////
//  _DEALLOCATE_object -
////////////////////////////////////////////////////////////////////////////////////////////
void _DEALLOCATE_object(void *object)
{
   //debugFree(object, debugMem);

//   printf( "Deallocating object %p \n", object );
   debugFree( object);
}

////////////////////////////////////////////////////////////////////////////////////////////
//  h_SDBM_hash_string -
////////////////////////////////////////////////////////////////////////////////////////////
unsigned int h_SDBM_hash_string(const char *string) {
  register unsigned int len, index, hash = 0;
  register char ch;

  if (string == NULL)
    return 0;

  len = strlen(string);

  if (len > H_SIGNIFICANT_CHARS) {
    len = H_SIGNIFICANT_CHARS;
  } // end if

  // PUBLIC DOMAIN ALGORITHM FOLLOWS
  for (index = 0; index < len; index++) {
    ch = string[index];
    hash = ch + (hash << 6) + (hash << 16) - hash;
  } // end for

  return (hash & 0x7FFFFFFF);
} // end dd_SDBM_hash_string

////////////////////////////////////////////////////////////////////////////////////////////
//  h_hash_integer -
////////////////////////////////////////////////////////////////////////////////////////////
unsigned int h_hash_integer( unsigned int key )
{
  int c2=0x27d4eb2d; // a prime or an odd constant
  key = (key ^ 61) ^ (key >> 16);
  key = key + (key << 3);
  key = key ^ (key >> 4);
  key = key * c2;
  key = key ^ (key >> 15);

  return key;
}
unsigned int h_hash_pixel( PIXELW key){
	unsigned int keyP= key.y*key.w+key.x;

//	printf("key.x= %d, key.y= %d, key.w= %d\n", key.x,key.y,key.w);
//	printf("keyP= %d\n", keyP);

	return h_hash_integer(keyP );
}

////////////////////////////////////////////////////////////////////////////////////////////
//  copyDocument -
////////////////////////////////////////////////////////////////////////////////////////////
void copyDocument(void *docSrc, void *docTarget)
{
   docTarget = docSrc;
}

////////////////////////////////////////////////////////////////////////////////////////////
//  _copy_n_bytes_from_key -
//
//  Copies characters from <key> to <target>.  The number of characters copied
//  will be determined by <n> or H_MAXIMUM_KEY_LENGTH,  whichever is smaller.
//  The target string <target> will always be  null-terminated at index <n> or
//  H_MAXIMUM_KEY_LENGTH, whichever is smaller.
//
//  Arguments <key> and <target> must not be NULL.
////////////////////////////////////////////////////////////////////////////////////////////
void _copy_n_bytes_from_key(const void * key, void * target, unsigned int n )
{
  register unsigned int i = 0;

  if (n > H_MAXIMUM_KEY_LENGTH)
    n = H_MAXIMUM_KEY_LENGTH;

  const BYTE * pSrc = (const BYTE *) key;
  BYTE *       pDst = (BYTE *) target;

  while (i < n)
  {
    *pDst = *pSrc;

    pSrc++;
    pDst++;

    i++;
  } // end while
}

////////////////////////////////////////////////////////////////////////////////////////////
//  _h_new_dictionary -
////////////////////////////////////////////////////////////////////////////////////////////
h_dictionary h_new_dictionary(int nType, int nCapacity)
{
#ifdef WIN32
  return (h_dictionary) _alloc_init_dict_with(nType, H_DEFAULT_HASHTABLE_WIN32_SIZE, nCapacity);
#else
  return (h_dictionary) _alloc_init_dict_with(nType, H_DEFAULT_HASHTABLE_SIZE, nCapacity);
#endif
} // end h_new_dictionary;

////////////////////////////////////////////////////////////////////////////////////////////
//  _alloc_init_dict_with -
////////////////////////////////////////////////////////////////////////////////////////////
_dictionary * _alloc_init_dict_with(int nType, unsigned int size, int nCapacity)
{
  _dictionary * new_dict;
  unsigned int index;

  size_t mem_size = sizeof(_dictionary) + (size * sizeof(_bucket));

  new_dict = (_dictionary *) debugMalloc(mem_size);
  //new_dict = (_dictionary *) malloc(sizeof(_dictionary) + (size * sizeof(_bucket)));

//  printf( "Dictionary created %p of size %lu \n", new_dict, (long unsigned int) mem_size );

  if (new_dict == NULL)
    return NULL;

  new_dict->nType = nType;
  new_dict->fixed_capacity = nCapacity;
  new_dict->fixed_entries  = NULL;
  new_dict->fixed_size     = 0;
  new_dict->fixed_used     = 0;
  new_dict->marked_for_disposal = 0;
  new_dict->disposal_in_progress = 0;
  new_dict->hashtable_size = size;
  new_dict->buckets_in_use = 0;
  new_dict->pending_lookups = 0;
  new_dict->pending_inserts = 0;
  new_dict->retained_entries = 0;
  new_dict->entry_count = 0;
  new_dict->hash_string = H_DEFAULT_HASH_FUNCTION;
  new_dict->copy_object = NULL;
  new_dict->will_insert_object = NULL;
  new_dict->did_insert_object = NULL;
  new_dict->will_remove_object = NULL;
  new_dict->did_remove_object = NULL;
  new_dict->will_dispose_dict = NULL;
  new_dict->did_dispose_dict = NULL;
  new_dict->first_entry = NULL;
  new_dict->last_entry = NULL;

  // initialise the hashtable buckets
  for (index = 0; index < size; index++)
  {
    _INIT_RWLOCK(&new_dict->bucket[index]);
//    new_dict->bucket[index].insert_lock = 1;
    new_dict->bucket[index].first = NULL;
  } // end for

  switch( new_dict->nType )
  {
    case DICT_TYPE_STRING:
      h_set_hash_string_function(new_dict, (h_hash_string_function) h_SDBM_hash_string );

      if ( new_dict->fixed_capacity > 0 )
      {
        new_dict->fixed_size    = (sizeof(_dict_entry) + DICT_DEFAULT_STRING_KEY_LENGTH) * ( new_dict->fixed_capacity + 2 );
        new_dict->fixed_entries = (_dict_entry *) debugMalloc( new_dict->fixed_size);

//        printf( "Fixed entries %p of size %lu \n", new_dict->fixed_entries, (long unsigned int) new_dict->fixed_size );
      }
    break;

    case DICT_TYPE_INT:
      h_set_hash_int_function(new_dict, (h_hash_int_function) h_hash_integer );

      if ( new_dict->fixed_capacity > 0 )
      {
        new_dict->fixed_size    = (sizeof(_dict_entry) + DICT_DEFAULT_INT_KEY_LENGTH) * ( new_dict->fixed_capacity + 2 );
        new_dict->fixed_entries = (_dict_entry *) debugMalloc( new_dict->fixed_size );

//        printf( "Fixed entries %p of size %lu \n", new_dict->fixed_entries, (long unsigned int) new_dict->fixed_size );
      }
    break;

    case DICT_TYPE_PIXEL:
      h_set_hash_pixel_function(new_dict, (h_hash_pixel_function) h_hash_pixel );
      if ( new_dict->fixed_capacity > 0 )
      {
        new_dict->fixed_size    = (sizeof(_dict_entry) + DICT_DEFAULT_PIXEL_KEY_LENGTH) * ( new_dict->fixed_capacity + 2 );
        new_dict->fixed_entries = (_dict_entry *) debugMalloc( new_dict->fixed_size );

//        printf( "Fixed entries %p of size %lu \n", new_dict->fixed_entries, (long unsigned int) new_dict->fixed_size );
      }
    break;
  }

  return new_dict;
} // end _alloc_init_dict_with;

////////////////////////////////////////////////////////////////////////////////////////////
//  h_set_hash_string_function
////////////////////////////////////////////////////////////////////////////////////////////
h_status h_set_hash_string_function(h_dictionary dict, h_hash_string_function hashf)
{
  _dictionary *this_dict = (_dictionary *) dict;

  // assert preconditions

  if (this_dict->buckets_in_use > 0)
    return H_OPERATION_NOT_PERMITTED_ON_NON_EMPTY_DICT;

  if (hashf == NULL)
    return H_INVALID_HASH_FUNCTION;

  // assign hash function to dictionary

  this_dict->hash_string = hashf;

  return H_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////
//  h_set_hash_int_function
////////////////////////////////////////////////////////////////////////////////////////////
h_status h_set_hash_int_function( h_dictionary dict, h_hash_int_function hashf )
{
  _dictionary *this_dict = (_dictionary *) dict;

  // assert preconditions

  if (this_dict->buckets_in_use > 0)
    return H_OPERATION_NOT_PERMITTED_ON_NON_EMPTY_DICT;

  if (hashf == NULL)
    return H_INVALID_HASH_FUNCTION;

  // assign hash function to dictionary

  this_dict->hash_integer = hashf;

  return H_SUCCESS;
}
////////////////////////////////////////////////////////////////////////////////////////////
//  h_set_hash_pixel_function
////////////////////////////////////////////////////////////////////////////////////////////
h_status h_set_hash_pixel_function( h_dictionary dict, h_hash_pixel_function hashf )
{
  _dictionary *this_dict = (_dictionary *) dict;

  // assert preconditions

  if (this_dict->buckets_in_use > 0)
    return H_OPERATION_NOT_PERMITTED_ON_NON_EMPTY_DICT;

  if (hashf == NULL)
    return H_INVALID_HASH_FUNCTION;

  // assign hash function to dictionary

  this_dict->hash_pixel = hashf;

  return H_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////
//  h_set_object_copy_function -
////////////////////////////////////////////////////////////////////////////////////////////
h_status h_set_object_copy_function(h_dictionary dict, h_object_copy_function copyf )
{
  _dictionary *this_dict = (_dictionary *) dict;

  // assert preconditions

  if (dict == NULL)
    return H_INVALID_DICTIONARY;

  if (this_dict->buckets_in_use > 0)
    return H_OPERATION_NOT_PERMITTED_ON_NON_EMPTY_DICT;

  if (copyf == NULL)
    return H_INVALID_COPY_FUNCTION;

  // assign copy function to dictionary

  this_dict->copy_object = copyf;

  return H_SUCCESS;
} // end _set_object_copy_function_for_dictionary;

////////////////////////////////////////////////////////////////////////////////////////////
//  _set_object_event_handler_for_dictionary -
////////////////////////////////////////////////////////////////////////////////////////////
h_status _set_object_event_handler_for_dictionary(
    h_dictionary           dict,
    h_event                event,
    h_object_event_handler handler )
{
  _dictionary *this_dict = (_dictionary *) dict;

  // assert preconditions

  if (dict == NULL)
    return H_INVALID_DICTIONARY;

  if (this_dict->buckets_in_use > 0)
    return H_OPERATION_NOT_PERMITTED_ON_NON_EMPTY_DICT;

  if (handler == NULL)
    return H_INVALID_EVENT_HANDLER;

  // assign handler to dictionary

  switch(event) {
    case H_WILL_INSERT_OBJECT:
      this_dict->will_insert_object = handler;
      break;
    case H_DID_INSERT_OBJECT:
      this_dict->did_insert_object = handler;
      break;
    case H_WILL_REMOVE_OBJECT:
      this_dict->will_remove_object = handler;
      break;
    case H_DID_REMOVE_OBJECT:
      this_dict->did_remove_object = handler;
      break;
    case H_WILL_DISPOSE_DICT:
      this_dict->will_dispose_dict = handler;
      break;
    case H_DID_DISPOSE_DICT:
      this_dict->did_dispose_dict = handler;
      break;
    default:
      return H_INVALID_EVENT_CODE;
  } // end switch;

  return H_SUCCESS;
} // end _set_object_event_handler_for_dictionary;

////////////////////////////////////////////////////////////////////////////////////////////
//  my_effective_key_and_length -
////////////////////////////////////////////////////////////////////////////////////////////
/*
char *my_effective_key_and_length(const char *key, unsigned int *keylen)
{
  //register char *start = key; // causes compiler warning -- ignore!
  //register unsigned int len = 0, offset = 0;

  const char *start = key; // causes compiler warning -- ignore!
  unsigned int len = 0, offset = 0;

#if (H_AUTOMATICALLY_TRIM_KEYS == TRUE)
  // trim any leading whitespace and tab
  while ((*start == 32) || (*start == 8)) {
    if (offset >= H_MAXIMUM_CHARS_TO_SCAN_WHEN_TRIMMING) {
      *keylen = 0;
      return NULL; // nothing but whitespace/tab
    } // end if
    start++;
    offset++;
  } // end while
#endif

  // find the end of key
  while ((key[offset] != 0) && (len < H_MAXIMUM_KEY_LENGTH))
  {
#if (H_PRINTABLE_7BIT_ASCII_KEYS_ONLY == TRUE)
    // abort if any chars other than printable 7bit ASCII found
    if ((key[offset] < 32) && (key[offset] > 126)) {
      *keylen = 0;
      return NULL; // illegal character found
    } // end if
#endif
    len++;
    offset++;
  } // end while

#if (H_AUTOMATICALLY_TRIM_KEYS == TRUE)
  // trim any trailing whitespace and tab
  offset--;
  while ((len > 0) && ((key[offset] == 32) || (key[offset] == 8))) {
    len--;
    offset--;
  } // end while
#endif

  *keylen = len;
  return start;
}
*/

///////////////////////////////////////////////////////////////////////////////////////////////
// _alloc_init_entry_with
//
// Returns  a pointer to a  newly allocated  and  intialised hash table
// containing <object>, <key> and <hash>.  Argument <keylen> is also required
// for better performance as it won't have to be calculated again.
//
///////////////////////////////////////////////////////////////////////////////////////////////
entry_ptr _alloc_init_entry_with(
      _dictionary *          this_dict,
      const void *           key,
      unsigned int           keylen,
      void *                 object,
      unsigned int           hash,
      int                  * lastIdx)
{
  _dict_entry *new_entry;

  if ( this_dict->fixed_entries == NULL )
  {
    new_entry = (_dict_entry *) debugMalloc( sizeof(_dict_entry) + keylen);
    if (new_entry == NULL)
      return NULL;
  }
  else
  {
    //  Exceeded caoacity size?
    if ( this_dict->fixed_used >= this_dict->fixed_capacity )
    {
      printf( "ERROR: if ( this_dict->fixed_used (%d) >= this_dict->fixed_capacity (%d))\n", this_dict->fixed_used, this_dict->fixed_capacity );
      return NULL;
    }

    size_t offset = 0;
    size_t size   = 0;

    switch( this_dict->nType )
    {
      case DICT_TYPE_STRING:
        size   = sizeof(_dict_entry) + DICT_DEFAULT_STRING_KEY_LENGTH;
        offset = size * this_dict->fixed_used;
      break;

      case DICT_TYPE_INT:
        size    = sizeof(_dict_entry) + DICT_DEFAULT_INT_KEY_LENGTH;
        offset = size * this_dict->fixed_used;
      break;
      case DICT_TYPE_PIXEL:
        size    = sizeof(_dict_entry) + DICT_DEFAULT_PIXEL_KEY_LENGTH;
        offset = size * this_dict->fixed_used;
      break;

    }

    //  If the new dictionary entry will exceed our buffer size...
    if ( ( offset + size ) >= this_dict->fixed_size )
    {
      printf( "ERROR: Offset exceeds the buffer size for hash table keys buffer.\nSize of dict entry %d\nfixed_used %d\nOffset %d\nfixed_size %d\n",
               size,
               this_dict->fixed_used,
               offset,
               this_dict->fixed_size
               );
      return NULL;
    }

    //  Compute next dictionary entry's address
    BYTE * ptr = (BYTE *) this_dict->fixed_entries;
    ptr += offset;

    new_entry = (_dict_entry *) ptr;

    this_dict->fixed_used++;
  }

  new_entry->marked_for_removal = 0;
  new_entry->hash = hash;
  new_entry->retain_count = 1;
  new_entry->object = object;
  new_entry->next = NULL;
  new_entry->previously_added = NULL;
  new_entry->subsequently_added = NULL;

  _copy_n_bytes_from_key( key, (void *) &new_entry->key[0], keylen);

  return new_entry;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// h_add_object_for_key2
///////////////////////////////////////////////////////////////////////////////////////////////
h_status h_add_object_for_key(
              h_dictionary           dict,
              const void           * key,
              void                 * object,
              int                  * lastIdx)
{
  _dictionary *this_dict = (_dictionary *) dict;
  entry_ptr    this_entry, new_entry = NULL;
  unsigned int hash, index, keylen = 0;
  int          lock_status;
  const char * string_key;

  // --------------------
  // assert preconditions
  // --------------------

  if (dict == NULL)
    return H_INVALID_DICTIONARY;

  if (object == NULL)
    return H_INVALID_OBJECT;

  if ( key == NULL )
    return H_INVALID_KEY;

  // for now we do not need this
  // determine effective key and its length
//  effective_key = my_effective_key_and_length(key, &keylen);

  // calculate the hash for the key
  hash = 0;

  switch( this_dict->nType )
  {
    case DICT_TYPE_STRING:
      string_key = (const char *) key;

      //  Empty string?   Exit
      if ( string_key[0] == '\0' )
        return H_INVALID_KEY;

      keylen = strlen( string_key ) + 1;

      //  If we have a dictionary with fixed capacity (having one buffer to store all dictionary entries)
      //  thst means we have a fixed key size, make sure the specified key to be added that not exceed the size
      if ( this_dict->fixed_capacity > 0 && keylen > DICT_DEFAULT_STRING_KEY_LENGTH )
        return H_INVALID_KEY;

      hash = this_dict->hash_string( string_key );
    break;

    case DICT_TYPE_INT:
      keylen = sizeof( int );
      hash   = this_dict->hash_integer( *((const int *) key) );
    break;
    case DICT_TYPE_PIXEL:
      keylen = sizeof( PIXEL );
      hash   = this_dict->hash_pixel( *((const PIXELW *) key) );

//      printf("hash= %d\n",hash);
    break;

  }

  // we've checked the key already and thus the hash wouldn't be zero
  // but a faulty user supplied hash function might return zero anyway
  if (hash == 0)
    return H_INVALID_HASH;

  // check if lookups are inhibited
  if (this_dict->disposal_in_progress == 1)
    return H_DICTIONARY_DISPOSAL_IN_PROGRESS;

  // ---------------------------------
  // let others know what we are doing
  // ---------------------------------
  this_dict->pending_inserts++;

  // ------------------------------
  // determine address for this key
  // ------------------------------
  index      = hash % this_dict->hashtable_size;

//  printf("index= %d\n",index);

  this_entry = this_dict->bucket[index].first;

  // ----------------------------------
  // obtain insert lock for this bucket
  // ----------------------------------
  _WRLOCK(&this_dict->bucket[index]);
  lock_status = this_dict->bucket[index].insert_lock;

  if (lock_status != 0)
  {
    this_dict->pending_inserts--;
    return H_UNABLE_TO_OBTAIN_RWLOCK;
  } // end if

  // ---------------------------------------
  // add new entry or replace existing entry
  // ---------------------------------------

  if (this_entry == NULL) /* bucket is empty */
  {

    // for now we do not need this
    // call event handler for H_WILL_INSERT_OBJECT
//    if (this_dict->will_insert_object != NULL)
//      this_dict->will_insert_object(object);

    // allocate and initialize a new entry
    // note, we've already checked object, effective_key, keylen and hash
    new_entry = _alloc_init_entry_with( this_dict, key, keylen, object, hash, lastIdx);

    if (new_entry == NULL)
    {
      _WRUNLOCK(&this_dict->bucket[index]);
      this_dict->pending_inserts--;
      return H_MEMORY_ALLOCATION_FAILED;
    } // end if

    // link the bucket to the new entry
    this_dict->bucket[index].first = new_entry;
    this_entry = new_entry;
/*
    {
    NODE *nD = (NODE *)this_entry->object;
    printf("h_add_object_for_key; this_entry->hash= %u, this_entry->object= %u\n", (unsigned int)this_entry->hash, (unsigned int)this_entry->object);
    printf("h_add_object_for_key; index= %d\n",index);
    printf("h_add_object_for_key; nD->parent.x= %d, nD->parent.y= %d, nD->rank= %d\n", nD->parent.x,nD->parent.y,nD->rank);
    }
*/
    // update counters
    this_dict->buckets_in_use++;
    this_dict->entry_count++;

    // update iteration list
    if ( this_dict->last_entry )
    {
       this_entry->previously_added = this_dict->last_entry;
       this_dict->last_entry->subsequently_added = this_entry;
    }
    this_dict->last_entry = this_entry;

    // for now we do not need this
    // call event handler for H_DID_INSERT_OBJECT
//    if (this_dict->did_insert_object != NULL)
//      this_dict->did_insert_object(object);
  }
  else if (this_entry->hash == hash)  /* key matches entry's key */
  {
    // check if object replacement is permitted
    if (this_dict->copy_object == NULL)
    {
      _WRUNLOCK(&this_dict->bucket[index]);
      this_dict->pending_inserts--;
      return H_OBJECT_REPLACEMENT_NOT_PERMITTED;
    } // end if

    // replace the stored object's contents
    this_dict->copy_object(object, this_entry->object);
  }
  else /* collision occurred */
  {

    // search for a match in the externally chained list
    while ((this_entry->next != NULL) &&
           (this_entry->next->hash != hash))
    {
      this_entry = this_entry->next;
    } // end while

    if (this_entry->next && this_entry->next->hash == hash)
    {
      // check if object replacement is permitted
      if (this_dict->copy_object == NULL)
      {
        _WRUNLOCK(&this_dict->bucket[index]);
        this_dict->pending_inserts--;
        return H_OBJECT_REPLACEMENT_NOT_PERMITTED;
      } // end if

      // replace the stored object's contents
      this_dict->copy_object(object, this_entry->object);
    }
    else
    {

      // for now we do not need this
      // call event handler for H_WILL_INSERT_OBJECT
//      if (this_dict->will_insert_object != NULL)
//        this_dict->will_insert_object(object);

      // allocate and initialise a new entry
      // note, we've already checked object, effective_key, keylen and hash
      new_entry = _alloc_init_entry_with( this_dict, key, keylen, object, hash, lastIdx);

      if (new_entry == NULL)
      {
        _WRUNLOCK(&this_dict->bucket[index]);
        this_dict->pending_inserts--;
        return H_MEMORY_ALLOCATION_FAILED;
      } // end if

      // link the new entry to end of externally chained list
      this_entry->next = new_entry;
      this_entry = new_entry;

      // update counters
      this_dict->entry_count++;

      // update iteration list
      this_entry->previously_added = this_dict->last_entry;
      this_dict->last_entry->subsequently_added = this_entry;
      this_dict->last_entry = this_entry;

      // for now we do not need this
      // call event handler for H_DID_INSERT_OBJECT
//      if (this_dict->did_insert_object != NULL)
//        this_dict->did_insert_object(object);
    } // end if

  } // end if

  _WRUNLOCK(&this_dict->bucket[index]);
  this_dict->pending_inserts--;

  return H_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// h_get_object_for_key
///////////////////////////////////////////////////////////////////////////////////////////////
void * h_get_object_for_key(h_dictionary dict, const void *key, BOOL retain)
{
  _dictionary *this_dict = (_dictionary *) dict;
  entry_ptr this_entry;
  unsigned int hash, index;
  const char * string_key;

  // --------------------
  // assert preconditions
  // --------------------
//  if(!stricmp(key, "9230662")){
//      lprintf( "Hash Tab ########;  start get_object_for_key\n");
  //}

  if (this_dict == NULL)
    return NULL;

  // for now we do not need this
//  if (this_dict->disposal_in_progress == TRUE)
//    return NULL;

  if ( key == NULL )
    return NULL;

  // calculate the hash for the effective key
  hash = 0;

  switch( this_dict->nType )
  {
    case DICT_TYPE_STRING:
      string_key = (const char *) key;

      //  Empty string?   Exit
      if ( string_key[0] == '\0' )
        return NULL;

      hash = this_dict->hash_string( string_key );
    break;

    case DICT_TYPE_INT:
      hash = this_dict->hash_integer( *((const int *) key) );
      break;
    case DICT_TYPE_PIXEL:
//    {
//      PIXELW *pTmp = 	((PIXELW *)key);
//   	  printf("h_get_object_for_key; key.x= %d, key.y= %d, key.w= %d\n", pTmp->x, pTmp->y, pTmp->w);
//    }
      hash = this_dict->hash_pixel( *((const PIXELW *) key) );
//      printf("h_get_object_for_key; hash= %d\n", hash);

    break;
  }


//  if(!stricmp(key, "9230662")){
//      lprintf( "Hash Tab ########;  hash= %i\n",hash);
//  }

  // we've checked the key already and thus the hash wouldn't be zero
  // but a faulty user supplied hash function might return zero anyway
  if (hash == 0)
    return NULL;

  // for now we do not need this
  // check if lookups are inhibited
//  if (this_dict->disposal_in_progress == TRUE)
//    return NULL;

  // ---------------------------------
  // let others know what we are doing
  // ---------------------------------

  this_dict->pending_lookups++;

  // ------------------------------
  // determine address for this key
  // ------------------------------

  index = hash % this_dict->hashtable_size;
//  printf("h_get_object_for_key; index= %d\n", index);
  this_entry = this_dict->bucket[index].first;

//  if(!stricmp(key, "9230662")){
//      lprintf( "Hash Tab ########;  index= %i\n",index);
//  }

  // ---------------------------
  // lookup entry at the address
  // ---------------------------

  if (this_entry == NULL)
  {
//    this_dict->pending_lookups++;
    this_dict->pending_lookups--;
    return NULL;
  }
  else if /* key matches entry's key */ (this_entry->hash == hash)
  {

// for now we do not need this
//    if (retain == TRUE)
//      this_entry->retain_count++;
/*
	  {
  NODE *nD = (NODE *)this_entry;
  printf("h_get_object_for_key; this_entry->hash= %u, this_entry->object= %u\n", (unsigned int)hash, (unsigned int)this_entry->object);
  printf("h_get_object_for_key; index= %d\n", index);
  printf("h_get_object_for_key; nD->parent.x= %d, nD->parent.y= %d, nD->rank= %d\n", nD->parent.x,nD->parent.y,nD->rank);
	  }
*/
    this_dict->pending_lookups--;
    return this_entry->object;
  }
  else /* collision occurred */
  {
    // search for a match in the externally chained list
    while ((this_entry->next != NULL) && (this_entry->next->hash != hash))
    {
      this_entry = this_entry->next;
    } // end while

    if (this_entry->next != NULL && this_entry->next->hash == hash) /* match found */
    {
// for now we do not need this
//      if (retain == TRUE)
//        this_entry->retain_count++;

      this_dict->pending_lookups--;
//      return this_entry->object;
      return this_entry->next->object;
    }
    else /* no match found */
    {
      this_dict->pending_lookups--;
      return NULL;
    } // end if
  } // end if
}

///////////////////////////////////////////////////////////////////////////////////////////////
// h_remove_object_for_key
///////////////////////////////////////////////////////////////////////////////////////////////
h_status h_remove_object_for_key(h_dictionary dict, const void * key)
{
  _dictionary *this_dict = (_dictionary *) dict;
  entry_ptr this_entry, predecessor = NULL;
  unsigned int hash, index;
  const char *string_key;
  void *object;

  // --------------------
  // assert preconditions
  // --------------------

  if (this_dict == NULL)
    return H_INVALID_DICTIONARY;

  if ( key == NULL )
    return H_INVALID_KEY;

  // calculate the hash for the effective key
  hash = 0;

  switch( this_dict->nType )
  {
    case DICT_TYPE_STRING:
      string_key = (const char *) key;

      //  Empty string?   Exit
      if ( string_key[0] == '\0' )
        return H_INVALID_KEY;

      hash = this_dict->hash_string( string_key );
    break;

    case DICT_TYPE_INT:
      hash = this_dict->hash_integer( *((const int *) key) );
    break;
    case DICT_TYPE_PIXEL:
      hash = this_dict->hash_pixel( *((const PIXELW *) key) );
    break;

  }

  // we've checked the key already and thus the hash wouldn't be zero
  // but a faulty user supplied hash function might return zero anyway
  if (hash == 0)
    return H_INVALID_HASH;

  // ------------------------------
  // determine address for this key
  // ------------------------------

  index = hash % this_dict->hashtable_size;
  this_entry = this_dict->bucket[index].first;

  // ---------------------------
  // lookup entry at the address
  // ---------------------------

  if (this_entry == NULL) /* bucket is empty */
  {
    return H_NO_SUCH_KEY;
  }
  else if (this_entry->hash == hash)  /* key matches entry's key */
  {
    // mark entry for removal, but don't remove it just yet
    this_entry->marked_for_removal = 1;

    if ( this_entry->next )
    {
      predecessor = this_entry;
    }
  }
  else /* collision occurred */
  {

    // search for a match in the externally chained list
    while ((this_entry->next != NULL) &&
//         (this_entry->next->hash != hash)) {
         (this_entry->hash != hash))
    {
      predecessor = this_entry;
      this_entry = this_entry->next;
    } // end while

    if (this_entry->hash == hash)  /* match found */
    {
      // mark entry for removal, but don't remove it just yet
      this_entry->marked_for_removal = 1;
    }
    else /* no match found */
    {
      return H_NO_SUCH_KEY;
    } // end if
  } // end if

  // if this entry has a pending removal and its retain count is 1,
  // then we will need to remove it now

  if ((this_entry->retain_count == 1) &&
      (this_entry->marked_for_removal == 1))
  {
    this_entry->retain_count = 0;
    object = this_entry->object;

    // for now we do not need this
    // call event handler WILL_REMOVE_OBJECT
//    if (this_dict->will_remove_object != NULL)
//      this_dict->will_remove_object(object);

    // remove the entry
    if (this_entry && this_dict->bucket[index].first == this_entry)
    {
      this_dict->bucket[index].first = this_entry->next;
    }
    else
    {
       if (predecessor && this_entry)
       {
         predecessor->next = this_entry->next;
       }
    }

    // remove object
    this_entry->object = NULL;

    // update counters
    this_dict->buckets_in_use--;
    this_dict->entry_count--;

    // update iteration list header, if needed

    if (this_dict->first_entry == this_entry)
      this_dict->first_entry = this_entry->subsequently_added;
    if (this_dict->last_entry == this_entry)
      this_dict->last_entry = this_entry->previously_added;

    // remove from iteration list
    if(this_entry->previously_added)
       this_entry->previously_added->subsequently_added = this_entry->subsequently_added;
    if(this_entry->subsequently_added)
       this_entry->subsequently_added->previously_added = this_entry->previously_added;

    // deallocate entry
//    lprintf( "Hash Tab ########; this_entry= 0x%x\n", this_entry);
    if ( this_dict->fixed_entries == NULL )
    {
      _DEALLOCATE_entry(this_entry);
    }

    // for now we do not need this
    // call event handler DID_REMOVE_OBJECT
//    if (this_dict->did_remove_object != NULL)
//      this_dict->did_remove_object(object);

  } // end if

  return H_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// h_update_object_for_key
///////////////////////////////////////////////////////////////////////////////////////////////
void * h_update_object_for_key(h_dictionary dict, const void *key, BOOL retain, void *object)
{
  _dictionary *this_dict = (_dictionary *) dict;
  entry_ptr this_entry;
  unsigned int hash, index;
  const char * string_key;

  // --------------------
  // assert preconditions
  // --------------------
//  if(!stricmp(key, "9230662")){
//      lprintf( "Hash Tab ########;  start get_object_for_key\n");
  //}

  if (this_dict == NULL)
    return NULL;

  if ( key == NULL )
    return NULL;

  // calculate the hash for the effective key
  hash = 0;

  switch( this_dict->nType )
  {
    case DICT_TYPE_STRING:
      string_key = (const char *) key;

      //  Empty string?   Exit
      if ( string_key[0] == '\0' )
        return NULL;

      hash = this_dict->hash_string( string_key );
    break;

    case DICT_TYPE_INT:
      hash = this_dict->hash_integer( *((const int *) key) );
    break;

    case DICT_TYPE_PIXEL:
    	hash = this_dict->hash_pixel( *((const PIXELW *) key) );

   	break;
  }

  // we've checked the key already and thus the hash wouldn't be zero
  // but a faulty user supplied hash function might return zero anyway
  if (hash == 0)
    return NULL;

  // for now we do not need this
  // check if lookups are inhibited
//  if (this_dict->disposal_in_progress == TRUE)
//    return NULL;

  // ---------------------------------
  // let others know what we are doing
  // ---------------------------------

  this_dict->pending_lookups++;

  // ------------------------------
  // determine address for this key
  // ------------------------------

  index = hash % this_dict->hashtable_size;
  this_entry = this_dict->bucket[index].first;

  // ---------------------------
  // lookup entry at the address
  // ---------------------------

  if (this_entry == NULL) /* bucket is empty */
  {
//    this_dict->pending_lookups++;
    this_dict->pending_lookups--;
    return NULL;
  }
  else if (this_entry->hash == hash)  /* key matches entry's key */
  {

// for now we do not need this
//    if (retain == TRUE)
//      this_entry->retain_count++;

    this_dict->pending_lookups--;
    this_entry->object = object;

    return this_entry->object;
  }
  else /* collision occurred */
  {
    // search for a match in the externally chained list
    while ((this_entry->next != NULL) &&
           (this_entry->next->hash != hash))
    {
      this_entry = this_entry->next;
    } // end while

    if (this_entry->next != NULL && this_entry->next->hash == hash) /* match found */
    {

      // for now we do not need this
//      if (retain == TRUE)
//        this_entry->retain_count++;

      this_dict->pending_lookups--;
//      return this_entry->object;
      this_entry->next->object = object;
      return this_entry->next->object;
    }
    else /* no match found */
    {
      this_dict->pending_lookups--;
      return NULL;
    } // end if
  } // end if

} // end get_object_for_key;

h_status h_clean_dictionary(h_dictionary dict)
{
  _dictionary *this_dict = (_dictionary *) dict;
  entry_ptr this_entry, successor = NULL;
//  unsigned int hash, index, keylen = 0;
  unsigned int index;
//  char *effective_key;
//  void *object;

  // --------------------
  // assert preconditions
  // --------------------

  if (dict == NULL)
    return H_INVALID_DICTIONARY;

  for ( index = 0 ; index < this_dict->hashtable_size ; index++ )
  {
    this_entry = this_dict->bucket[index].first;

    if (this_entry == NULL)  /* bucket is empty */
      continue;

    while( this_entry )
    {
      successor = this_entry->next;
      this_dict->buckets_in_use--;
      this_dict->entry_count--;

      if ( this_dict->fixed_entries == NULL )
      {
        _DEALLOCATE_entry(this_entry);
      }

      this_dict->bucket[ index ].first = successor;
      this_entry = successor;
    }
  }

  this_dict->fixed_used = 0;



  return H_SUCCESS;
} // end h_clean_dictionary;



h_status h_free_Fixeddictionary(h_dictionary dict)
{
  _dictionary *this_dict = (_dictionary *) dict;


  // --------------------
  // assert preconditions
  // --------------------
  if (dict == NULL)
    return H_INVALID_DICTIONARY;

  if ( this_dict->fixed_entries != NULL )
	  _DEALLOCATE_dictionary(dict);

  return H_SUCCESS;
} // end h_free_Fixeddictionary;

