#include <stdlib.h>
#include <string.h>

#include "ds.h"

const char *ds_string_type = "string";
typedef struct {
	size_t length;
	unsigned char data[0];
} ds_string;

static void ds_string_undef(ds *self) {
	free(self->data);
	self->type    = 0;
	self->data    = 0;
	self->undef   = 0;
	self->copy    = 0;
	self->compare = 0;
}
static ds * ds_string_copy(const ds *self) {
	ds_string *obj = self->data;
	ds *ret;

	if(!(ret = ds_any_new())) goto fail1;
	if(ds_string_set(ret, obj->length, obj->data)) goto fail2;
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
	ds_string *obj;

	ds_any_undef(self);
	if(length == -1) length = strlen(value) + 1;
	if(! (obj = self->data = malloc(sizeof(ds_string) + length))) return ds_error_sys;

	memcpy(obj->data, value, length);
	obj->length = length;

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
int ds_string_get(ds *self, size_t *length, char **value) {  // XXX: Should the last parameter be const?
	if(self->type != ds_string_type) {ds_error = ds_error_type; return ds_error;}
	if(length)
		*length = ((ds_string *)self->data)->length;
	if(value)
		*value = ((ds_string *)self->data)->data;
	return 0;
}


ds * ds_string_join(ds *self, ds *joiner) {
	size_t newlength, index;
	ds *elem, *new;
	ds_string *newobj;

	if(joiner->type != ds_string_type) {ds_error = ds_error_type; goto fail1;}
	if(self->type != ds_array_type) {ds_error = ds_error_type; goto fail1;}

	newlength = 0;
	index = 0;
	while(!ds_array_get(self, &elem, index)) {
		ds_any_free(elem);  /* We know the ref count is >= 2 since it's also in the array */
		if(elem->type != ds_string_type) {ds_error = ds_error_type; goto fail1;}
		newlength += ((ds_string *)elem->data)->length;
		index++;
	}
	if(ds_error != ds_error_bound)
		goto fail1;
	newlength++; /* Null-terminator */

	if(!(new = ds_any_new())) goto fail1;
	if(!(newobj = malloc(sizeof(ds_string) + newlength))) {ds_error = ds_error_sys; goto fail2;}
	self->data    = newobj;
	self->type    = ds_string_type;
	self->undef   = ds_string_undef;
	self->copy    = ds_string_copy;
	self->compare = ds_string_compare;

	index = 0;
	newlength = 0;
	while(!ds_array_get(self, &elem, index)) {
		ds_any_free(elem);  /* We know the ref count is >= 2 since it's also in the array */
		memcpy(newobj->data + newlength, ((ds_string *)elem->data)->data, ((ds_string *)elem->data)->length);
		newlength += ((ds_string *)elem->data)->length;
		index++;
	}

	return new;

fail2:	ds_any_free(new);
fail1:	return 0;
}
