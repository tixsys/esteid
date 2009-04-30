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


QString fileSize( qint64 size )
{
	double sizeStr = (double)size/1024;
	return QString( "%1 %2" )
		.arg( QString::number( sizeStr > 1024 ? sizeStr/ 1024 : sizeStr, 'f', 2 ) )
		.arg( sizeStr > 1024 ? "MB" : "KB" );
}

QString parseCertInfo( const QString &in )
{
	if( !in.contains( "\\x" ) )
		return in;

	QString res;
	bool firstByte = true;
	ushort data;
	int i = 0;
	while( i < in.size() )
	{
		if( in.mid( i, 2 ) == "\\x" )
		{
			if( firstByte )
				data = in.mid( i+2, 2 ).toUShort( 0, 16 );
			else
				res += QChar( (data<<8) + in.mid( i+2, 2 ).toUShort( 0, 16 ) );
			i += 4;
		}
		else
		{
			if( firstByte )
				data = in[i].unicode();
			else
				res += QChar( (data<<8) + in[i].unicode() );
			++i;
		}
		firstByte = !firstByte;
	}
	return res;
}

QString parseName( const QString &in )
{
	QString ret = in.toLower();
	bool firstChar = true;
	for( QString::iterator i = ret.begin(); i != ret.end(); ++i )
	{
		if( !firstChar && !i->isLetter() )
			firstChar = true;

		if( firstChar && i->isLetter() )
		{
			*i = i->toUpper();
			firstChar = false;
		}
	}
	return ret;
}



MainWindow::MainWindow( QWidget *parent )
:	QWidget( parent )
{
	setupUi( this );
	homeOpenUtility->hide();
	viewCDocDocsContentView->header()->setStretchLastSection( false );
	viewCDocDocsContentView->header()->setResizeMode( 0, QHeaderView::Stretch );
	viewCDocDocsContentView->header()->setResizeMode( 1, QHeaderView::ResizeToContents );

	QButtonGroup *buttonGroup = new QButtonGroup( this );
	buttonGroup->addButton( homeCreateCDoc, HomeCreateCDoc );
	buttonGroup->addButton( homeViewCDoc, HomeViewCDoc );
	buttonGroup->addButton( introCDocBack, IntroCDocBack );
	buttonGroup->addButton( introCDocNext, IntroCDocNext );
	buttonGroup->addButton( viewCDocAddFile, ViewCDocAddFile );
	buttonGroup->addButton( viewCDocAddRecipient, ViewCDocAddRecipient );
	buttonGroup->addButton( viewCDocBrowse, ViewCDocBrowse );
	buttonGroup->addButton( viewCDocClose, ViewCDocClose );
	buttonGroup->addButton( viewCDocCrypt, ViewCDocCrypt );
	buttonGroup->addButton( viewCDocEmail, ViewCDocEmail );
	buttonGroup->addButton( viewCDocRemoveFile, ViewCDocRemoveFile );
	buttonGroup->addButton( viewCDocSaveAs, ViewCDocSaveAs );
	connect( buttonGroup, SIGNAL(buttonClicked(int)),
		SLOT(buttonClicked(int)) );

	appTranslator = new QTranslator( this );
	qtTranslator = new QTranslator( this );
	QApplication::instance()->installTranslator( appTranslator );
	QApplication::instance()->installTranslator( qtTranslator );

	doc = new CryptDoc( this );
	connect( doc, SIGNAL(error(QString,int)), SLOT(showWarning(QString,int)) );
	connect( doc, SIGNAL(dataChanged()), SLOT(showCardStatus()) );

	comboLanguages->setItemData( 0, "et" );
	comboLanguages->setItemData( 1, "en" );
	comboLanguages->setItemData( 2, "ru" );
	comboLanguages->setItemData( 3, "de" );
	comboLanguages->setItemData( 4, "lv" );
	comboLanguages->setItemData( 5, "lt" );
	comboLanguages->setCurrentIndex(
		comboLanguages->findData( SettingsValues().value( "Main/Language", "et" ) ) );
	on_comboLanguages_activated( comboLanguages->currentIndex() );

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
	case HomeCreateCDoc:
		if( !SettingsValues().value( "Main/Intro", true ).toBool() )
		{
			introCDocCheck->setChecked( false );
			setCurrentPage( Intro );
			break;
		}
	case IntroCDocNext:
	case ViewCDocAddFile:
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
	case ViewCDocAddRecipient:
	{
		if( doc->isEncrypted() )
			return;

		KeyAddDialog *key = new KeyAddDialog( this );
		connect( key, SIGNAL(selected(QList<CKey>)), SLOT(addKeys(QList<CKey>)) );
		key->show();
		break;
	}
	case ViewCDocCrypt:
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
	case ViewCDocRemoveFile:
	{
		QAbstractItemModel *m = viewCDocDocsContentView->model();

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
	case ViewCDocBrowse:
		QDesktopServices::openUrl( QString( "file://" )
			.append( QFileInfo( doc->fileName() ).absolutePath() ) );
		break;
	case ViewCDocEmail:
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
	case HomeViewCDoc:
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
	case IntroCDocBack:
	case ViewCDocClose:
		doc->clear();
		setCurrentPage( Home );
		break;
	case ViewCDocSaveAs:
	{
		QString dir = QFileDialog::getExistingDirectory( this,
			tr("Select folder where files will be stored"),
			QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation ) );
		if( !dir.isEmpty() )
		{
			QAbstractItemModel *m = viewCDocDocsContentView->model();
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

void MainWindow::on_buttonSettings_clicked() { (new Settings( this ))->show(); }

void MainWindow::on_comboLanguages_activated( int index )
{
	SettingsValues().setValue( "Main/Language", comboLanguages->itemData( index ).toString() );

	appTranslator->load( QString( ":/translations/%1" )
		.arg( comboLanguages->itemData( index ).toString() ) );
	qtTranslator->load( QString( ":/translations/qt_%1" )
		.arg( comboLanguages->itemData( index ).toString() ) );

	retranslateUi( this );
	showCardStatus();
}

void MainWindow::on_introCDocCheck_stateChanged( int state )
{ SettingsValues().setValue( "Main/Intro", state == Qt::Checked ); }

void MainWindow::on_viewCDocDocsContentView_doubleClicked( const QModelIndex &index )
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
		viewCDocLocation->setText( tr("Container: <b>%1</b>").arg( doc->fileName() ) );
		viewCDocAddFile->setDisabled( doc->isEncrypted() );
		viewCDocSaveAs->setDisabled( doc->isEncrypted() );
		viewCDocRemoveFile->setDisabled( doc->isEncrypted() );
		viewCDocAddRecipient->setDisabled( doc->isEncrypted() );

		viewCDocCrypt->setText( doc->isEncrypted() ? tr("Decrypt") : tr("Encrypt") );

		viewCDocDocsContentView->clear();
		Qt::ItemFlags flags;
		if( !doc->isEncrypted() )
			flags = Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
		Q_FOREACH( const CDocument &file, doc->documents() )
		{
			QTreeWidgetItem *i = new QTreeWidgetItem( viewCDocDocsContentView );
			i->setText( 0, file.filename );
			i->setText( 1, file.size );
			i->setFlags( flags );
			i->setCheckState( 0, Qt::Unchecked );
			i->setData( 1, Qt::TextAlignmentRole, (int)Qt::AlignRight|Qt::AlignVCenter );
			i->setData( 0, Qt::ToolTipRole, file.filename );
			viewCDocDocsContentView->addTopLevelItem( i );
		}

		Q_FOREACH( KeyWidget *w, viewCDocKeysContent->findChildren<KeyWidget*>() )
			w->deleteLater();
		int i = 0;
		Q_FOREACH( const CKey &k, doc->keys() )
		{
			KeyWidget *key = new KeyWidget( k, i, doc->isEncrypted(), viewCDocKeysContent );
			connect( key, SIGNAL(remove(int)), SLOT(removeKey(int)) );
			viewCDocKeysContentLayout->insertWidget( i, key );
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
