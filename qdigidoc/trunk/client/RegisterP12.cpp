/*
 * QDigiDocClient
 *
 * Copyright (C) 2009 Jargo K�ster <jargo@innovaatik.ee>
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

#include "RegisterP12.h"

#include "Settings.h"

#include <QDesktopServices>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QUrl>

RegisterP12::RegisterP12( const QString &cert, QWidget *parent )
:	QWidget( parent )
{
	setupUi( this );
	p12Cert->setText( cert );
}

void RegisterP12::on_buttonBox_accepted()
{
	QFileInfo file( p12Cert->text() );
	if( !file.isFile() )
	{
		QMessageBox::warning( this, windowTitle(),
			tr("No OCSP PKCS#12 certificate selected") );
		return;
	}

	QString dest = QString( "%1/%2" )
		.arg( QDesktopServices::storageLocation( QDesktopServices::DataLocation ) )
		.arg( file.fileName() );

	if( QFile::exists( dest ) )
		QFile::remove( dest );
	if( !QFile::copy( p12Cert->text(), dest ) )
	{
		QMessageBox::warning( this, windowTitle(), tr("Failed to copy file") );
		return;
	}

	SettingsValues s;
	s.setValue( "Main/pkcs12Cert", dest );
	s.setValue( "Main/pkcs12Pass", p12Pass->text() );
	close();
}

void RegisterP12::on_info_linkActivated( const QString &url )
{ QDesktopServices::openUrl( url ); }

void RegisterP12::on_p12Button_clicked()
{
	QString cert = QFileDialog::getOpenFileName( this, tr("Select PKCS#12 certificate"),
		p12Cert->text(), tr("PKCS#12 Certificates (*.p12 *.p12d)") );
	if( !cert.isEmpty() )
		p12Cert->setText( cert );
}
