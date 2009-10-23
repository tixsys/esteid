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
#include "SettingsDialog.h"
#include "common/Common.h"
#include "common/PinDialog.h"
#include "common/Settings.h"

#include <QApplication>
#include <QDateTime>
#include <QDesktopServices>
#include <QDragEnterEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QSslCertificate>
#include <QTextStream>
#include <QTranslator>
#include <QUrl>

#if defined(Q_OS_MAC)
#include <QMenu>
#include <QMenuBar>
#endif

MainWindow::MainWindow( QWidget *parent )
:	QWidget( parent )
{
	qRegisterMetaType<QSslCertificate>("QSslCertificate");

	setupUi( this );
	cards->hide();

	setWindowFlags( Qt::Window | Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint );
#if QT_VERSION >= 0x040500
	setWindowFlags( windowFlags() | Qt::WindowCloseButtonHint );
#else
	setWindowFlags( windowFlags() | Qt::WindowSystemMenuHint );
#endif

	QApplication::instance()->installEventFilter( this );

	Common *common = new Common( this );
	QDesktopServices::setUrlHandler( "browse", common, "browse" );
	QDesktopServices::setUrlHandler( "mailto", common, "mailTo" );

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

	appTranslator = new QTranslator( this );
	commonTranslator = new QTranslator( this );
	qtTranslator = new QTranslator( this );
	QApplication::instance()->installTranslator( appTranslator );
	QApplication::instance()->installTranslator( commonTranslator );
	QApplication::instance()->installTranslator( qtTranslator );

	doc = new CryptoDoc( this );
	connect( cards, SIGNAL(activated(QString)), doc, SLOT(selectCard(QString)) );
	connect( doc, SIGNAL(error(QString,int,QString)), SLOT(showWarning(QString,int,QString)) );
	connect( doc, SIGNAL(dataChanged()), SLOT(showCardStatus()) );

	connect( viewContentView, SIGNAL(remove(int)),
		SLOT(removeDocument(int)) );
	connect( viewContentView, SIGNAL(save(int,QString)),
		doc, SLOT(saveDocument(int,QString)) );

	cards->hack();
	languages->hack();
	lang << "et" << "en" << "ru";
	on_languages_activated( lang.indexOf(
		Settings().value( "Main/Language", "et" ).toString() ) );

	close = new QAction( tr("Close"), this );
	close->setShortcut( Qt::CTRL + Qt::Key_W );
	connect( close, SIGNAL(triggered()), this, SLOT(closeDoc()) );
	addAction( close );
#if defined(Q_OS_MAC)
	QMenuBar *bar = new QMenuBar;
	QMenu *menu = bar->addMenu( tr("&File") );
	QAction *pref = menu->addAction( tr("Settings"), this, SLOT(showSettings()) );
	pref->setMenuRole( QAction::PreferencesRole );
	menu->addAction( close );
#endif

	QStringList args = qApp->arguments();
	if( args.size() > 1 )
	{
		args.removeAt( 0 );
		params = args;
		buttonClicked( HomeCreate );
	}
}

bool MainWindow::addFile( const QString &file )
{
	if( doc->isNull() )
	{
		Settings s;
		s.beginGroup( "Crypto" );
		QFileInfo info( file );
		QString docname = QString( "%1/%2.cdoc" )
			.arg( s.value( "DefaultDir", info.absolutePath() ).toString() )
			.arg( info.fileName() );

		bool select = s.value( "AskSaveAs", false ).toBool();
		if( !select && QFile::exists( docname ) )
		{
			QMessageBox::StandardButton b = QMessageBox::warning( this, windowTitle(),
				tr( "%1 already exists.<br />Do you want replace it?" ).arg( docname ),
				QMessageBox::Yes | QMessageBox::No, QMessageBox::No );
			select = b == QMessageBox::No;
		}

		if( !Common::canWrite( docname ) )
		{
			select = true;
			showWarning( tr("You dont have permissions to write file %1").arg( docname ) );
		}

		while( select )
		{
			docname = QFileDialog::getSaveFileName(
				this, tr("Save file"), docname, tr("Documents (*.cdoc)") );
			if( docname.isEmpty() )
				return false;
			if( QFileInfo( docname ).suffix().toLower() != "cdoc" )
				docname.append( ".cdoc" );
			if( !Common::canWrite( docname ) )
				showWarning( tr("You dont have permissions to write file %1").arg( docname ) );
			else
				select = false;
		}

		if( QFile::exists( docname ) )
			QFile::remove( docname );
		doc->create( docname );
	}

	if( !QFile::exists( file ) )
	{
		showWarning( tr("File does not exists %1").arg( file ) );
		return false;
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

void MainWindow::buttonClicked( int button )
{
	switch( button )
	{
	case HeadSettings:
		showSettings();
		break;
	case HeadHelp:
		QDesktopServices::openUrl( QUrl( "http://support.sk.ee/" ) );
		break;
	case HomeView:
	{
		QString file = QFileDialog::getOpenFileName( this, tr("Open container"),
			QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation ),
			tr("Documents (*.cdoc *.CDOC)") );
		if( !file.isEmpty() && doc->open( file ) )
			setCurrentPage( View );
		break;
	}
	case HomeCreate:
		if( Settings().value( "Crypto/Intro", true ).toBool() )
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
			params.clear();
			if( !doc->isNull() )
			{
				setCurrentPage( View );
				break;
			}
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

			QLabel *progress = new QLabel( tr("Decrypting"), view );
			progress->setAlignment( Qt::AlignCenter );
			progress->setFixedSize( 300, 20 );
			progress->setStyleSheet( "font: bold; border: 1px solid black; background-color: white;" );
			progress->move( view->geometry().center() - progress->geometry().center() );
			progress->show();
			QApplication::processEvents();

			doc->decrypt( p.text() );

			progress->deleteLater();

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

void MainWindow::closeDoc()
{
	if( SettingsDialog *d = findChild<SettingsDialog*>() )
		d->reject();
	else
		buttonClicked( ViewClose );
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
		if( u.scheme() == "file" )
			params << u.toLocalFile();
	}
	buttonClicked( HomeCreate );
}

bool MainWindow::eventFilter( QObject *o, QEvent *e )
{
	if( e->type() == QEvent::FileOpen )
	{
		QFileOpenEvent *o = static_cast<QFileOpenEvent*>(e);
		params << o->file();
		buttonClicked( HomeCreate );
		return true;
	}
	else
		return QWidget::eventFilter( o, e );
}

void MainWindow::on_introCheck_stateChanged( int state )
{
	Settings().setValue( "Crypto/Intro", state == Qt::Unchecked );
	introNext->setEnabled( state == Qt::Checked );
}

void MainWindow::on_languages_activated( int index )
{
	Settings().setValue( "Main/Language", lang[index] );
	appTranslator->load( ":/translations/" + lang[index] );
	commonTranslator->load( ":/translations/common_" + lang[index] );
	qtTranslator->load( ":/translations/qt_" + lang[index] );
	retranslateUi( this );
	languages->setCurrentIndex( index );
	introNext->setText( tr( "Next" ) );
	showCardStatus();
	updateView();
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

		KeyAddDialog *key = new KeyAddDialog( doc, this );
		connect( key, SIGNAL(updateView()), SLOT(updateView()) );
		key->move( pos() );
		key->show();
	}
	else if( url == "browse" )
	{
		QUrl url = QUrl::fromLocalFile( doc->fileName() );
		url.setScheme( "browse" );
		QDesktopServices::openUrl( url );
	}
	else if( url == "email" )
	{
		QUrl url;
		url.setScheme( "mailto" );
		url.addQueryItem( "subject", QFileInfo( doc->fileName() ).fileName() );
		url.addQueryItem( "attachment", doc->fileName() );
		QDesktopServices::openUrl( url );
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
		{
			QString file = QString( "%1/%2" )
				.arg( dir ).arg( m->index( i, 0 ).data().toString() );
			if( QFile::exists( file ) )
			{
				QMessageBox::StandardButton b = QMessageBox::warning( this, windowTitle(),
					tr( "%1 already exists.<br />Do you want replace it?" ).arg( file ),
					QMessageBox::Yes | QMessageBox::No, QMessageBox::No );
				if( b == QMessageBox::No )
				{
					file = QFileDialog::getSaveFileName( this, tr("Save file"), file );
					if( file.isEmpty() )
						continue;
				}
			}
			doc->saveDocument( i, file );
		}
	}
	else if( url == "openUtility" )
	{
		if( !Common::startDetached( "qesteidutil" ) )
			showWarning( tr("Failed to start process '%1'").arg( "qesteidutil" ), -1 );
	}
}

void MainWindow::removeDocument( int index )
{
	doc->removeDocument( index );
	setCurrentPage( View );
}

void MainWindow::removeKey( int id )
{
	doc->removeKey( id );
	setCurrentPage( View );
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

		int j = 0;
		QList<CKey> keys = doc->keys();
		for( QList<CKey>::const_iterator i = keys.constBegin(); i != keys.constEnd(); ++i )
		{
			KeyWidget *key = new KeyWidget( *i, j, doc->isEncrypted(), viewKeys );
			connect( key, SIGNAL(remove(int)), SLOT(removeKey(int)) );
			viewKeysLayout->insertWidget( j++, key );
		}

		viewCrypt->setText( doc->isEncrypted() ? tr("Decrypt") : tr("Encrypt") );
		viewCrypt->setEnabled(
			(!doc->isEncrypted() && viewContentView->model()->rowCount()) ||
			(doc->isEncrypted() && keys.contains( CKey( doc->authCert() ) )) );
		break;
	}
	default: break;
	}
}

void MainWindow::showCardStatus()
{
	if( !doc->activeCard().isEmpty() && !doc->authCert().isNull() )
		infoCard->setText( Common::tokenInfo( Common::AuthCert, doc->activeCard(), doc->authCert() ) );
	else if( !doc->activeCard().isEmpty() )
		infoCard->setText( tr("Loading data") );
	else if( doc->activeCard().isEmpty() )
		infoCard->setText( tr("No card in reader") );

	viewCrypt->setEnabled(
		(!doc->isEncrypted() && viewContentView->model()->rowCount()) ||
		(doc->isEncrypted() && doc->keys().contains( CKey( doc->authCert() ) )) );

	cards->clear();
	cards->addItems( doc->presentCards() );
	cards->setVisible( doc->presentCards().size() > 1 );
	cards->setCurrentIndex( cards->findText( doc->activeCard() ) );
}

void MainWindow::showSettings()
{
	SettingsDialog e( this );
	e.addAction( close );
	e.exec();
}

void MainWindow::showWarning( const QString &msg )
{
	QMessageBox d( QMessageBox::Warning, windowTitle(), msg, QMessageBox::Close | QMessageBox::Help, this );
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

void MainWindow::showWarning( const QString &msg, int err, const QString &errmsg )
{
	QString s( msg );
	switch( err )
	{
	case 60:
		s += "<br />" + tr("Wrong PIN or PIN is blocked");
		break;
	case -1: break;
	default:
		s += "<br />" + tr("Error code: %1").arg( err );
		if( !errmsg.isEmpty() )
			s += QString(" (%1)").arg( errmsg );
	}
	showWarning( s );
}

void MainWindow::updateView() { setCurrentPage( Pages(stack->currentIndex()) ); }
