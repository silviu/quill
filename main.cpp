#include <QApplication>
#include "mainwindowimpl.h"
#include "chatwindowimpl.h"
#include <pthread.h>
#include <stdio.h>

//

QString from_whom, message;

void add_buddy(QString username, QListWidget* listWidget)
{
	printf("ADAUG IN LISTA:[%s]\n", username.toStdString().c_str());
	listWidget->addItem(new QListWidgetItem(QIcon("/home/luther/pidgi/ui/bb8.jpg"), username, listWidget));
}

void rem_buddy(QListWidgetItem* item)
{
	printf("SCOT DIN LISTA:[%s]\n", item->text().toStdString().c_str());
	delete(item);
}

bool is_in_list(QString user, QListWidget* listWidget)
{
	for(int row = 0; row < listWidget->count(); row++) {
		QListWidgetItem *item = listWidget->item(row);
		if (item->text() == user)
			return true;
	}
	return false;
}

bool is_in_map(QString user)
{
	extern map<string, user_info> user_list;
	map<string, user_info>::iterator it;
	int i;
	for(it = user_list.begin(), i = 0; it != user_list.end(); ++it, i++) {
		if (it->first == user.toStdString())
			return true;
	}
	return false;
}

void* populate_list(void* listWidget)
{
	while(1){
		
		listWidget = (QListWidget*) listWidget;
		extern map<string, user_info> user_list;
		map<string, user_info>::iterator it;
		int i;
		for(it = user_list.begin(), i = 0; it != user_list.end(); ++it, i++) {
			QString other_user = QString::fromStdString(it->first);
			if (!is_in_list(other_user, (QListWidget*)listWidget))
				add_buddy(other_user, (QListWidget*)listWidget);
		}
		
		for(int row = 0; row < ((QListWidget*)listWidget)->count(); row++) {
			QListWidgetItem *item = ((QListWidget*)listWidget)->item(row);
			if (!is_in_map(item->text())) {
				rem_buddy(item);
				break;
			}
		}
	sleep(5);
	}
	return NULL;
}

void* check_for_messages(void*)
{
	while(1){
		extern map<string, user_info> user_list;
		map<string, user_info>::iterator it;
		int i;
		for(it = user_list.begin(), i = 0; it != user_list.end(); ++it, i++) {
			if (it->second.msg.size() > 0) {
				printf("WE HAVE MESSAHE IN MAIN\n");
				QString title= "SSS";
				return NULL;
			}
		}
	sleep(10);
	}
	return NULL;
}


int main(int argc, char ** argv)
{
	QApplication app( argc, argv );
	MainWindowImpl* win = new MainWindowImpl();
	win->show();
	
	QListWidget* listWidget = win->get_listWidget();
	pthread_t tid;
	pthread_create(&tid, NULL, &populate_list, listWidget);

	app.connect( &app, SIGNAL( lastWindowClosed() ), &app, SLOT( quit() ) );
	return app.exec();
	
	pthread_t tidi;
	pthread_create(&tidi, NULL, &check_for_messages, NULL);
	win->open_extern_chat("SSS");
}
