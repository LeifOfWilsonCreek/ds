#include <stdlib.h>

#include "ds.h"

int ds_errno = 0;
const int ds_err_sys = -1;
const int ds_err_type = -2;


int ds_any_undef(ds *self) {
	if(! ds_any_defined(self))
		return 0;
	return self->undef(self);
}
ds * ds_any_copy(const ds *self) {
	if(! ds_any_defined(self))
		return ds_any_new();
	return self->copy(self);
}
int ds_any_compare(const ds *self, const ds *other, int *result) {
	if(! ds_any_defined(self) && ! ds_any_defined(other)) {
		*result = 0;  /* undef == undef */
		return 0;
	}
	if(self->compare == other->compare)
		return self->compare(self, other, result);
	ds_errno = ds_err_type;
	return ds_errno;
}
ds * ds_any_new() {
	ds *ret;
	if(!(ret = malloc(sizeof(*ret)))) {ds_errno = ds_err_sys; return 0;}
	ret->type = 0;
	ret->data = 0;
	ret->ref_count = 1;
	ret->undef = 0;
	ret->copy = 0;
	ret->compare = 0;
	return ret;
}
int ds_any_defined(const ds *self) {
	return (int)self->type;
}
int ds_any_free(ds *self) {
	int err;
	self->ref_count--;
	if(self->ref_count)
		return 0;
	err = ds_any_undef(self);
	free(self);
	return err;
}

// XXX: If ds_*_set()s fail, they leaves it undef
// XXX: Need some type checking

































typedef struct {
	long long data;
} ds_int;

const char *ds_int_type = "integer";
static int ds_int_undef(ds *self) {
	free(self->data);
	self->type    = 0;
	self->data    = 0;
	self->undef   = 0;
	self->copy    = 0;
	self->compare = 0;
	return 0;
}
static ds * ds_int_copy(const ds *self) {
	ds *ret;

	if(!(ret = ds_any_new())) goto fail1;
	if(ds_int_set(ret, ((ds_int *)self->data)->data)) goto fail2;
	return ret;

fail2:	ds_any_free(ret);
fail1:	return 0;
}
static int ds_int_compare(const ds *self, const ds *other, int *result) {
	ds_int *first = self->data;
	ds_int *second = other->data;

	if(first->data == second->data)
		*result = 0;
	else if(first->data > second->data)
		*result = 1;
	else
		*result = -1;
	return 0;
}
int ds_int_set(ds *self, long long value) {
	if(ds_any_defined(self))
		ds_any_undef(self);
	if(! (self->data = malloc(sizeof(ds_int)))) {ds_errno = ds_err_sys; return ds_errno;}
	((ds_int *)self->data)->data = value;
	self->type    = ds_int_type;
	self->undef   = ds_int_undef;
	self->copy    = ds_int_copy;
	self->compare = ds_int_compare;
	return 0;
}
ds * ds_int_new(long long value) {
	ds *ret;
	if(!(ret = ds_any_new())) goto fail1;
	if(ds_int_set(ret, value)) goto fail2;
	return ret;

fail2:	ds_any_free(ret);
fail1:	return 0;
}
int ds_int_get(ds *self, long long *value) {
	if(self->type != ds_int_type) {ds_errno = ds_err_type; return ds_errno;}
	*value = ((ds_int *)self->data)->data;
	return 0;
}



























typedef struct {
	long double data;
} ds_float;

const char *ds_float_type = "float";
static int ds_float_undef(ds *self) {
	free(self->data);
	self->type    = 0;
	self->data    = 0;
	self->undef   = 0;
	self->copy    = 0;
	self->compare = 0;
	return 0;
}
static ds * ds_float_copy(const ds *self) {
	ds *ret;

	if(!(ret = ds_any_new())) goto fail1;
	if(ds_float_set(ret, ((ds_float *)self->data)->data)) goto fail2;
	return ret;

fail2:	ds_any_free(ret);
fail1:	return 0;
}
static int ds_float_compare(const ds *self, const ds *other, int *result) {
	ds_float *first = self->data;
	ds_float *second = other->data;

	if(first->data == second->data)
		*result = 0;
	else if(first->data > second->data)
		*result = 1;
	else
		*result = -1;
	return 0;
}
int ds_float_set(ds *self, long double value) {
	if(ds_any_defined(self))
		ds_any_undef(self);
	if(! (self->data = malloc(sizeof(ds_float)))) {ds_errno = ds_err_sys; return ds_errno;}
	((ds_float *)self->data)->data = value;
	self->type    = ds_float_type;
	self->undef   = ds_float_undef;
	self->copy    = ds_float_copy;
	self->compare = ds_float_compare;
	return 0;
}
ds * ds_float_new(long double value) {
	ds *ret;
	if(!(ret = ds_any_new())) goto fail1;
	if(ds_float_set(ret, value)) goto fail2;
	return ret;

fail2:	ds_any_free(ret);
fail1:	return 0;
}
int ds_float_get(ds *self, long double *value) {
	if(self->type != ds_float_type) {ds_errno = ds_err_type; return ds_errno;}
	*value = ((ds_float *)self->data)->data;
	return 0;
}



























typedef struct {
	size_t length;
	unsigned char data[0];
} ds_string;

const char *ds_string_type = "string";
static int ds_string_undef(ds *self) {
	free(self->data);
	self->type    = 0;
	self->data    = 0;
	self->undef   = 0;
	self->copy    = 0;
	self->compare = 0;
	return 0;
}
static ds * ds_string_copy(const ds *self) {
	ds *ret;

	if(!(ret = ds_any_new())) goto fail1;
	if(ds_string_set(ret, ((ds_string *)self->data)->length, ((ds_string *)self->data)->data)) goto fail2;
	return ret;

fail2:	ds_any_free(ret);
fail1:	return 0;
}
static int ds_string_compare(const ds *self, const ds *other, int *result) {
	ds_string *first = self->data;
	ds_string *second = other->data;
	size_t index = 0;

	while(1) {
		if(index == first->length && index == second->length) {
			*result = 0;
			return 0;
		}
		if(index == first->length) {
			*result = -1;
			return 0;
		}
		if(index == second->length) {
			*result = 1;
			return 0;
		}
		*result = (int)first->data[index] - (int)second->data[index];
		if(*result)
			return 0;
		index++;
}	}
int ds_string_set(ds *self, size_t length, const char *value) {
	if(ds_any_defined(self))
		ds_any_undef(self);
	if(length == -1) length = strlen(value) + 1;
	if(! (self->data = malloc(sizeof(ds_string) + length))) return ds_err_sys;
	memcpy(((ds_string *)self->data)->data, value, length);
	self->type    = ds_string_type;
	self->undef   = ds_string_undef;
	self->copy    = ds_string_copy;
	self->compare = ds_string_compare;
	return 0;
}
ds * ds_string_new(size_t length, const char *value) {
	ds *ret;
	if(!(ret = ds_any_new())) goto fail1;
	if(ds_string_set(ret, length, value)) goto fail2;
	return ret;

fail2:	ds_any_free(ret);
fail1:	return 0;
}
int ds_string_get(ds *self, size_t *length, const char **value) {
	if(self->type != ds_string_type) {ds_errno = ds_err_type; return ds_errno;}
	if(length)
		*length = ((ds_string *)self->data)->length;
	if(value)
		*value = ((ds_string *)self->data)->data;
	return 0;
}











#if 0
typedef struct {
	size_t elem_alloc; /* Number of elements allocated (index_zero + num_elems <= elem_alloc) */
	size_t min_alloc;  /* Normally 0 unless tweaked by user to avoid reallocing (gets set back to 0 on ds_array_undef()) */
	size_t index_zero; /* Index (in data) of the zero'th element */
	size_t num_elems;  /* Number of valid elements */
	ds *data[0];
} ds_array;

const char *ds_array_type = "array";
static int ds_array_undef(ds *self) {
	ds_array *obj = self->data;
	while(obj->num_elems--)
		ds_any_free(obj->data[obj->index_zero + obj->num_elems]);
	free(self->data);
	self->type    = 0;
	self->data    = 0;
	self->undef   = 0;
	self->copy    = 0;
	self->compare = 0;
	return 0;
}
static ds * ds_array_copy(const ds *self) {
	ds *ret, *tmp;

// XXX: If any of these copies warn, it's considered an error
	if(!(ret = ds_array_new())) goto fail1;
	if(ds_array_setalloc(ret, self->num_elems)) goto fail2;

	index = -1;
	while(++index < self->num_elems) {
		if(!(tmp = ds_any_copy(self->data[self->index_zero + index]))) goto fail2;
		if(ds_array_push(ret, tmpds)) goto fail3;
	}
	return ret;

fail3:	ds_any_free(tmpds);
fail2:	ds_any_free(ret);
fail1:	return 0;
}

ds * ds_array_new() {
	ds *ret;

	if(!(ret = ds_any_new())) return 0;

}


static int ds_array_compare(const ds *self, const ds *other, int *result) {
	ds_array *first = self->data;
	ds_array *second = other->data;

	if(first->data == second->data)
		*result = 0;
	else if(first->data > second->data)
		*result = 1;
	else
		*result = -1;
	return 0;
}



/**/ int ds_array_set(ds *ds, size_t index, const void *elem);
int ds_array_setalloc(ds *ds, size_t size) {
	self->min_alloc = size;
// XXX: realloc here
	return 0;
}
/**/ int ds_array_push(ds *ds, const ds *elem);
/**/ int ds_array_pop(ds *ds, ds **elem);
/**/ int ds_array_push0(ds *ds, const ds *elem);
int ds_array_pop0(ds *ds, ds **elem) {
//	if(! self->num_elems) 
	return 0;
}
int ds_array_new(ds **ds) {
	if(!(*ds = malloc(sizeof(**ds)))) return DS_ESYS;
	(*ds)->elem_alloc = 0;
	(*ds)->min_alloc = 0;
	(*ds)->index_zero = 0;
	(*ds)->num_elems = 0;
	return 0;
}



//typedef struct {
//} ds_hash;

const char *ds_hash_type = "hash";

/**/ int ds_hash_new(ds_hash **ds);
/**/ int ds_hash_copy(const ds_hash *ds, ds_hash **copy);
/**/ int ds_hash_undef(ds_hash *ds);
/**/ int ds_hash_set(ds_hash *ds, const ds_string *key, const void *elem);
#endif
