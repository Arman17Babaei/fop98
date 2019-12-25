#include "phew.h"
#include "page_handler.h"

int main()
{
	modify_to_start();
	page_maker();
	current_page = &start_page;
	while (1) {
		make_view();
		get_input();
	}
}
