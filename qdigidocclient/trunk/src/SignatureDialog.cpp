/*
 * QDigiDocClient
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

#include "SignatureDialog.h"

#include "common/CertificateWidget.h"
#include "common/SslCertificate.h"

#include <libdigidoc++/Document.h>

#include <QDateTime>
#include <QSslKey>
#include <QTextStream>

SignatureWidget::SignatureWidget( const DigiDocSignature &signature, unsigned int signnum, QWidget *parent )
:	QWidget( parent )
,	num( signnum )
,	s( signature )
{
	const SslCertificate cert = s.cert();
	QLabel *c = new QLabel( this );
	QString content;
	QTextStream st( &content );
	if( cert.isTempel() )
		st << "<img src=\":/images/ico_stamp_blue_16.png\">";
	else
		st << "<img src=\":/images/ico_person_blue_16.png\">";
	st << "<b>" << SslCertificate::formatName( cert.subjectInfoUtf8( "GN" ) ) << " "
		<< SslCertificate::formatName( cert.subjectInfoUtf8( "SN" ) ) << "</b><br />";
	st << s.location() << "<br />";
	st << tr("Signed on") << " " << s.dateTime().toString( "dd. MMMM yyyy kell hh:mm" ) << "<br />";
	st << tr("Signature is") << " " << ( s.isValid() ? tr("valid") : tr("not valid") );
	c->setText( content );

	QLabel *b = new QLabel( QString( "<a href=\"details\">%1</a><br /><a href=\"remove\">%2</a>" )
		.arg( tr("Show details") ).arg( tr("Remove") ), this );
	connect( b, SIGNAL(linkActivated(QString)), SLOT(link(QString)) );

	QVBoxLayout *v = new QVBoxLayout( this );
	v->addWidget( c );
	v->addWidget( b, 0, Qt::AlignRight );
}

void SignatureWidget::link( const QString &url )
{
	if( url == "details" )
		SignatureDialog( s, this ).exec();
	else if( url == "remove" )
		Q_EMIT removeSignature( num );
}



SignatureDialog::SignatureDialog( const DigiDocSignature &signature, QWidget *parent )
:	QDialog( parent )
,	s( signature )
{
	setupUi( this );

	const SslCertificate c = s.cert();
	QString titleText = QString("%1 %2 %3")
		.arg( SslCertificate::formatName( c.subjectInfoUtf8( "GN" ) ) )
		.arg( SslCertificate::formatName( c.subjectInfoUtf8( "SN" ) ) )
		.arg( c.subjectInfo( "serialNumber" ) );
	title->setText( titleText );
	setWindowTitle( titleText );

	if( s.isValid() )
		error->setText( tr("Signature is valid") );
	else
		error->setText( tr("Signature is not valid (%1)")
			.arg( s.lastError().isEmpty() ? tr("Unknown error") : s.lastError() ) );

	const QStringList l = s.locations();
	signerCity->setText( l.value( 0 ) );
	signerState->setText( l.value( 1 ) );
	signerZip->setText( l.value( 2 ) );
	signerCountry->setText( l.value( 3 ) );

	signerRole->setText( s.role() );

	// Certificate info
	addItem( tr("Signing time"), s.dateTime().toString( "dd.MM.yyyy hh:mm:ss" ) );
	addItem( tr("Signature type"), c.publicKey().algorithm() == QSsl::Rsa ? "RSA" : "DSA" );
	addItem( tr("Signature format"), s.mediaType() );
	addItem( tr("Signed file count"), QString::number( s.parent()->documents().size() ) );
	addItem( tr("Certificate serialnumber"), c.serialNumber() );
	addItem( tr("Certificate valid at"), c.effectiveDate().toString( "dd.MM.yyyy" ) );
	addItem( tr("Certificate valid until"), c.expiryDate().toString( "dd.MM.yyyy" ) );
	addItem( tr("Certificate issuer"), c.issuerInfo( QSslCertificate::CommonName ) );
	signatureView->resizeColumnToContents( 0 );

	// OCSP info
}

void SignatureDialog::addItem( const QString &variable, const QString &value )
{
	QTreeWidgetItem *i = new QTreeWidgetItem( signatureView );
	i->setText( 0, variable );
	i->setText( 1, value );
	signatureView->addTopLevelItem( i );
}

void SignatureDialog::showCertificate()
{ CertificateDialog( s.cert(), this ).exec(); }
