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
	
	KeyEventFilter(QObject* obj)
	{
		obj->installEventFilter(this);
	}

	bool eventFilter(QObject*, QEvent* event);
	
};
#endif // __KEYEVENTFILTER_H__
