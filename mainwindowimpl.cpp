#include <QtGui>
#include "mainwindowimpl.h"
//
MainWindowImpl::MainWindowImpl( QWidget * parent, Qt::WFlags f) 
	: QMainWindow(parent, f)
{
	setupUi(this);
	pathName = QApplication::applicationDirPath();
	connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));
	connect(actionAdd, SIGNAL(triggered()), this, SLOT(add_buddy()));

}

void MainWindowImpl::about()
{
	QFileDialog dialog(this);																	// create a file dialog for selecting the files we want to work on
	dialog.setFileMode(QFileDialog::ExistingFiles);						// make sure we only display files
	dialog.setViewMode(QFileDialog::Detail);									// get a detailed view of the files
	dialog.setDirectory(pathName);														// set the current path to the last used path
	QStringList fileNames;																		// This holds the list of files that the user selects
	QString type;																							// This holds the command line switch for either access or modified time
	dialog.exec();
}

void MainWindowImpl::add_buddy()
{
	QString new_buddy = "Lucian";
	listWidget->addItem(new_buddy);
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