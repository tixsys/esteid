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

#include "PinDialog.h"

#include "SslCertificate.h"

#include <QDialogButtonBox>
#include <QPushButton>
#include <QRegExpValidator>

PinDialog::PinDialog( PinType type, const QSslCertificate &cert, QWidget *parent, Qt::WindowFlags flags )
:	QInputDialog( parent, flags )
{
	SslCertificate c = cert;
	QString title = QString( "%1 %2 %3" )
		.arg( SslCertificate::formatName( c.subjectInfoUtf8( "GN" ) ) )
		.arg( SslCertificate::formatName( c.subjectInfoUtf8( "SN" ) ) )
		.arg( c.subjectInfo( "serialNumber" ) );
	setWindowTitle( title );

	if( type == Pin1Type )
	{
		setLabelText( tr("<b>%1</b><br />"
			"Selected action requires auth certificate.<br />"
			"For using auth certificate enter PIN1").arg( title ) );
		regexp.setPattern( "\\d{4,12}" );
	}
	else
	{
		setLabelText( tr("<b>%1</b><br />"
			"Selected action requires sign certificate.<br />"
			"For using sign certificate enter PIN2").arg( title ) );
		regexp.setPattern( "\\d{5,12}" );
	}
	setTextEchoMode( QLineEdit::Password );
	okButtonText();
	QLineEdit *t = findChild<QLineEdit*>();
	if( t )
	{
		t->setValidator( new QRegExpValidator( regexp, t ) );
		connect( t, SIGNAL(textEdited(QString)), SLOT(textEdited(QString)) );
		textEdited( QString() );
	}
}

void PinDialog::textEdited( const QString &text )
{
	QDialogButtonBox *box = findChild<QDialogButtonBox*>();
	if( box )
		box->button( QDialogButtonBox::Ok )->setEnabled( regexp.exactMatch( text ) );
}
