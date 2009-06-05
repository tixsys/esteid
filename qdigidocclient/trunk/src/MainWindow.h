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

#pragma once

#include "ui_MainWindow.h"

#include <QStringList>

class DigiDoc;
class QTranslator;

class MainWindow: public QWidget, private Ui::MainWindow
{
	Q_OBJECT

public:
	MainWindow( QWidget *parent = 0 );

private Q_SLOTS:
	void buttonClicked( int button );
	void on_introCheck_stateChanged( int state );
	void on_languages_activated( int index );
	void on_signMobile_toggled( bool checked );
	void on_settings_clicked();
	void openFile( const QModelIndex &index );
	void parseLink( const QString &link );
	void showCardStatus();
	void showWarning( const QString &msg );
	void viewAction( const QModelIndex &index );
	void viewSignaturesRemove( unsigned int num );

private:
	enum Pages {
		Home,
		Intro,
		Sign,
		View,
	};
	enum Buttons {
		UtilityOpen,
		HomeCrypt,
		HomeSign,
		IntroBack,
		IntroNext,
		SignAddFile,
		SignCancel,
		SignRemoveFile,
		SignSaveAs,
		SignSign,
		HomeView,
		ViewAddSignature,
		ViewClose,
	};
	bool addFile( const QString &file );
	void dragEnterEvent( QDragEnterEvent *e );
	void dropEvent( QDropEvent *e );
	void loadDocuments( QTreeWidget *view );
	void parseParams();
	void setCurrentPage( Pages page );

	DigiDoc	*doc;
	QTranslator *appTranslator, *qtTranslator;
	QHash<int,QString> lang;
	QStringList params;

	QPushButton *introNext, *signButton, *viewAddSignature;
};
