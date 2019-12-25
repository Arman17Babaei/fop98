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
		
////////////////////////////////////////////////////////////////
void modify_to_start()
{
	setbuf(stdout, NULL);
	strcpy(TITLE_COLOR, "red");
	strcpy(OPTION_COLOR, "cyan");
	strcpy(RESULT_COLOR, "yellow");
}

void clear()
{
	system("@cls||clear");
}

void setstr(char* str1, char*str2)
{
	memcpy(str1, str2, strlen(str2) + 1);
}

void set_color(int bold, char* str)
{
	char res[30];
	setstr(res, "\033[");
	if (bold) strcat(res, "1;");
	else strcat(res, "0;");
	if (!strcmp(str, "gray")) strcat(res, "30m");
	if (!strcmp(str, "red")) strcat(res, "31m");
	if (!strcmp(str, "green")) strcat(res, "32m");
	if (!strcmp(str, "yellow")) strcat(res, "33m");
	if (!strcmp(str, "blue")) strcat(res, "34m");
	if (!strcmp(str, "magneta")) strcat(res, "35m");
	if (!strcmp(str, "cyan")) strcat(res, "36m");
	if (!strcmp(str, "white")) strcat(res, "37m");
	printf("%s", res); 
}

void reset_color()
{
	printf("\033[0m");
}

void combine(char* res, int cnt, ...)
{
	va_list list;
	va_start(list, cnt);
	setstr(res, "");
	for(int i = 0; i < cnt; i ++)
	{
		strcat(res, va_arg(list, char*));
	}
	va_end(list);
}

void send_message(char* message)
{
	int server_socket;
	struct sockaddr_in servaddr;
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket == -1)
	{
		printf("Socket creation failed...\n");
		exit(0);
	}
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);

	if (connect(server_socket, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) 
	{
		printf("Connection to server failed...\n");
		exit(0);
	}
	char request[MAX_WINDOW_WIDTH];
	bzero(request, sizeof(request));
	for (int i = 0; message[i] != '\0'; ++i)
	{
		request[i] = message[i];
		request[i+1] = '\n';
	}
	send(server_socket, request, sizeof(request), 0);
	char answer[MAX_WINDOW_WIDTH];
	bzero(answer, sizeof(answer));
	printf("Waiting for response...");
	recv(server_socket, answer, sizeof(answer), 0);
	setstr(result, answer);
	printf("From server: %s\n", answer);
	shutdown(server_socket, SHUT_RDWR);
}
