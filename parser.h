#ifndef _parser_h
#define _parser_h

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_ITEM_COUNT 100
#define MAX_TITLE_LENGTH 100
#define MAX_JSON_LENGTH 10000
#define MAX_ARRAY_LENGTH 100
#define MAX_STRING_LENGTH 200

typedef struct {
	int item_count;
	void* item[100]; // ITEM*

	int end_pos;
} JSON;

typedef struct {
	char type;

	char title[MAX_TITLE_LENGTH];

	double d_part;
	int i_part;
	char c_part;
	char s_part[MAX_STRING_LENGTH];
	void* a_part[MAX_ARRAY_LENGTH];
	int a_len;
	JSON* j_part;

	int end_pos;
} ITEM;

void make_it_no_space(char* it);
ITEM* ibo__parse_it(char* it);
JSON* jackie__parse_it(char* it);
void fill_array(char* it, ITEM* item);
void read_value(char* it, ITEM* item, int x);


#endif
