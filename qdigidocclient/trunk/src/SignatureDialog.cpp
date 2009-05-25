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

#include "CertificateWidget.h"
#include "SslCertificate.h"

#include <digidoc/Document.h>

#include <QDateTime>
#include <QSslKey>

SignatureWidget::SignatureWidget( const DigiDocSignature &signature, unsigned int signnum, QWidget *parent )
:	QWidget( parent )
,	num( signnum )
,	s( signature )
{
	const SslCertificate cert = s.cert();
	QLabel *c = new QLabel( this );
	c->setText( tr("<b>%1 %2</b><br />%3<br />Signed on %4<br />Signature is %5")
		.arg( SslCertificate::formatName( cert.subjectInfoUtf8( "GN" ) ) )
		.arg( SslCertificate::formatName( cert.subjectInfoUtf8( "SN" ) ) )
		.arg( s.location() )
		.arg( s.dateTime().toString( "dd. MMMM yyyy kell hh:mm" ) )
		.arg( s.isValid() ? tr("valid") : tr("not valid") ) );

	QPushButton *b = new QPushButton( tr("Show details"), this );
	connect( b, SIGNAL(clicked()), SLOT(showSignature()) );

	QPushButton *r = new QPushButton( tr("Remove"), this );
	connect( r, SIGNAL(clicked()), SLOT(removeSignature()) );

	QHBoxLayout *h = new QHBoxLayout();
	h->addItem( new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
	h->addWidget( b );
	h->addWidget( r );

	QVBoxLayout *v = new QVBoxLayout( this );
	v->addWidget( c );
	v->addLayout( h );
}

void SignatureWidget::removeSignature() { Q_EMIT removeSignature( num ); }
void SignatureWidget::showSignature() { SignatureDialog( s, this ).exec(); }



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
	title->setStyleSheet( QString( "background-color: %1" ).arg( s.isValid() ? "green" : "red" ) );
	error->setVisible( !s.lastError().isEmpty() );
	error->setText( s.lastError() );

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
