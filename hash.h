#include <sys/types.h>

/* hash functions */
extern const char *ds_hash_type;

extern struct ds *ds_hash_copy(ds *self);
extern ds *ds_hash_get(ds *self, const char *key);
int ds_hash_compare(ds *self, ds *other, int *result);
extern int ds_hash_set_elem(ds *self, const char *key, ds *value);
extern ds *ds_hash_set(ds *self, ...);
extern void ds_hash_undef_elem(ds *self, const char *key);
extern void ds_hash_undef(ds *self);



