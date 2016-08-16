/*
 * Clipboard Sharer
 *
 * Clipboard Sharer by Dan Krusi is licensed under a Creative Commons
 * Attribution-NonCommercial 3.0 Unported License.
 *
 * Copyright 2011 Dan Krusi
 *
 * www.dankrusi.com
 *
 */

#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{

	// GUI
	ui->setupUi(this);
	this->setWindowTitle("Clipboard Sharer Settings");
	this->setWindowIcon(QIcon(":/images/icon.png"));
	qApp->setQuitOnLastWindowClosed(false);

	// Load settings
	QSettings settings("Nerves","ClipboardSharer");
	ui->clipboardRefresh->setValue(settings.value("clipboard-refresh", "200").toInt());
	ui->localPort->setValue(settings.value("local-port", "7373").toInt());
	ui->remoteMachine->setText(settings.value("remote-machine", "").toString());
	ui->remotePort->setValue(settings.value("remote-port", "7373").toInt());

	// Override from command line?
	if(qApp->argc() > 1) ui->localPort->setValue(QString(qApp->argv()[1]).toInt());
	if(qApp->argc() > 2) ui->remoteMachine->setText(QString(qApp->argv()[2]));
	if(qApp->argc() > 3) ui->remotePort->setValue(QString(qApp->argv()[3]).toInt());

	// Tray icon and menu
	trayIconMenu = new QMenu(this);
	QAction *settingsAction = trayIconMenu->addAction("Settings");
	connect(settingsAction,SIGNAL(triggered()),this,SLOT(show()));
	QAction *aboutAction = trayIconMenu->addAction("About");
	connect(aboutAction,SIGNAL(triggered()),this,SLOT(about()));
	QAction *quitAction = trayIconMenu->addAction("Quit");
	connect(quitAction,SIGNAL(triggered()),qApp,SLOT(quit()));
	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setContextMenu(trayIconMenu);
	#ifdef Q_OS_MAC
		trayIcon->setIcon(QIcon(":/images/icondark.png"));
	#else
		trayIcon->setIcon(QIcon(":/images/icon.png"));
	#endif
	trayIcon->show();

	// Clipboard watcher
	clipboard = QApplication::clipboard();
	clipboardWatchTimer = new QTimer();
	connect(clipboardWatchTimer,SIGNAL(timeout()),this,SLOT(checkClipboardForChanges()));
	clipboardWatchTimer->start();
	clipboardWatchTimer->setInterval(ui->clipboardRefresh->value());

	// Server
	tcpServer = new QTcpServer(this);
	if (!tcpServer->listen(QHostAddress::Any,ui->localPort->value())) {
	// Error
		QMessageBox msg;
		msg.setText(QString("Could not start the clipboard server on port %1.").arg(ui->localPort->value()));
		msg.exec();
	} else {
		connect(tcpServer, SIGNAL(newConnection()), this, SLOT(acceptConnection()));
	}

}

MainWindow::~MainWindow()
{
	// Save settings
	QSettings settings("Nerves","ClipboardSharer");
	settings.setValue("clipboard-refresh",ui->clipboardRefresh->value());
	settings.setValue("local-port",ui->localPort->value());
	settings.setValue("remote-machine",ui->remoteMachine->text());
	settings.setValue("remote-port",ui->remotePort->value());

	// Cleanup
	delete ui;
	delete trayIconMenu;
	delete trayIcon;
	delete tcpServer;
	delete clipboardWatchTimer;
}


void MainWindow::changeEvent(QEvent *e)
{
	QMainWindow::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}


void MainWindow::checkClipboardForChanges() {

	if(clipboard && clipboard->text() != clipboardString) {

		// Register change
		clipboardString = clipboard->text();
		qDebug() << "clipboard changed:" << QString(clipboardString).replace("\n","\\n");

		// Send change to remote machine
		QTcpSocket client;
		client.connectToHost(ui->remoteMachine->text(),ui->remotePort->value());
		client.waitForConnected();
		if(client.isWritable()) {
			client.write(clipboardString.toAscii());
			client.waitForBytesWritten(2000);
			client.close();
		}
	}
}


void MainWindow::acceptConnection() {
	// Get the next pending connection
	tcpClient = tcpServer->nextPendingConnection();
	connect(tcpClient, SIGNAL(readyRead()), this, SLOT(startRead()));
}

void MainWindow::startRead()
{

	// Get the data from client
	QString data = tcpClient->readAll();
	qDebug() << "new clipboard:" << data;
	clipboardString = data;
	clipboard->setText(data);

}

void MainWindow::about()
{

	QMessageBox msg;
	msg.setText(QString("Clipboard Sharer %1 \n\nClipboard Sharer is a small program that can share a clipboard between two computers.\n\nClipboard Sharer by Dan Krusi is licensed under a Creative Commons Attribution-NonCommercial 3.0 Unported License.\n\nCopyright 2011 Dan Krusi\n\nwww.dankrusi.com").arg(APP_VERSION));
	msg.exec();

}
