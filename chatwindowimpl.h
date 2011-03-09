#ifndef __CHATWINDOWIMPL_H__
#define __CHATWINDOWIMPL_H__

#include <QWidget>
#include "ui_chatwindow.h"
#include "common.h"

class ChatWindowImpl : public QWidget, public Ui::ChatWindow
{
Q_OBJECT
public:
	ChatWindowImpl( QWidget * parent = 0, Qt::WFlags f = 0);
	void change_title(QString user_name);
	QString get_time();
	QString title_string;
	void add_text_to_browser_from(QString message);
	void add_text_to_browser();
	
private:
	void closeEvent(QCloseEvent *event);
	void keyPressEvent(QKeyEvent * event);
	void append_standard_header(QString from_whom);
};
#endif