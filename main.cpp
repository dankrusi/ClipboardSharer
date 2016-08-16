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

#include <QtGui/QApplication>
#include <QSettings>

#include "MainWindow.h"

int main(int argc, char *argv[])
{
	// Load app
    QApplication a(argc, argv);
	MainWindow w;
    return a.exec();
}
