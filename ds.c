#include <stdlib.h>

#include "ds.h"

int ds_error = 0;
const int ds_error_sys = 1;
const int ds_error_undef = 2;
const int ds_error_type = 3;
const int ds_error_bound = 4;

static const char *ds_error_strings[] = {
	"no error",
	"undefined value",
	"type mismatch",
	0
};
const char * ds_error_string(int error) {
	return ds_error_strings[error];
}
void ds_any_undef(ds *self) {
	if(! ds_any_defined(self))
		return;
	self->undef(self);
}
ds * ds_any_copy(const ds *self) {
	if(! ds_any_defined(self))
		return ds_any_new();
	return self->copy(self);
}
int ds_any_compare(const ds *self, const ds *other, int *result) {
	if(! ds_any_defined(self) && ! ds_any_defined(other)) {
		*result = 0;  /* undef == undef */
		ds_error = ds_error_undef;
		return ds_error;
	}
	if(! ds_any_defined(self)) {
		*result = -1;
		ds_error = ds_error_undef;
		return ds_error;
	}
	if(! ds_any_defined(other)) {
		*result = 1;
		ds_error = ds_error_undef;
		return ds_error;
	}
	if(self->compare == other->compare)
		return self->compare(self, other, result);
	ds_error = ds_error_type;
	return ds_error;
}
ds * ds_any_new() {
	ds *ret;
	if(!(ret = malloc(sizeof(*ret)))) {ds_error = ds_error_sys; return 0;}
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
void ds_any_free(ds *self) {
	self->ref_count--;
	if(self->ref_count)
		return;
	ds_any_undef(self);
	free(self);
}
void ds_any_incref(ds *self) {
	self->ref_count++;
}

// XXX: If ds_*_set()s fail, they leaves it undef
// XXX: Need some type checking
