#if 0
#include <stdio.h>

#include "ds.c"

void panic(const char *file, const char *function, int line) {
	fprintf(stderr, "%s %s %i: %s\n", file, function, line, ds_strerr(ds_error));
	exit(1);
}

#define panic() panic(__FILE__, __FUNCTION__, __LINE__)

int main() {
	ds *array;
	ds *foo;

	if(!(foo = ds_any_new())) panic();
	if(ds_string_set(foo, -1, "test string")) panic();


	if(!(foo = ds_any_new())) panic();
	if(ds_string_set(foo, -1, "test string")) panic();
	puts(((ds_string *)foo->data)->data);
	ds_any_free(foo);

	if(!(foo = ds_string_new(-1, "test string2"))) panic();
	puts(((ds_string *)foo->data)->data);
	ds_any_free(foo);

	return 0;
}
#endif


#include <stdlib.h>
#include <stdio.h>
#include "ds.h"

/*
*	testds.c
*		A program written for testing the various functionality of ds.c
*/

// the following are the various values used in the various tests.
int testInt 		= 555;
float testFloat 	= 555.555;
char *testString	= "This is a test String.";
int testArray[5]	= {5, 5, 5, 5, 5};
//testHash...

// the following are the declarations of the various functions

//these use any_new and ds_type_set to function
void testAnyDS();
void testIntDS(int myInt);
void testFloatDS(float myFloat);
void testStringDS(char *myString);
void testArrayDS(int *myArray);
//void testHashDS(testHash...);

//when these are written, should use ds_type_new to work.

int main()
{
	printf("main\n");
	testAnyDS();
	testIntDS(testInt);
	testFloatDS(testFloat);
	testStringDS(testString);
	testArrayDS(testArray);
//	testHashDS(testHash);

	return 0;
}

void testAnyDS()
{
	ds *noTypeDS;

	printf("testAnyDS\n");
	noTypeDS = ds_any_new();
}

void testIntDS(int myInt)
{
	ds *intTypeDS;	
	long long value;

	printf("testIntDS\n");
	intTypeDS = ds_any_new();
	ds_int_set(intTypeDS, myInt);
	ds_int_get(intTypeDS, &value);
	printf("%lli\n", value);
}

void testFloatDS(float myFloat)
{
	ds *floatTypeDS;

	printf("testFloatDS\n");
	floatTypeDS = ds_any_new();
	ds_float_set(floatTypeDS, myFloat);
}

void testStringDS(char *myString)
{
	ds *stringTypeDS;

	printf("testStringDS\n");
	stringTypeDS = ds_any_new();
	ds_string_set(stringTypeDS, -1, myString);
}

void testArrayDS(int *myArray)
{
	ds *arrayTypeDS;

	printf("testArrayDS\n");
	arrayTypeDS = ds_any_new();
//	ds_array_set(arrayTypeDS, myArray);
}

//void testHashDS(testHash...)
//{
//	ds *hashTypeDS;
//}
