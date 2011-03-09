#ifndef MAINWINDOWIMPL_H
#define MAINWINDOWIMPL_H
//
#include <QMainWindow>
#include "ui_mainwindow.h"
#include "common.h"
#include <stdio.h>
#include "chatwindowimpl.h"

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
	static void open_extern_chat(QString title);
	void refresh_label();

private slots:
	void about();
	void sign_in();
	void init();
	void add_buddy(QString user_name);
	void add_buddy_hard();
	QString get_buddy_name();
	void open_chat_window(QListWidgetItem*);
	void username_clear();
	void password_clear();

private:
	bool is_in_list(QString user);
	void keyPressEvent(QKeyEvent * event);
	void closeEvent(QCloseEvent* event);
};
#endif




