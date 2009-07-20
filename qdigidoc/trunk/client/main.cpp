/*
 * QDigiDocClient
 *
 * Copyright (C) 2009 Jargo K�ster <jargo@innovaatik.ee>
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
#include "RegisterP12.h"
#include "version.h"

#include <QApplication>

#include <openssl/ssl.h>

int main( int argc, char *argv[] )
{
	SSL_load_error_strings();
	SSL_library_init();

	QApplication a( argc, argv );
	a.setApplicationName( APP );
	a.setApplicationVersion( VER_STR( FILE_VER_DOT ) );
	a.setOrganizationDomain( DOMAINURL );
	a.setOrganizationName( ORG );
	a.setStyleSheet(
		"QDialogButtonBox { dialogbuttonbox-buttons-have-icons: 0; }\n"
		"* { font: 12px \"Arial, Liberation Sans\"; }"
	);
	QPalette p = a.palette();
	p.setBrush( QPalette::Link, QBrush( "#509B00" ) );
	p.setBrush( QPalette::LinkVisited, QBrush( "#509B00" ) );
	a.setPalette( p );

	int ret = 0;
	QStringList args = a.arguments();
	if( args.size() > 1 && args.value(1).right(4) == "p12d" )
	{
		RegisterP12 *w = new RegisterP12( args.value(1) );
		w->show();
		ret = a.exec();
		delete w;
	}
	else
	{
		MainWindow *w = new MainWindow();
		if( w->isLoaded() )
		{
			w->show();
			ret = a.exec();
		}
		delete w;
	}

	return ret;
}
