#include <stdio.h>

#include "ds.h"

void panic(const char *file, const char *function, int line) {
	fprintf(stderr, "%s %s %i: %s\n", file, function, line, ds_error_string(ds_error));
	exit(1);
}

#define panic() panic(__FILE__, __FUNCTION__, __LINE__)

int main() {
//	ds *array;
	ds *foo;
	char *ptr;

	if(!(foo = ds_any_new())) panic();
	if(ds_string_set(foo, -1, "test string")) panic();


	if(!(foo = ds_any_new())) panic();
	if(ds_string_set(foo, -1, "test string")) panic();
	ds_string_get(foo, 0, &ptr);
	puts(ptr);
	ds_any_free(foo);

	if(!(foo = ds_string_new(-1, "test string2"))) panic();
	ds_string_get(foo, 0, &ptr);
	puts(ptr);
	ds_any_free(foo);

	return 0;
}
