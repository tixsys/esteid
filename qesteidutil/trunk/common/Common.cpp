/*
 * QEstEidCommon
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

#include "Common.h"

#include <QDesktopServices>
#include <QProcess>
#include <QUrl>

#ifdef Q_OS_WIN32
#include <QDir>
#include <QFile>
#include <QLibrary>

#include <windows.h>
#include <mapi.h>
#endif

Common::Common( QObject *parent ): QObject( parent ) {}

QString Common::fileSize( quint64 bytes )
{
	const quint64 kb = 1024;
	const quint64 mb = 1024 * kb;
	const quint64 gb = 1024 * mb;
	const quint64 tb = 1024 * gb;
	if( bytes >= tb )
		return QString( "%1 TB" ).arg( qreal(bytes) / tb, 0, 'f', 3 );
	if( bytes >= gb )
		return QString( "%1 GB" ).arg( qreal(bytes) / gb, 0, 'f', 2 );
	if( bytes >= mb )
		return QString( "%1 MB" ).arg( qreal(bytes) / mb, 0, 'f', 1 );
	if( bytes >= kb )
		return QString( "%1 KB" ).arg( bytes / kb );
	return QString( "%1 B" ).arg( bytes );
}

void Common::mailTo( const QUrl &url )
{
#ifdef Q_OS_WIN32
	QString file = url.queryItemValue( "attachment" );
	QByteArray filePath = QDir::toNativeSeparators( file ).toLatin1();
	QByteArray fileName = QFileInfo( file ).fileName().toLatin1();
	QByteArray subject = url.queryItemValue( "subject" ).toLatin1();

	MapiFileDesc doc[1];
	doc[0].ulReserved = 0;
	doc[0].flFlags = 0;
	doc[0].nPosition = -1;
	doc[0].lpszPathName = filePath.data();
	doc[0].lpszFileName = fileName.data();
	doc[0].lpFileType = NULL;

	// Create message
	MapiMessage message;
	message.ulReserved = 0;
	message.lpszSubject = subject.data();
	message.lpszNoteText = "";
	message.lpszMessageType = NULL;
	message.lpszDateReceived = NULL;
	message.lpszConversationID = NULL;
	message.flFlags = 0;
	message.lpOriginator = NULL;
	message.nRecipCount = 0;
	message.lpRecips = NULL;
	message.nFileCount = 1;
	message.lpFiles = (lpMapiFileDesc)&doc;

	QLibrary lib("mapi32");
	typedef ULONG (PASCAL *SendMail)(ULONG,ULONG,MapiMessage*,FLAGS,ULONG);
	SendMail mapi = (SendMail)lib.resolve("MAPISendMail");
	if( mapi )
	{
		int status = mapi( NULL, 0, &message, MAPI_LOGON_UI|MAPI_DIALOG, 0 );
		if( status == SUCCESS_SUCCESS )
			return;
	}
#else
	QDesktopServices::openUrl( url );
#endif
}

bool Common::startDetached( const QString &program )
{ return startDetached( program, QStringList() ); }

bool Common::startDetached( const QString &program, const QStringList &arguments )
{
#ifdef Q_OS_MAC
	return QProcess::startDetached( "open", QStringList() << "-a" << program << arguments );
#else
	return QProcess::startDetached( program, arguments );
#endif
}

QUrl Common::toUrl( const QString &path )
{
#ifdef Q_OS_WIN32
	return QString( "file:///" ).append( path );
#else
	return QString( "file://" ).append( path );
#endif
}
