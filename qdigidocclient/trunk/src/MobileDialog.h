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
#include <QHttp>
#include <QSslError>
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

private Q_SLOTS:
    void on_buttonNext_clicked();
    void sslErrors(const QList<QSslError> &);
    void httpRequestFinished( int id, bool error );

    void startSessionResult( const QDomElement &element );
    void createSignedDocResult( const QDomElement &element );
    void addFilesResult( const QDomElement &element );
    void startSignResult( const QDomElement &element );
    void getSignStatus();
    void getSignStatusResult( const QDomElement &element );
    void getSignedDocResult( const QDomElement &element );

private:
    enum Pages {
            Start,
            Challenge,
    };
    enum MobileAction {
        StartSession,
        CreateSignedDoc,
        AddDataFile,
        StartSign,
        SignStatus,
        GetSignedDoc,
        CloseSession
    };

    DigiDoc *m_doc;
    QHttp *m_http;
    QTimer *m_timer;
	QString signature;

    int addedFiles;
    int sessionCode;
    QHash< int, QByteArray > m_callBackList;

    void startSession();
    void createSignedDoc();
    void addFiles();
    void startSign();
    void getSignedDoc();

    QByteArray insertBody( MobileAction action, const QByteArray &body ) const;
};
