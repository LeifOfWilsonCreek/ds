/* array functions */
extern const char *ds_array_type;
extern int ds_array_set(ds *self, ...);                                 /* Sets self to an array containing the specified ds's (the list is null-termed) */
extern ds * ds_array_new();
extern int ds_array_get(ds *self, ds **value, int index);
extern ds * ds_array_slice(ds *self, size_t offset, size_t length);     /* Returns a new array (which must be freed) containing a slice of self */
extern int ds_array_splice(ds *s, size_t o, size_t l, const ds *insert);/* Replaces the elements o to o+l with the elements in the array insert */
extern int ds_array_push(ds *self, ds *elem);                           /* Appends after the last element */
extern ds * ds_array_pop(ds *self);                                     /* Removes the last element */
extern int ds_array_push0(ds *self, ds *elem);                          /* Inserts at the 0th element (like unshift) */
extern ds * ds_array_pop0(ds *self);                                    /* Removes the 0th element (like shift) */
extern int ds_array_setalloc(ds *self, size_t size);                    /* Sets how many elements to expect the array to grow to (does not add elements) */
