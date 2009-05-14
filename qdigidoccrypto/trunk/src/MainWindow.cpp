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

#include <QApplication>
#include <QDateTime>
#include <QDesktopServices>
#include <QDragEnterEvent>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QSslCertificate>
#include <QTranslator>
#include <QUrl>

#ifdef Q_OS_WIN32
#include <QLibrary>
#include <windows.h>
#include <mapi.h>
#endif

MainWindow::MainWindow( QWidget *parent )
:	QWidget( parent )
{
	setupUi( this );
	//homeOpenUtility->hide();
	viewContentView->header()->setStretchLastSection( false );
	viewContentView->header()->setResizeMode( 0, QHeaderView::Stretch );
	viewContentView->header()->setResizeMode( 1, QHeaderView::ResizeToContents );

	QButtonGroup *buttonGroup = new QButtonGroup( this );
	buttonGroup->addButton( homeCreate, HomeCreate );
	buttonGroup->addButton( homeView, HomeView );
	buttonGroup->addButton( introBack, IntroBack );
	buttonGroup->addButton( introNext, IntroNext );
	buttonGroup->addButton( viewAddFile, ViewAddFile );
	buttonGroup->addButton( viewAddRecipient, ViewAddRecipient );
	buttonGroup->addButton( viewBrowse, ViewBrowse );
	buttonGroup->addButton( viewClose, ViewClose );
	buttonGroup->addButton( viewCrypt, ViewCrypt );
	buttonGroup->addButton( viewEmail, ViewEmail );
	buttonGroup->addButton( viewRemoveFile, ViewRemoveFile );
	buttonGroup->addButton( viewSaveAs, ViewSaveAs );
	connect( buttonGroup, SIGNAL(buttonClicked(int)),
		SLOT(buttonClicked(int)) );

	appTranslator = new QTranslator( this );
	qtTranslator = new QTranslator( this );
	QApplication::instance()->installTranslator( appTranslator );
	QApplication::instance()->installTranslator( qtTranslator );

	doc = new CryptDoc( this );
	connect( doc, SIGNAL(error(QString,int)), SLOT(showWarning(QString,int)) );
	connect( doc, SIGNAL(dataChanged()), SLOT(showCardStatus()) );

	languages->setItemData( 0, "et" );
	languages->setItemData( 1, "en" );
	languages->setItemData( 2, "ru" );
	languages->setItemData( 3, "de" );
	languages->setItemData( 4, "lv" );
	languages->setItemData( 5, "lt" );
	languages->setCurrentIndex( languages->findData(
		SettingsValues().value( "Main/Language", "et" ) ) );
	on_languages_activated( languages->currentIndex() );

	QStringList args = qApp->arguments();
	if( args.size() > 1 )
	{
		for( int i = 1; i < args.size(); ++i )
		{
			const QFileInfo f( args[i] );
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
	}
}

bool MainWindow::addFile( const QString &file )
{
	if( doc->isNull() )
	{
		QFileInfo info( file );
		QString docname = QString( "%1%2%3.cdoc" )
			.arg( SettingsValues().value( "Main/DefaultDir", info.absolutePath() ).toString() )
			.arg( QDir::separator() )
			.arg( info.fileName() );

		if( SettingsValues().value( "Main/AskSaveAs", false ).toBool() ||
			QFile::exists( docname ) )
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
	case HomeCreate:
		if( !SettingsValues().value( "Main/Intro", true ).toBool() )
		{
			introCheck->setChecked( false );
			setCurrentPage( Intro );
			break;
		}
	case IntroNext:
	case ViewAddFile:
	{
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
	case ViewAddRecipient:
	{
		if( doc->isEncrypted() )
			return;

		KeyAddDialog *key = new KeyAddDialog( this );
		connect( key, SIGNAL(selected(QList<CKey>)), SLOT(addKeys(QList<CKey>)) );
		key->show();
		break;
	}
	case ViewCrypt:
		if( doc->isEncrypted() )
		{
			bool ok;
			QString pin = QInputDialog::getText( 0, "QDigiDocCrypto",
				QObject::tr("Selected action requires auth certificate.\n"
					"For using auth certificate enter PIN1"),
				QLineEdit::Password,
				QString(), &ok );
			if( !ok )
				break;
			doc->decrypt( pin );
		}
		else
		{
			if( doc->encrypt() )
				doc->save();
		}
		setCurrentPage( View );
		break;
	case ViewRemoveFile:
	{
		QAbstractItemModel *m = viewContentView->model();

		QStringList files;
		for( int i = 0; i < m->rowCount(); ++i )
		{
			if( m->index( i, 0 ).data( Qt::CheckStateRole ) == Qt::Checked )
				files << m->index( i, 0 ).data().toString();
		}

		if( files.empty() )
			break;

		QMessageBox::StandardButtons btn = QMessageBox::warning(
			this, "QDigiDocCrypto",
			tr("Are you sure you want remove files %1").arg( files.join(", ") ),
			QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel );

		if( btn == QMessageBox::Cancel )
			break;

		for( int i = m->rowCount() - 1; i >= 0; --i )
		{
			if( m->index( i, 0 ).data( Qt::CheckStateRole ) == Qt::Checked )
				doc->removeDocument( i );
		}
		setCurrentPage( View );
		break;
	}
	case ViewBrowse:
		QDesktopServices::openUrl( QString( "file://" )
			.append( QFileInfo( doc->fileName() ).absolutePath() ) );
		break;
	case ViewEmail:
	{
#ifdef Q_OS_WIN32
		QByteArray filePath = doc->fileName().toLatin1();
		QByteArray fileName = QFileInfo( doc->fileName() ).fileName().toLatin1();

		MapiFileDesc doc[1];
		doc[0].ulReserved = 0;
		doc[0].flFlags = 0;
		doc[0].nPosition = -1;
		doc[0].lpszPathName = filePath.data();
		doc[0].lpszFileName = fileName.data();
		doc[0].lpFileType = NULL;

		// Create message
		MapiMessage message;
		message.ulReserved = 0;
		message.lpszSubject = "";
		message.lpszNoteText = "";
		message.lpszMessageType = NULL;
		message.lpszDateReceived = NULL;
		message.lpszConversationID = NULL;
		message.flFlags = 0;
		message.lpOriginator = NULL;
		message.nRecipCount = 0;
		message.lpRecips = NULL;
		message.nFileCount = 1;
		message.lpFiles = (lpMapiFileDesc)&doc;

		QLibrary lib("mapi32");
		typedef ULONG (PASCAL *SendMail)(ULONG,ULONG,MapiMessage*,FLAGS,ULONG);
		SendMail mapi = (SendMail)lib.resolve("MAPISendMail");
		if( mapi )
		{
			int status = mapi( NULL, 0, &message, MAPI_LOGON_UI|MAPI_DIALOG, 0 );
			if( status == SUCCESS_SUCCESS )
				break;
		}
		showWarning( tr("Failed to send email"), -1 );
#else
		QDesktopServices::openUrl( QString( "mailto:?subject=%1&attachment=%2" )
			.arg( QFileInfo( doc->fileName() ).fileName() )
			.arg( doc->fileName() ) );
#endif
		break;
	}
	case HomeView:
	{
		QString file = QFileDialog::getOpenFileName( this, tr("Open container"),
			QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation ),
			tr("Documents (*.cdoc)") );
		if( !file.isEmpty() )
		{
			doc->open( file );
			setCurrentPage( View );
		}
		break;
	}
	case IntroBack:
	case ViewClose:
		doc->clear();
		setCurrentPage( Home );
		break;
	case ViewSaveAs:
	{
		QString dir = QFileDialog::getExistingDirectory( this,
			tr("Select folder where files will be stored"),
			QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation ) );
		if( !dir.isEmpty() )
		{
			QAbstractItemModel *m = viewContentView->model();
			for( int i = 0; i < m->rowCount(); ++i )
			{
				if( m->index( i, 0 ).data( Qt::CheckStateRole ) == Qt::Checked )
					doc->saveDocument( i, dir );
			}
		}
		break;
	}
	default: break;
	}
}

void MainWindow::dragEnterEvent( QDragEnterEvent *e )
{
	if( e->mimeData()->hasUrls() &&
		(stack->currentIndex() == Home ||
		 stack->currentIndex() == Intro ||
		 (stack->currentIndex() == View && !doc->isEncrypted()) ) )
		e->acceptProposedAction();
}

void MainWindow::dropEvent( QDropEvent *e )
{
	Q_FOREACH( const QUrl &u, e->mimeData()->urls() )
	{
		if( u.isRelative() || u.scheme() == "file" )
		{
#ifdef Q_OS_WIN32
			QFileInfo file( u.path().remove( 0, 1 ) );
#else
			QFileInfo file( u.path() );
#endif
			if( !file.isFile() )
				continue;
			else if( file.suffix().toLower() == "cdoc" && doc->isNull() )
			{
				doc->open( file.filePath() );
				break;
			}
			else if( !addFile( file.filePath() ) )
				break;
		}
	}
	setCurrentPage( View );
}

void MainWindow::on_introCheck_stateChanged( int state )
{ SettingsValues().setValue( "Main/Intro", state == Qt::Checked ); }

void MainWindow::on_languages_activated( int index )
{
	SettingsValues().setValue( "Main/Language", languages->itemData( index ).toString() );

	appTranslator->load( QString( ":/translations/%1" )
		.arg( languages->itemData( index ).toString() ) );
	qtTranslator->load( QString( ":/translations/qt_%1" )
		.arg( languages->itemData( index ).toString() ) );

	retranslateUi( this );
	showCardStatus();
}

void MainWindow::on_settings_clicked() { (new Settings( this ))->show(); }

void MainWindow::on_viewContentView_doubleClicked( const QModelIndex &index )
{
	if( doc->isNull() || doc->isEncrypted() )
		return;

	QList<CDocument> list = doc->documents();
	if( list.isEmpty() || index.row() >= list.size() )
		return;

	doc->saveDocument( index.row(), QDir::tempPath() );
	QDesktopServices::openUrl( QString( "file://%1%2%3" )
		.arg( QDir::tempPath() )
		.arg( QDir::separator() )
		.arg( list[index.row()].filename ) );
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
		viewFileName->setText( tr("Container: <b>%1</b>").arg( doc->fileName() ) );
		viewAddFile->setDisabled( doc->isEncrypted() );
		viewSaveAs->setDisabled( doc->isEncrypted() );
		viewRemoveFile->setDisabled( doc->isEncrypted() );
		viewAddRecipient->setDisabled( doc->isEncrypted() );

		viewCrypt->setText( doc->isEncrypted() ? tr("Decrypt") : tr("Encrypt") );

		viewContentView->clear();
		Qt::ItemFlags flags;
		if( !doc->isEncrypted() )
			flags = Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
		Q_FOREACH( const CDocument &file, doc->documents() )
		{
			QTreeWidgetItem *i = new QTreeWidgetItem( viewContentView );
			i->setText( 0, file.filename );
			i->setText( 1, file.size );
			i->setFlags( flags );
			i->setCheckState( 0, Qt::Unchecked );
			i->setData( 1, Qt::TextAlignmentRole, (int)Qt::AlignRight|Qt::AlignVCenter );
			i->setData( 0, Qt::ToolTipRole, file.filename );
			viewContentView->addTopLevelItem( i );
		}

		Q_FOREACH( KeyWidget *w, viewKeys->findChildren<KeyWidget*>() )
			w->deleteLater();
		int i = 0;
		Q_FOREACH( const CKey &k, doc->keys() )
		{
			KeyWidget *key = new KeyWidget( k, i, doc->isEncrypted(), viewKeys );
			connect( key, SIGNAL(remove(int)), SLOT(removeKey(int)) );
			viewKeysLayout->insertWidget( i, key );
			++i;
		}
		break;
	}
	default: break;
	}
}

void MainWindow::showCardStatus()
{
	setCurrentPage( (Pages)stack->currentIndex() );
}

void MainWindow::showWarning( const QString &msg, int err )
{
	QString s( msg );
	if( err != -1 )
		s.append( tr("\nError code: %1").arg( err ) );
	QMessageBox::warning( this, "QDigDocCrypto", s );
}
