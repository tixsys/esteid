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

#include "CertificateWidget.h"
#include "LdapSearch.h"
#include "SslCertificate.h"

#include <QDateTime>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QHeaderView>
#include <QMessageBox>
#include <QProgressBar>
#include <QRegExpValidator>
#include <QTextStream>

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

	addItem( tr("Recipient"), k.recipient );
	addItem( tr("Crypt method"), k.type );
	addItem( tr("ID"), k.id );
	addItem( tr("Expires"), key.cert.expiryDate().toString("dd.MM.yyyy hh:mm:ss") );
	addItem( tr("Issuer"), key.cert.issuerInfo( QSslCertificate::CommonName ) );
	view->resizeColumnToContents( 0 );
}

void KeyDialog::addItem( const QString &parameter, const QString &value )
{
	QTreeWidgetItem *i = new QTreeWidgetItem( view );
	i->setText( 0, parameter );
	i->setText( 1, value );
	view->addTopLevelItem( i );
}

void KeyDialog::showCertificate()
{ (new CertificateWidget( k.cert, this ))->show(); }



KeyAddDialog::KeyAddDialog( QWidget *parent )
:	QWidget( parent )
{
	setupUi( this );
	setAttribute( Qt::WA_DeleteOnClose );
	setWindowFlags( Qt::Dialog );
	connect( addCard, SIGNAL(clicked()), SIGNAL(addCardCert()) );

	skView->header()->setStretchLastSection( false );
	skView->header()->setResizeMode( 0, QHeaderView::Stretch );
	skView->header()->setResizeMode( 1, QHeaderView::ResizeToContents );
	skView->header()->setResizeMode( 2, QHeaderView::ResizeToContents );
	connect( skView, SIGNAL(doubleClicked(QModelIndex)), SLOT(on_add_clicked()) );

	usedView->header()->setStretchLastSection( false );
	usedView->header()->setResizeMode( 0, QHeaderView::Stretch );
	usedView->header()->setResizeMode( 1, QHeaderView::ResizeToContents );
	usedView->header()->setResizeMode( 2, QHeaderView::ResizeToContents );
	loadHistory();

	ldap = new LdapSearch( this );
	connect( ldap, SIGNAL(searchResult(CKey)), SLOT(showResult(CKey)) );
	connect( ldap, SIGNAL(error(QString,int)), SLOT(showError(QString,int)) );

	sscode->setValidator( new QRegExpValidator( QRegExp( "[0-9]{11}" ), this ) );
	sscode->setFocus();
	add->setEnabled( false );
	progress->setVisible( false );
}

void KeyAddDialog::disableSearch( bool disable )
{
	progress->setVisible( disable );
	search->setDisabled( disable );
	skView->setDisabled( disable );
	sscode->setDisabled( disable );
}

void KeyAddDialog::loadHistory()
{
	QFile f( QString( "%1/certhistory.txt" )
		.arg( QDesktopServices::storageLocation( QDesktopServices::DataLocation ) ) );
	if( !f.open( QIODevice::ReadOnly ) )
		return;

	QTextStream s( &f );
	while( true )
	{
		QString line = s.readLine();
		if( line.isEmpty() )
			break;
		QStringList list = line.split( ';' );

		QTreeWidgetItem *i = new QTreeWidgetItem( usedView );
		i->setText( 0, list.value( 0 ) );
		i->setText( 1, list.value( 1 ) );
		i->setText( 2, list.value( 2 ) );
		usedView->addTopLevelItem( i );
	}
	f.close();
}

void KeyAddDialog::on_add_clicked()
{
	if( skKeys.isEmpty() )
		return;

	saveHistory();
	Q_EMIT selected( skKeys );
}

void KeyAddDialog::on_addFile_clicked()
{
	QString file = QFileDialog::getOpenFileName( this, "QDigiDocCrypto",
		QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation ),
		tr("Certificates (*.pem *.cer *.crt)") );
	if( file.isEmpty() )
		return;

	QFile f( file );
	if( !f.open( QIODevice::ReadOnly ) )
	{
		QMessageBox::warning( this, "QDigiDocCrypto", tr("Failed to open certifiacte") );
		return;
	}

	CKey k;
	k.cert = QSslCertificate( &f, QSsl::Pem );
	if( k.cert.isNull() )
		k.cert = QSslCertificate( &f, QSsl::Der );
	if( !k.cert.isNull() )
	{
		k.recipient = SslCertificate( k.cert ).subjectInfoUtf8( QSslCertificate::CommonName );
		Q_EMIT selected( QList<CKey>() << k );
	}
	else
		QMessageBox::warning( this, "QDigiDocCrypto", tr("Failed to read certificate") );

	f.close();
}

void KeyAddDialog::on_search_clicked()
{
	if( sscode->text().size() != 11 )
	{
		QMessageBox::warning( this, "QDigiDocCrypto",
			tr("Social security number is not valid!") );
	}
	else
	{
		skView->clear();
		add->setEnabled( false );
		disableSearch( true );
		ldap->search( QString( "(serialNumber=%1)" ).arg( sscode->text() ) );
	}
}

void KeyAddDialog::on_usedView_itemDoubleClicked( QTreeWidgetItem *item, int )
{
	sscode->setText( item->text( 0 ).split( ',' ).value( 2 ) );
	tabWidget->setCurrentIndex( 0 );
	on_search_clicked();
}

void KeyAddDialog::saveHistory()
{
	Q_FOREACH( const CKey &k, skKeys )
	{
		if( usedView->findItems( k.recipient, Qt::MatchExactly ).isEmpty() )
		{
			QTreeWidgetItem *i = new QTreeWidgetItem( usedView );
			i->setText( 0, k.recipient );
			i->setText( 1, k.cert.issuerInfo( "CN" ) );
			i->setText( 2, k.cert.expiryDate().toString( "dd.MM.yyyy" ) );
			usedView->addTopLevelItem( i );
		}
	}

	QString path = QDesktopServices::storageLocation( QDesktopServices::DataLocation );
	QDir().mkpath( path );
	QFile f( path.append( "/certhistory.txt" ) );
	if( !f.open( QIODevice::WriteOnly|QIODevice::Truncate ) )
		return;

	QTextStream s( &f );
	QTreeWidgetItem *top = usedView->invisibleRootItem();
	for( int i = 0; i < top->childCount(); ++i )
	{
		QTreeWidgetItem *child = top->child( i );
		s << child->text( 0 ) << ';';
		s << child->text( 1 ) << ';';
		s << child->text( 2 ) << '\n';
	}
	f.close();
}

void KeyAddDialog::showError( const QString &msg, int err )
{
	QString s( msg );
	if( err != -1 )
		s.append( tr("\nError code: %1").arg( err ) );
	disableSearch( false );
	QMessageBox::warning( this, "QDigDocCrypto", s );
}

void KeyAddDialog::showResult( const CKey &key )
{
	skKeys.clear();
	skKeys << key;

	QTreeWidgetItem *i = new QTreeWidgetItem( skView );
	i->setText( 0, key.recipient );
	i->setText( 1, key.cert.issuerInfo( QSslCertificate::CommonName ) );
	i->setText( 2, key.cert.expiryDate().toString( "dd.MM.yyyy" ) );
	skView->addTopLevelItem( i );

	add->setEnabled( true );
	disableSearch( false );
}
