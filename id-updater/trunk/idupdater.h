
#include "ui_idupdater.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QDomElement>
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

private:
	bool m_autocheck,m_autoupdate;
	void enableInstall(bool enable,bool enableCheck = true);
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
