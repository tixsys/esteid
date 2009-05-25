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

#include "CertificateWidget.h"

#include "SslCertificate.h"

#include <QDateTime>
#include <QDesktopServices>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QSslKey>

class CertificateDialogPrivate
{
public:
	QSslCertificate cert;
};

CertificateDialog::CertificateDialog( QWidget *parent )
:	QDialog( parent )
,	d( new CertificateDialogPrivate )
{
	setupUi( this );
	tabWidget->removeTab( 2 );
}

CertificateDialog::CertificateDialog( const QSslCertificate &cert, QWidget *parent )
:	QDialog( parent )
,	d( new CertificateDialogPrivate )
{
	setupUi( this );
	setCertificate( cert );
	tabWidget->removeTab( 2 );
}

CertificateDialog::~CertificateDialog() { delete d; }

void CertificateDialog::addItem( const QString &variable, const QString &value, const QVariant &valueext )
{
	QTreeWidgetItem *t = new QTreeWidgetItem( parameters );
	t->setText( 0, variable );
	t->setText( 1, value );
	t->setData( 1, Qt::UserRole, valueext );
	parameters->addTopLevelItem( t );
}

QByteArray CertificateDialog::addHexSeparators( const QByteArray &data ) const
{
	QByteArray ret = data;
	for( int i = 2; i < ret.size(); i += 3 )
		ret.insert( i, ' ' );
	return ret;
}

void CertificateDialog::on_parameters_itemClicked( QTreeWidgetItem *item, int )
{
	if( !item->data( 1, Qt::UserRole ).isNull() )
		parameterContent->setPlainText( item->data( 1, Qt::UserRole ).toString() );
	else
		parameterContent->setPlainText( item->text( 1 ) );
}

void CertificateDialog::on_save_clicked()
{
	QString file = QFileDialog::getSaveFileName( this,
		tr("Save certificate"),
		QString( "%1%2%3.pem" )
			.arg( QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation ) )
			.arg( QDir::separator() )
			.arg( d->cert.subjectInfo( "serialNumber" ) ),
		tr("Certificates (*.pem *.crt *.cer)") );
	if( file.isEmpty() )
		return;

	QFile f( file );
	if( f.open( QIODevice::WriteOnly ) )
	{
		f.write( d->cert.toPem() );
		f.close();
	}
	else
		QMessageBox::warning( this, tr("Save certificate"), tr("Failed to save file") );
}

void CertificateDialog::setCertificate( const QSslCertificate &cert )
{
	d->cert = cert;
	SslCertificate c = cert;
	QString i;
	QTextStream s( &i );
	s << "<b>" << tr("Certificate Information:") << "</b><br />";
	s << "<hr>";
	s << "<b>" << tr("This certificate is intended for following purpose(s):") << "</b>";
	s << "<ul>";
	Q_FOREACH( const QString &ext, c.keyUsage() )
		s << "<li>" << ext << "</li>";
	s << "</ul>";
	s << "<br /><br /><br /><br />";
	//s << tr("* Refer to the certification authority's statement for details.") << "<br />";
	s << "<hr>";
	s << "<p style='margin-left: 30px;'>";
	s << "<b>" << tr("Issued to:") << "</b> " << c.subjectInfoUtf8( QSslCertificate::CommonName );
	s << "<br /><br /><br />";
	s << "<b>" << tr("Issued by:") << "</b> " << c.issuerInfo( QSslCertificate::CommonName );
	s << "<br /><br /><br />";
	s << "<b>" << tr("Valid from") << "</b> " << c.effectiveDate().toString( "dd.MM.yyyy" ) << " ";
	s << "<b>" << tr("to") << "</b> "<< c.expiryDate().toString( "dd.MM.yyyy" );
	s << "</p>";
	info->setHtml( i );

	addItem( tr("Version"), "V" + c.versionNumber() );
	addItem( tr("Serial number"), QString( "%1 (0x%2)" )
		.arg( c.serialNumber().constData() )
		.arg( QString::number( c.serialNumber().toInt(), 16 ) ) );
	addItem( tr("Signature algorithm"), "sha1RSA" );

	QStringList text, textExt;
	QList<QByteArray> subjects;
	subjects << "CN" << "OU" << "O" << "C";
	Q_FOREACH( const QByteArray &subject, subjects )
	{
		const QString &data = c.issuerInfo( subject );
		if( data.isEmpty() )
			continue;
		text << data;
		textExt << QString( "%1 = %2" ).arg( subject.constData() ).arg( data );
	}
	addItem( tr("Issuer"), text.join( ", " ), textExt.join( "\n" ) );
	addItem( tr("Valid from"), c.effectiveDate().toString( "dd.MM.yyyy hh:mm:ss" ) );
	addItem( tr("Vaild to"), c.expiryDate().toString( "dd.MM.yyyy hh:mm:ss" ) );

	subjects.clear();
	text.clear();
	textExt.clear();
	subjects << "serialNumber" << "GN" << "SN" << "CN" << "OU" << "O" << "C";
	Q_FOREACH( const QByteArray &subject, subjects )
	{
		const QString &data = c.subjectInfoUtf8( subject );
		if( data.isEmpty() )
			continue;
		text << data;
		textExt << QString( "%1 = %2" ).arg( subject.constData() ).arg( data );
	}
	addItem( tr("Subject"), text.join( ", " ), textExt.join( "\n" ) );
	addItem( tr("Public key"), QString("RSA (%1)").arg( c.publicKey().length() ),
		addHexSeparators( c.publicKey().toDer().toHex() ) );
	addItem( tr("Certificate policies"), c.policies().join( ", " ) );
}
