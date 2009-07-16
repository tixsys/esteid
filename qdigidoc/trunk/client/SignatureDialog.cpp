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
{
	const SslCertificate cert = s.cert();
	QString content;
	QTextStream st( &content );

	if( cert.isTempel() )
		st << "<img src=\":/images/ico_stamp_blue_16.png\">";
	else
		st << "<img src=\":/images/ico_person_blue_16.png\">";

	st << "<b>" << SslCertificate::formatName( cert.subjectInfoUtf8( "GN" ) ) << " "
		<< SslCertificate::formatName( cert.subjectInfoUtf8( "SN" ) ) << "</b>";

	if( extended )
	{
		st << "<br />" << s.location() << "<br />";
		st << tr("Signed on") << " " << s.dateTime().toString( "dd. MMMM yyyy kell hh:mm" );
	}
	else
	{
		QString tooltip;
		QTextStream t( &tooltip );
		QString location = s.location();
		if( !location.isEmpty() )
			t << location << "<br />";
		QString role = s.role();
		if( !role.isEmpty() )
			t << role << "<br />";
		t << tr("Signed on") << " " << s.dateTime().toString( "dd. MMMM yyyy kell hh:mm" );
		setToolTip( tooltip );
	}

	st << "<table width=\"100%\" cellpadding=\"0\" cellspacing=\"0\"><tr>";
	st << "<td>" << tr("Signature is") << " ";
	if( s.isValid() )
		st << "<font color=\"green\">" << tr("valid") << "</font>";
	else
		st << "<font color=\"red\">" << tr("not valid") << "</font>";
	st << "</td><td align=\"right\">";
	st << "<a href=\"details\">" << tr("Show details") << "</a>";
	st << "</td></tr><tr><td></td>";
	st << "<td align=\"right\">";
	st << "<a href=\"remove\">" << tr("Remove") << "</a>";
	st << "</td></tr></table>";

	setText( content );

	connect( this, SIGNAL(linkActivated(QString)), SLOT(link(QString)) );
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
	tabWidget->removeTab( 2 );

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
	t = ocspView;
	addItem( t, tr("Signature type"), "" );
	addItem( t, tr("Type"), s.digestMethod() );
	addItem( t, tr("Certificate serialnumber"), "" );
	addItem( t, tr("Time"), "" );
	addItem( t, tr("SHA1 hash"), "" );
	addItem( t, tr("Subject name"), "" );
	t->resizeColumnToContents( 0 );
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
