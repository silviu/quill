#include <QtGui>
#include "chatwindowimpl.h"
#include "client.h"
#include <pthread.h>
#include "common.h"

string mesaj;

ChatWindowImpl::ChatWindowImpl( QWidget * parent, Qt::WFlags f) : QWidget(parent, f)
{
	setupUi(this);
	connect(sendButton, SIGNAL(clicked()), this, SLOT(add_text_to_browser()));
	textEdit->setFocus(Qt::OtherFocusReason);
}

void ChatWindowImpl::change_title(QString user_name)
{
	QString window_title = "Chat with "  + user_name;
	setWindowTitle(window_title);
}

QString ChatWindowImpl::get_time()
{
	QDateTime dateTime = QDateTime::currentDateTime();
	QString dateTimeString = dateTime.toString();
	return dateTimeString;
}

void ChatWindowImpl::add_text_to_browser()
{
	QString message = textEdit->toPlainText();
	mesaj = message.toStdString();
	
	if (message != "") {
		text += "me(" + get_time() + "): " + message;
		textShow->setFontWeight(200);
		textShow->setText(text);
		text += "\n";
		
		struct argu_struct args;
		args.arg1 = "luther";
		args.arg2 =  message.toStdString();
		pthread_t tid;
		pthread_create(&tid, NULL, &send_message_gui, (void*) &args);
	}
	textEdit->clear();
}