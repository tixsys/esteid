#pragma once

#include <QWebView>

#include "jsextender.h"
#include "jscardmanager.h"
#include "jsesteidcard.h"
#include "jscertdata.h"

class MainWindow : public QWebView
{
    Q_OBJECT

public:
    MainWindow( QWidget *parent = 0 );

private:
    JsExtender *m_jsExtender;
    JsCardManager *jsCardManager;
    JsEsteidCard *jsEsteidCard;
};
