#include <stdlib.h>

#include "ds.h"

int ds_errno = 0;
const int ds_err_sys = 1;
const int ds_err_undef = 2;
const int ds_err_type = 3;

static const char *ds_errno_strings[] = {
	"no error",
	"undefined value",
	"type mismatch",
	0
};
const char * ds_errstr(int error) {
	return ds_errno_strings[error];
}
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
		ds_errno = ds_err_undef;
		return ds_errno;
	}
	if(! ds_any_defined(self)) {
		*result = -1;
		ds_errno = ds_err_undef;
		return ds_errno;
	}
	if(! ds_any_defined(other)) {
		*result = 1;
		ds_errno = ds_err_undef;
		return ds_errno;
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
void ds_any_incref(ds *self) {
	self->ref_count++;
}

// XXX: If ds_*_set()s fail, they leaves it undef
// XXX: Need some type checking
