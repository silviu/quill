#include "keyeventfilter.h"

bool KeyEventFilter::eventFilter(QObject*, QEvent* event)
{
	if(event->type() == (QEvent::Type)1234){
		printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n\n\n");
		extern vector<string> new_messages;
		extern map<string, ChatWindowImpl*> opened_chats;
		extern map<string, user_info> user_list;
		map<string, user_info>::iterator user_list_it;
		map<string, ChatWindowImpl*>::iterator opened_chats_it;
			
		for (unsigned int i = 0; i < new_messages.size(); i++) {
			opened_chats_it = opened_chats.find(new_messages[i]);
			if (opened_chats_it != opened_chats.end()) {	
				user_list_it = user_list.find(new_messages[i]);
				if (user_list_it != user_list.end()) {
					for (unsigned int j = 0; j < user_list_it->second.msg.size(); j++)
						opened_chats_it->second->add_text_to_browser_from(QString::fromStdString(user_list_it->second.msg[j]));
					user_list_it->second.msg.clear();
				}
			}
			else {
				ChatWindowImpl* chat = new ChatWindowImpl();
				chat->title_string = QString::fromStdString(new_messages[i]); 
				chat->change_title(chat->title_string);
				chat->show();
				opened_chats.insert(pair<string, ChatWindowImpl*>(chat->title_string.toStdString(), chat));
				user_list_it = user_list.find(new_messages[i]);
				if (user_list_it != user_list.end()) {
					for (unsigned int j = 0; j < user_list_it->second.msg.size(); j++)
						chat->add_text_to_browser_from(QString::fromStdString(user_list_it->second.msg[j]));
					user_list_it->second.msg.clear();
				}
			}
		}
	}
	return false;
}