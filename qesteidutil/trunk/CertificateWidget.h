#pragma once

#include "ui_CertificateWidget.h"

class QSslCertificate;

class CertificateWidget: public QWidget, private Ui::CertificateWidget
{
	Q_OBJECT
public:
	CertificateWidget( QWidget *parent = 0 );
	CertificateWidget( const QSslCertificate &cert, QWidget *parent = 0 );

	void setCertificate( const QSslCertificate &cert );

private slots:
	void on_parameters_itemClicked( QTreeWidgetItem *item, int column );
	QByteArray addHexSeparators( const QByteArray &data ) const;
};
