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
		ret_val += data[offset] 
		* primes[offset % ((sizeof(primes) / sizeof(primes[0])))];
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

int ds_hash_undef(ds *self)
{
	return 0;
}
struct ds *ds_hash_copy(ds *self)
{
	struct ds *me = (void *)self,
		*new_me = malloc(me->num_entries * sizeof(struct bkt *) + sizeof(ds_hash)) );

	memset(new_me, 0, me->num_entries * sizeof(struct bkt *) + sizeof(ds_hash)) )
	*new_me = *me;


}

/* Returns 0 if equal, positive if not equal */

int ds_hash_compare(ds *self, ds *other)
{
	return 0;
}

int ds_hash_undef_elem(ds *self, const char *key)
{
	int hash_value = hash(key, strlen(key)) % self->table_size;
	ds_hash *me = (void *)self;
	struct bkt *last_bucket = me->bucket + hashvalue, *bucket = me->bucket[hash_value];
	void *temp;

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
	int hash_value = hash(key, strlen(key)) % self->table_size;
	ds_hash *me = (void *)self;
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
	if(ds_any_undef(self)) return 0;
	
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

//--FIXME:  Do the adds.
	while((temp = va_arg(list, char *)) && (temp2 = va_arg(list, ds *))) ds_hash_set_elem(self, temp, temp2);
	return self;
}
