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
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRegExpValidator>
#include <QVBoxLayout>

PinDialog::PinDialog( PinType type, const QSslCertificate &cert, QWidget *parent )
:	QDialog( parent )
{
	QDialogButtonBox *buttons = new QDialogButtonBox(
		QDialogButtonBox::Ok|QDialogButtonBox::Cancel, Qt::Horizontal, this );
	ok = buttons->button( QDialogButtonBox::Ok );
	ok->setAutoDefault( true );
	connect( buttons, SIGNAL(accepted()), SLOT(accept()) );
	connect( buttons, SIGNAL(rejected()), SLOT(reject()) );

	QLabel *label = new QLabel( this );
	m_text = new QLineEdit( this );
	m_text->setEchoMode( QLineEdit::Password );
	m_text->setFocus();
	QVBoxLayout *l = new QVBoxLayout( this );
	l->addWidget( label );
	l->addWidget( m_text );
	l->addWidget( buttons );

	SslCertificate c = cert;
	QString title = QString( "%1 %2 %3" )
		.arg( SslCertificate::formatName( c.subjectInfoUtf8( "GN" ) ) )
		.arg( SslCertificate::formatName( c.subjectInfoUtf8( "SN" ) ) )
		.arg( c.subjectInfo( "serialNumber" ) );
	setWindowTitle( title );

	if( type == Pin1Type )
	{
		label->setText( tr("<b>%1</b><br />"
			"Selected action requires auth certificate.<br />"
			"For using auth certificate enter PIN1").arg( title ) );
		regexp.setPattern( "\\d{4,12}" );
	}
	else
	{
		label->setText( tr("<b>%1</b><br />"
			"Selected action requires sign certificate.<br />"
			"For using sign certificate enter PIN2").arg( title ) );
		regexp.setPattern( "\\d{5,12}" );
	}
	m_text->setValidator( new QRegExpValidator( regexp, m_text ) );
	connect( m_text, SIGNAL(textEdited(QString)), SLOT(textEdited(QString)) );
	textEdited( QString() );
}

QString PinDialog::text() const { return m_text->text(); }

void PinDialog::textEdited( const QString &text )
{ ok->setEnabled( regexp.exactMatch( text ) ); }
