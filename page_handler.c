#include "phew.h"
#include "start_page.h"
#include "main_page.h"
#include "chat_page.h"

void page_maker()
{
	make_start_page();
	make_main_page();
	make_chat_page();
}

void make_view()
{
	clear();
	set_color(1, TITLE_COLOR);
	printf("%s\n", current_page->title);
	reset_color();
	for (int index = 0; index < current_page->option_count; index++)
	{
		set_color(1, OPTION_COLOR);
		printf("%d: %s\n", index, current_page->options[index]);
		reset_color();
	}
	set_color(0, RESULT_COLOR);
	printf("%s\n", result);
	reset_color();
}



void get_input()
{
	char input[MAX_WINDOW_WIDTH];
	scanf("%[^\n]%*c", input); 
	(*current_page->interpreter)(input);
}
