/*
 * At this point, nothing in this file has been tested at all.  I am committing
 * it at this point so there is a copy elsewhere, and so that the rest of my
 * group does not panic because of lack of hash stuff.
 */

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "ds.h"

static int primes[] =
{
	0,	/* The first value is never used here because it corresponds to a null in the string. */
	#include "primes.lst"
};

/*
 * This algorithm is similar to PERL's hash function.
 * The main difference is that is uses a much bigger
 * list of primes (1k, ouch).  I should investigate 
 * how much impact that has.
 */
 
static int hash(const void *in, size_t size)
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
	int table_size;		/* Size of bucket table allocated */
	int num_entries;	/* Number of entries in the hash*/
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

ds *ds_hash_get(ds *self, const char *key)
{
	int hash_value = hash(key, strlen(key));
	struct bkt *bucket;
	ds_hash *me = self->data;

	bucket = me->bucket[hash_value];

	while(bucket)
	{
		if(!strcmp(key, bucket->key))
		{
			bucket->data->ref_count++;
			return bucket->data;
		}
		bucket = bucket->next;
	}
	return 0;
}

/* Returns 0 if equal, positive if not equal */

int ds_hash_compare(ds *self, ds *other)
{
	return 0;
}

int ds_hash_undef_elem(ds *self, const char *key)
{
	ds_hash *me = self->data;
	int hash_value = hash(key, strlen(key)) % me->table_size;
	struct bkt *last_bucket = (void *)(me->bucket + hash_value), *bucket = me->bucket[hash_value];

	while(bucket)
	{
		if(!strcmp(key, bucket->key))
		{
			last_bucket->next = bucket->next;
			free(bucket);
			me->num_entries--;
			return 0;
		}
		last_bucket = bucket;
		bucket = bucket->next;
	}

	return ds_error_undef;
}

int ds_hash_set_elem(ds *self, const char *key, ds *value)
{
	ds_hash *me = self->data;
	int hash_value = hash(key, strlen(key)) % me->table_size;
	struct bkt *bucket = me->bucket[hash_value];
	void *temp;

	while(bucket)
	{
		if(!strcmp(key, bucket->key))
		{
			temp = bucket;

			if(!(bucket = realloc(bucket, strlen(key) + 1) + sizeof(struct bkt)))
			{
				bucket = temp;
				return ds_error_sys;
			}
			strcpy(bucket->key, key);
			return 0;
		}
		bucket = bucket->next;
	}

	if(!(bucket = malloc(strlen(key) + 1) + sizeof(struct bkt))) return ds_error_sys;
	strcpy(bucket->key, key);
	bucket->next = me->bucket[hash_value];
	me->bucket[hash_value] = bucket;
	me->num_entries++;

	return 0;
}

ds *ds_hash_set(ds *self, ...)
{
	va_list list;
	int num_entries = 0;
	ds_hash *my_data;
	void *temp, *temp2;

	/* ds_error is initialized by ds_any_undef, so we don't need to touch it. */
	ds_any_undef(self);

	va_start(list, self);

	/*
	 * Now we look at the va_list and see how many elements are in it.  We
	 * allocate a hash that should be able to contain all the elements with
	 * very few collisions in the table.
	 */

	while(va_arg(list, void *)) num_entries++;
	num_entries /= 2;	/* For every 2 values passed, there is 1 key and one value. */
	va_end(list);
	va_start(list, self);

	/*
	 * We malloc the struct and the hash table as one allocation, to avoid
	 * having lots of little allocations.
	 */
	if(!(my_data = self->data = malloc(num_entries * sizeof(struct bkt *) + sizeof(ds_hash)) ))
	{
		/* System call returned error, indicate check errno */
		ds_error = ds_error_sys;
		va_end(list);
		return 0;
	}

	my_data->table_size = num_entries * sizeof(struct bkt *);
	my_data->num_entries = num_entries;
	memset(my_data->bucket, 0, my_data->table_size);

	self->type = ds_hash_type;
	self->undef = ds_hash_undef;
	self->copy = (void *)ds_hash_copy;
	self->compare = (void *)ds_hash_compare;

	while((temp = va_arg(list, char *)) && (temp2 = va_arg(list, ds *)))
		ds_hash_set_elem(self, temp, temp2);

	va_end(list);
	return self;
}

