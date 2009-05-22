#pragma once

#include <QThread>

#include <QMutex>
#include <QSslCertificate>

class Poller: public QThread
{
	Q_OBJECT

public:
    Poller( QObject *parent = 0 );
	~Poller();

	void lock();
	void run();
	void unlock();

Q_SIGNALS:
	void dataChanged( const QSslCertificate &auth, const QSslCertificate &sign );

private:
	bool terminate;
	QMutex m;
	QString selectedCard;
};
