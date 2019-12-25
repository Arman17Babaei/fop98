#ifndef _main_page_h
#define _main_page_h

#include "phew.h"

void send_user_message(char* input);
void refresh();
void get_channel_members();
void leave_channel();
void chat_page_interpreter(char* input);
void make_chat_page();

#endif
