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

#include "CertificateWidget.h"

#include "SslCertificate.h"

#include <QDateTime>
#include <QDesktopServices>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QSslKey>

class CertificateWidgetPrivate
{
public:
	QSslCertificate cert;
};

CertificateWidget::CertificateWidget( QWidget *parent )
:	QWidget( parent )
,	d( new CertificateWidgetPrivate )
{
	setupUi( this );
	setAttribute( Qt::WA_DeleteOnClose );
	setWindowFlags( Qt::Dialog );
	tabWidget->removeTab( 2 );
}

CertificateWidget::CertificateWidget( const QSslCertificate &cert, QWidget *parent )
:	QWidget( parent )
,	d( new CertificateWidgetPrivate )
{
	setupUi( this );
	setAttribute( Qt::WA_DeleteOnClose );
	setWindowFlags( Qt::Dialog );
	setCertificate( cert );
}

CertificateWidget::~CertificateWidget() { delete d; }

QByteArray CertificateWidget::addHexSeparators( const QByteArray &data ) const
{
	QByteArray ret = data;
	for( int i = 2; i < ret.size(); i += 3 )
		ret.insert( i, ' ' );
	return ret;
}

void CertificateWidget::on_parameters_itemClicked( QTreeWidgetItem *item, int )
{
	if( !item->data( 1, Qt::UserRole ).isNull() )
		parameterContent->setPlainText( item->data( 1, Qt::UserRole ).toString() );
	else
		parameterContent->setPlainText( item->text( 1 ) );
}

void CertificateWidget::on_save_clicked()
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

void CertificateWidget::setCertificate( const QSslCertificate &cert )
{
	d->cert = cert;
	SslCertificate c = cert;
	QString i;
	QTextStream s( &i );
	s << "<b>" << tr("Certificate Information:") << "</b><br />";
	s << "<hr>";
	s << "<b>" << tr("This certificate is intended for following purpose(s):") << "</b><ul>";
	foreach( const QString &ext, c.keyUsage() )
		s << "<li>" << ext << "</li>";
	s << "</ul><br /><br /><br /><br />";
	s << tr("* Refer to the certification authority's statement for details.") << "<br />";
	s << "<hr>";
	s << "<p style='margin-left: 30px;'>";
	s << "<b>" << tr("Issued to:") << "</b> " << c.subjectInfoUtf8( QSslCertificate::CommonName ) << "<br /><br /><br />";
	s << "<b>" << tr("Issued by:") << "</b> " << c.issuerInfo( QSslCertificate::CommonName ) << "<br /><br /><br />";
	s << "<b>" << tr("Valid from") << "</b> " << c.effectiveDate().toString( "dd.MM.yyyy" ) << " ";
	s << "<b>" << tr("to") << "</b> "<< c.expiryDate().toString( "dd.MM.yyyy" );
	s << "</p>";
	info->setHtml( i );

	QTreeWidgetItem *t;

	t = new QTreeWidgetItem( parameters );
	t->setText( 0, tr("Version") );
	t->setText( 1, "V" + c.versionNumber() );
	parameters->addTopLevelItem( t );

	t = new QTreeWidgetItem( parameters );
	t->setText( 0, tr("Serial number") );
	t->setText( 1, QString( "%1 (0x%2)" )
		.arg( c.serialNumber().constData() )
		.arg( QString::number( c.serialNumber().toInt(), 16 ) ) );
	parameters->addTopLevelItem( t );

	t = new QTreeWidgetItem( parameters );
	t->setText( 0, tr("Signature algorithm") );
	t->setText( 1, "sha1RSA" );
	parameters->addTopLevelItem( t );

	QString issuer;
	issuer += c.issuerInfo( "CN" ) + ", ";
	issuer += c.issuerInfo( "OU" ) + ", ";
	issuer += c.issuerInfo( "O" ) + ", ";
	issuer += c.issuerInfo( "C" );
	t = new QTreeWidgetItem( parameters );
	t->setText( 0, tr("Issuer") );
	t->setText( 1, issuer );
	parameters->addTopLevelItem( t );

	t = new QTreeWidgetItem( parameters );
	t->setText( 0, tr("Valid from") );
	t->setText( 1, c.effectiveDate().toString( "dd.MM.yyyy hh:mm:ss" ) );
	parameters->addTopLevelItem( t );

	t = new QTreeWidgetItem( parameters );
	t->setText( 0, tr("Vaild to") );
	t->setText( 1, c.expiryDate().toString( "dd.MM.yyyy hh:mm:ss" ) );
	parameters->addTopLevelItem( t );

	QString subject;
	subject += c.subjectInfo( "serialNumber" ) + ", ";
	subject += c.subjectInfoUtf8( "GN" ) + ", ";
	subject += c.subjectInfoUtf8( "SN" ) + ", ";
	subject += c.subjectInfoUtf8( "CN" ) + ", ";
	subject += c.subjectInfo( "OU" ) + ", ";
	subject += c.subjectInfo( "O" ) + ", ";
	subject += c.subjectInfo( "C" );
	t = new QTreeWidgetItem( parameters );
	t->setText( 0, tr("Subject") );
	t->setText( 1, subject );
	parameters->addTopLevelItem( t );

	t = new QTreeWidgetItem( parameters );
	t->setText( 0, tr("Public key") );
	t->setText( 1, tr("RSA (%1)").arg( c.publicKey().length() ) );
	t->setData( 1, Qt::UserRole, addHexSeparators( c.publicKey().toDer().toHex() ) );
	parameters->addTopLevelItem( t );
}
