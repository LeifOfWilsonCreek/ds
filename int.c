#include <stdlib.h>

#include "ds.h"

const char *ds_int_type = "integer";
typedef struct {
	long long data;
} ds_int;

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
	if(ds_any_undef(self)) return ds_error;
	if(! (self->data = malloc(sizeof(ds_int)))) {ds_error = ds_error_sys; return ds_error;}
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
	if(self->type != ds_int_type) {ds_error = ds_error_type; return ds_error;}
	*value = ((ds_int *)self->data)->data;
	return 0;
}
