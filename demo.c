#if 0
#include <stdio.h>

#include "ds.h"

int main() {
	ds *foo;

	if(!(foo = ds_any_new())) printf("Failed 1: ds_errno = %i\n", ds_errno), exit(1);
	if(ds_string_set(foo, -1, "test string")) printf("Failed 2: ds_errno = %i\n", ds_errno), exit(1);
	puts(((ds_string *)foo->data)->data);
	ds_any_free(foo);

	if(!(foo = ds_string_new(-1, "test string"))) printf("Failed 3: ds_errno = %i\n", ds_errno), exit(1);
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
	int *value;

	printf("testIntDS\n");
	intTypeDS = ds_any_new();
	ds_int_set(intTypeDS, myInt);
	ds_int_get(intTypeDS, value);
	printf(value);
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
