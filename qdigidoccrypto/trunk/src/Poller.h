#pragma once

#include <QThread>

#include <QMutex>
#include <QSslCertificate>

class Poller: public QThread
{
	Q_OBJECT

public:
    Poller( QObject *parent = 0 );

	void lock();
	void run();
	void unlock();

Q_SIGNALS:
	void dataChanged( const QSslCertificate &auth, const QSslCertificate &sign );

private:
	QMutex m;
	QString selectedCard;
};
