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

#include "common/Common.h"
#include "common/IKValidator.h"
#include "common/Settings.h"
#include "common/PinDialog.h"
#include "common/SslCertificate.h"
#include "common/sslConnect.h"

#include "MobileDialog.h"
#include "Poller.h"
#include "PrintSheet.h"
#include "SettingsDialog.h"
#include "SignatureDialog.h"

#include <digidocpp/Document.h>

#include <QApplication>
#include <QDateTime>
#include <QDesktopServices>
#include <QDragEnterEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QPrintPreviewDialog>
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

	QApplication::instance()->installEventFilter( this );
	
	signContentView->setColumnHidden( 2, true );
	viewContentView->setColumnHidden( 3, true );

	Common *common = new Common( this );
	QDesktopServices::setUrlHandler( "mailto", common, "mailTo" );

	cards->hide();
	connect( cards, SIGNAL(activated(QString)), SLOT(selectCard()) );
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

	buttonGroup->addButton( settings, HeadSettings );
	buttonGroup->addButton( help, HeadHelp );

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

	Settings s;
	QLocale::setDefault( QLocale( QLocale::Estonian, QLocale::Estonia ) );
	cards->hack();
	languages->hack();
	lang << "et" << "en" << "ru";
	on_languages_activated( lang.indexOf(
		s.value( "Main/Language", "et" ).toString() ) );

	s.beginGroup( "Client" );
	doc->setConfValue( DigiDoc::ProxyHost, s.value( "proxyPort" ) );
	doc->setConfValue( DigiDoc::ProxyPort, s.value( "proxyHost" ) );
	doc->setConfValue( DigiDoc::PKCS12Cert, s.value( "pkcs12Cert" ) );
	doc->setConfValue( DigiDoc::PKCS12Pass, s.value( "pkcs12Pass" ) );

	infoMobileCell->setText( s.value( "MobileNumber" ).toString() );
	infoMobileCode->setText( s.value( "MobileCode" ).toString() );

	QStringList args = qApp->arguments();
	if( args.size() > 1 )
	{
		args.removeAt( 0 );
		params = args;
		buttonClicked( HomeSign );
	}
}

bool MainWindow::addFile( const QString &file )
{
	if( doc->isNull() )
	{
		Settings s;
		s.beginGroup( "Client" );
		QFileInfo info( file );
		QString docname = QString( "%1/%2.%3" )
			.arg( s.value( "DefaultDir", info.absolutePath() ).toString() )
			.arg( info.fileName() )
			.arg( s.value( "type" ,"bdoc" ).toString() );

		bool ask = s.value( "AskSaveAs", false ).toBool();
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
	case HeadSettings:
		SettingsDialog( this ).exec();
		break;
	case HeadHelp:
		QDesktopServices::openUrl( QUrl( "http://support.sk.ee/" ) );
		break;
	case HomeView:
	{
		QString file = QFileDialog::getOpenFileName( this, tr("Open container"),
			QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation ),
			tr("Documents (*.bdoc *.ddoc)") );
		if( !file.isEmpty() && doc->open( file ) )
			setCurrentPage( View );
		break;
	}
	case HomeCrypt:
		if( !Common::startDetached( "qdigidoccrypto", QStringList() << doc->fileName() ) )
			showWarning( tr("Failed to start process 'qdigidoccrypto'") );
		break;
	case HomeSign:
		if( Settings().value( "Client/Intro", true ).toBool() )
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
		if( !doc->signatures().isEmpty() )
		{
			setCurrentPage( View );
			break;
		}
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
		if ( !checkAccessCert() )
			break;

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
		SettingsDialog::saveSignatureInfo( signRoleInput->text(),
			signResolutionInput->text(), signCityInput->text(),
			signStateInput->text(), signZipInput->text(),
			signCountryInput->text() );
		SettingsDialog::saveMobileInfo( infoMobileCode->text(), infoMobileCell->text() );
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
		if( u.scheme() == "file" )
			params << u.toLocalFile();
	}
	buttonClicked( HomeSign );
}

bool MainWindow::eventFilter( QObject *o, QEvent *e )
{
	if( e->type() == QEvent::FileOpen )
	{
		QFileOpenEvent *o = static_cast<QFileOpenEvent*>(e);
		params << o->file();
		buttonClicked( HomeSign );
		return true;
	}
	else
		return QWidget::eventFilter( o, e );
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
{ Settings().setValue( "Client/Intro", state == Qt::Unchecked ); }

void MainWindow::on_languages_activated( int index )
{
	Settings().setValue( "Main/Language", lang[index] );
	appTranslator->load( ":/translations/" + lang[index] );
	commonTranslator->load( ":/translations/common_" + lang[index] );
	qtTranslator->load( ":/translations/qt_" + lang[index] );
	retranslateUi( this );
	languages->setCurrentIndex( index );
	introNext->setText( tr( "Next" ) );
	signButton->setText( tr("Sign") );
	viewAddSignature->setText( tr("Add signature") );
	showCardStatus();
	setCurrentPage( (Pages)stack->currentIndex() );
}

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
		QDesktopServices::openUrl( QUrl::fromLocalFile( QFileInfo( doc->fileName() ).absolutePath() ) );
	}
	else if( link == "email" )
	{
		QUrl url;
		url.setScheme( "mailto" );
		url.addQueryItem( "subject", QFileInfo( doc->fileName() ).fileName() );
		url.addQueryItem( "attachment", doc->fileName() );
		QDesktopServices::openUrl( url );
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
				else
					QFile::remove( file );
			}
			doc->saveDocument( i, file );
		}
	}
	else if( link == "openUtility" )
	{
		if( !Common::startDetached( "qesteidutil" ) )
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

		Settings s;
		s.beginGroup( "Client" );
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

			bool willExpire = SslCertificate::toLocalTime( c.expiryDate() ) <= QDateTime::currentDateTime().addDays( 105 );
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
	setCurrentPage( (Pages)stack->currentIndex() );
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
	if( index.column() != 2 && index.column()  != 3 )
		return;
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
		windowTitle(), msg,
		QMessageBox::Ok|QMessageBox::Cancel, QMessageBox::Cancel );
	if( b == QMessageBox::Cancel )
		return;
	doc->removeSignature( num );
	doc->save();
	setCurrentPage( doc->signatures().isEmpty() ? Sign : View );
}

bool MainWindow::checkAccessCert()
{
	Settings s;
	s.beginGroup( "Client" );

	QString certFile = QString::fromStdString( doc->getConfValue( DigiDoc::PKCS12Cert, s.value( "pkcs12Cert" ) ) );
	QString certPass = QString::fromStdString( doc->getConfValue( DigiDoc::PKCS12Pass, s.value( "pkcs12Pass" ) ) );

	if ( certFile.size() && QFile::exists( certFile ) )
	{
		QFile f( certFile.toLatin1() );
		if( f.open( QIODevice::ReadOnly ) )
		{
			SslCertificate cert = SslCertificate::fromPKCS12( f.readAll(), certPass.toLatin1() );
			f.close();
			if ( !cert.isNull() && cert.isValid() && cert.expiryDate() > QDateTime::currentDateTime().addDays( 8 ) )
				return true;
		}
	}

	if ( QMessageBox::question( this,
								tr( "Server access certificate" ),
								tr( "Did not find any server access certificate!\nStart downloading?" ),
								QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes )  != QMessageBox::Yes )
		return false;

	doc->poller->lock();
	PinDialog p( PinDialog::Pin1Type, doc->signCert(), qApp->activeWindow() );
	if( !p.exec() )
	{
		doc->poller->unlock();
		return false;
	}
	std::vector<unsigned char> buffer;

	SSLConnect *sslConnect = new SSLConnect( this );
	try {
		buffer = sslConnect->getUrl( p.text().toStdString(), 0, SSLConnect::AccessCert, "");
	} catch( ... ) {}

	delete sslConnect;
	doc->poller->unlock();

	QByteArray result = buffer.size() ? QByteArray( (char *)&buffer[0], buffer.size() ) : QByteArray();

	if ( result.isEmpty() )
	{
		QMessageBox::warning( this,
								tr( "Server access certificate" ),
								tr( "Error downloading server access certificate!" ),
								QMessageBox::Ok );
		return false;
	}

	QDomDocument domDoc;
	if ( !domDoc.setContent( result ) )
	{
		QMessageBox::warning( this,
								tr( "Server access certificate" ),
								tr( "Error parsing server access certificate result!" ),
								QMessageBox::Ok );
		return false;
	}

	QDomElement e = domDoc.documentElement();
	if ( !e.elementsByTagName( "StatusCode" ).size() )
	{
		QMessageBox::warning( this,
								tr( "Server access certificate" ),
								tr( "Error parsing server access certificate result!" ),
								QMessageBox::Ok );
		return false;
	}

	int statusCode = e.elementsByTagName( "ResponseStatus" ).item(0).toElement().text().toInt();
	//0 - ok
	//1 - need to order cert manually from SK web
	//2 - got error, show message from MessageToDisplay element
	if ( statusCode == 1 )
	{
		QDesktopServices::openUrl( QUrl( "http://www.sk.ee/toend/" ) );
		return false;
	}
	if ( statusCode == 2 )
	{
		QMessageBox::warning( this,
								tr( "Server access certificate" ),
								tr( "Error downloading server access certificate!\n%1" )
									.arg( e.elementsByTagName( "MessageToDisplay" ).item(0).toElement().text() ),
								QMessageBox::Ok );
		return false;
	}

	QString dest = QString( "%1%2%3.p12" )
		.arg( QDesktopServices::storageLocation( QDesktopServices::DataLocation ) )
		.arg( QDir::separator() )
		.arg( doc->signCert().subjectInfo( "serialNumber" ) );

	if( QFile::exists( dest ) )
		QFile::remove( dest );
	QFile file( dest );
	if ( !file.open( QIODevice::WriteOnly ) )
	{
		QMessageBox::warning( this, tr( "Server access certificate" ), tr("Failed to save server access certificate file to %1!").arg( dest ) );
		return false;
	}
	file.write( QByteArray::fromBase64( e.elementsByTagName( "TokenData" ).item(0).toElement().text().toLatin1() ) );
	file.close();

	s.setValue( "pkcs12Cert", dest );
	s.setValue( "pkcs12Pass", e.elementsByTagName( "TokenPassword" ).item(0).toElement().text() );

	doc->setConfValue( DigiDoc::PKCS12Cert, s.value( "pkcs12Cert" ) );
	doc->setConfValue( DigiDoc::PKCS12Pass, s.value( "pkcs12Pass" ) );
	return true;
}
