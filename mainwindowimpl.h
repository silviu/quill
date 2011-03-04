#ifndef MAINWINDOWIMPL_H
#define MAINWINDOWIMPL_H
//
#include <QMainWindow>
#include "ui_mainwindow.h"
#include "common.h"
#include <stdio.h>

//
class MainWindowImpl : public QMainWindow, public Ui::MainWindow
{
Q_OBJECT
public:
	MainWindowImpl( QWidget * parent = 0, Qt::WFlags f = 0 );
	QString username;
	void sleep(int time);
	void populate_list();
	QListWidget* get_listWidget();
	void open_extern_chat(QString title);


friend class ChatWindowImpl;
private slots:
	void about();
	void sign_in();
	void init();
	void add_buddy(QString user_name);
	void add_buddy_hard();
	void refresh_label();
	QString get_buddy_name();
	void open_chat_window(QListWidgetItem*);
	void username_clear();
	void password_clear();

private:
	bool is_in_list(QString user);
protected:
	bool event(QObject *obj, QEvent *ev);
	
};

class KeyEventFilter: public QObject
{
public:
	KeyEventFilter(): QObject(qApp)
	{
		qApp->installEventFilter(this);
	}

	bool eventFilter(QObject* watched, QEvent* event)
	{
		if(event->type() == (QEvent::Type)1234)
			printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n\n\n");
		return false;
	}
};
#endif




