/* string functions - may include nulls if length is specified */
extern const char *ds_string_type;
extern int ds_string_set(ds *self, size_t length, const char *value);   /* if(length == -1) length = strlen(value) + 1; */
extern ds * ds_string_new(size_t length, const char *value);
extern int ds_string_get(ds *self, size_t *length, char **value);
