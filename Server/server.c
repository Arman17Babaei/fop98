#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include "cJSON.h"
#include <netinet/ip.h> 
#include <dirent.h>

#define MAX 80 
#define PORT 12345
#define SA struct sockaddr 

#define MAXCHANNELUSERS 100
#define TOKENSIZE 80

void init_server();
typedef struct {
	char username[100];
	char password[100];
	int channel;
	char auth_token[100];
	int is_online;
	int last_message;
} User;

typedef struct {
	int m_id;
	char channel[100];
	char sender[100];
	char content[1000];
} Message;

typedef struct {
	char name[100];
	int members[100];
	int member_count;
	int message_count;
} Channel;

User online_users[1000];
Channel channels[1000];
int user_count = 0;
int channel_count = 0;

void init_server();
void process(int client_socket);
void setstr(char* str1, char*str2);
void combine(char* res, int cnt, ...);
void make_auth(User* user) ;
void save_user(User* user); 
void save_channel(Channel* channel);
void save_messages(Message* message);
User* init_user(char* username, char* password);
char* register_user(char* buffer);
char* login_user (char* buffer);
char* create_channel(char* buffer);
void load_channels();
char* join_channel(char* buffer);
char* send_message(char* buffer);
void add_message(User* user, Channel* channel, char* message);
int get_last_message(Channel* channel);
int to_num(char* str);
char* refresh(char* buffer);
char* wrong(char* buffer);
char* get_members(char* buffer);
char* leave(char* buffer);
char* logout(char* buffer);

void process(int client_socket) 
{ 
	char buffer[1000]; 
	int n; 
	bzero(buffer, sizeof(buffer)); 
	recv(client_socket, buffer, sizeof(buffer), 0); 
	printf("%s\n", buffer);
	char* (*interpreter)(char*);
	int pos = 0;

	if (strncmp(buffer, "register", 8) == 0) pos = 9, interpreter = register_user;
	else if (strncmp(buffer, "login", 5) == 0) pos = 6, interpreter = login_user;
	else if (strncmp(buffer, "create channel", 14) == 0) pos = 15, interpreter = create_channel;
	else if (strncmp(buffer, "join channel", 12) == 0) pos = 13, interpreter = join_channel;
	else if (strncmp(buffer, "send", 4) == 0) pos = 5, interpreter = send_message;
	else if (strncmp(buffer, "refresh", 7) == 0) pos = 8, interpreter = refresh;
	else if (strncmp(buffer, "channel members", 15) == 0) pos = 16, interpreter = get_members;
	else if (strncmp(buffer, "leave", 5) == 0) pos = 6, interpreter = leave;
	else if (strncmp(buffer, "logout", 6) == 0) pos = 7, interpreter = logout;
	else pos = 0, interpreter = wrong;

	char* res = interpreter(buffer + pos);

	send(client_socket, res, 10000, 0); 
	printf("sent:<<%s>>\n", res);
	free(res);

	if (strncmp("exit", buffer, 4) == 0) { 
		printf("Server stopping...\n"); 
		return; 
	} 
} 


int main() {
	load_channels();
	while(1)
	{
		init_server();
	}
}

char* logout(char* query)
{
	query[strlen(query)-1] = '\0';
	int pos = strlen(query);
	char* res = (char*) malloc(1000);
	printf("refresh: %s\n", query);
	for (int i = 0; i < user_count; i++)
	{
		if (online_users[i].is_online && strcmp(query, online_users[i].auth_token) == 0)
		{
			User* user = &online_users[i];
			online_users[i].channel = -1;
			online_users[i].auth_token[0] = '\0';
			online_users[i].is_online = 0;
			strcpy(res, "{\"type\":\"Successful\",\"content\":\"\"}");
			return res;
		}
	}
	sprintf(res, "{\"type\":\"Error\",\"content\":\"invalid auth\"}\n");
	return res;
}


char* leave(char* query)
{
	query[strlen(query)-1] = '\0';
	int pos = strlen(query);
	char* res = (char*) malloc(1000);
	printf("refresh: %s\n", query);
	for (int i = 0; i < user_count; i++)
	{
		if (online_users[i].is_online && strcmp(query, online_users[i].auth_token) == 0)
		{
			User* user = &online_users[i];
			if (online_users[i].channel == -1)
			{
				sprintf(res, "{\"type\":\"Error\",\"content\":\"out of channels:(\"}");
				return res;
			}
			online_users[i].channel = -1;
			strcpy(res, "{\"type\":\"Successful\",\"content\":\"\"}");
			return res;
		}
	}
	sprintf(res, "{\"type\":\"Error\",\"content\":\"invalid auth\"}\n");
	return res;
}


char* wrong(char* query)
{
	char* res = malloc(1000);
	strcpy(res, "{\"type\":\"Error\",\"content\":\"What do you want?\"}");
	return res;
}

char* get_members(char* query)
{
	query[strlen(query)-1] = '\0';
	int pos = strlen(query);
	char* res = (char*) malloc(10000);
	printf("members: %s\n", query);
	for (int i = 0; i < user_count; i++)
	{
		if (online_users[i].is_online && strcmp(query, online_users[i].auth_token) == 0)
		{
			User* user = &online_users[i];
			if (online_users[i].channel == -1)
			{
				sprintf(res, "{\"type\":\"Error\",\"content\":\"out of channels:(\"}");
				return res;
			}
			Channel* channel = &channels[online_users[i].channel];
			printf("i e man %d\n", i);
			strcpy(res, "{\"type\":\"List\",\"content\":[");
			for (int j = 0; j < channel->member_count; j ++)
			{
				printf("investigating memeber %d\n", channel->members[j]);
				if (online_users[channel->members[j]].is_online == 0) 
				{
					printf("chon offe\n"); 
					continue;
				}
				if (online_users[channel->members[j]].channel != online_users[i].channel) 
				{
					printf("chon va3 yeki dige bud\n");
				   	continue;
				}
				strcat(res, "\"");
				strcat(res, online_users[channel->members[j]].username);
				strcat(res, "\",");
			}
			if (res[strlen(res) - 1] == ',') res[strlen(res)-1] = '\0';
			strcat(res, "]}");
			return res;
		}
	}
	sprintf(res, "{\"type\":\"Error\",\"content\":\"invalid auth\"}\n");
	return res;

}

char* refresh(char* query)
{
	query[strlen(query)-1] = '\0';
	int pos = strlen(query);
	char* res = (char*) malloc(10000);
	printf("refresh: %s\n", query);
	for (int i = 0; i < user_count; i++)
	{
		if (online_users[i].is_online && strcmp(query, online_users[i].auth_token) == 0)
		{
			User* user = &online_users[i];
			if (online_users[i].channel == -1)
			{
				sprintf(res, "{\"type\":\"Error\",\"content\":\"out of channels:(\"}");
				return res;
			}
			Channel* channel = &channels[user->channel];
			strcpy(res, "{\"type\":\"List\",\"content\":[");

			printf("!@#%s!@#\n", res);
			int t = get_last_message(channel);
			for (int i = user->last_message; i < t; i ++)
			{

				FILE* file;
				char file_add[120];
				sprintf(file_add, "data/Messages/%s_%d", channel->name, i);
				printf("opened %s\n", file_add);
				if ((file = fopen(file_add, "r")) == NULL) 
				{
					strcat(res, "{\"sender\":\"unknown\",\"content\":\"somebody touched my message\"},");
				}
				else
				{
					char tmp[1000];
					char sender[100];
					char message[1000];
					fgets(sender, 100, file);
					sender[strlen(sender)-1] = '\0';
					fgets(message, 1000, file);
					message[strlen(message)-1] = '\0';
					sprintf(tmp, "{\"sender\":\"%s\",\"content\":\"%s\"},", sender, message);
					strcat(res, tmp);
					fclose(file);
				}
				if (i == t -1) res[strlen(res)-1] = '\0';
			}
			strcat(res, "]}");
			user->last_message = t;
			return res;
		}
	}
	sprintf(res, "{\"type\":\"Error\",\"content\":\"invalid auth\"}\n");
	return res;
}

char* send_message(char* query)
{
	query[strlen(query)-1] = '\0';
	int pos = 0;
	char* res = (char*) malloc(1000);
	printf("%s\n", query);
	for(int i = 0; i < strlen(query); i ++)
	{
		if (query[i] == ',') 
		{
			printf("%s peida shod %d", query, i);
			pos = i;
			break;
		}
	}
	query[pos] = '\0';
	for (int i = 0; i < user_count; i++)
	{
		if (online_users[i].is_online && strcmp(query + pos + 2, online_users[i].auth_token) == 0)
		{
			if (online_users[i].channel == -1)
			{
				sprintf(res, "{\"type\":\"Error\",\"content\":\"out of channels:(\"}");
				return res;
			}
			add_message(&online_users[i], &channels[online_users[i].channel], query);
			strcpy(res, "{\"type\":\"Successful\",\"content\":\"\"}");
			return res;
		}
	}
	sprintf(res, "{\"type\":\"Error\",\"content\":\"invalid auth\"}");
	return res;
}

void add_message(User* user, Channel* channel, char* message)
{
	char file_add[120];
	sprintf(file_add, "data/Messages/%s_%d", channel->name, get_last_message(channel));
	FILE* file = fopen(file_add, "w");
	fprintf(file, "%s\n%s\n", user->username, message);
	fclose(file);
}

int to_num(char* str)
{
	int res = 0;
	for (int i = 0; str[i] != '\0'; res *= 10, res += str[i++]-'0');
	return res;
}

int get_last_message(Channel* channel)
{
	DIR* dir;
	struct dirent *ent;
	dir = opendir("data/Messages");
	int mx = -1;
	char tmp[110];
	strcpy(tmp, channel->name);
	strcat(tmp, "_");
	while ((ent = readdir(dir)) != NULL)
	{
		if (strncmp(ent->d_name, tmp, strlen(tmp)) == 0) 
		{
			int x = to_num(ent->d_name + strlen(tmp));
			if (mx < x) mx = x;
		}
	}
	return mx + 1;
}
char* join_channel(char* query)
{
	query[strlen(query)-1] = '\0';
	int pos = 0;
	char* res = (char*) malloc(1000);
	printf("%s\n", query);
	for(int i = 0; i < strlen(query); i ++)
	{
		if (query[i] == ',') 
		{
			printf("%s peida shod %d", query, i);
			pos = i;
			break;
		}
	}
	query[pos] = '\0';
	for (int i = 0; i < user_count; i++)
	{
		if (online_users[i].is_online && strcmp(query + pos + 2, online_users[i].auth_token) == 0)
		{
			if (online_users[i].channel != -1)
			{
				sprintf(res, "{\"type\":\"Error\",\"content\":\"user was joined in <%s>\"}", channels[online_users[i].channel].name);
				return res;
			}

			for (int j = 0; j < channel_count; j ++)
			{
				if (strcmp(channels[j].name, query) == 0)
				{
					online_users[i].channel = j;
					sprintf(res, "{\"type\":\"Successful\",\"content\":\"\"}");
					for(int x = 0; x < channels[j].member_count; x ++)
						if (channels[j].members[x] == i) return res;
					channels[j].members[channels[j].member_count++] = i;
					return res;
				}
			}
			sprintf(res, "{\"type\":\"Error\",\"content\":\"channel not found\"}");
			return res;
		}
	}
	sprintf(res, "{\"type\":\"Error\",\"content\":\"invalid auth\"}");
	return res;
}


char* create_channel(char* query) 
{
	query[strlen(query)-1] = '\0';
	int pos = 0;
	char* res = (char*) malloc(1000);
	printf("%s\n", query);
	for(int i = 0; i < strlen(query); i ++)
	{
		if (query[i] == ',') 
		{
			printf("%s peida shod %d", query, i);
			pos = i;
			break;
		}
	}
	query[pos] = '\0';
	for (int i = 0; i < user_count; i++)
	{
		if (online_users[i].is_online && strcmp(query + pos + 2, online_users[i].auth_token) == 0)
		{
			for (int i = 0; i < channel_count; i ++)
			{
				if (strcmp(channels[i].name, query) == 0)
				{
					sprintf(res, "{\"type\":\"Error\",\"content\":\"channel already exists\"}");
					return res;
				}
			}
			strcpy(channels[channel_count].name, query);
			channels[channel_count].message_count = 0;
			save_channel(&channels[channel_count]);
			channels[channel_count].member_count = 1;
			channels[channel_count].members[0] = i;
			online_users[i].channel = channel_count;
			channel_count++;
			sprintf(res, "{\"type\":\"Successfull\",\"content\":\"\"}");
			return res;
		}
	}
	sprintf(res, "{\"type\":\"Error\",\"content\":\"invalid auth\"}");
	return res;
}

char* login_user(char* query)
{
	query[strlen(query)-1] = '\0';
	int pos = 0;
	char* res = (char*) malloc(1000);
	printf("%s\n", query);
	for(int i = 0; i < strlen(query); i ++)
	{
		if (query[i] == ',') 
		{
			printf("%s peida shod %d", query, i);
			pos = i;
			break;
		}
	}
	query[pos] = '\0';
	DIR* dir;
	struct dirent *ent;
	dir = opendir("data/Users");
	while ((ent = readdir(dir)) != NULL)
	{
		if (strncmp(ent->d_name, query, strlen(query)) == 0) 
		{
			char* file_add = (char*)malloc(100);
			sprintf(file_add, "%s%s", "data/Users/", ent->d_name);
			FILE* file = fopen(file_add, "r");
			char* u = (char*)malloc(100);
			char* p = (char*)malloc(100);
			printf("%s vs %s\n", query, ent->d_name);
			fscanf(file, "%s\n%s", u, p);
			strcpy(online_users[user_count].username, u);
			strcpy(online_users[user_count].password, p);
			if (strcmp(query, u) == 0) 
			{
				printf("$%s#%s\n", query + pos + 2, p);
				if (strcmp(query + pos + 2, p) == 0)
				{
					make_auth(&online_users[user_count]);	
					sprintf(res, "{\"type\":\"AuthToken\",\"content\":\"%s\"}\n", 
							online_users[user_count].auth_token);
					online_users[user_count].is_online = 1;
					online_users[user_count].channel = -1;
					online_users[user_count].last_message = 0;
					printf("%s\n", res);
					user_count++;
					return res;
				}
				else
				{
					sprintf(res, "{\"type\":\"Error\",\"content\":\"wrong pass:(\"}\n");
					return res;
				}
			}
			fclose(file);
			free(u); free(p); free(file_add);
		}
	}
	sprintf(res, "{\"type\":\"Error\",\"content\":\"user not found :(\"}\n");
	return res;
}


char* register_user(char* query)
{
	query[strlen(query)-1] = '\0';
	int pos = 0;
	char* res = (char*) malloc(1000);
	for(int i = 0; i < strlen(query); i ++)
	{
		if (query[i] == ',') 
		{
			pos = i;
			break;
		}
	}
	query[pos] = '\0';
	DIR* dir;
	struct dirent *ent;
	dir = opendir("data/Users");
	while ((ent = readdir(dir)) != NULL)
	{
		if (strncmp(ent->d_name, query, strlen(ent->d_name)-4) == 0 && strlen(ent->d_name) > 4) 
		{
			sprintf(res, "{\"type\":\"Error\",\"content\":\"duplicate user :(\"}\n");
			return res;
		}
	}
	init_user(query, query + pos + 2);
	sprintf(res, "{\"type\":\"Successful\", \"content\":\"\"}\n");
	return res;
}

void init_server()
{
	int server_socket, client_socket;
	struct sockaddr_in server, client;

	// Create and verify socket
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket == -1) {
		printf("Socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");

	// Assign IP and port
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(PORT);

	// Bind newly created socket to given IP and verify
	if ((bind(server_socket, (SA*)&server, sizeof(server))) != 0) {
		printf("Socket binding failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully bound..\n");

	// Now server is ready to listen and verify
	while(1){
		if ((listen(server_socket, 5)) != 0) {
			printf("Listen failed...\n");
			exit(0);
		}
		else
			printf("Server listening..\n");

		// Accept the data packet from client and verify
		socklen_t len = sizeof(client);
		client_socket = accept(server_socket, (SA*)&client, &len);
		if (client_socket < 0) {
			printf("Server accceptance failed...\n");
			exit(0);
		}
		else
			printf("Server acccepted the client..\n");

		// Function for chatting between client and server
		process(client_socket);
	}
	// Close the socket
	shutdown(server_socket, SHUT_RDWR);
}


void setstr(char* str1, char*str2)
{
	memcpy(str1, str2, strlen(str2) + 1);
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

void make_auth(User* user) 
{
	srand(time(NULL));
	for (int i = 0; i < TOKENSIZE; i ++) 
		user->auth_token[i] = 'a' + (rand() % 26);
	user->auth_token[TOKENSIZE] = '\0';
}

void save_user(User* user) 
{
	char file_address[200];
	combine(file_address, 3, "data/Users/", user->username, ".usr");
	FILE* file = fopen(file_address, "w");
	fprintf(file, "%s\n", user->username);
	fprintf(file, "%s\n", user->password);
	fclose(file);
}

void save_channel(Channel* channel)
{
	FILE* file = fopen("data/Channels.data", "a");
	fprintf(file, "%s\n", channel->name);
	fclose(file);
}

void save_messages(Message* message)
{
	char file_address[200];
	sprintf(file_address, "data/Messages/%s_%d.msg", 
			message->channel, message->m_id);
	FILE* file = fopen(file_address, "w");
	fprintf(file, "%s\n%s", message->sender, message->content);
	fclose(file);
}

User* init_user(char* username, char* password) {
	User* user = (User*) malloc(sizeof(User));
	strcpy(user->username, username);
	strcpy(user->password, password);
	save_user(user);
	return user;
}

void load_channels()
{
	FILE* file = fopen("data/Channels.data", "r");
	char channel[100];
	while (fgets(channel, 100, file) != NULL)
	{
		printf("&%s&\n", channel);
		channel[strlen(channel)-1] = '\0';
		strcpy(channels[channel_count].name, channel);
		channel_count ++;
	}
}
