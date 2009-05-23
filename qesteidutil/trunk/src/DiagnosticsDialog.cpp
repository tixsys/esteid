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
#include "cardlib/DynamicLibrary.h"
#include "cardlib/SmartCardManager.h"
#include "cardlib/EstEidCard.h"

#include <QDebug>
#include <QTextStream>

DiagnosticsDialog::DiagnosticsDialog( QWidget *parent )
:	QDialog( parent )
{
	setupUi( this );
	setAttribute( Qt::WA_DeleteOnClose );

	QString info;
	QTextStream s( &info );

	s << "<b>" << tr("Version:") << "</b> ";
	s << QCoreApplication::applicationVersion() << "<br />";
	s << "<br />";
	s << "<b>" << tr("Library paths:") << "</b> ";
	s << QCoreApplication::libraryPaths().join( ";" ) << "<br />";
	s << "<br />";
	s << tr("<b>Libraries</b>") << "<br />";
#ifdef WIN32
	s << getLibVersion( "advapi32") << "<br />";
#else
	s << getLibVersion( "pcsclite" ) << "<br />";
#endif
	s << getLibVersion( "opensc-pkcs11" ) << "<br />";
	s << getLibVersion( "engine_pkcs11" ) << "<br />";
	s << "<br />";

	s << "<b>" << tr("Card readers") << "</b><br />";
	s << getReaderInfo();
	s << "<br />";

	diagnosticsText->setHtml( info );
}

QString DiagnosticsDialog::getLibVersion( const QString &lib ) const
{
	QString r = lib;
	try
	{
		DynamicLibrary l( lib.toLatin1() );
		r += " (" + QString::fromStdString( l.getVersionStr() ) + ")";
	}
	catch( const std::runtime_error & ) {}

	return r;
}

QString DiagnosticsDialog::getReaderInfo() const
{
	QString d;
	QTextStream s( &d );

	QHash<QString,QString> readers;
	SmartCardManager *m = 0;
	EstEidCard *card = 0;
	try {
		m = new SmartCardManager();
		int readersCount = m->getReaderCount( true );
		for( int i = 0; i < readersCount; i++ )
		{
			QString reader = QString::fromStdString( m->getReaderName( i ) );
			if ( !QString::fromStdString( m->getReaderState( i ) ).contains( "EMPTY" ) )
			{
				if ( !card )
					card = new EstEidCard( *m );
				card->connect( i, true );
				readers[reader] = tr( "ID - %1" ).arg( QString::fromStdString( card->readCardID() ) );
			}
			else
				readers[reader] = "";
		}
	} catch( std::runtime_error &e ) {
		s << "<br /><b>" << tr("Error reading card data: ") << "</b>" << e.what();
	}
	if ( card )
		delete card;
	delete m;

	for( QHash<QString,QString>::const_iterator i = readers.constBegin();
		i != readers.constEnd(); ++i )
	{
		s << "* " << i.key();
		if( !i.value().isEmpty() )
			s << "<p style='margin-left: 10px; margin-top: 0px; margin-bottom: 0px; margin-right: 0px;'>" << i.value() << "</p>";
		else
			s << "<br />";
	}

	return d;
}
