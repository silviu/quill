#include <QtGui>
#include "chatwindowimpl.h"
#include "client.h"
#include <pthread.h>
#include "common.h"
#include <QTextCursor>



string mesaj;
string from_who;
string to_who;

ChatWindowImpl::ChatWindowImpl( QWidget * parent, Qt::WFlags f) : QWidget(parent, f)
{
	setupUi(this);
	connect(sendButton, SIGNAL(clicked()), this, SLOT(add_text_to_browser()));
	textEdit->setFocus(Qt::OtherFocusReason);
}

void ChatWindowImpl::closeEvent(QCloseEvent*)
{
	extern map<string, ChatWindowImpl*> opened_chats;
	opened_chats.erase(title_string.toStdString());
}

void ChatWindowImpl::keyPressEvent(QKeyEvent * event)
{
	if (event->key() == Qt::Key_Return) {
		printf("ENTERRRRRRRRRRRRRRRRR\n\n");
		return;
	}
	
	if (event->key() == Qt::Key_Escape) {
		printf("ESCAPE\n");
		close();
		return;
	}
}

void ChatWindowImpl::change_title(QString user_name)
{
	QString window_title = "Chat with "  + user_name;
	to_who = user_name.toStdString();
	setWindowTitle(window_title);
}

QString ChatWindowImpl::get_time()
{
	QDateTime dateTime = QDateTime::currentDateTime();
	QString dateTimeString = dateTime.toString();
	return dateTimeString;
}

void ChatWindowImpl::add_text_to_browser_from(QString message)
{
	append_standard_header(title_string);
	textShow->setFontWeight(50);
	textShow->setTextColor(Qt::black);
	textShow->textCursor().insertText(message);
}

void ChatWindowImpl::append_standard_header(QString from_whom)
{
	textShow->setFontWeight(60);
	if (from_whom == "me")
		textShow->setTextColor(Qt::darkGreen);
	else
		textShow->setTextColor(Qt::darkRed);
	QString standard_header = from_whom + "(" + get_time() + "): "; 
	textShow->append(standard_header);
}

void ChatWindowImpl::add_text_to_browser()
{
	QString message = textEdit->toPlainText();
	mesaj = message.toStdString();
		
	if (message != "") {
		append_standard_header("me");
		textShow->setFontWeight(55);
		textShow->setTextColor(Qt::black);
		textShow->textCursor().insertText(message);
		
		struct argu_struct args;
		args.arg1 = "luther";
		args.arg2 =  message.toStdString();
		pthread_t tid;
		pthread_create(&tid, NULL, &send_message_gui, (void*) &args);
	}
	textEdit->clear();
}