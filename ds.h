/* DS means "Data Structure" */

#include <sys/types.h>

/* STRUCT DECLARATIONS */

/* This data type is used to refer to any storage unit.  The data element may
 * point to any of the structs below. */
typedef struct ds {
	const char *type;   /* Null pointer here means "undef" */
	void *data;
	int ref_count;
	int (*undef)();
	struct ds *(*copy)();
	int (*compare)();
} ds;


/* ERROR CODES */
extern int ds_errno;
extern const int ds_err_sys;    /* A system call returned an error - check errno */
extern const int ds_err_type;   /* A parameter was of the wrong type */
extern const int ds_err_undef;  /* A parameter which may not be undefined was */


/* FUNCTION PROTOTYPES */

/* General functions - These may be used on any ds type from below for the data
 * structure passed in. */
extern int ds_any_undef(ds *self);                                      /* Frees data associated with the ds and sets its value to "undefined"; ds is still allocated for use */
extern ds * ds_any_copy(const ds *self);                                /* Makes a copy as stores it in other */
extern int ds_any_compare(const ds *self, const ds *other, int *result);/* Compares the two, stores 0, a negative, or a positive in result */
extern ds * ds_any_new();                                               /* Creates a new ds with value "undef" */
extern int ds_any_defined(const ds *self);                              /* Returns true if the ds has a "defined" (meaningful) value */   // XXX: Non-conformal return value
extern int ds_any_free(ds *self);                                       /* Decrements ref_count, frees the ds if necessary */

/* integer functions */
extern const char *ds_int_type;
extern int ds_int_set(ds *self, long long value);
extern ds * ds_int_new(long long value);
extern int ds_int_get(ds *self, long long *value);

/* float functions */
extern const char *ds_float_type;
extern int ds_float_set(ds *self, long double value);
extern ds * ds_float_new(long double value);
extern int ds_float_get(ds *self, long double *value);

/* string functions - may include nulls if length is specified */
extern const char *ds_string_type;
extern int ds_string_set(ds *self, size_t length, const char *value);   /* if(length == -1) length = strlen(value) + 1; */
extern ds * ds_string_new(size_t length, const char *value);
extern int ds_string_get(ds *self, size_t *length, const char **value);

/* array functions */
extern const char *ds_array_type;
extern int ds_array_set(ds *self, ...);                                 /* Sets self to an array containing the specified ds's (the list is null-termed) */
extern ds * ds_array_new();
extern int ds_array_get(ds *self, ds **value, int index);
ds * ds_array_slice(ds *self, size_t offset, size_t length);
extern int ds_array_push(ds *self, ds *elem);                           /* push appends after the last element */
extern ds * ds_array_pop(ds *self);                                     /* pop removes the last element */
extern int ds_array_push0(ds *self, ds *elem);                          /* push0 inserts at the 0th element (like unshift) */
extern ds * ds_array_pop0(ds *self);                                    /* pop0 removes the 0th element (like shift) */
extern int ds_array_setalloc(ds *self, size_t size);                    /* Sets how many elements to expect the array to grow to (does not add elements) */

/* hash functions */
//extern const char *ds_hash_type;
//...

/* Notes:

 *  - Functions which return a pointer:
 *     - Return NULL (zero) on failure
 *     - 
 *  - Functions which return an integer:
 *     - Set ds_errno and return ds_errno on error
 *     - Return zero on success
 *
 *  - A ds is typeless, but the data stored with it is typed.  So you can call
 * ds_float_set() to assign an int to a ds that contained an array, but you
 * can't use ds_any_compare() to compare a float and an array.
 *
 *  - Undef may be stored and retrieved, but most other operations will return
 * an error if it's used.  ds_any_compare() will return ds_err_undef in ds_errno
 * if either or both parameters are undefined, but it will still store a result.
 * It will store 0 if both are undef.  If one or the other is undef, it treats
 * the undef value as the smallest value.
 *
 *  - The difference between ds_any_undef() and ds_any_free() is that the former
 * assigns the "undef" value to the ds and keep it available for use.  The
 * frees the ds itself, after which the pointer to it is invalid.
 *
 *  - Using recursive data structures is STRONGLY discouraged.  Most operations
 * will cause a stack overflow if a recursive structure is passed to them.  This
 * might be fixed in distant future versions.  Don't hold your breath.
 *
 *  - Functions that operate on entire "containers" (arrays, for example) also
 * operate on the elements contained.  For example, calling ds_any_undef() on
 * an array calls ds_any_free() every element in the array; calling
 * ds_any_copy() on a hash duplicates all the elements contained in the hash;
 * calling ds_any_compare() on a complex data structure compares all elements
 * and sub-structures.  All elements stored in containers must be of type ds.
 */

/* All functions return 0 on success, a negative value to indicate an error,
 * and a positive value to indicate success with a warning. */
