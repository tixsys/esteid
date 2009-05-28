/*
 * QEstEidUtil
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

#include <QString>
#include <QObject>
#include <QSslCertificate>

#include "cardlib/common.h"
#include "cardlib/EstEidCard.h"

class JsCertData : public QObject
{
    Q_OBJECT

public:
    JsCertData( QObject *parent );
	~JsCertData();

    enum CertType {
        AuthCert,
        SignCert
    };

	QSslCertificate cert() const;
    void loadCert(EstEidCard *card, CertType ct);

private:
    EstEidCard *m_card;
    QSslCertificate *m_qcert;

public slots:
    QString toPem();
    QString getEmail();
    QString getSubjCN();
    QString getSubjSN();
    QString getSubjO();
    QString getSubjOU();
    QString getValidFrom();
    QString getValidTo();
    QString getIssuerCN();
    QString getIssuerO();
    QString getIssuerOU();
	QString getSerialNum();

	bool	isTempel();
	bool	isValid();
};
