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
#include "common/SslCertificate.h"

#include <QApplication>
#include <QDateTime>
#include <QDesktopServices>
#include <QDragEnterEvent>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QProcess>
#include <QSslCertificate>
#include <QTextStream>
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
	qRegisterMetaType<QSslCertificate>("QSslCertificate");

	setupUi( this );
	setWindowFlags( Qt::Window | Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint );
#if QT_VERSION >= 0x040500
	setWindowFlags( windowFlags() | Qt::WindowCloseButtonHint );
#else
	setWindowFlags( windowFlags() | Qt::WindowSystemMenuHint );
#endif

	cards->hide();
	cards->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
	connect( cards, SIGNAL(activated(QString)), SLOT(selectCard()) );
	languages->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );

	QButtonGroup *buttonGroup = new QButtonGroup( this );

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
			QMessageBox::StandardButton b = QMessageBox::warning( this, "QDigiDocClient",
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
				"QDigiDocClient",
				tr("Container contains file with same name, ovewrite?"),
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
			bool ok;
			SslCertificate c = doc->authCert();
			QString title = QString( "%1 %2 %3" )
				.arg( SslCertificate::formatName( c.subjectInfoUtf8( "GN" ) ) )
				.arg( SslCertificate::formatName( c.subjectInfoUtf8( "SN" ) ) )
				.arg( c.subjectInfo( "serialNumber" ) );
			QString pin = QInputDialog::getText( this, title,
				QObject::tr("<b>%1</b><br />"
					"Selected action requires auth certificate.<br />"
					"For using auth certificate enter PIN1").arg( title ),
				QLineEdit::Password, QString(), &ok );
			if( !ok )
				break;
			doc->decrypt( pin );

			if( doc->isSigned() )
			{
				QMessageBox::StandardButton b = QMessageBox::warning( this, "QDigiDocCrypto",
					tr("This container contains signature! Open with QDigiDocClient?"),
					QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes );
				if( b != QMessageBox::Yes )
					break;
				QString file = QString( doc->fileName() ).append( ".ddoc" );
				if( doc->saveDDoc( file ) )
				{
#ifdef Q_OS_MAC
					if( !QProcess::startDetached( "open", QStringList() << "-a" << "qdigidocclient" << file ) )
#else
					if( !QProcess::startDetached( "qdigidocclient", QStringList() << file ) )
#endif
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
	case HomeView:
	{
		QString file = QFileDialog::getOpenFileName( this, tr("Open container"),
			QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation ),
			tr("Documents (*.cdoc)") );
		if( !file.isEmpty() && doc->open( file ) )
			setCurrentPage( View );
		break;
	}
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
#ifdef Q_OS_WIN32
			params << u.path().remove( 0, 1 );
#else
			params << u.path();
#endif
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

void MainWindow::on_settings_clicked() { Settings( this ).exec(); }

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
		key->show();
	}
	else if( url == "browse" )
	{
#ifdef Q_OS_WIN32
		QString url( "file:///" );
#else
		QString url( "file://" );
#endif
		QDesktopServices::openUrl( url.append( QFileInfo( doc->fileName() ).absolutePath() ) );
	}
	else if( url == "email" )
	{
#ifdef Q_OS_WIN32
		QByteArray filePath = QDir::toNativeSeparators( doc->fileName() ).toLatin1();
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
				return;
		}
		showWarning( tr("Failed to send email"), -1 );
#else
		QDesktopServices::openUrl( QString( "mailto:?subject=%1&attachment=%2" )
			.arg( QFileInfo( doc->fileName() ).fileName() )
			.arg( doc->fileName() ) );
#endif
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
#ifdef Q_OS_MAC
		if( !QProcess::startDetached( "open", QStringList() << "-a" << "qesteidutil") )
#else
		if( !QProcess::startDetached( "qesteidutil" ) )
#endif
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

		s << tr("Name") << ": <font color=\"black\">"
			<< SslCertificate::formatName( c.subjectInfoUtf8( "GN" ) ) << " "
			<< SslCertificate::formatName( c.subjectInfoUtf8( "SN" ) ) << "</font><br />";
		s << tr("Personal code") << ": <font color=\"black\">"
			<< c.subjectInfo( "serialNumber" ) << "</font><br />";
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
	QMessageBox::warning( this, "QDigDocCrypto", s );
}
