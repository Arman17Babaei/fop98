#include "phew.h"

void register_user(char* input)
{
	char username[MAX_WINDOW_WIDTH];
	char password[MAX_WINDOW_WIDTH];
	char* colon_mark = strrchr(input, ':');
	int len = colon_mark - input - 2;
	memcpy(username, input + 2, len);
	username[len] = '\0';

	setstr(password, colon_mark + 1);
	char message[MAX_WINDOW_WIDTH];
	setstr(message, "register ");
	strcat(message, username);
	setstr(USERNAME, username);
	strcat(message, ", ");
	strcat(message, password);
	setstr(PASSWORD, password);
	send_message(message);
}

void login_user(char* input)
{
	char username[MAX_WINDOW_WIDTH];
	char password[MAX_WINDOW_WIDTH];
	char* colon_mark = strrchr(input, ':');
	int len = colon_mark - input - 2;
	memcpy(username, input + 2, len);
	username[len] = '\0';

	setstr(password, colon_mark + 1);
	char message[MAX_WINDOW_WIDTH];
	setstr(message, "login ");
	strcat(message, username);
	setstr(USERNAME, username);
	strcat(message, ", ");
	strcat(message, password);
	setstr(PASSWORD, password);
	send_message(message);
}

void start_page_interpreter(char* input)
{
	if (*input == '0') 
	{
		setstr(result, "we would have logged in");
		login_user(input);
		if (result[9] == 'A')
		{
			int token_start = 31;
			int len = strlen(result) - token_start - 3;
			memcpy(token, result+token_start, len);
			token[len] = '\0';
			combine(result, 2, "You have logged in as user:", USERNAME);
			current_page = &main_page;
		}
		printf("Your fucking token is:%s", token);
	}
	if (*input == '1') 
	{
		setstr(result, "we are registering");
	   	register_user(input);
		if (result[9] == 'S')
		{
			combine(result, 4, "Registered with user: ", USERNAME, " and pass: ", PASSWORD);
		}
	}
	if (*input == '2') 
	{
		printf("We are shutting the program down\n"), exit(0);
	}
}

void make_start_page()
{
	setstr(start_page.title, "START PAGE; WELCOME");

	start_page.option_count = 3;

	setstr(start_page.options[0], "Log In (eg 0 username:password)");
	setstr(start_page.options[1], "Register (eg 1 username:password)");
	setstr(start_page.options[2], "Exit");

	start_page.interpreter = &start_page_interpreter;
}
