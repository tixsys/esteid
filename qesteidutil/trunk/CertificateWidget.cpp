#include "CertificateWidget.h"

#include "SslCertificate.h"

#include <QDateTime>
#include <QTextStream>
#include <QSslKey>

CertificateWidget::CertificateWidget( QWidget *parent )
:	QWidget( parent )
{
	setupUi( this );
	setAttribute( Qt::WA_DeleteOnClose );
	setWindowFlags( Qt::Dialog );
	tabWidget->removeTab( 2 );
}

CertificateWidget::CertificateWidget( const QSslCertificate &cert, QWidget *parent )
:	QWidget( parent )
{
	setupUi( this );
	setAttribute( Qt::WA_DeleteOnClose );
	setWindowFlags( Qt::Dialog );
	setCertificate( cert );
}

QByteArray CertificateWidget::addHexSeparators( const QByteArray &data ) const
{
	QByteArray ret = data;
	for( int i = 2; i < ret.size(); i += 3 )
		ret.insert( i, ' ' );
	return ret;
}

void CertificateWidget::on_parameters_itemClicked( QTreeWidgetItem *item, int )
{
	if( !item->data( 1, Qt::UserRole ).isNull() )
		parameterContent->setPlainText( item->data( 1, Qt::UserRole ).toString() );
	else
		parameterContent->setPlainText( item->text( 1 ) );
}

void CertificateWidget::setCertificate( const QSslCertificate &cert )
{
	SslCertificate c = cert;
	QString i;
	QTextStream s( &i );
	s << "<b>Certificate Information:</b><br />";
	s << "<hr>";
	s << "<b>This certificate is intended for following purpose(s):</b><br />";
	s << "<br /><br /><br /><br />";
	s << "* Refer to the certification authority's statement for details.<br />";
	s << "<hr>";
	s << "<p style='margin-left: 30px;'>";
	s << "<b>Issued to:</b> " << c.subjectInfo( QSslCertificate::CommonName ) << "<br /><br /><br />";
	s << "<b>Issued by:</b> " << c.issuerInfo( QSslCertificate::CommonName ) << "<br /><br /><br />";
	s << "<b>Valid from</b> " << c.effectiveDate().toString( "dd.MM.yyyy" ) << " <b>to</b> "
		<< c.expiryDate().toString( "dd.MM.yyyy" );
	s << "</p>";
	info->setHtml( i );

	QTreeWidgetItem *t;

	t = new QTreeWidgetItem( parameters );
	t->setText( 0, "Version" );
	t->setText( 1, "V" + c.versionNumber() );
	parameters->addTopLevelItem( t );

	QByteArray key = c.publicKey().toDer().toHex();
	for( int i = 2; i < key.size(); i += 3 )
		key.insert( i, ' ' );
	t = new QTreeWidgetItem( parameters );
	t->setText( 0, "Serial number" );
	t->setText( 1, QString( "%1 (0x%2)" )
		.arg( c.serialNumber().constData() )
		.arg( QString::number( c.serialNumber().toInt(), 16 ) ) );
	parameters->addTopLevelItem( t );

	t = new QTreeWidgetItem( parameters );
	t->setText( 0, "Signature algorithm" );
	t->setText( 1, "sha1RSA" );
	parameters->addTopLevelItem( t );

	QString issuer;
	issuer += c.issuerInfo( "CN" ) + ", ";
	issuer += c.issuerInfo( "OU" ) + ", ";
	issuer += c.issuerInfo( "O" ) + ", ";
	issuer += c.issuerInfo( "C" );
	t = new QTreeWidgetItem( parameters );
	t->setText( 0, "Issuer" );
	t->setText( 1, issuer );
	parameters->addTopLevelItem( t );

	t = new QTreeWidgetItem( parameters );
	t->setText( 0, "Valid from" );
	t->setText( 1, c.effectiveDate().toString( "dd.MM.yyyy hh:mm:ss" ) );
	parameters->addTopLevelItem( t );

	t = new QTreeWidgetItem( parameters );
	t->setText( 0, "Vaild to" );
	t->setText( 1, c.expiryDate().toString( "dd.MM.yyyy hh:mm:ss" ) );
	parameters->addTopLevelItem( t );

	QString subject;
	subject += c.subjectInfo( "serialNumber" ) + ", ";
	subject += c.subjectInfo( "GN" ) + ", ";
	subject += c.subjectInfo( "SN" ) + ", ";
	subject += c.subjectInfo( "CN" ) + ", ";
	subject += c.subjectInfo( "OU" ) + ", ";
	subject += c.subjectInfo( "O" ) + ", ";
	subject += c.subjectInfo( "C" );
	t = new QTreeWidgetItem( parameters );
	t->setText( 0, "Subject" );
	t->setText( 1, subject );
	parameters->addTopLevelItem( t );

	t = new QTreeWidgetItem( parameters );
	t->setText( 0, "Public key" );
	t->setText( 1, "RSA(1024)" );
	t->setData( 1, Qt::UserRole, addHexSeparators( c.publicKey().toDer().toHex() ) );
	parameters->addTopLevelItem( t );
}
