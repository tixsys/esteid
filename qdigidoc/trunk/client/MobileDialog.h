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

#pragma once

#include <QDialog>
#include <QDomElement>
#include <QHash>
#include <QHttp>
#include <QSslError>
#include <QTime>
#include <QTimer>

#include "ui_MobileDialog.h"

class DigiDoc;

class MobileDialog : public QDialog, private Ui::MobileDialog
{
	Q_OBJECT

public:
	MobileDialog( DigiDoc *doc, QWidget *parent = 0 );
	void setSignatureInfo( const QString &city, const QString &state, const QString &zip,
							const QString &country, const QString &role, const QString &role2 );
	void sign( const QString &ssid, const QString &cell );
	QString fName;

private Q_SLOTS:
	void sslErrors(const QList<QSslError> &);
	void httpRequestFinished( int id, bool error );

	void startSessionResult( const QDomElement &element );
	void getSignStatusResult( const QDomElement &element );
	void getSignStatus();
	void updateStatus();

private:
	enum MobileAction {
		MobileCreateSignature,
		GetMobileCreateSignatureStatus
	};

	bool getFiles();
	QString escapeChars( const QString &in ) const;
	QString insertBody( MobileAction action, const QString &body ) const;
	void startSession();

	DigiDoc *m_doc;
	QHttp *m_http;
	QTimer *m_timer, *statusTimer;
	QTime startTime;
	QString signature, files;

	int sessionCode;
	QHash< int, QByteArray > m_callBackList;
	QHash< QByteArray, QString > lang, mobileResults;
};
