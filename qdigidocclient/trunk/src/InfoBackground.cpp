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

#include "InfoBackground.h"

#include <QPainter>
#include <QStyleOption>

InfoBackground::InfoBackground( QWidget *parent )
:	QWidget( parent )
{}

void InfoBackground::paintEvent( QPaintEvent * )
{
	QStyleOption o;
	o.initFrom( this );
	QPainter p( this );

	p.save();
	p.setOpacity( 0.6 );
	p.setPen( Qt::NoPen );
	p.setBrush( Qt::white );
	p.drawRoundedRect( o.rect, 5, 5 );
	p.restore();
}
