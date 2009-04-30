/*
 * QDigiDocCrypto
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

#include "KeyDialog.h"

#include "LdapSearch.h"

#include <QDateTime>
#include <QDesktopServices>
#include <QDir>
#include <QMessageBox>
#include <QTemporaryFile>
#include <QUrl>

KeyWidget::KeyWidget( const CKey &key, int id, bool encrypted, QWidget *parent )
:	QWidget( parent )
,	m_id( id )
,	m_key( key )
{
	QLabel *c = new QLabel( key.recipient, this );

	QPushButton *b = new QPushButton( tr("Show details"), this );
	connect( b, SIGNAL(clicked()), SLOT(showDetails()) );

	QHBoxLayout *h = new QHBoxLayout();
	h->addItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
	h->addWidget( b );

	if( !encrypted )
	{
		QPushButton *d = new QPushButton( tr("Remove"), this );
		connect( d, SIGNAL(clicked()), SLOT(remove()) );
		h->addWidget( d );
	}

	QVBoxLayout *v = new QVBoxLayout( this );
	v->addWidget( c );
	v->addLayout( h );
}

void KeyWidget::remove() { Q_EMIT remove( m_id ); }
void KeyWidget::showDetails() { (new KeyDialog( m_key, this ))->show(); }


KeyDialog::KeyDialog( const CKey &key, QWidget *parent )
:	QWidget( parent )
,	k( key )
{
	setupUi( this );
	setAttribute( Qt::WA_DeleteOnClose );
	setWindowFlags( Qt::Dialog );

	title->setText( k.recipient );
	title->setStyleSheet( QString( "background-color: green" ) );

	QSslCertificate cert( key.certPem, QSsl::Pem );
	// Recipient
	QTreeWidgetItem *i;
	i = new QTreeWidgetItem( view );
	i->setText( 0, tr("Recipient") );
	i->setText( 1, k.recipient );
	view->addTopLevelItem( i );

	i = new QTreeWidgetItem( view );
	i->setText( 0, tr("Crypt method") );
	i->setText( 1, k.type );
	view->addTopLevelItem( i );

	i = new QTreeWidgetItem( view );
	i->setText( 0, tr("ID") );
	i->setText( 1, k.id );
	view->addTopLevelItem( i );

	i = new QTreeWidgetItem( view );
	i->setText( 0, tr("Expires") );
	i->setText( 1, cert.expiryDate().toString("dd.MM.yyyy hh:mm:ss") );
	view->addTopLevelItem( i );

	i = new QTreeWidgetItem( view );
	i->setText( 0, tr("Issuer") );
	i->setText( 1, cert.issuerInfo( QSslCertificate::CommonName ) );
	view->addTopLevelItem( i );

	view->resizeColumnToContents( 0 );
}

void KeyDialog::showCertificate()
{
	QTemporaryFile cert( QString( "%1%2XXXXXX.cer" )
		.arg( QDir::tempPath() )
		.arg( QDir::separator() ) );
	cert.setAutoRemove( false );
	if( !cert.open() )
		false;
	cert.write( QSslCertificate( k.certPem, QSsl::Pem ).toDer() );
	QString file = cert.fileName();
	cert.close();

	QDesktopServices::openUrl( file.prepend( "file://" ) );
}

KeyAddDialog::KeyAddDialog( QWidget *parent )
:	QWidget( parent )
{
	setupUi( this );
	setAttribute( Qt::WA_DeleteOnClose );
	setWindowFlags( Qt::Dialog );

	skView->header()->setStretchLastSection( false );
	skView->header()->setResizeMode( 0, QHeaderView::Stretch );
	skView->header()->setResizeMode( 1, QHeaderView::ResizeToContents );
	skView->header()->setResizeMode( 1, QHeaderView::ResizeToContents );

	ldap = new LdapSearch( this );
	connect( ldap, SIGNAL(searchResult(CKey)), SLOT(showResult(CKey)) );
	connect( ldap, SIGNAL(error(QString,int)), SLOT(showError(QString,int)) );
}

void KeyAddDialog::on_add_clicked()
{ if( !skKeys.isEmpty() ) Q_EMIT selected( skKeys ); }

void KeyAddDialog::on_search_clicked()
{
	skView->clear();
	ldap->search( QString( "(serialNumber=%1)" ).arg( sscode->text() ) );
}

void KeyAddDialog::showError( const QString &msg, int err )
{
	QString s( msg );
	if( err != -1 )
		s.append( tr("\nError code: %1").arg( err ) );
	QMessageBox::warning( this, "QDigDocCrypto", s );
}

void KeyAddDialog::showResult( const CKey &key )
{
	skKeys.clear();
	skKeys << key;

	QSslCertificate cert( key.certPem, QSsl::Pem );

	QTreeWidgetItem *i = new QTreeWidgetItem( skView );
	i->setText( 0, key.recipient );
	i->setText( 1, cert.issuerInfo( QSslCertificate::CommonName ) );
	i->setText( 2, cert.effectiveDate().toString( "dd.MM.yyyy" ) );
	skView->addTopLevelItem( i );
}
