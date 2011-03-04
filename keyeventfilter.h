#ifndef __KEYEVENTFILTER_H__
#define __KEYEVENTFILTER_H__

#include "chatwindowimpl.h"

class KeyEventFilter: public QObject
{
public:
	KeyEventFilter(): QObject(qApp)
	{
		qApp->installEventFilter(this);
	}

	bool eventFilter(QObject* watched, QEvent* event)
	{
		if(event->type() == (QEvent::Type)1234){
			printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n\n\n");
			ChatWindowImpl* chat = new ChatWindowImpl();
			chat->show();
		}
	return false;
	}
};
#endif // __KEYEVENTFILTER_H__
