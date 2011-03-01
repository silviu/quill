#ifndef __CHATWINDOWIMPL_H__
#define __CHATWINDOWIMPL_H__

#include <QWidget>
#include "ui_chatwindow.h"

class ChatWindowImpl : public QWidget, public Ui::ChatWindow
{
Q_OBJECT
public:
	ChatWindowImpl( QWidget * parent = 0, Qt::WFlags f = 0 );
	void change_title(QString user_name);
private slots:

	
};
#endif