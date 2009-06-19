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

#include "MainWindow.h"

#include "KeyDialog.h"
#include "Settings.h"
#include "common/Common.h"
#include "common/PinDialog.h"
#include "common/SslCertificate.h"

#include <QApplication>
#include <QDateTime>
#include <QDesktopServices>
#include <QDragEnterEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QTranslator>
#include <QUrl>

MainWindow::MainWindow( QWidget *parent )
:	QWidget( parent )
{
	qRegisterMetaType<QSslCertificate>("QSslCertificate");

	setupUi( this );
	setWindowFlags( Qt::Window | Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint );
#if QT_VERSION >= 0x040500
	setWindowFlags( windowFlags() | Qt::WindowCloseButtonHint );
#else
	setWindowFlags( windowFlags() | Qt::WindowSystemMenuHint );
#endif

	Common *common = new Common( this );
	QDesktopServices::setUrlHandler( "mailto", common, "mailTo" );

	cards->hide();
	cards->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
	connect( cards, SIGNAL(activated(QString)), SLOT(selectCard()) );
	languages->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );

	QButtonGroup *buttonGroup = new QButtonGroup( this );

	buttonGroup->addButton( settings, HeadSettings );
	buttonGroup->addButton( help, HeadHelp );

	buttonGroup->addButton( homeCreate, HomeCreate );
	buttonGroup->addButton( homeView, HomeView );

	introNext = introButtons->addButton( tr( "Next" ), QDialogButtonBox::ActionRole );
	buttonGroup->addButton( introNext, IntroNext );
	buttonGroup->addButton( introButtons->button( QDialogButtonBox::Cancel ), IntroBack );

	viewCrypt = viewButtons->addButton( tr("Encrypt"), QDialogButtonBox::ActionRole );
	buttonGroup->addButton( viewCrypt, ViewCrypt );
	buttonGroup->addButton( viewButtons->button( QDialogButtonBox::Close ), ViewClose );
	connect( buttonGroup, SIGNAL(buttonClicked(int)),
		SLOT(buttonClicked(int)) );

	connect( viewLinks, SIGNAL(linkActivated(QString)), SLOT(parseLink(QString)) );
	connect( viewContentLinks, SIGNAL(linkActivated(QString)), SLOT(parseLink(QString)) );
	connect( viewKeysLinks, SIGNAL(linkActivated(QString)), SLOT(parseLink(QString)) );

	appTranslator = new QTranslator( this );
	commonTranslator = new QTranslator( this );
	qtTranslator = new QTranslator( this );
	QApplication::instance()->installTranslator( appTranslator );
	QApplication::instance()->installTranslator( commonTranslator );
	QApplication::instance()->installTranslator( qtTranslator );

	doc = new CryptDoc( this );
	connect( cards, SIGNAL(activated(QString)), doc, SLOT(selectCard(QString)), Qt::QueuedConnection );
	connect( doc, SIGNAL(error(QString,int,QString)), SLOT(showWarning(QString,int,QString)) );
	connect( doc, SIGNAL(dataChanged()), SLOT(showCardStatus()) );

	lang[0] = "et";
	lang[1] = "en";
	lang[2] = "ru";
	on_languages_activated( lang.key(
		SettingsValues().value( "Main/Language", "et" ).toString() ) );

	QStringList args = qApp->arguments();
	if( args.size() > 1 )
	{
		args.removeAt( 0 );
		params = args;
		buttonClicked( HomeCreate );
	}
}

void MainWindow::addCardCert()
{
	doc->addCardCert();
	setCurrentPage( View );
}

bool MainWindow::addFile( const QString &file )
{
	if( doc->isNull() )
	{
		QFileInfo info( file );
		QString docname = QString( "%1/%3.cdoc" )
			.arg( SettingsValues().value( "Main/DefaultDir", info.absolutePath() ).toString() )
			.arg( info.fileName() );

		bool ask = SettingsValues().value( "Main/AskSaveAs", false ).toBool();
		bool select = false;
		if( !ask && QFile::exists( docname ) )
		{
			QMessageBox::StandardButton b = QMessageBox::warning( this, windowTitle(),
				tr( "%1 already exists.<br />Do you want replace it?" ).arg( docname ),
				QMessageBox::Yes | QMessageBox::No, QMessageBox::No );
			select = b == QMessageBox::No;
		}

		if( ask || select )
		{
			docname = QFileDialog::getSaveFileName(
				this, tr("Save file"), docname, tr("Documents (*.cdoc)") );
			if( docname.isEmpty() )
				return false;
		}

		if( QFile::exists( docname ) )
			QFile::remove( docname );
		doc->create( docname );
	}

	// Check if file exist and ask confirmation to overwrite
	QList<CDocument> docs = doc->documents();
	for( int i = 0; i < docs.size(); ++i )
	{
		if( QFileInfo( docs[i].filename ).fileName() ==
			QFileInfo( file ).fileName() )
		{
			QMessageBox::StandardButton btn = QMessageBox::warning( this,
				windowTitle(), tr("Container contains file with same name, ovewrite?"),
				QMessageBox::Yes | QMessageBox::No, QMessageBox::No );
			if( btn == QMessageBox::Yes )
			{
				doc->removeDocument( i );
				break;
			}
			else
				return true;
		}
	}

	doc->addFile( file, "" );
	return true;
}

void MainWindow::addKeys( const QList<CKey> &keys )
{
	if( keys.isEmpty() )
		return;
	Q_FOREACH( const CKey &key, keys )
		doc->addKey( key );
	setCurrentPage( View );
}

void MainWindow::buttonClicked( int button )
{
	switch( button )
	{
	case HeadSettings:
		Settings( this ).exec();
		break;
	case HeadHelp:
		QDesktopServices::openUrl( QUrl( "http://support.sk.ee/" ) );
		break;
	case HomeView:
	{
		QString file = QFileDialog::getOpenFileName( this, tr("Open container"),
			QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation ),
			tr("Documents (*.cdoc)") );
		if( !file.isEmpty() && doc->open( file ) )
			setCurrentPage( View );
		break;
	}
	case HomeCreate:
		if( SettingsValues().showIntro() )
		{
			introCheck->setChecked( false );
			introNext->setEnabled( false );
			setCurrentPage( Intro );
			break;
		}
	case IntroNext:
	{
		if( !params.isEmpty() )
		{
			parseParams();
			break;
		}
		QStringList list = QFileDialog::getOpenFileNames( this, tr("Select documents"),
			QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation ) );
		if( !list.isEmpty() )
		{
			Q_FOREACH( const QString &file, list )
			{
				if( !addFile( file ) )
					return;
			}
			setCurrentPage( View );
		}
		else if( doc->isNull() )
			setCurrentPage( Home );
		break;
	}
	case IntroBack:
	case ViewClose:
		doc->clear();
		setCurrentPage( Home );
		break;
	case ViewCrypt:
		if( doc->isEncrypted() )
		{
			PinDialog p( PinDialog::Pin1Type, doc->authCert(), this );
			if( !p.exec() )
				break;

			setEnabled( false );
			QLabel *progress = new QLabel( tr("Decrypting"), view );
			progress->setAlignment( Qt::AlignCenter );
			progress->setFixedSize( 300, 20 );
			progress->setStyleSheet( "font: bold; border: 1px solid black; background-color: white;" );
			progress->move( view->geometry().center() - progress->geometry().center() );
			progress->show();
			QApplication::processEvents();

			doc->decrypt( p.textValue() );

			progress->deleteLater();
			setEnabled( true );

			if( doc->isSigned() )
			{
				QMessageBox::StandardButton b = QMessageBox::warning( this, windowTitle(),
					tr("This container contains signature! Open with QDigiDocClient?"),
					QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes );
				if( b != QMessageBox::Yes )
					break;
				QString file = QString( doc->fileName() ).append( ".ddoc" );
				if( doc->saveDDoc( file ) )
				{
					if( !Common::startDetached( "qdigidocclient", QStringList() << file ) )
						showWarning( tr("Failed to start process '%1'").arg( "qdigidocclient" ), -1 );
				}
			}
		}
		else
		{
			if( doc->encrypt() )
				doc->save();
		}
		setCurrentPage( View );
		break;
	default: break;
	}
}

void MainWindow::dragEnterEvent( QDragEnterEvent *e )
{
	if( e->mimeData()->hasUrls() && !doc->isEncrypted() )
		e->acceptProposedAction();
}

void MainWindow::dropEvent( QDropEvent *e )
{
	Q_FOREACH( const QUrl &u, e->mimeData()->urls() )
	{
		if( u.isRelative() || u.scheme() == "file" )
			params << Common::toPath( u );
	}
	buttonClicked( HomeCreate );
}

void MainWindow::on_introCheck_stateChanged( int state )
{
	SettingsValues().setValue( "Main/Intro", state == Qt::Unchecked );
	introNext->setEnabled( state == Qt::Checked );
}

void MainWindow::on_languages_activated( int index )
{
	SettingsValues().setValue( "Main/Language", lang[index] );
	appTranslator->load( ":/translations/" + lang[index] );
	commonTranslator->load( ":/translations/common_" + lang[index] );
	qtTranslator->load( ":/translations/qt_" + lang[index] );
	retranslateUi( this );
	languages->setCurrentIndex( index );
	introNext->setText( tr( "Next" ) );
	showCardStatus();
	setCurrentPage( (Pages)stack->currentIndex() );
}

void MainWindow::on_viewContentView_clicked( const QModelIndex &index )
{
	QList<CDocument> list = doc->documents();
	if( list.isEmpty() || index.row() >= list.size() )
		return;

	switch( index.column() )
	{
	case 2:
	{
		QString dir = QFileDialog::getExistingDirectory( this,
			tr("Select folder where file will be stored"),
			QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation ) );
		if( !dir.isEmpty() )
			doc->saveDocument( index.row(), dir );
		break;
	}
	case 3:
		doc->removeDocument( index.row() );
		setCurrentPage( (Pages)stack->currentIndex() );
		break;
	default: break;
	}
}

void MainWindow::parseLink( const QString &url )
{
	if( url == "addFile" )
	{
		QStringList list = QFileDialog::getOpenFileNames( this, tr("Select documents"),
			QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation ) );
		if( list.isEmpty() )
			return;
		Q_FOREACH( const QString &file, list )
			addFile( file );
		setCurrentPage( View );
	}
	else if( url == "addRecipient" )
	{
		if( doc->isEncrypted() )
			return;

		KeyAddDialog *key = new KeyAddDialog( this );
		connect( key, SIGNAL(addCardCert()), SLOT(addCardCert()) );
		connect( key, SIGNAL(selected(QList<CKey>)), SLOT(addKeys(QList<CKey>)) );
		key->move( pos() );
		key->show();
	}
	else if( url == "browse" )
	{
		QDesktopServices::openUrl( Common::toUrl( QFileInfo( doc->fileName() ).absolutePath() ) );
	}
	else if( url == "email" )
	{
		QDesktopServices::openUrl( QString( "mailto:?subject=%1&attachment=%2" )
			.arg( QFileInfo( doc->fileName() ).fileName() )
			.arg( doc->fileName() ) );
	}
	else if( url == "saveAll" )
	{
		QString dir = QFileDialog::getExistingDirectory( this,
			tr("Select folder where files will be stored"),
			QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation ) );
		if( dir.isEmpty() )
			return;
		QAbstractItemModel *m = viewContentView->model();
		for( int i = 0; i < m->rowCount(); ++i )
			doc->saveDocument( i, dir );
	}
	else if( url == "openUtility" )
	{
		if( !Common::startDetached( "qesteidutil" ) )
			showWarning( tr("Failed to start process '%1'").arg( "qesteidutil" ), -1 );
	}
}

void MainWindow::parseParams()
{
	Q_FOREACH( const QString &param, params )
	{
		const QFileInfo f( param );
		if( !f.isFile() )
			continue;
		if( doc->isNull() && f.suffix().toLower() == "cdoc" )
		{
			doc->open( f.absoluteFilePath() );
			break;
		}
		else if( !addFile( f.absoluteFilePath() ) )
			break;
	}
	if( !doc->isNull() )
		setCurrentPage( View );
	params.clear();
}

void MainWindow::removeKey( int id )
{
	doc->removeKey( id );
	setCurrentPage( View );
}

void MainWindow::selectCard()
{
	infoCard->setText( tr("Loading data") );
	infoLogo->setText( QString() );
}

void MainWindow::setCurrentPage( Pages page )
{
	stack->setCurrentIndex( page );
	switch( page )
	{
	case View:
	{
		viewFileName->setText( tr("Container: <b>%1</b>")
			.arg( QDir::toNativeSeparators( doc->fileName() ) ) );

		viewLinks->setVisible( doc->isEncrypted() );
		viewContentLinks->setHidden( doc->isEncrypted() );
		viewKeysLinks->setHidden( doc->isEncrypted() );

		viewContentView->setColumnHidden( 2, doc->isEncrypted() );
		viewContentView->setColumnHidden( 3, doc->isEncrypted() );
		viewContentView->setContent( doc->documents() );

		Q_FOREACH( KeyWidget *w, viewKeys->findChildren<KeyWidget*>() )
			w->deleteLater();

		int i = 0;
		bool hasKey = false;
		Q_FOREACH( const CKey &k, doc->keys() )
		{
			KeyWidget *key = new KeyWidget( k, i, doc->isEncrypted(), viewKeys );
			connect( key, SIGNAL(remove(int)), SLOT(removeKey(int)) );
			viewKeysLayout->insertWidget( i, key );
			hasKey = (k.cert == doc->authCert());
			++i;
		}

		viewCrypt->setText( doc->isEncrypted() ? tr("Decrypt") : tr("Encrypt") );
		viewCrypt->setEnabled( !doc->isEncrypted() || hasKey );
		break;
	}
	default: break;
	}
}

void MainWindow::showCardStatus()
{
	infoLogo->setText( QString() );
	QString content;
	if( !doc->activeCard().isEmpty() && !doc->authCert().isNull() )
	{
		const SslCertificate c = doc->authCert();
		QTextStream s( &content );

		if( c.isTempel() )
		{
			s << tr("Company") << ": <font color=\"black\">"
				<< SslCertificate::formatName( c.subjectInfoUtf8( "CN" ) ) << "</font><br />";
			s << tr("Register code") << ": <font color=\"black\">"
				<< c.subjectInfo( "serialNumber" ) << "</font><br />";
		}
		else
		{
			s << tr("Name") << ": <font color=\"black\">"
				<< SslCertificate::formatName( c.subjectInfoUtf8( "GN" ) ) << " "
				<< SslCertificate::formatName( c.subjectInfoUtf8( "SN" ) ) << "</font><br />";
			s << tr("Personal code") << ": <font color=\"black\">"
				<< c.subjectInfo( "serialNumber" ) << "</font><br />";
		}
		s << tr("Card in reader") << ": <font color=\"black\">"
			<< doc->activeCard() << "</font><br />";

		bool willExpire = c.expiryDate() <= QDateTime::currentDateTime().addDays( 100 );
		s << tr("Auth certificate is") << " ";
		if( doc->authCert().isValid()  )
		{
			s << "<font color=\"green\">" << tr("valid") << "</font>";
			if( willExpire )
				s << "<br /><font color=\"red\">" << tr("Your certificates will be expire") << "</font>";
		}
		else
			s << "<font color=\"red\">" << tr("expired") << "</font>";

		if( !c.isValid() || willExpire )
		{
			infoLogo->setText( QString(
				"<p align=\"center\"><a href=\"openUtility\">"
				"<img src=\":/images/warning.png\"><br />"
				"<font color=\"red\">%1</font></a></p>" ).arg( "Open utility" ) );
		}
		else if( c.isTempel() )
			infoLogo->setText( "<img src=\":/images/ico_stamp_blue_75.png\">" );
		else
			infoLogo->setText( "<img src=\":/images/ico_person_blue_75.png\">" );
	}
	else if( doc->activeCard().isEmpty() )
		content += tr("No card in reader");
	infoCard->setText( content );

	bool hasKey = false;
	if( !doc->authCert().isNull() )
	{
		Q_FOREACH( const CKey &key, doc->keys() )
		{
			hasKey = (key.cert == doc->authCert());
			break;
		}
	}
	viewCrypt->setEnabled( !doc->isEncrypted() || hasKey );

	cards->clear();
	cards->addItems( doc->presentCards() );
	cards->setVisible( doc->presentCards().size() > 1 );
	cards->setCurrentIndex( cards->findText( doc->activeCard() ) );
}

void MainWindow::showWarning( const QString &msg, int err, const QString &errmsg )
{
	QString s( msg );
	if( err != -1 )
		s += tr("<br />Error code: %1").arg( err );
	if( !errmsg.isEmpty() )
		s += QString(" (%1)").arg( errmsg );
	QMessageBox d( QMessageBox::Warning, windowTitle(), s, QMessageBox::Close | QMessageBox::Help, this );
	if( d.exec() == QMessageBox::Help )
	{
		QUrl u( "http://support.sk.ee/" );
		u.addQueryItem( "searchquery", msg );
		u.addQueryItem( "searchtype", "all" );
		u.addQueryItem( "_m", "core" );
		u.addQueryItem( "_a", "searchclient" );
		QDesktopServices::openUrl( u );
	}
}
