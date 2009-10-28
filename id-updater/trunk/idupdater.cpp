#include "idupdater.h"
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDomDocument>
#include <QDir>
#include <QTime>
#include "InstallChecker.h"

idupdater::idupdater(QString baseUrl,bool autocheck,bool autoupdate) : QMainWindow(),
	m_autocheck(autocheck),m_autoupdate(autoupdate),m_baseUrl(baseUrl) {
	
	setupUi(this);
	manager = new QNetworkAccessManager(this);
	downloadManager = new QNetworkAccessManager(this);
	connect(manager, SIGNAL(finished(QNetworkReply*)),
		this, SLOT(netReplyFinished(QNetworkReply*)));
	connect(downloadManager, SIGNAL(finished(QNetworkReply*)),
		this, SLOT(netDownloadFinished(QNetworkReply*)));
	groupBoxProduct->setVisible(false);
	enableInstall(false);
	if (m_autocheck) 
		m_checkUpdatesButton->click();
}

idupdater::~idupdater() {
	delete manager;
	}

void idupdater::enableInstall(bool enable,bool enableCheck) {
	m_installButton->setEnabled(enable);
	m_checkUpdatesButton->setEnabled(enableCheck);
	if (enable && m_autoupdate)
		m_installButton->click();
	}

void idupdater::status(QString msg) {
	m_updateStatus->setText(msg);
	}
void idupdater::fail(QString msg) {
	m_updateStatus->setText("Failed: " + msg);
	enableInstall(false);
	}

void idupdater::netReplyFinished(QNetworkReply* reply) {
	groupBoxProduct->setVisible(true);
	status("Check completed");
	QDomDocument doc;
	doc.setContent(reply);
	if (!doc.hasChildNodes()) return fail("could not load a valid update file");
	QDomNodeList nodes = doc.elementsByTagName("product");
	if (nodes.length() != 1 ) return fail("expected one product");
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
		close();
	}

void idupdater::netDownloadFinished(QNetworkReply* reply) {
	status("Download finished, starting installation...");
	QFile tmp(QDir::tempPath() + "/" + product.attribute("filename")) ;
	if (tmp.open(QFile::ReadWrite)) {
		if (reply) tmp.write(reply->readAll());
		tmp.close();
		QString tgt = QDir::toNativeSeparators(tmp.fileName());
		if (!InstallChecker::verifyPackage(tgt.toStdWString()))
			return fail("Downloaded package integrity check failed");
		if (InstallChecker::installPackage(tgt.toStdWString())) {
			status("Package installed");
			close();
			}
		else 
			return fail("Package installation failed");
		}
	enableInstall(true);
	}

void idupdater::checkUpdates() {
	enableInstall(false);
	status("Checking for update..");	
	QUrl url(m_baseUrl + "products.xml");
	if (url.scheme() != "http" &&
		url.scheme() != "ftp" &&
		url.scheme() != "https" 
		) return fail("Unsupported protocol in url");
	manager->get(QNetworkRequest(url));
	}

void idupdater::startInstall() {
	enableInstall(false,false);
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
	status("Downloading...");
	}

void idupdater::startUninstall() {
	netDownloadFinished(NULL); //debugging
	enableInstall(false,false);
	}
