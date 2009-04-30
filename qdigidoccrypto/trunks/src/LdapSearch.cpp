/*
 * QDigiDocCrypto
 *
 * Copyright (C) 2009 Jargo Kster <jargo@innovaatik.ee>
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

#include "LdapSearch.h"

#include <QTimerEvent>

#ifndef Q_OS_WIN32
#define LDAP_TIMEVAL timeval
#endif

LdapSearch::LdapSearch( QObject *parent )
:	QObject( parent )
{
	ldap = ldap_init( "ldap.sk.ee", 389 );
	if( !ldap )
	{
		Q_EMIT error( tr("Failed to init ldap"), -1 );
		return;
	}

	int err = ldap_simple_bind_s( ldap, NULL, NULL );
	if( err )
		Q_EMIT error( tr("Failed to init ldap"), err );
}

LdapSearch::~LdapSearch() { ldap_unbind_s( ldap ); }

void LdapSearch::search( const QString &search )
{
	char *attrs[3] = { "cn", "userCertificate;binary", '\0' };

	int err = ldap_search_ext( ldap, "c=EE", LDAP_SCOPE_SUBTREE,
		search.toLatin1().data(), NULL, 0, NULL, NULL, NULL, 1, &msg_id );
	if( err )
		Q_EMIT error( tr("Failed to init ldap search"), err );

	startTimer( 5000 );
}

void LdapSearch::timerEvent( QTimerEvent *e )
{
	LDAPMessage *result;
	LDAP_TIMEVAL t;
	t.tv_sec = 5;
	t.tv_usec = 0;
	int err = ldap_result( ldap, msg_id, LDAP_MSG_ONE, &t, &result );
	if( err != LDAP_RES_SEARCH_ENTRY && err != LDAP_RES_SEARCH_RESULT )
	{
		Q_EMIT error( tr("Failed to get result"), err);
		return;
	}

	LDAPMessage *entry = ldap_first_entry( ldap, result );
	if( entry == NULL )
	{
		Q_EMIT error( tr("Empty result"), -1 );
		killTimer( e->timerId() );
		return;
	}

	char **name;
	berval **cert;
	BerElement *pos;
	char *attr = ldap_first_attribute( ldap, entry, &pos );
	do
	{
		if( qstrcmp( attr, "cn" ) == 0 )
			name = ldap_get_values( ldap, entry, attr );
		else if( qstrcmp( attr, "userCertificate;binary" ) == 0 )
			cert = ldap_get_values_len( ldap, entry, attr );
	}
	while( (attr = ldap_next_attribute( ldap, entry, pos ) ) );

	CKey key;
	key.certPem = QSslCertificate( QByteArray( cert[0]->bv_val, cert[0]->bv_len ), QSsl::Der ).toPem();
	key.recipient = QString::fromUtf8( name[0] );
	Q_EMIT searchResult( key );

	ldap_value_free( name );
	ldap_value_free_len( cert );
	ldap_msgfree( result );

	killTimer( e->timerId() );
}
