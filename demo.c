#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ds.h"

ds * ds_interpret(char *);
ds * hashSet(char *);

int main()
{
	ds *testTypes;
	ds *hashValues;
	ds *stuff;

	long long value;
	long double value2;
	char *str;
	size_t str_size;

	char string[] = "[24, 1.5, 'testing string', hash['some', 'thing', 'more', 'so'] ]";	
	char *string2 = "some";
	char *string3 = "more";

	testTypes = ds_any_new();
	hashValues = ds_any_new();
	
	testTypes = ds_interpret(string);

	//INT
	ds_array_get(testTypes, &stuff, 0);
	ds_int_get(stuff, &value);
	printf("Integer value:  %lli\n", value);

	//FLOAT	
	ds_array_get(testTypes, &stuff, 1);
	ds_float_get(stuff, &value2);
	printf("Float value: %Lf\n", value2);

	//STRING
	ds_array_get(testTypes, &stuff, 2);
	ds_string_get(stuff, &str_size, &str);
	printf("String: %s\n", str);

	//HASH
	ds_array_get(testTypes, &stuff, 3);
	hashValues = ds_hash_get(stuff, string2);
	ds_string_get(hashValues, &str_size, &str);
	printf("Key: %s -> Value: %s\n", string2, str);

	hashValues = ds_hash_get(stuff, string3);
	ds_string_get(hashValues, &str_size, &str);
	printf("Key: %s -> Value: %s\n", string3, str);

	ds_any_free(testTypes);
	return 0;
}

//****************************************************
//ds_interpret takes a string argument in the form of:
//[int, float, 'string', hash['key', 'value']]
//The string is parsed and each element is added to 
//it's appropriate DS object.  Then each object is 
//added to a DS array, which is then returned to 
//main.
//****************************************************
ds * ds_interpret(char ds_string[])
{
	ds *arrayType;
	ds *intType;
	ds *floatType;
	ds *stringType;
	ds *hashType;	

	char *intString;
	char *floatString;
	char *strString;
	char *hashString;

	arrayType	= ds_any_new();
	intType      	= ds_any_new();
	floatType	= ds_any_new();
	stringType	= ds_any_new();
	hashType	= ds_any_new();
	
	intString = strtok(ds_string, "[,");
	floatString = strtok(NULL, ",");
	strString = strtok(NULL, ",");
	hashString = strtok(NULL, "]");
	
	strString = strtok(strString, "\'");
	strString = strtok(NULL, "\'");
	
	ds_int_set(intType, atoi(intString));
	ds_float_set(floatType, atof(floatString));
	ds_string_set(stringType, -1, strString);
	hashType = hashSet(hashString);
	
	ds_array_set(arrayType, intType, floatType, stringType, hashType, NULL);
	
	return arrayType;	

}
//***************************************************************
//hashSet recieves a string in the form of 
//hash['key1', 'value1', 'key2', 'value2', ... , 'keyN', 'valueN'
//the string strips out each key, value pair and insert them into
//a hash using ds_hash_set_elem.  hashSet returns a ds pointer 
//of type hash
//***************************************************************
ds * hashSet(char a[])
{
	ds *hashDS;
	ds *value;
	ds *value2;	

	char *tmp;
	char *key;
	char *key2;

	hashDS = ds_any_new();
	value = ds_any_new();
	value2 = ds_any_new();

	tmp = strtok(a, "[");
	tmp = strtok(NULL, "[");
	tmp = strtok(tmp, "',");
	key = tmp;
	
	tmp = strtok(NULL, "'");
	tmp = strtok(NULL, "'");
	ds_string_set(value, -1, tmp);

	tmp = strtok(NULL, "'");
	tmp = strtok(NULL, "'");
	key2 = tmp;

	tmp = strtok(NULL, "'");
	tmp = strtok(NULL, "'");
	ds_string_set(value2, -1, tmp);

	ds_hash_set(hashDS, key, value, key2, value2, NULL);
	
	ds_any_free(value);
	ds_any_free(value2);

	return hashDS;
}
