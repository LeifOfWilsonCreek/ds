#include <stdlib.h>

#include "ds.h"

const char *ds_string_type = "string";
typedef struct {
	size_t length;
	unsigned char data[0];
} ds_string;

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
	if(ds_any_undef(self)) return ds_errno;
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
