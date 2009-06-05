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

#include "QMobileSigner.h"
#include "DigiDoc.h"

#include <digidoc/WDoc.h>

#include <QDebug>

using namespace digidoc;

QMobileSigner::QMobileSigner( const digidoc::WDoc &w ) throw(SignException)
:	wdoc( w )
,	signature( 0 )
{
	try
	{
		unsigned int count = wdoc.signatureCount();
		for( unsigned int i = 0; i < count; ++i )
			signature = wdoc.getSignature( i );
	}
	catch( const Exception &e ) {
		qDebug() << e.getMsg().data();
	}
	if ( signature )
	{
		setSignatureProductionPlace( signature->getProductionPlace() );
		setSignerRole( signature->getSignerRole() );
	}
}

X509* QMobileSigner::getCert() throw(SignException)
{
	if ( !signature )
		return NULL;
	return signature->getSigningCertificate().getX509();
}

void QMobileSigner::sign( const Digest &, Signature &s ) throw(SignException)
{
	std::vector<unsigned char> sign = signature->getSignatureValue();
	memcpy(s.signature, &sign[0], sign.size());
	s.length = sign.size();
}
