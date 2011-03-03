#include <QtGui>
#include <QImage>

#include "mainwindowimpl.h"
#include "chatwindowimpl.h"
#include "client.h"
#include <pthread.h>
#include "common.h"


MainWindowImpl::MainWindowImpl( QWidget * parent, Qt::WFlags f) 
	: QMainWindow(parent, f)
{
	setupUi(this);
	init();
	
	connect(signinButton, SIGNAL(clicked()), this, SLOT(sign_in()));
	connect(usernameEdit, SIGNAL(selectionChanged()), this, SLOT(username_clear()));
	connect(passwordEdit, SIGNAL(selectionChanged()), this, SLOT(password_clear()));



	connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));
	connect(AddButton, SIGNAL(clicked()), this, SLOT(add_buddy_hard()));
	connect(listWidget, SIGNAL( itemDoubleClicked( QListWidgetItem* ) ), this, SLOT( open_chat_window(QListWidgetItem*)));	
}

void MainWindowImpl::init()
{
	chatWidget->setVisible(false);
	loginWidget->setVisible(true);
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
	args.arg3 = strdup("1234");
	args.arg4 = strdup("/download");
	pthread_t tid;
	pthread_create(&tid, NULL, &make_connection, (void*) &args);
	pthread_detach(tid);
	signinButton->setFlat(true);
	
	extern int connection_state;
	while(connection_state != CONNECTED && connection_state != USER_EXISTS){
		printf("CONNECTION_STATE=[%d]\n", connection_state);
		QTime dieTime = QTime::currentTime().addSecs(2);
		while( QTime::currentTime() < dieTime )
			QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
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
	
	signinButton->setFlat(false);
	loginWidget->setVisible(false);
	chatWidget->setVisible(true);
	
	refresh_label();
	add_buddy(new QListWidgetItem(QIcon("/home/luther/pidgi/ui/bb8.jpg"),username, listWidget));
}

void MainWindowImpl::about()
{
	QMessageBox::about(this, tr("About"), tr("Credits: <p> Silviu Grijincu </p>"));
}

void MainWindowImpl::open_chat_window(QListWidgetItem* item)
{
	ChatWindowImpl* chat = new ChatWindowImpl();
	chat->change_title(item->text());
	chat->show();
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
		add_buddy(new QListWidgetItem(QIcon(":/ui/bb8.jpg"),name,listWidget));
	
}

void MainWindowImpl::add_buddy(QListWidgetItem* iTem1)
{
	listWidget->addItem(iTem1);
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