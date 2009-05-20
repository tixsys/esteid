/*
 * QDigiDocCrypto
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

#include "ui_MainWindow.h"

#include "CryptDoc.h"

class QTranslator;

class MainWindow: public QWidget, private Ui::MainWindow
{
	Q_OBJECT

public:
	MainWindow( QWidget *parent = 0 );

private Q_SLOTS:
	void addKeys( const QList<CKey> &keys );
	void buttonClicked( int button );
	void on_introCheck_stateChanged( int state );
	void on_languages_activated( int index );
	void on_settings_clicked();
	void on_viewContentView_doubleClicked( const QModelIndex &index );
	void showCardStatus();
	void removeKey( int id );
	void showWarning( const QString &msg, int err );

private:
	enum Pages {
		Home,
		Intro,
		View,
	};
	enum Buttons {
		HomeCreate,
		HomeOpenUtility,
		HomeView,
		IntroBack,
		IntroNext,
		ViewAddFile,
		ViewAddRecipient,
		ViewBrowse,
		ViewClose,
		ViewCrypt,
		ViewEmail,
		ViewRemoveFile,
		ViewSaveAs,
	};
	bool addFile( const QString &file );
	void closeEvent( QCloseEvent *e );
	void dragEnterEvent( QDragEnterEvent *e );
	void dropEvent( QDropEvent *e );
	bool saveDocument();
	void setCurrentPage( Pages page );

	CryptDoc	*doc;
	QTranslator *appTranslator, *qtTranslator;
	QHash<int,QString> lang;
};
