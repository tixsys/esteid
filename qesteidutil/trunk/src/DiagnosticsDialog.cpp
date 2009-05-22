/*
 * QEstEidUtil
 *
 * Copyright (C) 2009 Jargo Kõster <jargo@innovaatik.ee>
 * Copyright (C) 2009 Raul Metsma <raul@innovaatik.ee>
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
 *
 */

#include "DiagnosticsDialog.h"

#include "cardlib/common.h"
#include "cardlib/SmartCardManager.h"
#include "cardlib/EstEidCard.h"

#include <QDebug>

DiagnosticsDialog::DiagnosticsDialog( QWidget *parent )
:	QDialog( parent )
{
	setupUi( this );
	setAttribute( Qt::WA_DeleteOnClose );

	getDiagnosticDetails();
}

void DiagnosticsDialog::getDiagnosticDetails()
{
	QStringList d;
	d << tr("<b>Version:</b> %1<br />").arg( QCoreApplication::applicationVersion() );
	d << tr( "<b>Library paths:</b> %1<br />" ).arg( QCoreApplication::libraryPaths().join( ";" ) );

	d << getReaderInfo();
	diagnosticsText->setHtml( d.join( "<br />" ) );
}

QStringList DiagnosticsDialog::getReaderInfo() const
{
	QStringList d;
	try {
		SmartCardManager *m = new SmartCardManager();
		QStringList readers;
		int readersCount = m->getReaderCount( true );
		EstEidCard *card = 0;
		for( int i = 0; i < readersCount; i++ )
		{
			readers << QString::fromStdString( m->getReaderName( i ) );
			if ( !QString::fromStdString( m->getReaderState( i ) ).contains( "EMPTY" ) )
			{
				if ( !card )
					card = new EstEidCard( *m );
				card->connect( i, true );
				readers << tr( "&#160;&#160;&#160;&#160;&#160;&#160;ID - %1" ).arg( QString::fromStdString( card->readCardID() ) );
			}
		}
		if ( readers.size() > 0 )
			d << tr( "<b>Card readers</b><br />&#160;&#160;&#160;%1" ).arg( readers.join( "<br />&#160;&#160;&#160;" ) );
		if ( card )
			delete card;
		delete m;
	} catch( std::runtime_error &e ) {
		qDebug() << e.what();
	}
	return d;
}
