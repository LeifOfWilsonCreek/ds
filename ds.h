/* DS means "Data Structure" */

#include <sys/types.h>

/* STRUCT DECLARATIONS */

/* This data type is used to refer to any storage unit. */
typedef struct ds {
	const char *type;
	void *data;
	int ref_count;
	int (*undef)();
	struct ds *(*copy)();
	int (*compare)();
} ds;


/* ERROR CODES */

extern int ds_error;
extern const int ds_error_sys;    /* A system call returned an error - check errno */
extern const int ds_error_undef;  /* A parameter which may not be undefined was */
extern const int ds_error_type;   /* A parameter was of the wrong type */
extern const int ds_error_bound;  /* A parameter was out of bounds */


/* FUNCTION PROTOTYPES */

/* General functions - These may be used on any ds type from below for the data
 * structure passed in. */
extern const char * ds_error_string(int error);                         /* Returns description of error from ds_error */
extern int ds_any_undef(ds *self);                                      /* Frees data associated with the ds and sets its value to "undefined"; ds is still allocated for use */
extern ds * ds_any_copy(const ds *self);                                /* Makes a copy as stores it in other */
extern int ds_any_compare(const ds *self, const ds *other, int *result);/* Compares the two, stores 0, a negative, or a positive in result */
extern ds * ds_any_new();                                               /* Creates a new ds with value "undef" */
extern int ds_any_defined(const ds *self);                              /* Returns true if the ds has a "defined" (meaningful) value */   // XXX: Non-conformal return value
extern int ds_any_free(ds *self);                                       /* Decrements ref_count, frees the ds if necessary */
extern void ds_any_incref(ds *self);                                    /* Incs ref_count, normally unnecessary unless the object is to be shared somehow */

#include "int.h"
#include "float.h"
#include "string.h"
#include "array.h"
#include "hash.h"

/* Notes:
 *
 *  - Functions which return a pointer:
 *     - On success, return a must pointer to a new object which must be freed
 *     - On error, set ds_error and return NULL (zero)
 *  - Functions which return an integer:
 *     - On success, return zero
 *     - On error, set ds_error and return ds_error (non-zero)
 *
 *  - Every pointer which is returned must be passed to ds_any_free.  If it has
 * been added to a container, it should still be passed to ds_any_free anyway to
 * decrement its ref_count.  For example, ds_array_push and ds_array_set
 * increment the element's ref_count, and the caller should pass it to
 * ds_any_free to decrement its ref_count when the caller is finished with it.
 * ds_array_pop does not change the ref_count because the object was deleted
 * from the array and given to the caller, so the caller should pass it to
 * ds_any_free when the caller is finished with it.  ds_array_get increments the
 * ref count because the object remains in the array and is passed back to the
 * caller.
 *
 *  - A ds is typeless, but the data stored with it is typed, so you can call
 * ds_float_set to assign an int to a ds that contained an array, but you can't
 * use ds_any_compare to compare a float and an array.
 *
 *  - Undef may be stored and retrieved, but most other operations will return
 * an error if it's used.  ds_any_compare will return ds_error_undef in ds_error
 * if either or both parameters are undefined, but it will still store a result.
 * It will store 0 if both are undef.  If one or the other is undef, it treats
 * the undef value as the smallest value.
 *
 *  - The difference between ds_any_undef and ds_any_free is that the former
 * assigns the "undef" value to the ds and keep it available for use. The latter
 * frees the ds itself, after which the pointer to it is invalid.  Both free
 * data associated with it.  Both cause the elements of containers to be passed
 * to ds_any_free.
 *
 *  - Using recursive data structures is STRONGLY discouraged.  Most operations
 * will cause a stack overflow if a recursive structure is passed to them.  This
 * might be fixed in distant future versions.  Don't hold your breath.
 *
 *  - Functions that operate on entire "containers" (arrays, for example) also
 * operate on the elements contained.  For example, calling ds_any_undef on an
 * array calls ds_any_free every element in the array; calling ds_any_copy on a
 * hash duplicates all the elements contained in the hash; calling
 * ds_any_compare on a complex data structure compares all elements and sub-
 * structures.  All elements stored in containers must be of type ds.
 */

/* All functions return 0 on success, a negative value to indicate an error,
 * and a positive value to indicate success with a warning. */
