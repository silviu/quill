#include <QtGui>
#include "chatwindowimpl.h"
//
ChatWindowImpl::ChatWindowImpl( QWidget * parent, Qt::WFlags f) : QWidget(parent, f)
{
	setupUi(this);
}

void ChatWindowImpl::change_title(QString user_name)
{
	QString window_title = "Chat with "  + user_name;
	setWindowTitle(window_title);
}