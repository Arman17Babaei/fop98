#include "phew.h"
#include "parser.h"

void send_user_message(char* input)
{
	combine(result, 4, "send ", input + 2, ", ", token);
	send_message(result);
}

void refresh()
{
	combine(result, 2, "refresh ", token);
	send_message(result);
	//cJSON *json = cJSON_Parse(result);
	//cJSON *type = cJSON_GetObjectItemCaseSensitive(json, "type");
	JSON* json = jackie__parse_it(result);
	ITEM* messages = (ITEM*)json->item[1];
	//if (type->valuestring != "List") return;
	//cJSON *messages = cJSON_GetObjectItemCaseSensitive(json, "content");
	//cJSON *message = NULL;
	combine(result, 1, "\n");
	char tmp[MAX_WINDOW_WIDTH];
	for (int i = 0; i < messages->a_len; i ++)
	{
		JSON* message = ((ITEM*) messages->a_part[i])->j_part;
		combine(tmp, 4, 
				((ITEM*)message->item[0])->s_part,
				": ", 
				((ITEM*)message->item[1])->s_part,
			   	"\n");
		strcat(result, tmp);
		//printf("## %s\n", cJSON_GetObjectItemCaseSensitive(message, "content")->valuestring);
	}
}

void get_channel_members()
{
	combine(result, 2, "channel members ", token);
	send_message(result);
	printf(result);
	JSON* json = jackie__parse_it(result);
	//cJSON *json = cJSON_Parse(result);
	//cJSON *type = cJSON_GetObjectItemCaseSensitive(json, "type");
	//if (type->valuestring != "List") return;
	//cJSON *members = cJSON_GetObjectItemCaseSensitive(json, "content");
	//cJSON *member = NULL;
	combine(result, 1, "\n");
	char tmp[MAX_WINDOW_WIDTH];
	ITEM* members = (ITEM*) json->item[1];
	//cJSON_ArrayForEach(member, members)
	for (int i = 0; i < members->a_len; i ++)
	{
		combine(tmp, 2, 
				((ITEM*)members->a_part[i])->s_part,
			   	"\n");
		strcat(result, tmp);
	}
}

void leave_channel()
{
	combine(result, 2, "leave ", token);
	send_message(result);
}
	

void chat_page_interpreter(char* input)
{
	if (*input == '0') 
	{
		setstr(result, "we would have sent message");
		send_user_message(input);
		if (result[9] == 'S')
		{
			combine(result, 1, "you sent your message :D");
			// refresh maybe
		} 
	}
	if (*input == '1') 
	{
		setstr(result, "we are refreshing");
	   	refresh();
	}
	if (*input == '2')
	{
		setstr(result, "We are getting channel members");
		get_channel_members();
	}
	if (*input == '3') 
	{
		printf("We are leaving the channel\n");
		leave_channel();
		if (result[9] == 'S')
		{
			combine(result, 1, "You successfully left the channel");
			current_page = &main_page;
		}
	}
}

void make_chat_page()
{
	setstr(chat_page.title, "Chat Menu; You Can't Unsend...");

	chat_page.option_count = 4;

	setstr(chat_page.options[0], "Send Message (eg 0 This is my message)");
	setstr(chat_page.options[1], "Refresh");
	setstr(chat_page.options[2], "Channel Members");
	setstr(chat_page.options[3], "Leave Channel");
	
	chat_page.interpreter = &chat_page_interpreter;
}
