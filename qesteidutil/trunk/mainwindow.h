#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "jsextender.h"
#include "jscardmanager.h"
#include "jsesteidcard.h"
#include "jscertdata.h"

class QActionGroup;
class QWebView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

private:
    JsExtender *m_jsExtender;
    JsCardManager *jsCardManager;
    JsEsteidCard *jsEsteidCard;

    QWebView *webView;
};

#endif // MAINWINDOW_H
