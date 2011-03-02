#include <QtGui>
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

	connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));
	connect(AddButton, SIGNAL(clicked()), this, SLOT(add_buddy_hard()));
	connect(listWidget, SIGNAL( itemDoubleClicked( QListWidgetItem* ) ), this, SLOT( open_chat_window(QListWidgetItem*)));	
}

void MainWindowImpl::init()
{
	struct arg_struct args;
	args.arg1 = strdup("luther");
	args.arg2 = strdup("127.0.0.1");
	args.arg3 = strdup("1234");
	args.arg4 = strdup("/download");
	pthread_t tid;
	pthread_create(&tid, NULL, &make_connection, (void*) &args);
	
	refresh_label();
	add_buddy(new QListWidgetItem(QIcon("/home/luther/pidgi/ui/bb8.jpg"),"Silviu",listWidget));
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
		add_buddy(new QListWidgetItem(QIcon("/home/luther/pidgi/ui/bb8.jpg"),name,listWidget));
	
}

void MainWindowImpl::add_buddy(QListWidgetItem* iTem1)
{
	listWidget->addItem(iTem1);
	refresh_label();
}

// Extract a file path from a full path/filename
QString MainWindowImpl::extractFilePath(QString fn)
{
	if(fn == "")																																	// If we are sent an empty string......
		return("");																																	// ...then return and empty string
	while(fn.right(1) != "/")																											// Find the last path seperator
	{
		fn = fn.left(fn.size() - 1);
	}
	return(fn);																																		// Send back the path
}