#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "ds.h"

const char *ds_array_type = "array";
/* These arrays are wrapped for efficiency. */
typedef struct {
	size_t elem_alloc; /* Number of elements allocated (index_zero + num_elems <= elem_alloc) */
	size_t min_alloc;  /* Normally 0 unless tweaked by user to avoid reallocing (gets set back to 0 on ds_array_set()) */
	size_t index_zero; /* Index (in data) of the zero'th element */
	size_t num_elems;  /* Number of valid elements */
	ds *data[0];
} ds_array;

/* Array memory management */
#if 1   /* Is a comparason and branch faster than mod? */
#	define ELEMENT(obj, index) ((obj)->data[(obj)->index_zero + (index) < (obj)->elem_alloc ? (obj)->index_zero + (index) : (obj)->index_zero + (index) - (obj)->elem_alloc])
#else
#	define ELEMENT(obj, index) ((obj)->data[((obj)->index_zero + (index)) % (obj)->elem_alloc])
#endif

/* Reallocate an array - also resets index_zero to zero.
 * length must be >= obj->num_elems */
static int ds_array_resize(ds *self, size_t length) {
	ds_array *newobj;
	ds_array *oldobj = self->data;

	if(oldobj->elem_alloc == length) return 0;
	if(oldobj->index_zero + oldobj->num_elems <= oldobj->elem_alloc  /* Array DID not wrap (before expansion) */
	&& oldobj->index_zero + oldobj->num_elems <= length) {           /* Array WILL not wrap (before contraction) */
		/* Use realloc() - it saves reconstruction time and might save us a block move */
		if(!(newobj = realloc(oldobj, sizeof(*newobj) + length * sizeof(newobj->data[0])))) {ds_error = ds_error_sys; return ds_error;}
		newobj->elem_alloc = length;
		self->data = newobj;
		return 0;
	}

	if((newobj = malloc(sizeof(*newobj) + length * sizeof(newobj->data[0])))) {ds_error = ds_error_sys; return ds_error;}
	newobj->elem_alloc = length;
	newobj->min_alloc = oldobj->min_alloc;
	newobj->index_zero = 0;
	newobj->num_elems = oldobj->num_elems;
	if(oldobj->index_zero + oldobj->num_elems > oldobj->elem_alloc) {
		memcpy(&newobj->data[0], &oldobj->data[oldobj->index_zero], (oldobj->elem_alloc - oldobj->index_zero) * sizeof(oldobj->data[0]));
		memcpy(&newobj->data[oldobj->elem_alloc - oldobj->index_zero], &oldobj->data[0], (oldobj->num_elems - oldobj->elem_alloc + oldobj->index_zero) * sizeof(oldobj->data[0]));
	} else {
		memcpy(&newobj->data[0], &oldobj->data[oldobj->index_zero], oldobj->num_elems * sizeof(oldobj->data[0]));
	}
	free(oldobj);
	self->data = newobj;
	return 0;
}
static void ds_array_contract(ds *self) {
	ds_array *obj = self->data;

	if(obj->num_elems >= obj->elem_alloc / 3)
		return;
	ds_array_resize(self, obj->min_alloc > obj->num_elems ? obj->min_alloc : obj->num_elems);
	/* Failure doesn't matter if it's getting smaller */
}
static int ds_array_expand(ds *self, size_t length) {
	ds_array *obj = self->data;
	size_t newlength = obj->num_elems ? obj->num_elems : 1;

	if(length <= obj->elem_alloc) return 0;  /* Already big enough */
	while(newlength < length) newlength <<= 1;
	return ds_array_resize(self, newlength);
}
int ds_array_setalloc(ds *self, size_t length) {
	ds_array *obj = self->data;

	if(length <= obj->num_elems) {
		obj->min_alloc = length;
		ds_array_contract(self);
		return 0;
	}

	/* Resize the array */
	if(ds_array_resize(self, length)) return ds_error;
	obj = self->data;
	obj->min_alloc = length;

	return 0;
}
static void ds_array_moveelems(ds *self, size_t newoff, size_t oldoff, size_t length) {
	ds_array *obj = self->data;

	/* Adjust for index_zero first */
	oldoff += obj->index_zero;
	newoff += obj->index_zero;
	while(oldoff >= obj->elem_alloc) oldoff -= obj->elem_alloc;
	while(newoff >= obj->elem_alloc) newoff -= obj->elem_alloc;

	/* This is the drudgery of the different cases in how the array is
	 * wrapped before and after the move.  I can't make it any clearer
	 * without a marker board. */
	if(oldoff < newoff) {
		/* the old chunk can't wrap unless the new one does because oldoff < newoff */
		if(length <= obj->elem_alloc - newoff) {
			/* neither chunk wraps */
			memmove(&obj->data[newoff], &obj->data[oldoff], length * sizeof(obj->data[0]));
		} else
		  /* length > obj->elem_alloc - newoff) */
		  if(length <= obj->elem_alloc - oldoff) {
			/* new chunk wraps, old one doesn't */
			size_t first_chunk = obj->elem_alloc - newoff;
			size_t second_chunk = length - first_chunk;
			memmove(&obj->data[0], &obj->data[oldoff + first_chunk], second_chunk * sizeof(obj->data[0]));
			memmove(&obj->data[newoff], &obj->data[oldoff], first_chunk * sizeof(obj->data[0]));
		} else /* length > obj->elem_alloc - oldoff */ {
			/* both chunks wrap, but new wraps before old */
			size_t first_chunk = obj->elem_alloc - newoff;
			size_t second_chunk = newoff - oldoff;   /* elems between when new wraps and old wraps */
			size_t third_chunk = length - second_chunk - first_chunk;
			memmove(&obj->data[second_chunk], &obj->data[0], third_chunk * sizeof(obj->data[0]));
			memmove(&obj->data[0], &obj->data[oldoff + first_chunk], second_chunk * sizeof(obj->data[0]));
			memmove(&obj->data[newoff], &obj->data[oldoff], first_chunk * sizeof(obj->data[0]));
		}
	} else if(newoff < oldoff) {
		/* the new chunk can't wrap unless the old one does because newoff < oldoff */
		if(length <= obj->elem_alloc - oldoff) {
			/* neither chunk wraps */
			memmove(&obj->data[newoff], &obj->data[oldoff], length * sizeof(obj->data[0]));
		} else
		  /* length > obj->elem_alloc - oldoff) */
		  if(length <= obj->elem_alloc - newoff) {
			/* old chunk wraps, new one doesn't */
			size_t first_chunk = obj->elem_alloc - oldoff;
			size_t second_chunk = length - first_chunk;
			memmove(&obj->data[newoff], &obj->data[oldoff], first_chunk * sizeof(obj->data[0]));
			memmove(&obj->data[newoff + first_chunk], &obj->data[0], second_chunk * sizeof(obj->data[0]));
		} else /* length > obj->elem_alloc - newoff */ {
			/* both chunks wrap, but old wraps before new */
			size_t first_chunk = obj->elem_alloc - oldoff;
			size_t second_chunk = oldoff - newoff;   /* elems between when old wraps and new wraps */
			size_t third_chunk = length - second_chunk - first_chunk;
			memmove(&obj->data[newoff], &obj->data[oldoff], first_chunk * sizeof(obj->data[0]));
			memmove(&obj->data[newoff + first_chunk], &obj->data[0], second_chunk * sizeof(obj->data[0]));
			memmove(&obj->data[0], &obj->data[second_chunk], third_chunk * sizeof(obj->data[0]));
}	}	}


/* Primitive array functions */
static void ds_array_undef(ds *self) {
	ds_array *obj = self->data;
	size_t tmp = obj->num_elems;
	while(tmp--)
		ds_any_free(ELEMENT(obj, tmp));
	free(self->data);
	self->type    = 0;
	self->data    = 0;
	self->undef   = 0;
	self->copy    = 0;
	self->compare = 0;
}
static ds * ds_array_copy(const ds *self) {
	ds_array *oldobj = self->data, *newobj;
	ds *ret, *tmp;

	if(!(ret = ds_any_new())) goto fail1;
	if(ds_array_set(ret, 0)) goto fail2;
	if(ds_array_resize(ret, oldobj->min_alloc > oldobj->num_elems ? oldobj->min_alloc : oldobj->num_elems)) goto fail2;
	newobj = ret->data;
	newobj->min_alloc = oldobj->min_alloc;

	while(newobj->num_elems < oldobj->num_elems) {
		if(!(tmp = ds_any_copy(ELEMENT(oldobj, newobj->num_elems)))) goto fail2;
		newobj->data[newobj->num_elems] = tmp;
		newobj->num_elems++;
	}

	return ret;

fail2:	ds_any_free(ret);
fail1:	return 0;
}
static int ds_array_compare(const ds *self, const ds *other, int *result) {
	ds_array *obj1 = self->data;
	ds_array *obj2 = other->data;
	size_t index = 0;
	int err;

	while(1) {
		if(index == obj1->num_elems && index == obj2->num_elems) {
			*result = 0;
			return 0;
		}
		if(index == obj1->num_elems) {
			*result = -1;
			return 0;
		}
		if(index == obj2->num_elems) {
			*result = 1;
			return 0;
		}
		if((err = ds_any_compare(ELEMENT(obj1, index), ELEMENT(obj2, index), result)))
			return err;
		if(*result)
			return 0;
		index++;
}	}
int ds_array_set(ds *self, ...) {
	va_list elems;
	ds *elem;
	ds_array *obj;
	size_t num_elems;

	ds_any_undef(self);

	num_elems = 0;
	va_start(elems, self);
	while(va_arg(elems, typeof(elem))) num_elems++;
	va_end(elems);

	if(!(self->data = obj = malloc(sizeof(ds_array) + num_elems * sizeof(elem)))) {ds_error = ds_error_sys; return ds_error;}
	num_elems = 0;
	va_start(elems, self);
	while((elem = va_arg(elems, typeof(elem)))) {
		obj->data[num_elems++] = elem;
		ds_any_incref(elem);
	}
	va_end(elems);
	self->type    = ds_array_type;
	self->undef   = ds_array_undef;
	self->copy    = ds_array_copy;
	self->compare = ds_array_compare;
	obj->elem_alloc = num_elems;
	obj->min_alloc = 0;
	obj->index_zero = 0;
	obj->num_elems = num_elems;
	return 0;
}
void ds_array_truncate(ds *self, size_t length) {
	ds_array *obj = self->data;
	size_t index;

	index = length;
	while(index < obj->num_elems) {
		ds_any_free(ELEMENT(obj, index));
		index++;
}	}
int ds_array_setelem(ds *self, size_t index, ds *elem) {
	ds_array *obj = self->data;
	ds *tmpelem;
	size_t old_num_elems = obj->num_elems;

	if(obj->num_elems < index + 1) {
		if(ds_array_expand(self, index + 1)) goto fail1;
		while(obj->num_elems < index + 1) {
			if(!(tmpelem = ds_any_new())) goto fail2;
			ELEMENT(obj, obj->num_elems) = tmpelem;
			obj->num_elems++;
	}	}
	ds_any_incref(elem);
	ELEMENT(obj, index) = elem;
	return 0;

fail2:	ds_array_truncate(self, old_num_elems);
fail1:	return ds_error;
}

#if 0
XXX: should take a va_list like ds_array_set
ds * ds_array_new() {
	ds *ret;

	if(!(ret = ds_any_new())) goto fail1;
	if(ds_array_set(ret, 0)) goto fail2;
	return ret;

fail2:	free(ret);
fail1:	return 0;
}
#endif
int ds_array_get(ds *self, ds **value, int index) {
	ds_array *obj = self->data;
	if(index >= obj->num_elems) {ds_error = ds_error_bound; return ds_error;}
	*value = ELEMENT(obj, index);
	ds_any_incref(*value);
	return 0;
}
ds * ds_array_slice(ds *self, size_t offset, size_t length) {
	ds *ret;
	ds_array *obj, *newobj;

	obj = self->data;
	if(offset > obj->num_elems) {
		offset = obj->num_elems;
		length = 0;
	} else if(length > obj->num_elems - offset) {
		length = obj->num_elems - offset;
	}

	if(!(ret = ds_any_new())) goto fail1;
	if(ds_array_set(ret, 0)) goto fail2;
	if(ds_array_resize(ret, length)) goto fail2;

	newobj = ret->data; /* newobj->index_zero is zero */

//XXX: ref count
	if(obj->index_zero + offset + length > obj->elem_alloc) {
		memcpy(&newobj->data[0], &obj->data[obj->index_zero + offset], (obj->elem_alloc - obj->index_zero - offset) * sizeof(obj->data[0]));
		memcpy(&newobj->data[obj->elem_alloc - obj->index_zero - offset], &obj->data[0], (length - obj->elem_alloc + obj->index_zero + offset) * sizeof(obj->data[0]));
	} else {
		memcpy(&newobj->data[0], &obj->data[obj->index_zero + offset], length * sizeof(obj->data[0]));
	}

	return ret;

fail2:	ds_any_free(ret);
fail1:	return 0;
}
int ds_array_splice(ds *self, size_t before_len, size_t delete_len, const ds *insert) {
	ds_array *obj = self->data;
	ds_array *insert_obj;
	size_t insert_len, after_len, index;
	ds *elem;

	obj = self->data;
	after_len = obj->num_elems - before_len - delete_len;
	if(insert) {
		if(insert->type != ds_array_type) {ds_error = ds_error_type; return ds_error;}
		insert_obj = insert->data;
		insert_len = insert_obj->num_elems;
	} else {
		insert_obj = 0;
		insert_len = 0;
	}

	if(ds_array_expand(self, obj->num_elems - delete_len + insert_len)) return ds_error;

	index = 0;
	while(index < delete_len) {
		ds_any_free(ELEMENT(obj, before_len + index));
		index++;
	}

	if(before_len >= after_len) {
		/* It's faster to move the elements after the slice */
		ds_array_moveelems(self, before_len + insert_len, before_len + delete_len, after_len);
	} else {
		/* It's faster to move the elements before the slice */
		/* index_zero can't go negative and you can't move to a negative offset */
		obj->index_zero += obj->elem_alloc - insert_len + delete_len;
		while(obj->index_zero >= obj->elem_alloc) obj->index_zero -= obj->elem_alloc;
		ds_array_moveelems(self, 0, insert_len - delete_len, before_len);
	}
	obj->num_elems = before_len + insert_len + after_len;

	index = 0;
	while(index < insert_len) {    /* if(!insert) insert_len = 0; */
		elem = ELEMENT(insert_obj, index);
		ds_any_incref(elem);
		ELEMENT(obj, before_len + index) = elem;
		index++;
	}

	ds_array_contract(self);
	return 0;
}
int ds_array_push(ds *self, ds *elem) {
	ds_array *obj = self->data;
	if(ds_array_expand(self, obj->num_elems + 1)) return ds_error;

	obj = self->data;
	obj->num_elems++;
	ELEMENT(obj, obj->num_elems - 1) = elem;
	ds_any_incref(elem);
	return 0;
}
int ds_array_push0(ds *self, ds *elem) {
	ds_array *obj = self->data;
	if(ds_array_expand(self, obj->num_elems + 1)) return ds_error;

	obj = self->data;
	if(obj->index_zero)
		obj->index_zero--;
	else
		obj->index_zero = obj->elem_alloc - 1;
	obj->data[obj->index_zero] = elem;
	obj->num_elems++;
	ds_any_incref(elem);
	return 0;
}
ds * ds_array_pop(ds *self) {
	ds *ret;
	ds_array *obj = self->data;
	if(! obj->num_elems) {
		ds_error = 0;
		return 0;
	}
	ret = ELEMENT(obj, obj->num_elems - 1);
	obj->num_elems--;
	ds_array_contract(self);
	return ret;
}
ds * ds_array_pop0(ds *self) {
	ds *ret;
	ds_array *obj = self->data;
	if(! obj->num_elems) {
		ds_error = 0;
		return 0;
	}
	ret = obj->data[obj->index_zero];
	obj->num_elems--;
	obj->index_zero++;
	if(obj->index_zero == obj->elem_alloc)
		obj->index_zero = 0;
	ds_array_contract(self);
	return ret;
}


/* Leif's quicker-sort:
 * This algorithm has a best case of O(n) when all the input values are equal.
 * It is O(n log n) for sorted and reverse sorted arrays (these are all O(n^2)
 * for traditional qsort).  The worst case is still O(n^2), but only happens
 * when middle always happens to be an extreme value and there are no repeated
 * values.  O(n^2) is pretty rare now.  It could be made even less common by
 * tweaking middle. */

static inline void swap(ds **first, ds **second) {
	ds *tmp;
	tmp = *first;
	*first = *second;
	*second = tmp;
}
static int qsorter(ds *self, size_t offset, size_t length) {
	ds_array *obj = self->data;
	ds *middle = ELEMENT(obj, offset + length / 2); /* Number to compare with */
	int lt_items = 0;                               /* Number of items less than middle */
	int eq_items = 0;                               /* Number of items equal to middle */
	int gt_items = 0;                               /* Number of items greater than middle */
	int un_items = length;                          /* Number of unpartitioned items */

	if(un_items < 2) return 0;

	/* The array stays in the order lt_items, eq_items, un_items, gt_items. */

	/* This shrinks un_items until it is 0. */
	while(un_items) {
		int comp;
		if(ds_any_compare(ELEMENT(obj, offset + lt_items + eq_items), middle, &comp) && ds_error != ds_error_undef) {
			ds_error = ds_error_type;
			return ds_error;
		}
// Test code: printf("lt=%i, eq=%i, gt=%i, un=%i, comp=%i\n", lt_items, eq_items, gt_items, un_items, comp);
		if(comp < 0) {
			if(eq_items)
				swap(&ELEMENT(obj, offset + lt_items + eq_items), &ELEMENT(obj, offset + lt_items));
			lt_items++;
		} else if(comp > 0) {
			gt_items++;
			swap(&ELEMENT(obj, offset + lt_items + eq_items), &ELEMENT(obj, offset + length - gt_items));
		} else {
			eq_items++;
		}
		un_items--;
	}

	if(qsorter(self, offset, lt_items)) return ds_error;
	if(qsorter(self, offset + length - gt_items, gt_items)) return ds_error;
	return 0;
}

int ds_array_qsort(ds *self, size_t offset, size_t length) {
	if(self->type != ds_array_type) {ds_error = ds_error_type; return ds_error;}
	if(length == -1) {
		ds_array *obj = self->data;
		if(obj->num_elems < offset) {ds_error = ds_error_bound; return ds_error;}
		length = obj->num_elems - offset;
	}
	return qsorter(self, offset, length);
}
