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

#include <iostream>
#include <QDebug>
#include <QDateTime>
#include <QStringList>

#include "jscertdata.h"
#include "SslCertificate.h"

using namespace std;

JsCertData::JsCertData( QObject *parent )
:	QObject( parent )
{
    m_card = NULL;
	m_qcert = NULL;
}

JsCertData::~JsCertData()
{
	if( m_qcert )
		delete m_qcert;
}

QSslCertificate JsCertData::cert() const { return *m_qcert; }

void JsCertData::loadCert(EstEidCard *card, CertType ct)
{
    m_card = card;

    if (!m_card) {
        qDebug("No card");
        return;
    }

    std::vector<std::string> tmp;
    ByteVec certBytes;
    try {
        // Read certificate
        if (ct == AuthCert)
            certBytes = m_card->getAuthCert();
        else
            certBytes = m_card->getSignCert();

		if( m_qcert )
		{
			delete m_qcert;
			m_qcert = 0;
		}
        m_qcert = new QSslCertificate(QByteArray((char *)&certBytes[0], certBytes.size()), QSsl::Der);
    } catch (runtime_error &err ) {
//        doShowError(err);
        cout << "Error: " << err.what() << endl;
    }
}

QString JsCertData::toPem()
{
    if (!m_qcert)
        return "";

    return QString(m_qcert->toPem());
}

QString JsCertData::getEmail()
{
    if (!m_qcert)
        return "";

    QStringList mailaddresses = m_qcert->alternateSubjectNames().values(QSsl::EmailEntry);

    // return first email address
    if (!mailaddresses.isEmpty())
        return mailaddresses.first();
    else
        return "";
}

QString JsCertData::getSubjCN()
{
    if (!m_qcert)
        return "";

    return m_qcert->subjectInfo(QSslCertificate::CommonName);
}

QString JsCertData::getSubjO()
{
    if (!m_qcert)
        return "";

    return m_qcert->subjectInfo(QSslCertificate::Organization);
}

QString JsCertData::getSubjOU()
{
    if (!m_qcert)
        return "";

    return m_qcert->subjectInfo(QSslCertificate::OrganizationalUnitName);
}

QString JsCertData::getValidFrom()
{
    if (!m_qcert)
        return "";

    return m_qcert->effectiveDate().toString("dd. MMMM yyyy");
}

QString JsCertData::getValidTo()
{
    if (!m_qcert)
        return "";

    return m_qcert->expiryDate().toString("dd. MMMM yyyy");
}

QString JsCertData::getIssuerCN()
{
    if (!m_qcert)
        return "";

    return m_qcert->issuerInfo(QSslCertificate::CommonName);
}

QString JsCertData::getIssuerO()
{
    if (!m_qcert)
        return "";

    return m_qcert->issuerInfo(QSslCertificate::Organization);
}

QString JsCertData::getIssuerOU()
{
    if (!m_qcert)
        return "";

    return m_qcert->issuerInfo(QSslCertificate::OrganizationalUnitName);
}

bool JsCertData::isTempel()
{
	if (!m_qcert)
		return false;

	return SslCertificate( *m_qcert ).isTempel();
}

bool JsCertData::isValid()
{
    if (!m_qcert)
        return false;

	return m_qcert->expiryDate() >= QDateTime::currentDateTime();
}
