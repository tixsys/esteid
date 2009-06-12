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

#include "TreeWidget.h"

#include <QDesktopServices>
#include <QHeaderView>
#include <QMimeData>
#include <QUrl>

TreeWidget::TreeWidget( QWidget *parent )
:	QTreeWidget( parent )
{
	setColumnCount( 4 );
	header()->setStretchLastSection( false );
	header()->setResizeMode( 0, QHeaderView::Stretch );
	header()->setResizeMode( 1, QHeaderView::ResizeToContents );
	header()->setResizeMode( 2, QHeaderView::ResizeToContents );
	header()->setResizeMode( 3, QHeaderView::ResizeToContents );
	connect( this, SIGNAL(doubleClicked(QModelIndex)),
		SLOT(openFile(QModelIndex)) );
}

QMimeData* TreeWidget::mimeData( const QList<QTreeWidgetItem*> items ) const
{
	QList<QUrl> list;
	Q_FOREACH( QTreeWidgetItem *item, items )
		list << url( indexFromItem( item ) );
	QMimeData *data = new QMimeData();
	data->setUrls( list );
	return data;
}

QStringList TreeWidget::mimeTypes() const
{ return QStringList() << "text/uri-list"; }

void TreeWidget::openFile( const QModelIndex &index )
{ QDesktopServices::openUrl( url( index ) ); }

Qt::DropActions TreeWidget::supportedDropActions() const
{ return Qt::CopyAction; }

QString TreeWidget::url( const QModelIndex &item ) const
{
#ifdef Q_OS_WIN32
	QString url( "file:///" );
#else
	QString url( "file://" );
#endif
	url += item.model()->index( item.row(), 0 ).data( Qt::UserRole ).toString();
	return url;
}
