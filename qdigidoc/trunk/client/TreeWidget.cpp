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

#include "common/Common.h"

#include <digidocpp/Document.h>

#include <QDesktopServices>
#include <QFileInfo>
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

void TreeWidget::setContent( const QList<digidoc::Document> &docs )
{
	clear();
	Q_FOREACH( const digidoc::Document &file, docs )
	{
		QTreeWidgetItem *i = new QTreeWidgetItem( this );
		i->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled );

		QFileInfo info( QString::fromUtf8( file.getPath().data() ) );
		i->setText( 0, info.fileName() );
		i->setData( 0, Qt::ToolTipRole, info.fileName() );
		i->setData( 0, Qt::UserRole, info.absoluteFilePath() );

		i->setText( 1, Common::fileSize( info.size() ) );
		i->setData( 1, Qt::TextAlignmentRole, (int)Qt::AlignRight|Qt::AlignVCenter );
		i->setData( 1, Qt::ForegroundRole, Qt::gray );

		i->setData( 2, Qt::DecorationRole, QPixmap(":/images/ico_save.png") );
		i->setData( 2, Qt::ToolTipRole, tr("Save") );
		i->setData( 2, Qt::SizeHintRole, QSize( 20, 20 ) );

		i->setData( 3, Qt::DecorationRole, QPixmap(":/images/ico_delete.png") );
		i->setData( 3, Qt::ToolTipRole, tr("Remove") );
		i->setData( 3, Qt::SizeHintRole, QSize( 20, 20 ) );

		addTopLevelItem( i );
	}
}

Qt::DropActions TreeWidget::supportedDropActions() const
{ return Qt::CopyAction; }

QUrl TreeWidget::url( const QModelIndex &item ) const
{ return Common::toUrl( item.model()->index( item.row(), 0 ).data( Qt::UserRole ).toString() ); }
