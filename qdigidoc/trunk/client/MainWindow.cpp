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

#include "common/IKValidator.h"
#include "common/SslCertificate.h"

#include "MobileDialog.h"
#include "PrintSheet.h"
#include "Settings.h"
#include "SignatureDialog.h"

#include <digidocpp/Document.h>

#include <QApplication>
#include <QDateTime>
#include <QDesktopServices>
#include <QDragEnterEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QPrintPreviewDialog>
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

	signContentView->setColumnHidden( 2, true );
	viewContentView->setColumnHidden( 3, true );


	cards->hide();
	cards->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
	connect( cards, SIGNAL(activated(QString)), SLOT(selectCard()) );
	languages->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
	viewSignaturesError->hide();

	infoMobileCode->setValidator( new IKValidator( infoMobileCode ) );
	connect( infoMobileCode, SIGNAL(textEdited(QString)), SLOT(enableSign()) );
	connect( infoMobileCell, SIGNAL(textEdited(QString)), SLOT(enableSign()) );
	connect( infoSignMobile, SIGNAL(toggled(bool)), SLOT(showCardStatus()) );

	connect( signContentView, SIGNAL(clicked(QModelIndex)),
		SLOT(viewAction(QModelIndex)) );
	connect( viewContentView, SIGNAL(clicked(QModelIndex)),
		SLOT(viewAction(QModelIndex)) );

	QButtonGroup *buttonGroup = new QButtonGroup( this );

	buttonGroup->addButton( homeSign, HomeSign );
	buttonGroup->addButton( homeView, HomeView );
	buttonGroup->addButton( homeCrypt, HomeCrypt );

	introNext = introButtons->addButton( tr( "Next" ), QDialogButtonBox::ActionRole );
	buttonGroup->addButton( introNext, IntroNext );
	buttonGroup->addButton( introButtons->button( QDialogButtonBox::Cancel ), IntroBack );

	signButton = signButtons->addButton( tr("Sign"), QDialogButtonBox::AcceptRole );
	buttonGroup->addButton( signButton, SignSign );
	buttonGroup->addButton( signButtons->button( QDialogButtonBox::Cancel ), SignCancel );

	viewAddSignature = viewButtons->addButton( tr("Add signature"), QDialogButtonBox::ActionRole );
	buttonGroup->addButton( viewAddSignature, ViewAddSignature );
	buttonGroup->addButton( viewButtons->button( QDialogButtonBox::Close ), ViewClose );
	connect( buttonGroup, SIGNAL(buttonClicked(int)),
		SLOT(buttonClicked(int)) );

	connect( infoLogo, SIGNAL(linkActivated(QString)), SLOT(parseLink(QString)) );
	connect( signAddFile, SIGNAL(linkActivated(QString)), SLOT(parseLink(QString)) );
	connect( viewBrowse, SIGNAL(linkActivated(QString)), SLOT(parseLink(QString)) );
	connect( viewEmail, SIGNAL(linkActivated(QString)), SLOT(parseLink(QString)) );
	connect( viewPrint, SIGNAL(linkActivated(QString)), SLOT(parseLink(QString)) );
	connect( viewSaveAs, SIGNAL(linkActivated(QString)), SLOT(parseLink(QString)) );


	appTranslator = new QTranslator( this );
	commonTranslator = new QTranslator( this );
	qtTranslator = new QTranslator( this );
	QApplication::instance()->installTranslator( appTranslator );
	QApplication::instance()->installTranslator( commonTranslator );
	QApplication::instance()->installTranslator( qtTranslator );

	doc = new DigiDoc( this );
	connect( cards, SIGNAL(activated(QString)), doc, SLOT(selectCard(QString)), Qt::QueuedConnection );
	connect( doc, SIGNAL(error(QString)), SLOT(showWarning(QString)) );
	connect( doc, SIGNAL(dataChanged()), SLOT(showCardStatus()) );
	doc->init();

	SettingsValues s;
	QLocale::setDefault( QLocale( QLocale::Estonian, QLocale::Estonia ) );
	lang[0] = "et";
	lang[1] = "en";
	lang[2] = "ru";
	on_languages_activated( lang.key(
		s.value( "Main/Language", "et" ).toString() ) );

	QStringList args = qApp->arguments();
	if( args.size() > 1 )
	{
		args.removeAt( 0 );
		params = args;
		if( s.showIntro() )
			setCurrentPage( Intro );
		else
			parseParams();
	}

	doc->setConfValue( DigiDoc::ProxyHost, s.value( "Main/proxyPort" ) );
	doc->setConfValue( DigiDoc::ProxyPort, s.value( "Main/proxyHost" ) );
	doc->setConfValue( DigiDoc::PKCS12Cert, s.value( "Main/pkcs12Cert" ) );
	doc->setConfValue( DigiDoc::PKCS12Pass, s.value( "Main/pkcs12Pass" ) );

	infoMobileCell->setText( s.value( "Main/MobileNumber" ).toString() );
	infoMobileCode->setText( s.value( "Main/MobileCode" ).toString() );
}

bool MainWindow::addFile( const QString &file )
{
	if( doc->isNull() )
	{
		QFileInfo info( file );
		QString docname = QString( "%1/%2.%3" )
			.arg( SettingsValues().value( "Main/DefaultDir", info.absolutePath() ).toString() )
			.arg( info.fileName() )
			.arg( SettingsValues().value( "Main/type" ,"bdoc" ).toString() );

		bool ask = SettingsValues().value( "Main/AskSaveAs", false ).toBool();
		bool select = false;
		if( !ask && QFile::exists( docname ) )
		{
			QMessageBox::StandardButton b = QMessageBox::warning( this, "QDigiDocCrypto",
				tr( "%1 already exists.<br />Do you want replace it?" ).arg( docname ),
				QMessageBox::Yes | QMessageBox::No, QMessageBox::No );
			select = b == QMessageBox::No;
		}

		if( ask || select )
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
		if( QFileInfo( QString::fromUtf8( docs[i].getPath().data() ) ).fileName() ==
			QFileInfo( file ).fileName() )
		{
			QMessageBox::StandardButton btn = QMessageBox::warning( this,
				tr("File already in container"),
				tr("%1<br />already in container, ovewrite?")
					.arg( QFileInfo( file ).fileName() ),
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
#ifdef Q_OS_MAC
		if( !QProcess::startDetached( "open", QStringList() << "-a" << "qdigidoccrypto" << doc->fileName() ) )
#else
		if( !QProcess::startDetached( "qdigidoccrypto", QStringList() << doc->fileName() ) )
#endif
			showWarning( tr("Failed to start process 'qdigidoccrypto'") );
		break;
	case HomeSign:
		if( SettingsValues().showIntro() )
		{
			introCheck->setChecked( false );
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
				doc->save();
				setCurrentPage( View );
				break;
			}
		}
	case IntroBack:
	case ViewClose:
		doc->clear();
		setCurrentPage( Home );
		break;
	case SignSign:
	{
		if( infoSignCard->isChecked() )
		{
			if( !doc->sign( signCityInput->text(), signStateInput->text(),
					signZipInput->text(), signCountryInput->text(),
					signRoleInput->text(), signResolutionInput->text() ) )
				break;
				doc->save();
		}
		else
		{
			MobileDialog *m = new MobileDialog( doc, this );
			m->setSignatureInfo( signCityInput->text(),
				signStateInput->text(), signZipInput->text(),
				signCountryInput->text(), signRoleInput->text(),
				signResolutionInput->text() );
			m->sign( infoMobileCode->text().toLatin1(), infoMobileCell->text().toLatin1() );
			m->exec();
			if ( !m->fName.isEmpty() && doc->signMobile( m->fName ) )
			{
				doc->save();
			} else {
				m->deleteLater();
				break;
			}
			m->deleteLater();
		}
		Settings::saveSignatureInfo( signRoleInput->text(),
			signResolutionInput->text(), signCityInput->text(),
			signStateInput->text(), signZipInput->text(),
			signCountryInput->text() );
		Settings::saveMobileInfo( infoMobileCode->text(), infoMobileCell->text() );
		setCurrentPage( View );
		break;
	}
	case HomeView:
	{
		QString file = QFileDialog::getOpenFileName( this, tr("Open container"),
			QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation ),
			tr("Documents (*.bdoc *.ddoc)") );
		if( !file.isEmpty() && doc->open( file ) )
			setCurrentPage( View );
		break;
	}
	case ViewAddSignature:
		setCurrentPage( Sign );
		break;
	default: break;
	}
}

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

void MainWindow::enableSign()
{
	bool mobile = infoSignMobile->isChecked();

	if( mobile )
	{
		signSigner->setText( QString( "%1 (%2)" )
			.arg( infoMobileCell->text() ).arg( infoMobileCode->text() ) );
	}

	if( doc->isNull() ||
		(mobile && !IKValidator::isValid( infoMobileCode->text() )) ||
		(!mobile && !doc->signCert().isValid()) )
	{
		signButton->setEnabled( false );
		return;
	}

	bool cardOwnerSignature = false;
	const QByteArray serialNumber = mobile ?
		infoMobileCode->text().toLatin1() : doc->signCert().subjectInfo( "serialNumber" ).toLatin1();
	Q_FOREACH( const DigiDocSignature &c, doc->signatures() )
	{
		if( c.cert().subjectInfo( "serialNumber" ) == serialNumber )
		{
			cardOwnerSignature = true;
			break;
		}
	}
	signButton->setEnabled( !cardOwnerSignature );
}

void MainWindow::on_introCheck_stateChanged( int state )
{ SettingsValues().setValue( "Main/Intro", state == Qt::Unchecked ); }

void MainWindow::on_languages_activated( int index )
{
	SettingsValues().setValue( "Main/Language", lang[index] );
	appTranslator->load( ":/translations/" + lang[index] );
	commonTranslator->load( ":/translations/common_" + lang[index] );
	qtTranslator->load( ":/translations/qt_" + lang[index] );
	retranslateUi( this );
	languages->setCurrentIndex( index );
	introNext->setText( tr( "Next" ) );
	signButton->setText( tr("Sign") );
	viewAddSignature->setText( tr("Add signature") );
	showCardStatus();
}

void MainWindow::on_settings_clicked() { Settings( this ).exec(); }

void MainWindow::parseLink( const QString &link )
{
	if( link == "addFile" )
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
	}
	else if( link == "browse" )
	{
#ifdef Q_OS_WIN32
		QString url( "file:///" );
#else
		QString url( "file://" );
#endif
		QDesktopServices::openUrl( url.append( QFileInfo( doc->fileName() ).absolutePath() ) );
	}
	else if( link == "email" )
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
		showWarning( tr("Failed to send email") );
#else
		QDesktopServices::openUrl( QString( "mailto:?subject=%1&attachment=%2" )
			.arg( QFileInfo( doc->fileName() ).fileName() )
			.arg( doc->fileName() ) );
#endif
	}
	else if( link == "print" )
	{
		QPrintPreviewDialog *dialog = new QPrintPreviewDialog( this );
		dialog->setWindowFlags( dialog->windowFlags() | Qt::WindowMinMaxButtonsHint );
		PrintSheet *p = new PrintSheet( doc, dialog );
		p->setVisible( false );
		connect( dialog, SIGNAL(paintRequested(QPrinter*)), p, SLOT(print(QPrinter*)) );
		dialog->exec();
	}
	else if( link == "saveAs" )
	{
		QString dir = QFileDialog::getExistingDirectory( this,
			tr("Select folder where files will be stored"),
			QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation ) );
		if( dir.isEmpty() )
			return;
		QAbstractItemModel *m = viewContentView->model();
		for( int i = 0; i < m->rowCount(); ++i )
		{
			doc->saveDocument( i, QString( "%1/%2" )
				.arg( dir ).arg( m->index( i, 0 ).data().toString() ) );
		}
	}
	else if( link == "openUtility" )
	{
#ifdef Q_OS_MAC
		if( !QProcess::startDetached( "open", QStringList() << "-a" << "qesteidutil") )
#else
		if( !QProcess::startDetached( "qesteidutil" ) )
#endif
			showWarning( tr("Failed to start process 'qesteidutil'") );
	}
}

void MainWindow::parseParams()
{
	Q_FOREACH( const QString &param, params )
	{
		const QFileInfo f( param );
		if( !f.isFile() )
			continue;
		const QString suffix = f.suffix().toLower();
		if( doc->isNull() && (suffix == "bdoc" || suffix == "ddoc") )
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
	case Sign:
	{
		signContentView->setContent( doc->documents() );
		signContentView->setColumnHidden( 3, !doc->signatures().isEmpty() );

		SettingsValues s;
		s.beginGroup( "Main" );
		signRoleInput->setText( s.value( "Role" ).toString() );
		signResolutionInput->setText( s.value( "Resolution" ).toString() );
		signCityInput->setText( s.value( "City" ).toString() );
		signStateInput->setText( s.value( "State" ).toString() );
		signCountryInput->setText( s.value( "Country" ).toString() );
		signZipInput->setText( s.value( "Zip" ).toString() );
		s.endGroup();

		enableSign();
		signAddFile->setVisible( doc->signatures().isEmpty() );
		signButton->setFocus();
		break;
	}
	case View:
	{
		viewContentView->setContent( doc->documents() );

		Q_FOREACH( SignatureWidget *w, viewSignatures->findChildren<SignatureWidget*>() )
			w->deleteLater();

		int i = 0;
		bool cardOwnerSignature = false;
		QList<DigiDocSignature> signatures = doc->signatures();
		Q_FOREACH( const DigiDocSignature &c, signatures )
		{
			SignatureWidget *signature = new SignatureWidget( c, i, signatures.size() < 3, viewSignatures );
			viewSignaturesLayout->insertWidget( i, signature );
			connect( signature, SIGNAL(removeSignature(unsigned int)),
				SLOT(viewSignaturesRemove(unsigned int)) );
			if( !cardOwnerSignature )
			{
				cardOwnerSignature =
					(c.cert().subjectInfo( "serialNumber" ) == doc->signCert().subjectInfo( "serialNumber" ));
			}
			++i;
		}

		viewFileName->setText( QString( "%1 <b>%2</b>" )
			.arg( tr("Container:") )
			.arg( QDir::toNativeSeparators( doc->fileName() ) ) );
		viewFileName->setToolTip( QDir::toNativeSeparators( doc->fileName() ) );

		if( !signatures.isEmpty() && cardOwnerSignature )
			viewFileStatus->setText( tr("This container is signed by you") );
		else if( !signatures.isEmpty() && !cardOwnerSignature )
			viewFileStatus->setText( tr("You have not signed this container") );
		else
			viewFileStatus->setText( tr("Container is unsigned") );

		viewSignaturesLabel->setText( signatures.size() == 1 ? tr("Signature") : tr("Signatures") );
		break;
	}
	default: break;
	}
}

void MainWindow::showCardStatus()
{
	infoLogo->setText( QString() );
	signSigner->setText( QString() );

	if( infoSignMobile->isChecked() )
	{
		infoStack->setCurrentIndex( 1 );
		signSigner->setText( QString( "%1 (%2)" )
			.arg( infoMobileCell->text() ).arg( infoMobileCode->text() ) );
	}
	else
	{
		infoStack->setCurrentIndex( 0 );
		QString content;
		if( !doc->activeCard().isEmpty() && !doc->signCert().isNull() )
		{
			const SslCertificate c = doc->signCert();
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
			s << tr("Sign certificate is") << " ";
			if( doc->signCert().isValid()  )
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

			signSigner->setText( QString( "%1 %2 (%3)" )
				.arg( SslCertificate::formatName( c.subjectInfoUtf8( "GN" ) ) )
				.arg( SslCertificate::formatName( c.subjectInfoUtf8( "SN" ) ) )
				.arg( c.subjectInfo( "serialNumber" ) ) );
		}
		else if( doc->activeCard().isEmpty() )
			content = tr("No card in reader");
		infoCard->setText( content );
	}

	cards->clear();
	cards->addItems( doc->presentCards() );
	cards->setVisible( doc->presentCards().size() > 1 );
	cards->setCurrentIndex( cards->findText( doc->activeCard() ) );

	enableSign();
}

void MainWindow::showWarning( const QString &msg )
{
	QMessageBox d( QMessageBox::Warning, "QDigDocClient", msg, QMessageBox::Close | QMessageBox::Help, this );
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

void MainWindow::viewAction( const QModelIndex &index )
{
	QList<digidoc::Document> list = doc->documents();
	if( list.isEmpty() || index.row() >= list.size() )
		return;

	switch( index.column() )
	{
	case 2:
	{
		QString filepath = QFileDialog::getSaveFileName( this,
			tr("Save file"), QString( "%1/%2" )
				.arg( QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation ) )
				.arg( index.model()->index( index.row(), 0 ).data().toString() ) );
		if( !filepath.isEmpty() )
			doc->saveDocument( index.row(), filepath );
		break;
	}
	case 3:
		doc->removeDocument( index.row() );
		setCurrentPage( (Pages)stack->currentIndex() );
		break;
	default: break;
	}
}

void MainWindow::viewSignaturesRemove( unsigned int num )
{
	SslCertificate c = doc->signatures().at( num ).cert();
	QString msg = tr("Remove signature %1 %2 %3")
		.arg( SslCertificate::formatName( c.subjectInfoUtf8( "GN" ) ) )
		.arg( SslCertificate::formatName( c.subjectInfoUtf8( "SN" ) ) )
		.arg( c.subjectInfo( "serialNumber" ) );
	QMessageBox::StandardButton b = QMessageBox::warning( this,
		"QDigiDocClient", msg,
		QMessageBox::Ok|QMessageBox::Cancel, QMessageBox::Cancel );
	if( b == QMessageBox::Cancel )
		return;
	doc->removeSignature( num );
	doc->save();
	setCurrentPage( doc->signatures().isEmpty() ? Sign : View );
}
