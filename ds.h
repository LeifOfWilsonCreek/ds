/* DS means "Data Structure" */

#include <sys/types.h>

/* STRUCT DECLARATIONS */

/* This data type is used to refer to any storage unit.  The data element may
 * point to any of the structs below. */
typedef struct {
	void *data;   /* Null pointer means "undef" */
	int ref_count;
	int (*undef)();
	int (*copy)();
	int (*compare)();
} ds;

typedef struct {
	int64_t data;
} ds_int;

typedef struct {
	long double data;
} ds_float;

/* This can be a null-terminated string or a string of binary data */
typedef struct {
	size_t length;
	char data[0];
} ds_string;

typedef struct {
	size_t elem_alloc; /* Number of elements allocated (index_zero + num_elems <= elem_alloc) */
	size_t min_alloc;  /* Normally 0 unless tweaked by user to avoid reallocing (gets set back to 0 on ds_array_undef()) */
	size_t index_zero; /* Index (in data) of the zero'th element */
	size_t num_elems;  /* Number of valid elements */
	ds **data;
} ds_array;

//typedef struct {
	// dunno yet
//} ds_hash;




/* RETURN CODES */

/* All functions return 0 on success, a negative value to indicate an error,
 * and a positive value to indicate success with a warning. */

/* Error codes */
extern const int ds_err_sys;  /* A system call returned an error - check errno */
extern const int ds_err_type; /* A parameter was of the wrong type */

/* Warning codes */
/* ... None yet */


/* FUNCTION PROTOTYPES */

/* General functions - These may be used on any ds type from below for the data
 * structure passed in. */
int ds_any_undef(ds *self);                                             /* Frees data associated with the ds and sets its value to "undefined"; ds is still allocated for use */
int ds_any_copy(const ds *self, ds **other);                            /* Makes a copy as stores it in other */
int ds_any_compare(const ds *self, const ds *other, int *result);       /* Compares the two, stores 0, a negative, or a positive in result */
int ds_any_new(ds **self);                                              /* Creates a new ds with value "undef" */
int ds_any_defined(const ds *self);                                     /* Returns true if the ds has a "defined" (meaningful) value */   // XXX: Non-conformal return value
int ds_any_free(ds *self);                                              /* Decrements ref_count, frees the ds if necessary */





/* integer functions */
int ds_int_undef(ds *self);
int ds_int_copy(const ds *self, ds **copy);
int ds_int_compare(const ds *self, const ds *other, int *result);
int ds_int_set(ds *self, int64_t value);

/* float functions */
int ds_float_undef(ds *self);
int ds_float_copy(const ds *self, ds **copy);
int ds_float_compare(const ds *self, const ds *other, int *result);
int ds_float_set(ds *self, long double value);

/* string functions - may include nulls if length is specified */
int ds_string_undef(ds *self);
int ds_string_copy(const ds *self, ds **copy);
int ds_string_compare(const ds *self, const ds *other, int *result);
int ds_string_set(ds *self, size_t length, const char *value);    /* if(length == -1) length = strlen(value+1); */

/* array functions */
int ds_array_undef(ds *self);
int ds_array_copy(const ds *self, ds **copy);
int ds_array_compare(const ds *self, const ds *other, int *result);
int ds_array_set(ds *self, size_t index, const void *elem);
//int ds_array_setalloc(ds *self, size_t size);             /* Sets how many elements to expect the array to grow to (does not add elements) */
//int ds_array_push(ds *self, const void *elem);            /* push appends after the last element */
//int ds_array_pop(ds *self, void **elem);                  /* pop removes the last element */
//int ds_array_push0(ds *self, const void *elem);           /* push0 inserts at the 0th element (like unshift) */
//int ds_array_pop0(ds *self, void **elem);                 /* pop0 removes the 0th element (like shift) */

/* hash functions */
//int ds_hash_undef(ds_hash *self);
//int ds_hash_copy(const ds_hash *self, ds_hash **copy);
//int ds_hash_compare(const ds_hash *self, const ds_hash *other, int *result);
//int ds_hash_set(ds_hash *self, const ds *key, const void *elem);


/* Notes:
 *
 *  - Using recursive data structures is STRONGLY discouraged.  Most operations
 * will cause a stack overflow if a recursive structure is passed to them.  This
 * could possibly be fixed in distant future versions.  Don't hold your breath.
 *
 *  - Functions that operate on entire "containers" (arrays, for example) also
 * operate on the elements contained.  For example, calling ds_any_undef() on
 * an array calls ds_any_free() every element in the array; calling
 * ds_any_copy() on a hash duplicates all the elements contained in the hash;
 * calling ds_any_compare() on a complex data structure compares all elements
 * and sub-structures.  All elements stored in containers must be of type ds.
 */
