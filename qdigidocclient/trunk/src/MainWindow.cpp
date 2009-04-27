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

#include <digidoc/Document.h>

#include <QApplication>
#include <QDateTime>
#include <QDesktopServices>
#include <QDragEnterEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QPrinter>
#include <QPrintDialog>
#include <QSslCertificate>
#include <QTranslator>
#include <QUrl>
#include <QWebView>

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

	connect( signBDocDocsContentView, SIGNAL(doubleClicked(QModelIndex)),
		SLOT(openFile(QModelIndex)) );
	connect( viewBDocDocsFrameView, SIGNAL(doubleClicked(QModelIndex)),
		SLOT(openFile(QModelIndex)) );

	QButtonGroup *buttonGroup = new QButtonGroup( this );
	buttonGroup->addButton( homeSignBDoc, HomeSignBDoc );
	buttonGroup->addButton( introBDocBack, IntroBDocBack );
	buttonGroup->addButton( introBDocNext, IntroBDocNext );
	buttonGroup->addButton( signBDocAddFile, SignBDocAddFile );
	buttonGroup->addButton( signBDocCancel, SignBDocCancel );
	buttonGroup->addButton( signBDocRemoveFile, SignBDocRemoveFile );
	buttonGroup->addButton( signBDocSaveAs, SignBDocSaveAs );
	buttonGroup->addButton( signBDocSign, SignBDocSign );
	buttonGroup->addButton( homeViewBDoc, HomeViewBDoc );
	buttonGroup->addButton( viewBDocAddSignature, ViewBDocAddSignature );
	buttonGroup->addButton( viewBDocBrowse, ViewBDocBrowse );
	buttonGroup->addButton( viewBDocClose, ViewBDocClose );
	buttonGroup->addButton( viewBDocEmail, ViewBDocEmail );
	buttonGroup->addButton( viewBDocPrint, ViewBDocPrint );
	buttonGroup->addButton( viewBDocSaveAs, ViewBDocSaveAs );
	connect( buttonGroup, SIGNAL(buttonClicked(int)),
		SLOT(buttonClicked(int)) );

	appTranslator = new QTranslator( this );
	qtTranslator = new QTranslator( this );
	QApplication::instance()->installTranslator( appTranslator );
	QApplication::instance()->installTranslator( qtTranslator );

	bdoc = new DigiDoc( this );
	connect( bdoc, SIGNAL(error(QString)), SLOT(showWarning(QString)) );
	connect( bdoc, SIGNAL(dataChanged()), SLOT(showCardStatus()) );

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
			if( bdoc->isNull() && f.suffix().toLower() == "bdoc" )
			{
				bdoc->open( f.absoluteFilePath() );
				setCurrentPage( View );
				return;
			}
			else if( !addFile( f.absoluteFilePath() ) )
				return;
		}
		if( !bdoc->isNull() )
			setCurrentPage( Sign );
	}
}

bool MainWindow::addFile( const QString &file )
{
	if( bdoc->isNull() )
	{
		QFileInfo info( file );
		QString doc = QString( "%1%2%3.bdoc" )
			.arg( SettingsValues().value( "Main/DefaultDir", info.absolutePath() ).toString() )
			.arg( QDir::separator() )
			.arg( info.fileName() );

		if( SettingsValues().value( "Main/AskSaveAs", false ).toBool() ||
			QFile::exists( doc ) )
		{
			doc = QFileDialog::getSaveFileName(
				this, tr("Save file"), doc, tr("Documents (*.bdoc)") );
			if( doc.isEmpty() )
				return false;
		}

		if( QFile::exists( doc ) )
			QFile::remove( doc );
		bdoc->create( doc );
	}
	bdoc->addFile( file );
	return true;
}

void MainWindow::buttonClicked( int button )
{
	switch( button )
	{
	case HomeSignBDoc:
		if( !SettingsValues().value( "Main/Intro", true ).toBool() )
		{
			introBDocCheck->setChecked( false );
			setCurrentPage( SignIntro );
			break;
		}
	case IntroBDocNext:
	case SignBDocAddFile:
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
		else if( bdoc->isNull() )
			setCurrentPage( Home );
		break;
	}
	case SignBDocCancel:
		if ( bdoc->documents().size() == 0 )
		{
			setCurrentPage( Home );
		}
		else
		{
			QMessageBox msgBox( QMessageBox::Question, tr("Save container"),
				tr("You added %1 files to container, but these are not signed yet.\n"
					"Should I keep unsigned documents or remove these?")
				.arg( bdoc->documents().size() ) );
			QPushButton *cancel = msgBox.addButton( tr("Remove"), QMessageBox::ActionRole );
			QPushButton *keep = msgBox.addButton( tr("Keep"), QMessageBox::ActionRole );
			msgBox.exec();

			if ( msgBox.clickedButton() == cancel )
			{
				bdoc->clear();
				setCurrentPage( Home );
			}
			else if ( msgBox.clickedButton() == keep )
				setCurrentPage( View );
		}
		break;
	case SignBDocRemoveFile:
	{
		QAbstractItemModel *m = signBDocDocsContentView->model();

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
				bdoc->removeDocument( i );
		}
		setCurrentPage( Sign );
		break;
	}
	case SignBDocSaveAs:
	{
		QString dir = QFileDialog::getExistingDirectory( this,
			tr("Select folder where files will be stored"),
			QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation ) );
		if( !dir.isEmpty() )
		{
			QAbstractItemModel *m = signBDocDocsContentView->model();
			for( int i = 0; i < m->rowCount(); ++i )
			{
				if( m->index( i, 0 ).data( Qt::CheckStateRole ) == Qt::Checked )
					bdoc->saveDocument( i, dir );
			}
		}
		break;
	}
	case SignBDocSign:
		if( !bdoc->sign(
				signCityInput->text(),
				signStateInput->text(),
				signZipInput->text(),
				signCountryInput->text(),
				signRoleInput->text(),
				signResolutionInput->text() ) )
			break;
		bdoc->save();
		Settings::saveSignatureInfo(
			signRoleInput->text(),
			signResolutionInput->text(),
			signCityInput->text(),
			signStateInput->text(),
			signZipInput->text(),
			signCountryInput->text() );
		setCurrentPage( View );
		break;
	case HomeViewBDoc:
	{
		QString file = QFileDialog::getOpenFileName( this, tr("Open container"),
			QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation ),
			tr("Documents (*.bdoc)") );
		if( !file.isEmpty() )
		{
			bdoc->open( file );
			setCurrentPage( View );
		}
		break;
	}
	case ViewBDocAddSignature:
		setCurrentPage( Sign );
		break;
	case ViewBDocBrowse:
		QDesktopServices::openUrl( QString( "file://%1" )
			.arg( QFileInfo( bdoc->fileName() ).absolutePath() ) );
		break;
	case IntroBDocBack:
	case ViewBDocClose:
		bdoc->clear();
		setCurrentPage( Home );
		break;
	case ViewBDocEmail:
	{
#ifdef Q_OS_WIN32
		QByteArray filePath = bdoc->fileName().toLatin1();
		QByteArray fileName = QFileInfo( bdoc->fileName() ).fileName().toLatin1();

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
			.arg( QFileInfo( bdoc->fileName() ).fileName() )
			.arg( bdoc->fileName() ) );
#endif
		break;
	}
	case ViewBDocPrint:
	{
		QPrinter printer;
		QPrintDialog *dialog = new QPrintDialog( &printer, this );
		dialog->setWindowTitle( tr("Print verification sheet") );
		if( dialog->exec() != QDialog::Accepted )
			break;
		QWebView *view = new QWebView();
		PrintSheet doc( bdoc );
		view->setHtml( doc.html() );
		view->print( &printer );
		view->deleteLater();
		break;
	}
	case ViewBDocSaveAs:
	{
		QString dir = QFileDialog::getExistingDirectory( this,
			tr("Select folder where files will be stored"),
			QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation ) );
		if( !dir.isEmpty() )
		{
			QAbstractItemModel *m = viewBDocDocsFrameView->model();
			for( int i = 0; i < m->rowCount(); ++i )
			{
				if( m->index( i, 0 ).data( Qt::CheckStateRole ) == Qt::Checked )
					bdoc->saveDocument( i, dir );
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
		 stack->currentIndex() == SignIntro ||
		 stack->currentIndex() == Sign) )
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
			else if( file.suffix().toLower() == "bdoc" && bdoc->isNull() )
			{
				bdoc->open( file.filePath() );
				setCurrentPage( View );
				return;
			}
			else if( !addFile( file.filePath() ) )
				return;
		}
	}
	setCurrentPage( Sign );
}

void MainWindow::loadDocuments( QTreeWidget *view )
{
	view->clear();
	QList<QTreeWidgetItem*> items;
	Q_FOREACH( const digidoc::Document &file, bdoc->documents() )
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

void MainWindow::on_introBDocCheck_stateChanged( int state )
{ SettingsValues().setValue( "Main/Intro", state == Qt::Checked ); }

void MainWindow::openFile( const QModelIndex &index )
{
	if( bdoc->isNull() )
		return;

	QList<digidoc::Document> list = bdoc->documents();
	if( list.isEmpty() || index.row() >= list.size() )
		return;

	QDesktopServices::openUrl( QString( "file://" ).append(
		QString::fromStdString( list[index.row()].getPath() ) ) );
}

void MainWindow::setCurrentPage( Pages page )
{
	stack->setCurrentIndex( page );
	switch( page )
	{
	case Sign:
	{
		loadDocuments( signBDocDocsContentView );

		SettingsValues s;
		s.beginGroup( "Main" );
		signRoleInput->setText( s.value( "Role" ).toString() );
		signResolutionInput->setText( s.value( "Resolution" ).toString() );
		signCityInput->setText( s.value( "City" ).toString() );
		signStateInput->setText( s.value( "State" ).toString() );
		signCountryInput->setText( s.value( "Country" ).toString() );
		signZipInput->setText( s.value( "Zip" ).toString() );
		s.endGroup();

		signBDocAddFile->setEnabled( bdoc->signatures().isEmpty() );
		signBDocSign->setEnabled(
			signBDocDocsContentView->model()->rowCount() > 0 && bdoc->signCert().isValid() );
		break;
	}
	case View:
	{
		loadDocuments( viewBDocDocsFrameView );

		Q_FOREACH( SignatureWidget *w, viewBDocSignersContent->findChildren<SignatureWidget*>() )
			w->deleteLater();

		int i = 0;
		bool cardOwnerSignature = false;
		Q_FOREACH( const DigiDocSignature &c, bdoc->signatures() )
		{
			SignatureWidget *signature = new SignatureWidget( c, i, viewBDocSignersContent );
			viewBDocSignersContentLayout->insertWidget( i, signature );
			connect( signature, SIGNAL(removeSignature(uint)),
				SLOT(viewBDocSignersRemove(unsigned int)) );
			if( !cardOwnerSignature )
			{
				cardOwnerSignature =
					(c.cert().subjectInfo( "serialNumber" ) == bdoc->signCert().subjectInfo( "serialNumber" ));
			}
			++i;
		}

		viewBDocFile->setText( tr("Container: <b>%1</b>").arg( bdoc->fileName() ) );

		if( i > 0 && cardOwnerSignature )
			viewBDocStatus->setText( tr("This container is signed by you") );
		else if( i > 0 && !cardOwnerSignature )
			viewBDocStatus->setText( tr("You have not signed this container") );
		else
			viewBDocStatus->setText( tr("Container is unsigned") );

		viewBDocAddSignature->setEnabled( bdoc->signCert().isValid() && !cardOwnerSignature );
		break;
	}
	default: break;
	}
}

void MainWindow::showCardStatus()
{
	QString info;
	if( !bdoc->authCert().isNull() )
	{
		info += tr("Person %1 %2 card in reader<br />Person SSID: %3<br />")
			.arg( parseName( parseCertInfo( bdoc->authCert().subjectInfo( "GN" ) ) ) )
			.arg( parseName( parseCertInfo( bdoc->authCert().subjectInfo( "SN" ) ) ) )
			.arg( bdoc->signCert().subjectInfo( "serialNumber") );

		info += tr("Sign certificate is valid until: %1<br />")
			.arg( bdoc->signCert().expiryDate().date().toString( Qt::SystemLocaleShortDate ) );
		if( !bdoc->signCert().isValid() ) info += tr("Sign certificate is expired<br />");

		info += tr("Auth certificate is valid until: %1<br />")
			.arg( bdoc->authCert().expiryDate().date().toString( Qt::SystemLocaleShortDate ) );
		if( !bdoc->authCert().isValid() ) info += tr("Auth certificate is expired<br />");
	}
	else
		info += tr("No card in reader<br />");

	cardInfo->setText( info );

	homeOpenUtility->setVisible(
		!bdoc->authCert().isNull() && !bdoc->signCert().isNull() &&
		(!bdoc->authCert().isValid() || !bdoc->signCert().isValid()) );

	signSignerLabel->setText( QString( "%1 %2 (%3)" )
		.arg( parseName( parseCertInfo( bdoc->signCert().subjectInfo( "GN" ) ) ) )
		.arg( parseName( parseCertInfo( bdoc->signCert().subjectInfo( "SN" ) ) ) )
		.arg( bdoc->signCert().subjectInfo( "serialNumber") ) );

	setCurrentPage( (Pages)stack->currentIndex() );
}

void MainWindow::showWarning( const QString &msg )
{ QMessageBox::warning( this, "QDigDocClient", msg ); }

void MainWindow::viewBDocSignersRemove( unsigned int num )
{
	bdoc->removeSignature( num );
	bdoc->save();
	setCurrentPage( bdoc->signatures().isEmpty() ? Sign : View );
}
