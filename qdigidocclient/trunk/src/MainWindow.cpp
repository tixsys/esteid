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

#include "Settings.h"

#include <digidoc/Document.h>

#include <QApplication>
#include <QDateTime>
#include <QDesktopServices>
#include <QDragEnterEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QSslCertificate>
#include <QTemporaryFile>
#include <QTextStream>
#include <QTranslator>
#include <QUrl>



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
	if( ret.size() > 0 )
		ret[0] = ret[0].toUpper();
	return ret;
}



MainWindow::MainWindow( QWidget *parent )
:	QWidget( parent )
{
	setupUi( this );
	homeOpenUtility->hide();

	QButtonGroup *buttonGroup = new QButtonGroup( this );
	buttonGroup->addButton( homeSignBDoc, HomeSignBDoc );
	buttonGroup->addButton( introBDocBack, IntroBDocBack );
	buttonGroup->addButton( introBDocNext, IntroBDocNext );
	buttonGroup->addButton( signBDocAddFile, SignBDocAddFile );
	buttonGroup->addButton( signBDocCancel, SignBDocCancel );
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
	connect( bdoc, SIGNAL(dataChanged()), SLOT(reload()) );

	comboLanguages->setItemData( 0, "et" );
	comboLanguages->setItemData( 1, "en" );
	comboLanguages->setItemData( 2, "ru" );
	comboLanguages->setItemData( 3, "de" );
	comboLanguages->setItemData( 4, "lv" );
	comboLanguages->setItemData( 5, "lt" );
	comboLanguages->setCurrentIndex(
		comboLanguages->findData( SettingsValues().value( "Main/Language", "et" ) ) );
	on_comboLanguages_activated( comboLanguages->currentIndex() );
}

void MainWindow::addFile()
{
	QStringList list = QFileDialog::getOpenFileNames( this, tr("Select documents"),
		QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation ) );
	if( !list.isEmpty() )
	{
		Q_FOREACH( const QString &file, list )
			addFile( file );
		setCurrentPage( Sign );
	}
	else if( bdoc->isNull() )
		setCurrentPage( Home );
}

void MainWindow::addFile( const QString &file )
{
	if( bdoc->isNull() )
	{
		QFileInfo info( file );
		bdoc->create( QString( "%1%2%3.bdoc" )
			.arg( SettingsValues().value( "Main/DefaultDir", info.absolutePath() ).toString() )
			.arg( QDir::separator() )
			.arg( info.fileName() ) );
	}
	bdoc->addFile( file );
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
		addFile();
		break;
	case IntroBDocBack:
		setCurrentPage( Home );
		break;
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
	case SignBDocSign:
		bdoc->sign(
			signCityInput->text(),
			signStateInput->text(),
			signZipInput->text(),
			signCountryInput->text(),
			signRoleInput->text(),
			signResolutionInput->text() );
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
	case ViewBDocClose:
		bdoc->clear();
		setCurrentPage( Home );
		break;
	case ViewBDocEmail:
	{
#ifdef Q_OS_WIN32
		QFile c( bdoc->fileName() );
		QTemporaryFile f( QDir::tempPath() + "/XXXXXX.eml" );
		f.setAutoRemove( false );
		if( f.open() && c.open( QIODevice::ReadOnly ) )
		{
			QTextStream s( &f );
			s << "MIME-Version: 1.0\n";
			s << "Subject: " + QFileInfo( bdoc->fileName() ).fileName() + "\n";
			s << "Content-Type: multipart/mixed;\n";
			s << " boundary=\"----12345678901234\"\n";
			s << "\n";
			s << "------12345678901234\n";
			s << "Content-Type: application/octet-stream;\n";
			s << " name=\"" + QFileInfo( bdoc->fileName() ).fileName() + "\"\n";
			s << "Content-Transfer-Encoding: base64\n";
			s << "Content-Disposition: attachment;\n";
			s << " filename=\"" + QFileInfo( bdoc->fileName() ).fileName() + "\n";
			s << "\n";
			s << c.readAll().toBase64() + "\n";
			s << "------12345678901234--\n";

			QString fileName = f.fileName();
			c.close();
			f.close();
			QDesktopServices::openUrl( fileName );
		}
		else
#endif
			QDesktopServices::openUrl( QString( "mailto:?subject=%1&attachment=%2" )
				.arg( QFileInfo( bdoc->fileName() ).fileName() )
				.arg( bdoc->fileName() ) );
		break;
	}
	case ViewBDocPrint:
		break;
	case ViewBDocSaveAs:
	{
		QString dir = QFileDialog::getExistingDirectory( this,
			tr("Select folder where files will be stored"),
			QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation ) );
		if( !dir.isEmpty() )
			bdoc->saveDocuments( dir );
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
			QString file = u.path().remove( 0, 1 );
#else
			QString file = u.path();
#endif
			if( QFileInfo( file ).suffix().toLower() == "bdoc" && bdoc->isNull() )
			{
				bdoc->open( file );
				setCurrentPage( View );
				return;
			}
			else
				addFile( file );
		}
	}
	setCurrentPage( Sign );
}

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

void MainWindow::reload() { showCardStatus(); }
void MainWindow::signBDocDocsRemove( unsigned int num )
{
	bdoc->removeDocument( num );
	setCurrentPage( Sign );
}

void MainWindow::setCurrentPage( Pages page )
{
	Q_FOREACH( DocumentWidget *w, signBDocDocsScrollContent->findChildren<DocumentWidget*>() )
		w->deleteLater();
	Q_FOREACH( SignatureWidget *w, viewBDocSignersContent->findChildren<SignatureWidget*>() )
		w->deleteLater();
	stack->setCurrentIndex( page );
	switch( page )
	{
	case Sign:
	{
		SettingsValues s;
		s.beginGroup( "Main" );
		signRoleInput->setText( s.value( "Role" ).toString() );
		signResolutionInput->setText( s.value( "Resolution" ).toString() );
		signCityInput->setText( s.value( "City" ).toString() );
		signStateInput->setText( s.value( "State" ).toString() );
		signCountryInput->setText( s.value( "Country" ).toString() );
		signZipInput->setText( s.value( "Zip" ).toString() );
		s.endGroup();

		unsigned int i = 0;
		Q_FOREACH( const digidoc::Document &file, bdoc->documents() )
		{
			DocumentWidget *doc = new DocumentWidget( file, i, signBDocDocsScrollContent );
			signBDocDocsScrollContentLayout->insertWidget( i, doc );
			connect( doc, SIGNAL(removeDocument(unsigned int)), SLOT(signBDocDocsRemove(unsigned int)) );
			++i;
		}
		signBDocAddFile->setEnabled( bdoc->signatures().count() == 0 );
		signBDocSign->setEnabled( i > 0 && bdoc->signCert().isValid() );
		break;
	}
	case View:
	{
		QString content;
		Q_FOREACH( const digidoc::Document &file, bdoc->documents() )
		{
			QFileInfo info( QString::fromStdString( file.getPath() ) );
			content += QString( "<b>%1</b><br />%2<br />")
				   .arg( info.fileName() )
				   .arg( fileSize( info.size() ) );
		}
		viewBDocDocsContent->setText( content );

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
					(c.cert().subjectInfo( "serialNumber") == bdoc->signCert().subjectInfo( "serialNumber" ));
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
		info += tr("Kasutaja %1 %2 kaart on lugejas<br />")
			.arg( parseName( parseCertInfo( bdoc->authCert().subjectInfo( "GN" ) ) ) )
			.arg( parseName( parseCertInfo( bdoc->authCert().subjectInfo( "SN" ) ) ) );
	}
	else
		info += tr("Kaarti pole lugejas<br />");

	info += tr("Kasutaja isikukood: %1<br />").arg( bdoc->signCert().subjectInfo( "serialNumber") );

	info += tr("Sign certificate is valid until: %1<br />")
		.arg( bdoc->signCert().expiryDate().date().toString( Qt::SystemLocaleShortDate ) );
	if( !bdoc->signCert().isValid() ) info += tr("Sign certificate is expired<br />");

	info += tr("Auth certificate is valid until: %4<br />")
		.arg( bdoc->authCert().expiryDate().date().toString( Qt::SystemLocaleShortDate ) );
	if( !bdoc->authCert().isValid() ) info += tr("Auth certificate is expired<br />");

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

void MainWindow::on_buttonSettings_clicked()
{ (new Settings( this ))->show(); }

void MainWindow::viewBDocSignersRemove( unsigned int num )
{
	bdoc->removeSignature( num );
	bdoc->save();
	setCurrentPage( View );
}



DocumentWidget::DocumentWidget( const digidoc::Document &file, unsigned int docnum, QWidget *parent )
:	QWidget( parent )
,	num( docnum )
{
	QLabel *content = new QLabel( this );
	QFileInfo info( QString::fromStdString( file.getPath() ) );
	content->setText( QString( "<b>%1</b><br />%2<br />" )
	   .arg( info.fileName() )
	   .arg( fileSize( info.size() ) ) );

	QPushButton *b = new QPushButton( tr("Remove"), this );
	connect( b, SIGNAL(clicked()), SLOT(clicked()) );

	QVBoxLayout *v = new QVBoxLayout( this );
	v->addWidget( content );
	v->addWidget( b, 0, Qt::AlignRight );
}

void DocumentWidget::clicked() { Q_EMIT removeDocument( num ); }



SignatureWidget::SignatureWidget( const DigiDocSignature &signature, unsigned int signnum, QWidget *parent )
:	QWidget( parent )
,	num( signnum )
,	s( signature )
{
	QLabel *c = new QLabel( this );
	c->setText( tr("<b>%1 %2</b><br />%3<br />Signed on %4<br />Signature is %5")
		.arg( parseName( parseCertInfo( s.cert().subjectInfo( "GN" ) ) ) )
		.arg( parseName( parseCertInfo( s.cert().subjectInfo( "SN" ) ) ) )
		.arg( s.location() )
		.arg( s.dateTime().toString( "dd. MMMM yyyy kell hh:mm" ) )
		.arg( s.isValid() ? tr("valid") : tr("not valid") ) );

	QPushButton *b = new QPushButton( tr("Show details"), this );
	connect( b, SIGNAL(clicked()), SLOT(showSignature()) );

	QPushButton *r = new QPushButton( tr("Remove"), this );
	connect( r, SIGNAL(clicked()), SLOT(removeSignature()) );

	QHBoxLayout *h = new QHBoxLayout();
	h->addItem( new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
	h->addWidget( b );
	h->addWidget( r );

	QVBoxLayout *v = new QVBoxLayout( this );
	v->addWidget( c );
	v->addLayout( h );
}

void SignatureWidget::removeSignature() { Q_EMIT removeSignature( num ); }

void SignatureWidget::showSignature()
{
	QMessageBox b( this );
	b.setText( tr("<b>%1 %2 %3</b><br />%4<br />Signed on %5<br />Signature is %6")
		.arg( parseName( parseCertInfo( s.cert().subjectInfo( "GN" ) ) ) )
		.arg( parseName( parseCertInfo( s.cert().subjectInfo( "SN" ) ) ) )
		.arg( s.cert().subjectInfo( "serialNumber") )
		.arg( s.location() )
		.arg( s.dateTime().toString( "dd. MMMM yyyy kell hh:mm" ) )
		.arg( s.isValid() ? tr("valid") : tr("not valid") ) );
	b.setWindowTitle( tr("Signature info") );
	b.setStandardButtons( QMessageBox::Close );
	b.exec();
}
