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

#include "SignatureDialog.h"

#include <common/CertificateWidget.h>
#include <common/SslCertificate.h>

#include <digidocpp/Document.h>

#include <QDateTime>
#include <QSslKey>
#include <QTextStream>

SignatureWidget::SignatureWidget( const DigiDocSignature &signature, unsigned int signnum, bool extended, QWidget *parent )
:	QLabel( parent )
,	num( signnum )
,	s( signature )
,	test( false )
,	valid( false )
{
	setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Preferred );
	setWordWrap( true );
	const SslCertificate cert = s.cert();
	QString content;
	QTextStream st( &content );

	if( cert.isTempel() )
		st << "<img src=\":/images/ico_stamp_blue_16.png\">";
	else
		st << "<img src=\":/images/ico_person_blue_16.png\">";

	st << "<b>" << cert.toString( cert.isTempel() ? "CN" : "GN SN" ) << "</b>";

	if( extended )
	{
		if( !s.location().isEmpty() )
			st << "<br />" << s.location();
		if( !s.role().isEmpty() )
			st << "<br />" << s.role();
		st << "<br />" << tr("Signed on") << " "
			<< SslCertificate::formatDate( s.dateTime(), "dd. MMMM yyyy" ) << " "
			<< tr("time") << " "
			<< s.dateTime().toString( "hh:mm" );
	}
	else
	{
		QString tooltip;
		QTextStream t( &tooltip );
		if( !s.location().isEmpty() )
			t << s.location() << "<br />";
		if( !s.role().isEmpty() )
			t << s.role() << "<br />";
		t << tr("Signed on") << " "
			<< SslCertificate::formatDate( s.dateTime(), "dd. MMMM yyyy" ) << " "
			<< tr("time") << " "
			<< s.dateTime().toString( "hh:mm" );
		setToolTip( tooltip );
	}

	st << "<table width=\"100%\" cellpadding=\"0\" cellspacing=\"0\"><tr>";
	st << "<td>" << tr("Signature is") << " ";
	switch( s.validate() )
	{
	case DigiDocSignature::Valid: st << "<font color=\"green\">" << tr("valid"); valid = true; break;
	case DigiDocSignature::Invalid: st << "<font color=\"red\">" << tr("not valid"); break;
	case DigiDocSignature::Unknown: st << "<font color=\"red\">" << tr("unknown"); break;
	}
	if( (test = cert.isTest()) )
		st << " (" << tr("Test signature") << ")";
	st << "</font>";
	st << "</td><td align=\"right\">";
	st << "<a href=\"details\">" << tr("Show details") << "</a>";
	st << "</td></tr><tr><td></td>";
	st << "<td align=\"right\">";
	st << "<a href=\"remove\">" << tr("Remove") << "</a>";
	st << "</td></tr></table>";

	setText( content );

	connect( this, SIGNAL(linkActivated(QString)), SLOT(link(QString)) );
}

bool SignatureWidget::isTest() const { return test; }
bool SignatureWidget::isValid() const { return valid; }

void SignatureWidget::link( const QString &url )
{
	if( url == "details" )
		SignatureDialog( s, qobject_cast<QWidget*>(parent()) ).exec();
	else if( url == "remove" )
		Q_EMIT removeSignature( num );
}



SignatureDialog::SignatureDialog( const DigiDocSignature &signature, QWidget *parent )
:	QDialog( parent )
,	s( signature )
{
	setupUi( this );

	const SslCertificate c = s.cert();
	QString titleText = c.toString( c.isTempel() ? "CN serialNumber" : "GN SN serialNumber" );
	title->setText( titleText );
	setWindowTitle( titleText );

	switch( s.validate() )
	{
	case DigiDocSignature::Valid: error->setText( tr("Signature is valid") ); break;
	case DigiDocSignature::Invalid:
		error->setText( tr("Signature is not valid (%1)")
			.arg( s.lastError().isEmpty() ? tr("Unknown error") : s.lastError() ) );
		break;
	case DigiDocSignature::Unknown:
		error->setText( tr("Signature status unknown (%1)")
			.arg( s.lastError().isEmpty() ? tr("Unknown error") : s.lastError() ) );
		break;
	}

	const QStringList l = s.locations();
	signerCity->setText( l.value( 0 ) );
	signerState->setText( l.value( 1 ) );
	signerZip->setText( l.value( 2 ) );
	signerCountry->setText( l.value( 3 ) );

	QStringList roles = s.roles();
	signerRole->setText( roles.value(0) );
	signerResolution->setText( roles.value(1) );

	// Certificate info
	QTreeWidget *t = signatureView;
	addItem( t, tr("Signing time"), s.dateTime().toString( "dd.MM.yyyy hh:mm:ss" ) );
	addItem( t, tr("Signature type"), c.publicKey().algorithm() == QSsl::Rsa ? "RSA" : "DSA" );
	addItem( t, tr("Signature format"), s.mediaType() );
	addItem( t, tr("Signed file count"), QString::number( s.parent()->documents().size() ) );
	addItem( t, tr("Certificate serialnumber"), c.serialNumber() );
	addItem( t, tr("Certificate valid at"), SslCertificate::toLocalTime( c.effectiveDate() ).toString( "dd.MM.yyyy" ) );
	addItem( t, tr("Certificate valid until"), SslCertificate::toLocalTime( c.expiryDate() ).toString( "dd.MM.yyyy" ) );
	addItem( t, tr("Certificate issuer"), c.issuerInfo( QSslCertificate::CommonName ) );
	t->resizeColumnToContents( 0 );

	// OCSP info
	if( s.type() == DigiDocSignature::DDocType ||
		s.type() == DigiDocSignature::TMType )
	{
		SslCertificate ocsp = s.ocspCert();
		addItem( ocspView, tr("Certificate issuer"), ocsp.issuerInfo( QSslCertificate::CommonName ) );
		addItem( ocspView, tr("Certificate serialnumber"), ocsp.serialNumber() );
		addItem( ocspView, tr("Time"), s.dateTime().toString( "dd.MM.yyyy hh:mm:ss" ) );
		addItem( ocspView, tr("Hash value of validity confirmation"), ocsp.toHex( s.digestValue() ) );
		ocspView->resizeColumnToContents( 0 );
	}
	else
		tabWidget->removeTab( 2 );
}

void SignatureDialog::addItem( QTreeWidget *view, const QString &variable, const QString &value )
{
	QTreeWidgetItem *i = new QTreeWidgetItem( view );
	i->setText( 0, variable );
	i->setText( 1, value );
	view->addTopLevelItem( i );
}

void SignatureDialog::showCertificate()
{ CertificateDialog( s.cert(), this ).exec(); }

void SignatureDialog::showOCSPCertificate()
{ CertificateDialog( s.ocspCert(), this ).exec(); }
