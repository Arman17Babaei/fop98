#include "phew.h"

void create_channel(char* input)
{
	setstr(CHANNEL_NAME, input + 2);
	combine(result, 4, "create channel ", CHANNEL_NAME,", ", token);
	send_message(result);
}

void  join_channel(char* input)
{
	setstr(CHANNEL_NAME, input + 2);
	combine(result, 4, "join channel ", CHANNEL_NAME, ", ", token);
	send_message(result);
}

void logout()
{
	combine(result, 2, "logout ", token);
	send_message(result);
}
	
void main_page_interpreter(char* input)
{
	if (*input == '0') 
	{
		setstr(result, "we would have created cannel");
		create_channel(input);
		if (result[9] == 'S')
		{
			combine(result, 3, "Channel \"", CHANNEL_NAME, "\" created successfully!");
			current_page = &chat_page;
		} 
		printf("Your fucking channel name is: %s", CHANNEL_NAME);
	}
	if (*input == '1') 
	{
		setstr(result, "we are joining a channel");
	   	join_channel(input);
		if (result[9] == 'S')
		{
			combine(result, 3, "Joined in the channel \"", CHANNEL_NAME, "\" Successfully!");
			current_page = &chat_page;
		}
	}
	if (*input == '2')
	{
		setstr(result, "We are logging out");
		logout();
		if (result[9] == 'S')
		{
			combine(result, 1, "Forget about the user you logged in with");
			current_page = &start_page;
		}
	}
	if (*input == '3') 
	{
		logout();
		printf("We are shutting the program down"), exit(0);
	}
}

void make_main_page()
{
	setstr(main_page.title, "Main Menu; Choose wisely ;)");

	main_page.option_count = 4;

	setstr(main_page.options[0], "create Channel (eg 0 channel_name)");
	setstr(main_page.options[1], "Join Channel ( eg 1 channel_name)");
	setstr(main_page.options[2], "Logout");
	setstr(main_page.options[3], "Logout & Exit");
	
	main_page.interpreter = &main_page_interpreter;
}
