/* float functions */
extern const char *ds_float_type;
extern int ds_float_set(ds *self, long double value);
extern ds * ds_float_new(long double value);
extern int ds_float_get(ds *self, long double *value);
