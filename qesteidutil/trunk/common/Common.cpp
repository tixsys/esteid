/*
 * QEstEidCommon
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

#include "Common.h"

QString Common::fileSize( quint64 bytes )
{
	const quint64 kb = 1024;
	const quint64 mb = 1024 * kb;
	const quint64 gb = 1024 * mb;
	const quint64 tb = 1024 * gb;
	if( bytes >= tb )
		return QString( "%1 TB" ).arg( qreal(bytes) / tb, 0, 'f', 3 );
	if( bytes >= gb )
		return QString( "%1 GB" ).arg( qreal(bytes) / gb, 0, 'f', 2 );
	if( bytes >= mb )
		return QString( "%1 MB" ).arg( qreal(bytes) / mb, 0, 'f', 1 );
	if( bytes >= kb )
		return QString( "%1 KB" ).arg( bytes / kb );
	return QString( "%1 B" ).arg( bytes );
}
