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

void make_it_no_space(char* it) //correct
{
	int edited_pos = 0;
	int lvl = 0;
	for(int pos = 0; it[pos] != '\0'; pos++)
	{
		if (lvl % 2 == 1 || (it[pos] != ' ' && it[pos] != '\n' && it[pos] != '\t')) it[edited_pos++] = it[pos];
		if (it[pos] == '"') lvl ++;
	}
	printf("wtf: %s\n", it);
	it[edited_pos] = '\0';
}

void fill_array(char* it, ITEM* item)
{
	ITEM* element = (ITEM*) malloc(sizeof(ITEM));
	if (*it == ']') 
	{
		printf(it);
		item->end_pos ++;
		free(element);
		return;
	}
	read_value(it, element, 0);
	item->a_part[item->a_len++] = element;
	item->end_pos += element->end_pos;
	printf("%s :|\n", it);
	if (it[element->end_pos] == ',')  item->end_pos ++, fill_array(it + element->end_pos + 1, item);
	else fill_array(it + element->end_pos, item);
}
		
void read_value(char* it, ITEM* item, int x)
{
	if (*it == '{') 
	{
		item->type = 'j';
		item->j_part = jackie__parse_it(it);
		item->end_pos = x + item->j_part->end_pos;
		return;
	}
	if (*it == '[')
	{
		item->type = 'a';
		item->end_pos = x + 1;
		printf("array has been recognized\n");
		fill_array(it+1, item);
		return;
	}
	if (*it == '"') //correct
	{
		item->type = 's';
		printf(it);
		sscanf(it, "\"%[^\"]\"", item->s_part);
		printf("value: %s\n", item->s_part);
		item->end_pos = strlen(item->s_part) + 2 + x;
		return;
	}
	int pos = 0;
	int is_double = 0;
	while (it[pos] != ',' && it[pos] != '}' && it[pos] != ']')
		if (it[pos++] == '.') is_double = 1;
	item->end_pos = pos + x;
	if (is_double) //correct
	{
		item->type = 'd';
		sscanf(it, "%lf", &item->d_part);
		printf("value: %lf\n", item->d_part, item->end_pos);
		return;
	}
	else //correct
	{
		item->type = 'i';
		sscanf(it, "%d", &item->i_part);
		printf("value: %d %d\n", item->i_part, pos, item->end_pos);
		return;
	}
}

ITEM* ibo__parse_it(char* it)
{
	ITEM* item = (ITEM*) malloc(sizeof(ITEM));
	sscanf(it, "\"%[^\"]\":", item->title);
	printf("%s title: %s\n", it, item->title);
	int pos = strlen(item->title) + 3;
	read_value(it + pos, item, pos);
	return item;
}


JSON* jackie__parse_it(char* it)
{
	make_it_no_space(it);
	printf("no space: %s\n", it);
	int lvl = 0;
	JSON* json = (JSON*) malloc(sizeof(JSON));
	json->item_count = 0;
	for (int cursor_pos = 0; cursor_pos < strlen(it); cursor_pos ++)
	{
		if (it[cursor_pos] == '"')
		{
			json->item[json->item_count] = ibo__parse_it(it + cursor_pos);
			cursor_pos += ((ITEM*)(json->item[json->item_count++]))->end_pos;
		}
		if (it[cursor_pos] == '}') 
		{
			json->end_pos = cursor_pos+1;
			return json;
		}
		printf("%s\n", it + cursor_pos);
	}
}

int main1()
{
	char s[100] = "{\"json\":{\"type\": [12.25, 12], \"content\":\"salam\"}  , \"bazi\": 12}";
	JSON* ah = jackie__parse_it(s);
	printf("%d yaay\n", ((ITEM*)ah->item[1])->i_part);
	ah = ((ITEM*)ah->item[0])->j_part;
	printf("%lf good\n", ((ITEM*)((ITEM*)ah->item[0])->a_part[0])->d_part);
	printf("%s good\n", ((ITEM*)ah->item[1])->s_part);
}
