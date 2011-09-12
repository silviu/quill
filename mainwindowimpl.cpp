#include <QtGui>
#include <QImage>
#include <vector>

#include "mainwindowimpl.h"
#include "chatwindowimpl.h"
#include "client.h"
#include <pthread.h>
#include "common.h"
#include "keyeventfilter.h"

vector<string> users;

MainWindowImpl::MainWindowImpl( QWidget * parent, Qt::WFlags f) 
	: QMainWindow(parent, f)
{
	setupUi(this);
	init();
	new KeyEventFilter();
	connect(signinButton, SIGNAL(clicked()), this, SLOT(sign_in()));
	connect(usernameEdit, SIGNAL(selectionChanged()), this, SLOT(username_clear()));
	connect(passwordEdit, SIGNAL(selectionChanged()), this, SLOT(password_clear()));

	connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));
	connect(AddButton, SIGNAL(clicked()), this, SLOT(add_buddy_hard()));
	connect(listWidget, SIGNAL( itemDoubleClicked( QListWidgetItem* ) ), this, SLOT( open_chat_window(QListWidgetItem*)));
	connect(listWidget, SIGNAL( itemActivated( QListWidgetItem* ) ), this, SLOT( open_chat_window(QListWidgetItem*)));

}

void MainWindowImpl::init()
{
	chatWidget->setVisible(false);
	loginWidget->setVisible(true);
}

void MainWindowImpl::closeEvent(QCloseEvent*)
{
	extern map<string, ChatWindowImpl*> opened_chats;
	map<string, ChatWindowImpl*>::iterator opened_chats_it;
	for (opened_chats_it = opened_chats.begin(); opened_chats_it != opened_chats.end(); ++opened_chats_it)
		delete(opened_chats_it->second);
	exit(EXIT_SUCCESS);
}

void MainWindowImpl::keyPressEvent(QKeyEvent * event)
{
	if (event->key() == Qt::Key_Return) {
		if (loginWidget->isVisible())
			sign_in();
		return;
	}
}

void MainWindowImpl::sleep(int time)
{
	QTime dieTime = QTime::currentTime().addSecs(time);
		while( QTime::currentTime() < dieTime )
			QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

QListWidget* MainWindowImpl::get_listWidget()
{
	return listWidget;
}

bool MainWindowImpl::is_in_list(QString user)
{
	for(int row = 0; row < listWidget->count(); row++) {
		QListWidgetItem *item = listWidget->item(row);
		if (item->text() == user)
			return true;
	}
	return false;
}

void MainWindowImpl::populate_list()
{
	extern map<string, user_info> user_list;
	map<string, user_info>::iterator it;
	int i;
	for(it = user_list.begin(), i = 0; it != user_list.end(); ++it, i++) {
		QString other_user = QString::fromStdString(it->first);
		if (!is_in_list(other_user))
			add_buddy(other_user);
	}
}

void MainWindowImpl::sign_in()
{
	username = usernameEdit->text();
	
	if (username == "Username..." || "" == username) {
		errorLabel->setText("Please add your username.");
		return;
	}
	
	struct arg_struct args;
	args.arg1 = username.toStdString();
 	args.arg2 = strdup("127.0.0.1");
	args.arg3 = strdup("2222");
	args.arg4 = strdup("/download");
	
	pthread_t tid;
	pthread_create(&tid, NULL, &make_connection, (void*) &args);
	pthread_detach(tid);
	signinButton->setFlat(true);
	
	extern int connection_state;
	while(connection_state != CONNECTED && connection_state != USER_EXISTS){
		printf("CONNECTION_STATE=[%d]\n", connection_state);
		sleep(2);
	}
	
	if (connection_state == USER_EXISTS) {
			pthread_cancel(tid);
			connection_state = NO_STATE;
			errorLabel->setText("Username already online.");
			signinButton->setFlat(false);
			return;
	}
	
	if (connection_state == CONNECTION_PROBLEM) {
		pthread_cancel(tid);
		connection_state = NO_STATE;
		errorLabel->setText("Connection problem.");
		signinButton->setFlat(false);
		return;
	}
	
	extern string from_who;
	from_who = username.toStdString();
	
	signinButton->setFlat(false);
	loginWidget->setVisible(false);
	chatWidget->setVisible(true);
	listWidget->setFocus();
	
	refresh_label();
	//add_buddy(username);

	populate_list();
}

void MainWindowImpl::about()
{
	QMessageBox::about(this, tr("About"), tr("Credits: <p> Silviu Grijincu </p>"));
}

void MainWindowImpl::open_chat_window(QListWidgetItem* item)
{
	//ChatWindowImpl* chat = new ChatWindowImpl();
	//chat->change_title(item->text());
	//chat->show();
	open_extern_chat(item->text());
}

void MainWindowImpl::open_extern_chat(QString title)
{
	extern map<string, ChatWindowImpl*> opened_chats;
	map<string, ChatWindowImpl*>::iterator opened_chats_it;
	
	opened_chats_it = opened_chats.find(title.toStdString());
	if (opened_chats_it == opened_chats.end()) {
		ChatWindowImpl* chat = new ChatWindowImpl();
		chat->change_title(title);
		chat->title_string = title;
		chat->show();
		opened_chats.insert(pair<string, ChatWindowImpl*>(chat->title_string.toStdString(), chat));
	}
}

QString MainWindowImpl::get_buddy_name()
{
	QString str;
	QString answer = QInputDialog::getText(0, "Add new buddy", str);
	return answer;
}

void MainWindowImpl::refresh_label()
{
	int user_count = listWidget->count();
	QString str_num = QString::number(user_count);
	QString string = str_num + " users online.";
	label->setText(string);
}

void MainWindowImpl::add_buddy_hard()
{
	QString name = get_buddy_name();
	if (name != NULL)
		add_buddy(name);
	
}

void MainWindowImpl::add_buddy(QString username)
{
	printf("ADAUG IN LISTA:[%s]\n", username.toStdString().c_str());
	listWidget->addItem(new QListWidgetItem(QIcon("/home/luther/pidgi/ui/bb8.jpg"), username, listWidget));
	refresh_label();
}

void MainWindowImpl::username_clear()
{
	usernameEdit->setText("");
}

void MainWindowImpl::password_clear()
{
	passwordEdit->setText("");
}