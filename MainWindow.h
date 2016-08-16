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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QClipboard>
#include <QApplication>
#include <QTimer>
#include <QTcpServer>
#include <QTcpSocket>
#include <QtNetwork>

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow {

	Q_OBJECT

private:
	Ui::MainWindow *ui;
	QSystemTrayIcon *trayIcon;
	QMenu *trayIconMenu;

private:
	QString clipboardString;
	QTimer *clipboardWatchTimer;
	QClipboard *clipboard;
	QTcpServer *tcpServer;
	QTcpSocket *tcpClient;

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
	void changeEvent(QEvent *e);

public slots:
	void checkClipboardForChanges();
	void acceptConnection();
	void startRead();
	void about();

};

#endif // MAINWINDOW_H
