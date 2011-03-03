#ifndef MAINWINDOWIMPL_H
#define MAINWINDOWIMPL_H
//
#include <QMainWindow>
#include "ui_mainwindow.h"

//
class MainWindowImpl : public QMainWindow, public Ui::MainWindow
{
Q_OBJECT
public:
	MainWindowImpl( QWidget * parent = 0, Qt::WFlags f = 0 );
	QString username;
friend class ChatWindowImpl;
private slots:
	void about();
	void sign_in();
	void init();
	void add_buddy(QListWidgetItem* iTem1);
	void add_buddy_hard();
	void refresh_label();
	QString get_buddy_name();
	void open_chat_window(QListWidgetItem*);
	void username_clear();
	void password_clear();
private:
		QString pathName;
		QString extractFilePath(QString fn);
		
};
#endif




