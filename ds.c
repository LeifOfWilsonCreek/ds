#include <stdlib.h>

#include "ds.h"

const int ds_err_sys = -1;


int ds_any_undef(ds *self) {
	if(! self->data)
		return 0;
	return self->undef(self);
}
int ds_any_copy(const ds *self, ds **other) {
	if(! self->data)
		return ds_any_new(other);
	return self->copy(self, other);
}
int ds_any_compare(const ds *self, const ds *other) {
	if(self->compare)
		return self->compare(self, other);
	else if(other->compare)
		return -other->compare(other, self);
	else
		return 0;  /* undef == undef */
}
int ds_any_new(ds **self) {
	if(!(*self = malloc(sizeof(**self)))) return ds_err_sys;
	(*self)->data = 0;
	(*self)->ref_count = 1;
	(*self)->undef   = 0;
	(*self)->copy    = 0;
	(*self)->compare = 0;
	return 0;
}
int ds_any_defined(const ds *self) {
	if(self->data)
		return 1;
	else
		return 0;
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









int ds_int_undef(ds *self) {
	free(self->data);
	self->data    = 0;
	self->undef   = 0;
	self->copy    = 0;
	self->compare = 0;
	return 0;
}
int ds_int_copy(const ds *self, ds **other) {
	int err;

	if((err = ds_any_new(other))) goto fail1;
	if((err = ds_int_set(*other, ((ds_int *)self->data)->data))) goto fail2;
	return 0;

fail2:	ds_any_free(*other);
fail1:	return err;
}
int ds_int_compare(const ds *self, const ds *other) {
	ds_int first, second;

	first.data = ((ds_int *)self->data)->data;
	if(other->data)
		first.data = ((ds_int *)other->data)->data;
	else
		first.data = 0;   /* undef is like zero */

	if(first.data == second.data)
		return 0;
	if(first.data > second.data)
		return 1;
	return -1;
}
int ds_int_set(ds *self, int64_t value) {
	if(self->data)
		ds_any_undef(self);
	if(! (self->data = malloc(sizeof(ds_int)))) return ds_err_sys;
	self->undef   = ds_int_undef;
	self->copy    = ds_int_copy;
	self->compare = ds_int_compare;

	return 0;
}















int ds_float_undef(ds *self) {
	free(self->data);
	self->data    = 0;
	self->undef   = 0;
	self->copy    = 0;
	self->compare = 0;
	return 0;
}
int ds_float_copy(const ds *self, ds **other) {
	int err;

	if((err = ds_any_new(other))) goto fail1;
	if((err = ds_float_set(*other, ((ds_float *)self->data)->data))) goto fail2;
	return 0;

fail2:	ds_any_free(*other);
fail1:	return err;
}
int ds_float_compare(const ds *self, const ds *other) {
#if 0
	ds_float first, second;

	first.data = ((ds_float *)self->data)->data;
	if(other->data)
		first.data = ((ds_float *)other->data)->data;
	else
		first.data = 0;   /* undef is like zero */

	if(first.data == second.data)
		return 0;
	if(first.data > second.data)
		return 1;
#endif
	return -1;
}
int ds_float_set(ds *self, long double value) {
	if(self->data)
		ds_any_undef(self);
	if(! (self->data = malloc(sizeof(ds_float)))) return ds_err_sys;
	self->undef   = ds_float_undef;
	self->copy    = ds_float_copy;
	self->compare = ds_float_compare;

	return 0;
}













int ds_string_undef(ds *self) {
	free(((ds_string *)self->data)->data);
	free(self->data);
	self->data    = 0;
	self->undef   = 0;
	self->copy    = 0;
	self->compare = 0;
	return 0;
}

#if 0
int ds_string_copy(const ds *self, ds **other) {
	int err;

	if((err = ds_any_new(other))) goto fail1;
	if((err = ds_string_set(*other, ((ds_string *)self->data)->data))) goto fail2;
	return 0;

fail2:	ds_any_free(*other);
fail1:	return err;
}
int ds_string_copy(const ds_string *ds, ds_string **copy) {
	int err;
	if(! (*copy = malloc(sizeof(**copy)))) {err = DS_ESYS; goto fail1;}
	**copy = *ds;

	/* Dup the data */
	if((*copy)->data) {
		if(! ((*copy)->data = malloc((*copy)->length))) {err = DS_ESYS; goto fail2;}
		memcpy((*copy)->data, self->data, (*copy)->length);
	}
	return 0;

fail2:	free(*copy); *copy = 0;
fail1:	return err;
}
int ds_string_compare(const ds *self, const ds *other) {
	ds_string first, second;

	first.data = ((ds_string *)self->data)->data;
	if(other->data)
		first.data = ((ds_string *)other->data)->data;
	else
		first.data = 0;   /* undef is like zero */

	if(first.data == second.data)
		return 0;
	if(first.data > second.data)
		return 1;
	return -1;
}
int ds_string_set(ds *self, long double value) {
	if(self->data)
		ds_any_undef(self);
	if(! (self->data = malloc(sizeof(ds_string)))) return ds_err_sys;
	self->undef   = ds_string_undef;
	self->copy    = ds_string_copy;
	self->compare = ds_string_compare;

	return 0;
}
int ds_string_set(ds_string *ds, size_t length, const char *value) {
	unsigned char *data;
	if(length == -1) length = strlen(value) + 1;
	if(!(data = malloc(length))) return DS_ESYS;
	memcpy(data, value, length);
	if(self->data) free(self->data);
	self->data = data;
	return 0;
}






int ds_array_new(ds_array **ds) {
	if(!(*ds = malloc(sizeof(**ds)))) return DS_ESYS;
	(*ds)->type = ds_type_array;
	(*ds)->defined = 0;
	(*ds)->elem_alloc = 0;
	(*ds)->min_alloc = 0;
	(*ds)->index_zero = 0;
	(*ds)->num_elems = 0;
	return 0;
}
int ds_array_copy(const ds_array *ds, ds_array **copy) {
	int err;
	int index;
	void *tmpds;

	// XXX: If any of these ops warn, this still returns 0
	if((err = ds_array_new(copy)) < 0) goto fail1;
	if((err = ds_array_setalloc(*copy, self->num_elems)) < 0) goto fail2;
	index = -1;
	while(++index < self->num_elems) {
		if((err = ds_any_copy(self->data[self->index_zero + index], &tmpds)) < 0) goto fail2;
		if((err = ds_array_push(*copy, tmpds)) < 0) goto fail3;
	}
	return 0;

fail3:	ds_any_free(tmpds);
fail2:	ds_any_free(*copy);
fail1:	return err;
}
int ds_array_undef(ds_array *ds) {
	if(self->data) {
		while(self->num_elems--)
			ds_any_free(self->data[self->index_zero + self->num_elems]);
		free(self->data);
		self->data = 0;
	}
	self->min_alloc = 0;
	self->defined = 0;
	return 0;
}
/**/ int ds_array_set(ds_array *ds, size_t index, const void *elem);
int ds_array_setalloc(ds_array *ds, size_t size) {
	self->min_alloc = size;
	return 0;
}
/**/ int ds_array_push(ds_array *ds, const void *elem);
/**/ int ds_array_pop(ds_array *ds, void **elem);
/**/ int ds_array_push0(ds_array *ds, const void *elem);
int ds_array_pop0(ds_array *ds, void **elem) {
//	if(! self->num_elems) 
	return 0;
}


/**/ int ds_hash_new(ds_hash **ds);
/**/ int ds_hash_copy(const ds_hash *ds, ds_hash **copy);
/**/ int ds_hash_undef(ds_hash *ds);
/**/ int ds_hash_set(ds_hash *ds, const ds_string *key, const void *elem);
#endif
