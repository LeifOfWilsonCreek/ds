/* integer functions */
extern const char *ds_int_type;
extern int ds_int_set(ds *self, long long value);
extern ds * ds_int_new(long long value);
extern int ds_int_get(ds *self, long long *value);
