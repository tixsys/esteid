#include "Poller.h"

#include "SslCertificate.h"

#include <libdigidoc/DigiDocConfig.h>
#include <libdigidoc/DigiDocPKCS11.h>

#include <QHash>

Poller::Poller( QObject *parent )
:	QThread( parent )
{}

void Poller::lock() { m.lock(); }

void Poller::run()
{
	Q_FOREVER
	{
		sleep( 5 );
		if( !m.tryLock() )
			continue;

		CK_BYTE driver[200];
		snprintf( (char*)driver, sizeof(driver), "DIGIDOC_DRIVER_%d_FILE",
		  ConfigItem_lookup_int( "DIGIDOC_DEFAULT_DRIVER", 1 ) );
		LIBHANDLE lib = initPKCS11Library( ConfigItem_lookup( (const char*)driver ) );

		CK_ULONG count = 20;
		CK_SLOT_ID slotids[20];
		int err = GetSlotIds( (CK_SLOT_ID*)&slotids, &count );
		if( err != ERR_OK )
		{
			m.unlock();
			closePKCS11Library( lib, 0 );
			continue;
		}

		QHash<QString,int> cards;
		for( int i = 0; i < count; ++i )
		{
			CK_TOKEN_INFO tokeninfo;
			err = GetTokenInfo( &tokeninfo, slotids[i] );
			QString serialNumber = (char*)tokeninfo.serialNumber;
			if( !cards.contains( serialNumber ) )
				cards[serialNumber] = i;
		}
		closePKCS11Library( lib, 0 );

		if( selectedCard.isEmpty() && !cards.isEmpty() )
		{
			selectedCard = cards.begin().key();
			int slot = cards.begin().value();
			X509 *cert;
			findUsersCertificate( slot, &cert );
			QSslCertificate auth = SslCertificate::fromX509( (Qt::HANDLE*)cert );
			free( cert );
			findUsersCertificate( slot + 1, &cert );
			QSslCertificate sign = SslCertificate::fromX509( (Qt::HANDLE*)cert );
			free( cert );
			Q_EMIT dataChanged( auth, sign );
		}
		else if( !selectedCard.isEmpty() && !cards.contains( selectedCard ) )
		{
			selectedCard.clear();
			Q_EMIT dataChanged( QSslCertificate(), QSslCertificate() );
		}
		m.unlock();
	}
}

void Poller::unlock() { m.unlock(); }
