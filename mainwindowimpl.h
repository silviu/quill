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
private slots:
	void about();
	void add_buddy(QListWidgetItem* iTem1);
	void add_buddy_hard();
	void refresh_label();
	QString get_buddy_name();
private:
		QString pathName;
		QString extractFilePath(QString fn);
		
};
#endif




