/*
 * QEstEidUtil
 *
 * Copyright (C) 2009-2010  Estonian Informatics Centre
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

#include <qtsingleapplication.h>
#include <openssl/ssl.h>

#include "mainwindow.h"
#include "version.h"

int main(int argc, char *argv[])
{
	QtSingleApplication app(argc, argv);
	app.setApplicationName( APP );
	app.setApplicationVersion( VER_STR( FILE_VER_DOT ) );
	app.setOrganizationDomain( DOMAINURL );
	app.setOrganizationName( ORG );

	if( app.isRunning() )
	{
		app.activateWindow();
		return 0;
	}

	SSL_load_error_strings();
	SSL_library_init();

	MainWindow w;
	app.setActivationWindow( &w );
    w.show();
    return app.exec();
}
