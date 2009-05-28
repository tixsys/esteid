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

#include "MainWindow.h"

#include "PrintSheet.h"
#include "Settings.h"
#include "SignatureDialog.h"
#include "SslCertificate.h"

#include <digidoc/Document.h>

#include <QApplication>
#include <QDateTime>
#include <QDesktopServices>
#include <QDragEnterEvent>
#include <QFileDialog>
#include <QHeaderView>
#include <QMessageBox>
#include <QPrintPreviewDialog>
#include <QProcess>
#include <QSslCertificate>
#include <QTranslator>
#include <QUrl>

#ifdef Q_OS_WIN32
#include <QLibrary>
#include <windows.h>
#include <mapi.h>
#endif

static QString fileSize( qint64 size )
{
	double sizeStr = (double)size/1024;
	return QString( "%1 %2" )
		.arg( QString::number( sizeStr > 1024 ? sizeStr/ 1024 : sizeStr, 'f', 2 ) )
		.arg( sizeStr > 1024 ? "MB" : "KB" );
}



MainWindow::MainWindow( QWidget *parent )
:	QWidget( parent )
{
	setupUi( this );
	setWindowFlags( Qt::Window | Qt::CustomizeWindowHint |
		Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint );
#ifdef Q_OS_LINUX
	setStyleSheet( "* { font: 12pt \"Liberation Sans\";}\n\n" + styleSheet() );
#else
	setStyleSheet( "* { font: 12pt \"Arial\";}\n\n" + styleSheet() );
#endif
	cards->hide();
	cards->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
	languages->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
	//homeOpenUtility->hide();

	connect( signContentView, SIGNAL(doubleClicked(QModelIndex)),
		SLOT(openFile(QModelIndex)) );
	connect( viewContentView, SIGNAL(doubleClicked(QModelIndex)),
		SLOT(openFile(QModelIndex)) );

	QButtonGroup *buttonGroup = new QButtonGroup( this );
	buttonGroup->addButton( homeCrypt, HomeCrypt );
	//buttonGroup->addButton( homeOpenUtility, HomeOpenUtility );
	buttonGroup->addButton( homeSign, HomeSign );
	buttonGroup->addButton( introBack, IntroBack );
	buttonGroup->addButton( introNext, IntroNext );
	buttonGroup->addButton( signAddFile, SignAddFile );
	buttonGroup->addButton( signCancel, SignCancel );
	buttonGroup->addButton( signRemoveFile, SignRemoveFile );
	buttonGroup->addButton( signSaveAs, SignSaveAs );
	buttonGroup->addButton( signSign, SignSign );
	buttonGroup->addButton( homeView, HomeView );
	buttonGroup->addButton( viewAddSignature, ViewAddSignature );
	buttonGroup->addButton( viewBrowse, ViewBrowse );
	buttonGroup->addButton( viewClose, ViewClose );
	buttonGroup->addButton( viewCrypt, ViewCrypt );
	buttonGroup->addButton( viewEmail, ViewEmail );
	buttonGroup->addButton( viewPrint, ViewPrint );
	buttonGroup->addButton( viewSaveAs, ViewSaveAs );
	connect( buttonGroup, SIGNAL(buttonClicked(int)),
		SLOT(buttonClicked(int)) );

	appTranslator = new QTranslator( this );
	qtTranslator = new QTranslator( this );
	QApplication::instance()->installTranslator( appTranslator );
	QApplication::instance()->installTranslator( qtTranslator );

	doc = new DigiDoc( this );
	connect( doc, SIGNAL(error(QString)), SLOT(showWarning(QString)) );
	connect( doc, SIGNAL(dataChanged()), SLOT(showCardStatus()) );

	QLocale::setDefault( QLocale( QLocale::Estonian, QLocale::Estonia ) );
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
		if( SettingsValues().showIntro() )
			setCurrentPage( Intro );
		else
			parseParams();
	}
}

bool MainWindow::addFile( const QString &file )
{
	if( doc->isNull() )
	{
		QFileInfo info( file );
		QString docname = QString( "%1%2%3.bdoc" )
			.arg( SettingsValues().value( "Main/DefaultDir", info.absolutePath() ).toString() )
			.arg( QDir::separator() )
			.arg( info.fileName() );

		if( SettingsValues().value( "Main/AskSaveAs", false ).toBool() ||
			QFile::exists( docname ) )
		{
			docname = QFileDialog::getSaveFileName(
				this, tr("Save file"), docname, tr("Documents (*.bdoc *.ddoc)") );
			if( docname.isEmpty() )
				return false;
		}

		if( QFile::exists( docname ) )
			QFile::remove( docname );
		doc->create( docname );
	}

	// Check if file exist and ask confirmation to overwrite
	QList<digidoc::Document> docs = doc->documents();
	for( int i = 0; i < docs.size(); ++i )
	{
		if( QFileInfo( QString::fromStdString( docs[i].getPath() ) ).fileName() ==
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

	doc->addFile( file );
	return true;
}

void MainWindow::buttonClicked( int button )
{
	switch( button )
	{
	case HomeCrypt:
	case ViewCrypt:
		if( !saveDocument() )
			break;
		if( !QProcess::startDetached( "qdigidoccrypto", QStringList() << doc->fileName() ) )
			showWarning( tr("Failed to start process 'qdigidoccrypto'") );
		break;
	case HomeOpenUtility:
		if( !QProcess::startDetached( "qesteidutil" ) )
			showWarning( tr("Failed to start process 'qesteidutil'") );
		break;
	case HomeSign:
		if( SettingsValues().showIntro() )
		{
			introCheck->setChecked( false );
			setCurrentPage( Intro );
			break;
		}
	case IntroNext:
		if( !params.isEmpty() )
		{
			parseParams();
			break;
		}
	case SignAddFile:
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
			setCurrentPage( Sign );
		}
		else if( doc->isNull() )
			setCurrentPage( Home );
		break;
	}
	case SignCancel:
		if( !doc->documents().isEmpty() )
		{
			QMessageBox msgBox( QMessageBox::Question, tr("Save container"),
				tr("You added %1 files to container, but these are not signed yet.\n"
					"Should I keep unsigned documents or remove these?")
				.arg( doc->documents().size() ) );
			QPushButton *cancel = msgBox.addButton( tr("Remove"), QMessageBox::ActionRole );
			QPushButton *keep = msgBox.addButton( tr("Keep"), QMessageBox::ActionRole );
			msgBox.exec();

			if( msgBox.clickedButton() == keep )
			{
				setCurrentPage( View );
				break;
			}
		}
	case IntroBack:
	case ViewClose:
		if( !saveDocument() )
			break;
		doc->clear();
		setCurrentPage( Home );
		break;
	case SignRemoveFile:
	{
		QAbstractItemModel *m = signContentView->model();

		QStringList files;
		for( int i = 0; i < m->rowCount(); ++i )
		{
			if( m->index( i, 0 ).data( Qt::CheckStateRole ) == Qt::Checked )
				files << m->index( i, 0 ).data().toString();
		}

		if( files.empty() )
			break;

		QMessageBox::StandardButtons btn = QMessageBox::warning(
			this, "QDigiDocClient",
			tr("Are you sure you want remove files %1").arg( files.join(", ") ),
			QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel );

		if( btn == QMessageBox::Cancel )
			break;

		for( int i = m->rowCount() - 1; i >= 0; --i )
		{
			if( m->index( i, 0 ).data( Qt::CheckStateRole ) == Qt::Checked )
				doc->removeDocument( i );
		}
		setCurrentPage( Sign );
		break;
	}
	case SignSign:
		if( !doc->sign(
				signCityInput->text(),
				signStateInput->text(),
				signZipInput->text(),
				signCountryInput->text(),
				signRoleInput->text(),
				signResolutionInput->text() ) )
			break;
		Settings::saveSignatureInfo(
			signRoleInput->text(),
			signResolutionInput->text(),
			signCityInput->text(),
			signStateInput->text(),
			signZipInput->text(),
			signCountryInput->text() );
		setCurrentPage( View );
		break;
	case HomeView:
	{
		QString file = QFileDialog::getOpenFileName( this, tr("Open container"),
			QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation ),
			tr("Documents (*.bdoc *.ddoc)") );
		if( !file.isEmpty() )
		{
			if( doc->open( file ) )
				setCurrentPage( View );
		}
		break;
	}
	case ViewAddSignature:
		setCurrentPage( Sign );
		break;
	case ViewBrowse:
	{
		if( !saveDocument() )
			break;
#ifdef Q_OS_WIN32
		QString url( "file:///" );
#else
		QString url( "file://" );
#endif
		QDesktopServices::openUrl( url.append( QFileInfo( doc->fileName() ).absolutePath() ) );
		break;
	}
	case ViewEmail:
	{
		if( !saveDocument() )
			break;
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
		showWarning( tr("Failed to send email") );
#else
		QDesktopServices::openUrl( QString( "mailto:?subject=%1&attachment=%2" )
			.arg( QFileInfo( doc->fileName() ).fileName() )
			.arg( doc->fileName() ) );
#endif
		break;
	}
	case ViewPrint:
	{
		QPrintPreviewDialog *dialog = new QPrintPreviewDialog( this );
		PrintSheet *p = new PrintSheet( doc, dialog );
		p->setVisible( false );
		connect( dialog, SIGNAL(paintRequested(QPrinter*)), p, SLOT(print(QPrinter*)) );
		dialog->exec();
		break;
	}
	case SignSaveAs:
	case ViewSaveAs:
	{
		QString dir = QFileDialog::getExistingDirectory( this,
			tr("Select folder where files will be stored"),
			QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation ) );
		if( !dir.isEmpty() )
		{
			QAbstractItemModel *m = button == SignSaveAs ?
				signContentView->model() : viewContentView->model();
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

void MainWindow::closeEvent( QCloseEvent *e )
{ e->setAccepted( saveDocument() ); }

void MainWindow::dragEnterEvent( QDragEnterEvent *e )
{
	if( e->mimeData()->hasUrls() && stack->currentIndex() != View )
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
	if( stack->currentIndex() == Home && SettingsValues().showIntro() )
		setCurrentPage( Intro );
	else
		parseParams();
}

void MainWindow::loadDocuments( QTreeWidget *view )
{
	view->clear();
	QList<QTreeWidgetItem*> items;
	Q_FOREACH( const digidoc::Document &file, doc->documents() )
	{
		QTreeWidgetItem *i = new QTreeWidgetItem( view );
		QFileInfo info( QString::fromStdString( file.getPath() ) );
		i->setText( 0, info.fileName() );
		i->setText( 1, fileSize( info.size() ) );
		i->setFlags( Qt::ItemIsEnabled | Qt::ItemIsUserCheckable );
		i->setCheckState( 0, Qt::Unchecked );
		i->setData( 1, Qt::TextAlignmentRole, (int)Qt::AlignRight|Qt::AlignVCenter );
		i->setData( 0, Qt::ToolTipRole, info.fileName() );
		items << i;
	}
	view->insertTopLevelItems( 0, items );
	view->header()->setStretchLastSection( false );
	view->header()->setResizeMode( 0, QHeaderView::Stretch );
	view->header()->setResizeMode( 1, QHeaderView::ResizeToContents );
}

void MainWindow::on_introCheck_stateChanged( int state )
{ SettingsValues().setValue( "Main/Intro", state == Qt::Unchecked ); }

void MainWindow::on_languages_activated( int index )
{
	SettingsValues().setValue( "Main/Language", lang[index] );
	appTranslator->load( ":/translations/" + lang[index] );
	qtTranslator->load( ":/translations/qt_" + lang[index] );
	retranslateUi( this );
	languages->setCurrentIndex( index );
	showCardStatus();
}

void MainWindow::on_settings_clicked() { Settings( this ).exec(); }

void MainWindow::openFile( const QModelIndex &index )
{
	QList<digidoc::Document> list = doc->documents();
	if( list.isEmpty() || index.row() >= list.size() )
		return;

	QDesktopServices::openUrl( QString( "file://" ).append(
		QString::fromStdString( list[index.row()].getPath() ) ) );
}

void MainWindow::parseParams()
{
	Q_FOREACH( const QString &param, params )
	{
		const QFileInfo f( param );
		if( !f.isFile() )
			continue;
		if( doc->isNull() && f.suffix().toLower() == "bdoc" )
		{
			if( doc->open( f.absoluteFilePath() ) )
				setCurrentPage( View );
			params.clear();
			return;
		}
		else if( !addFile( f.absoluteFilePath() ) )
			break;
	}
	if( !doc->isNull() )
		setCurrentPage( Sign );
	params.clear();
}

bool MainWindow::saveDocument()
{
	if( !doc->isModified() )
		return true;

	QMessageBox::StandardButton btn = QMessageBox::warning( this,
		"QDigiDocClient", tr("Document has changed. Save changes?"),
		QMessageBox::Save | QMessageBox::Cancel | QMessageBox::Close,
		QMessageBox::Save );
	switch( btn )
	{
	case QMessageBox::Save: doc->save();
	case QMessageBox::Close: return true;
	case QMessageBox::Cancel:
	default: return false;
	}
}

void MainWindow::setCurrentPage( Pages page )
{
	stack->setCurrentIndex( page );
	switch( page )
	{
	case Sign:
	{
		loadDocuments( signContentView );

		SettingsValues s;
		s.beginGroup( "Main" );
		signRoleInput->setText( s.value( "Role" ).toString() );
		signResolutionInput->setText( s.value( "Resolution" ).toString() );
		signCityInput->setText( s.value( "City" ).toString() );
		signStateInput->setText( s.value( "State" ).toString() );
		signCountryInput->setText( s.value( "Country" ).toString() );
		signZipInput->setText( s.value( "Zip" ).toString() );
		s.endGroup();

		signAddFile->setEnabled( doc->signatures().isEmpty() );
		signSign->setEnabled(
			signContentView->model()->rowCount() > 0 && doc->signCert().isValid() );
		break;
	}
	case View:
	{
		loadDocuments( viewContentView );

		Q_FOREACH( SignatureWidget *w, viewSignatures->findChildren<SignatureWidget*>() )
			w->deleteLater();

		int i = 0;
		bool cardOwnerSignature = false;
		Q_FOREACH( const DigiDocSignature &c, doc->signatures() )
		{
			SignatureWidget *signature = new SignatureWidget( c, i, viewSignatures );
			viewSignaturesLayout->insertWidget( i, signature );
			connect( signature, SIGNAL(removeSignature(uint)),
				SLOT(viewSignaturesRemove(unsigned int)) );
			if( !cardOwnerSignature )
			{
				cardOwnerSignature =
					(c.cert().subjectInfo( "serialNumber" ) == doc->signCert().subjectInfo( "serialNumber" ));
			}
			++i;
		}

		viewFileName->setText( tr("Container: <b>%1</b>").arg( doc->fileName() ) );

		if( i > 0 && cardOwnerSignature )
			viewFileStatus->setText( tr("This container is signed by you") );
		else if( i > 0 && !cardOwnerSignature )
			viewFileStatus->setText( tr("You have not signed this container") );
		else
			viewFileStatus->setText( tr("Container is unsigned") );

		viewAddSignature->setEnabled( doc->signCert().isValid() && !cardOwnerSignature );
		break;
	}
	default: break;
	}
}

void MainWindow::showCardStatus()
{
	QString content;
	if( !doc->authCert().isNull() )
	{
		const SslCertificate c = doc->authCert();
		content += tr("Person <font color=\"black\">%1 %2</font> card in reader<br />Person SSID: %3")
			.arg( SslCertificate::formatName( c.subjectInfoUtf8( "GN" ) ) )
			.arg( SslCertificate::formatName( c.subjectInfoUtf8( "SN" ) ) )
			.arg( c.subjectInfo( "serialNumber" ) );

		QLocale l;
		content += tr("<br />Sign certificate is valid until <font color=\"black\">%1</font>")
			.arg( l.toString( doc->signCert().expiryDate(), "dd. MMMM yyyy" ) );
		if( !doc->signCert().isValid() )
			content += tr("<br /><font color=\"red\">Sign certificate is expired</font>");

		content += tr("<br />Auth certificate is valid until <font color=\"black\">%1</font>")
			.arg( l.toString( doc->authCert().expiryDate(), "dd. MMMM yyyy" ) );
		if( !doc->authCert().isValid() )
			content += tr("<br /><font color=\"red\">Auth certificate is expired</font>");
	}
	else
		content += tr("No card in reader");

	info->setText( content );

	/*homeOpenUtility->setVisible(
		!bdoc->authCert().isNull() && !bdoc->signCert().isNull() &&
		(!bdoc->authCert().isValid() || !bdoc->signCert().isValid()) );*/

	const SslCertificate s = doc->signCert();
	signSignerLabel->setText( QString( "%1 %2 (%3)" )
		.arg( SslCertificate::formatName( s.subjectInfoUtf8( "GN" ) ) )
		.arg( SslCertificate::formatName( s.subjectInfoUtf8( "SN" ) ) )
		.arg( s.subjectInfo( "serialNumber" ) ) );

	setCurrentPage( (Pages)stack->currentIndex() );
}

void MainWindow::showWarning( const QString &msg )
{ QMessageBox::warning( this, "QDigDocClient", msg ); }

void MainWindow::viewSignaturesRemove( unsigned int num )
{
	doc->removeSignature( num );
	doc->save();
	setCurrentPage( doc->signatures().isEmpty() ? Sign : View );
}
