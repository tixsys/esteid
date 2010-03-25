/*
 * id-updater: an utility to check updates for Estonian EID card MSI
 *
 * Copyright (C) 2009-2010  Estonian Informatics Centre
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "ui_idupdater.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtXml/QDomElement>
#include <QSystemTrayIcon>

class idupdater : public QMainWindow, private Ui::MainWindow {
    Q_OBJECT

public:
	idupdater(QString baseUrl,bool autocheck,bool autoupdate);
	~idupdater();

public slots:

private slots:
	void startInstall();
	void startUninstall();
	void checkUpdates();
	void netReplyFinished(QNetworkReply*);
	void netDownloadFinished(QNetworkReply*);
	void downloadProgress(qint64 recvd,qint64 total);
	void iconActivated(QSystemTrayIcon::ActivationReason reason);
	void cancel();
	void messageReceived(const QString &str);
	void activate();
private:
	bool m_autocheck,m_autoupdate;
	void enableInstall(bool enable);
	QString m_baseUrl;
	QDomElement product;
	void status(QString msg);
	void fail(QString);
	QNetworkAccessManager *manager;
	QNetworkAccessManager *downloadManager;

	QSystemTrayIcon *trayIcon;
	QMenu *trayIconMenu;

	QAction *restoreAction;
	QAction *quitAction;

	void createTrayIcon();
	void createActions();

protected:
     void closeEvent(QCloseEvent *event);
};
