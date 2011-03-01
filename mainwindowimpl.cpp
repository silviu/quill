#include <QtGui>
#include "mainwindowimpl.h"
//
MainWindowImpl::MainWindowImpl( QWidget * parent, Qt::WFlags f) 
	: QMainWindow(parent, f)
{
	setupUi(this);
	refresh_label();
	add_buddy(new QListWidgetItem(QIcon("/home/luther/pidgi/ui/bb8.jpg"),"Silviu",listWidget));
	pathName = QApplication::applicationDirPath();
	connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));
	connect(AddButton, SIGNAL(clicked()), this, SLOT(add_buddy_hard()));
}

void MainWindowImpl::about()
{
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::ExistingFiles);
	dialog.setViewMode(QFileDialog::Detail);
	dialog.setDirectory(pathName);
	QStringList fileNames;
	QString type;
	dialog.exec();
}

void MainWindowImpl::refresh_label()
{
	int user_count = listWidget->count();
	QString str_num = QString::number(user_count);
	QString string = str_num + " users online.";
	label->setText(string);
}

void MainWindowImpl::add_buddy_hard()
{
	add_buddy(new QListWidgetItem(QIcon("/home/luther/pidgi/ui/bb8.jpg"),"Lucian",listWidget));
	
}

void MainWindowImpl::add_buddy(QListWidgetItem* iTem1)
{
	listWidget->addItem(iTem1);
	refresh_label();
}

// Extract a file path from a full path/filename
QString MainWindowImpl::extractFilePath(QString fn)
{
	if(fn == "")																																	// If we are sent an empty string......
		return("");																																	// ...then return and empty string
	while(fn.right(1) != "/")																											// Find the last path seperator
	{
		fn = fn.left(fn.size() - 1);
	}
	return(fn);																																		// Send back the path
}