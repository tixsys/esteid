#include "idupdater.h"
#include <QUrl>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtXml/QDomDocument>
#include <QDir>
#include <QTime>
#include <QMenu>
#include <QCloseEvent>
#include "InstallChecker.h"

idupdater::idupdater(QString baseUrl,bool autocheck,bool autoupdate) : QMainWindow(),
	m_autocheck(autocheck),m_autoupdate(autoupdate),m_baseUrl(baseUrl) {

	Q_INIT_RESOURCE(idupdater);
	
	setupUi(this);

	createActions();
	createTrayIcon();

	manager = new QNetworkAccessManager(this);
	downloadManager = new QNetworkAccessManager(this);
	connect(manager, SIGNAL(finished(QNetworkReply*)),
		this, SLOT(netReplyFinished(QNetworkReply*)));
	connect(downloadManager, SIGNAL(finished(QNetworkReply*)),
		this, SLOT(netDownloadFinished(QNetworkReply*)));
	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
		this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
	connect(trayIcon, SIGNAL(messageClicked()),
		this, SLOT(activate()));
	
	groupBoxProduct->setVisible(false);

	enableInstall(false);

	trayIcon->show();

	if (m_autocheck) 
		checkUpdates();

	if (m_autoupdate)
		QApplication::postEvent(this,new QCloseEvent());
	setWindowIcon(QIcon(":/appicon.bmp"));
}

idupdater::~idupdater() {
	delete manager;
	}

void idupdater::createActions() {
	restoreAction = new QAction(tr("&Restore"), this);
	connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

	quitAction = new QAction(tr("&Quit"), this);
	connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
	}

void idupdater::createTrayIcon() {
     trayIconMenu = new QMenu(this);
     trayIconMenu->addAction(restoreAction);
     trayIconMenu->addAction(quitAction);

     trayIcon = new QSystemTrayIcon(this);
     trayIcon->setContextMenu(trayIconMenu);
	 trayIcon->setIcon(QIcon(":/appicon.bmp"));
	}

void idupdater::iconActivated(QSystemTrayIcon::ActivationReason reason)  {
	switch (reason) {
		case QSystemTrayIcon::Trigger:
		case QSystemTrayIcon::DoubleClick:
			showNormal();
			break;
		default:;
		}
	}

 void idupdater::closeEvent(QCloseEvent *event) {
     if (trayIcon->isVisible()) {
         hide();
         event->ignore();
	     }
	}

void idupdater::cancel() {
	QApplication::quit();
	}

void idupdater::enableInstall(bool enable) {
	m_installButton->setEnabled(enable);
	if (enable && QSystemTrayIcon::supportsMessages()) {
		trayIcon->showMessage(tr("New update available"), m_productName->text(), QSystemTrayIcon::Information, 100000);
		}
	if (enable && m_autoupdate) {
		m_installButton->click();
		}
	}

void idupdater::status(QString msg) {
	m_updateStatus->setText(msg);
	}
void idupdater::fail(QString msg) {
	m_updateStatus->setText(tr("Failed: ") + msg);
	enableInstall(false);
	}

void idupdater::netReplyFinished(QNetworkReply* reply) {
	groupBoxProduct->setVisible(true);
	status(tr("Check completed"));
	QDomDocument doc;
	doc.setContent(reply);
	if (!doc.hasChildNodes()) return fail(tr("could not load a valid update file"));
	QDomNodeList nodes = doc.elementsByTagName("product");
	if (nodes.length() != 1 ) return fail(tr("expected one product"));
	product = nodes.item(0).toElement();
	QString code = product.attribute("UpgradeCode");
	std::wstring version,availableVersion;
	InstallChecker::getInstalledVersion(code.toStdWString(),version);
	availableVersion = product.attribute("ProductVersion").toStdWString();

	m_productName->setText(product.attribute("ProductName","none"));
	m_productDescription->setText(product.attribute("ProductName","none"));
	m_installedVer->setText(QString::fromStdWString(version));
	m_availableVer->setText(product.attribute("ProductVersion","0"));
	
	enableInstall( version != availableVersion);
	if (version == availableVersion && m_autocheck) 
		cancel();
	}

void idupdater::netDownloadFinished(QNetworkReply* reply) {
	status(tr("Download finished, starting installation..."));
	QFile tmp(QDir::tempPath() + "/" + product.attribute("filename")) ;
	if (tmp.open(QFile::ReadWrite)) {
		if (reply) tmp.write(reply->readAll());
		tmp.close();
		QString tgt = QDir::toNativeSeparators(tmp.fileName());
		if (!InstallChecker::verifyPackage(tgt.toStdWString()))
			return fail(tr("Downloaded package integrity check failed"));
		if (InstallChecker::installPackage(tgt.toStdWString(),m_autoupdate)) {
			status(tr("Package installed"));
			cancel();
			}
		else 
			return fail(tr("Package installation failed"));
		}
	enableInstall(true);
	}

void idupdater::checkUpdates() {
	enableInstall(false);
	status(tr("Checking for update.."));	
	QUrl url(m_baseUrl + "products.xml");
	if (url.scheme() != "http" &&
		url.scheme() != "ftp" &&
		url.scheme() != "https" 
		) return fail(tr("Unsupported protocol in url"));
	manager->get(QNetworkRequest(url));
	}

void idupdater::startInstall() {
	enableInstall(false);
	QUrl url(m_baseUrl + product.attribute("filename"));
	QNetworkReply * reply = downloadManager->get(QNetworkRequest(url));
	connect(reply, SIGNAL(downloadProgress(qint64,qint64)),
		this, SLOT(downloadProgress(qint64,qint64)));
	m_downloadProgress->setValue(0);
	}

void idupdater::downloadProgress(qint64 recvd,qint64 total) {
	static QTime lastupdate = QTime::currentTime();
	static qint64 lastRecvd = 0;
	QTime current = QTime::currentTime();
	int timediff = lastupdate.msecsTo(current);
	if (timediff > 500) {
		int bytesPerMs = (recvd - lastRecvd) / timediff;
		m_downloadStatus->setText(QString::number( bytesPerMs ) + " KB/s");
		lastupdate = current;
		lastRecvd = recvd;
		}
	float perc = recvd * 1.0f/total;
	m_downloadProgress->setValue( perc * 100.0f);
	status(tr("Downloading..."));
	}

void idupdater::startUninstall() {
	netDownloadFinished(NULL); //debugging
	enableInstall(false);
	}

void idupdater::messageReceived(const QString &str) {
	if (str == "runupdate") {
		activate();
		checkUpdates();
		}
	}

void idupdater::activate(){
	activateWindow();
	showNormal();
	}	
