/*
 * At this point, nothing in this file has been tested at all.  I am committing
 * it at this point so there is a copy elsewhere, and so that the rest of my
 * group does not panic because of lack of hash stuff.
 */

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "ds.h"

static const int primes[] =
{
	#include "primes.lst"
};

/*
 * This algorithm is similar to PERL's hash function.
 * The main difference is that is uses a much bigger
 * list of primes (1k, ouch).  I should investigate 
 * how much impact that has.
 */
 
static unsigned int hash(const void *in, size_t size)
{
	const unsigned char *data = in;
	unsigned int offset = 0;
	int ret_val = 2;

	while(offset < size)
	{
		ret_val += data[offset] * primes[offset % ((sizeof(primes) / sizeof(primes[0])))];
		offset++;
	}
	return ret_val;
}

const char *ds_hash_type = "hash";

struct bkt
{
	ds *data;
	struct bkt *next;
	char key[0];
};


typedef struct
{
	int table_size;		/* Number of buckets allocated */
	int num_entries;	/* Number of entries in the hash */
	struct bkt *bucket[0];	/* This is going to be an array of linked lists. */
} ds_hash;

void ds_hash_undef(ds *self)
{
}

struct ds *ds_hash_copy(ds *self)
{
	ds_hash *me = self->data, *new_me;
	ds *new_self;
	int i = me->table_size;
	struct bkt *current_old_bucket, *current_new_bucket;
#if 0

	/* Allocate the new ds. */
	if(!(new_self = malloc(sizeof(ds))))
	{
		ds_error = ds_error_sys;
		return 0;
	}
	/* Allocate the new hash. */
	if(!(new_me = malloc(me->num_entries * sizeof(struct bkt *) + sizeof(ds_hash))))
	{
		free(new_self);
		ds_error = ds_error_sys;
		return 0;
	}
	memset(new_me, 0, me->num_entries * sizeof(struct bkt *) + sizeof(ds_hash));
	*new_self = *self;
	new_self->data = new_me;
	*new_me = *me;

	while(i--)
	{
		if(!me->bucket[i]) continue;

		/* Copy the first bucket. */
		current_bucket = me->bucket[i];
		if(!(new_me->bucket[i] = malloc(sizeof(*current_bucket) + strlen(current_bucket->key))))
			goto bad_karma;

		/* Then the data in that bucket */
		new_me->data 
= ds_any_copy(current_bucket->data);
		while(current_bucket->next)
		{
//			current_bucket->next = malloc(sizeof(struct bkt) + )

			current_bucket = current_bucket->next;
		}
	}

#endif
	return new_self;
bad_karma:
	/* FIXME : Undef the hash */
	ds_error = ds_error_sys;
	return 0;
}

/*
 * Finds the bucket with the given key.  If the linkto parameter is non-null, 
 * it is filled with the address of the pointer to the bucket, so that the
 * link to it can be restored without having to search manually or use a
 * double-linked list.
 */
static struct bkt *find_bkt(ds *self, const char *key, struct bkt **linkto)
{
	ds_hash *me = self->data;
	int hash_value = hash(key, strlen(key)) % me->table_size;
	struct bkt *bucket;

	bucket = me->bucket[hash_value];
	if(linkto) *linkto = (struct bkt *)(me->bucket + hash_value);

	while(bucket)
	{
		if(!strcmp(key, bucket->key)) return bucket;
		if(linkto) *linkto = bucket;
		bucket = bucket->next;
	}
	return 0;
}

ds *ds_hash_get(ds *self, const char *key)
{
	struct bkt *bucket = find_bkt(self, key, 0);

	if(!bucket) return 0;

	bucket->data->ref_count++;
	return bucket->data;
}

/* Returns 0 if equal, positive if not equal */
int ds_hash_compare(ds *self, ds *other)
{
	return 0;
}

void ds_hash_undef_elem(ds *self, const char *key)
{
	ds_hash *me = self->data;
	struct bkt *last_bucket;
	struct bkt *bucket = find_bkt(self, key, &last_bucket);

	if(!bucket) return;	/* No error, but not found. */

	last_bucket = bucket->next;
	ds_any_undef(bucket->data);
	free(bucket);

	me->num_entries--;
}

int ds_hash_set_elem(ds *self, const char *key, ds *value)
{
	ds_hash *me = self->data;
	int hash_value = hash(key, strlen(key)) % me->table_size;
	struct bkt *bucket = find_bkt(self, key, 0);
	if(bucket)
	{
		/* We found the key, so just change the data. */
		ds_any_undef(bucket->data);	/* Get rid of the old data. */
		if(!(bucket->data = ds_any_copy(value))) return ds_error;

		return 0;
	}

	/* If we got here, the entry does not exist. Create it. */
	if(!(bucket = malloc(strlen(key) + 1 + sizeof(struct bkt)))) return ds_error_sys;

	strcpy(bucket->key, key);			/* Copy the key.  */

#if 0
	if(!(bucket->data = ds_any_copy(value)))	/* Copy the data. */
	{
		free(bucket);
		return ds_error;
	}
#endif
	bucket->data = value;
	value->ref_count++;

	/* Insert the bucket into the chain. */
	bucket->next = me->bucket[hash_value];
	me->bucket[hash_value] = bucket;

	me->num_entries++;	/* Increase the entry count. */

	return 0;
}

ds *ds_hash_set(ds *self, ...)
{
	va_list list;
	int num_entries = 0;
	ds_hash *me;
	char *key;
	ds   *value;

	/* If the ds is not created, create it. */
	if(!self) if(!(self = ds_any_new())) return 0;
	/* ds_error is initialized by ds_any_undef, so we don't need to touch it. */
	if(self->ref_count) ds_any_undef(self);

	va_start(list, self);

	/*
	 * Now we look at the va_list and see how many elements are in it.  We
	 * allocate a hash that should be able to contain all the elements with
	 * very few collisions in the table.
	 */

	while(va_arg(list, void *)) num_entries++;
	num_entries /= 2;	/* For every 2 values passed, there is 1 key and one value. */
	va_end(list);
	va_start(list, self);	/* Reset the va_list for the actual entries. */

	/*
	 * We malloc the ds_hash and the hash table as one allocation, to avoid
	 * having lots of little allocations.
	 */
	if(!(me = self->data = malloc(num_entries * sizeof(struct bkt *) + sizeof(ds_hash)) ))
	{
		/* System call returned error, indicate check errno */
		ds_error = ds_error_sys;
		va_end(list);
		return 0;
	}

	/* Initialize the ds struct as a hash. */
	self->type = ds_hash_type;
	self->undef = ds_hash_undef;
	self->copy = (void *)ds_hash_copy;
	self->compare = (void *)ds_hash_compare;

	/* Initialize the ds_hash struct itself. */
	me->table_size = num_entries;
	me->num_entries = 0;	/* This will be set as the items are added. */
	memset(me->bucket, 0, me->table_size * sizeof(me->bucket[0]));

	while((key = va_arg(list, char *)) && (value = va_arg(list, ds *)))
		if(ds_hash_set_elem(self, key, value))
		{
			ds_hash_undef(self);
			return 0;
		}

	va_end(list);
	return self;
}

