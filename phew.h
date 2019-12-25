#ifndef _phew_h
#define _phew_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include "cJSON.h"

#define			PORT						12345
#define			MAX_WINDOW_WIDTH			8000
#define			MAX_COMMANDS				7
#define			TOKEN_SIZE					150

typedef struct {
	char title[MAX_WINDOW_WIDTH];
	char options[MAX_COMMANDS][MAX_WINDOW_WIDTH];
	int option_count;
	void (*interpreter)(char*);
} page;

page	start_page,
		main_page,
		chat_page;

page* current_page;

char	token			[TOKEN_SIZE],	
		result			[MAX_WINDOW_WIDTH],
		USERNAME		[MAX_WINDOW_WIDTH],
		PASSWORD		[MAX_WINDOW_WIDTH],
		CHANNEL_NAME	[MAX_WINDOW_WIDTH], 
		TITLE_COLOR		[TOKEN_SIZE],
		OPTION_COLOR	[TOKEN_SIZE],
		RESULT_COLOR	[TOKEN_SIZE];

void modify_to_start();
void clear();
void setstr(char* str1, char* str2); //str1 = str2;
void set_color(int bold, char* str); //str is color in lowercase
void reset_color();
void combine(char* res, int cnt, ...);
void send_message(char* message);
#endif
