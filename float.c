#include <stdlib.h>

#include "ds.h"

const char *ds_float_type = "float";
typedef struct {
	long double data;
} ds_float;

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
	if(ds_any_undef(self)) return ds_errno;
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
