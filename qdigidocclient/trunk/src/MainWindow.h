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

#include "DigiDoc.h"

class QTranslator;

class MainWindow: public QWidget, private Ui::MainWindow
{
	Q_OBJECT

public:
	MainWindow( QWidget *parent = 0 );

private Q_SLOTS:
	void buttonClicked( int button );
	void showCardStatus();
	void on_buttonSettings_clicked();
	void on_comboLanguages_activated( int index );
	void on_introBDocCheck_stateChanged( int state );
	void reload();
	void showWarning( const QString &msg );
	void viewBDocSignersRemove( unsigned int num );

private:
	enum Pages {
		Home,
		SignIntro,
		Sign,
		View,
	};
	enum Buttons {
		HomeSignBDoc,
		IntroBDocBack,
		IntroBDocNext,
		SignBDocAddFile,
		SignBDocCancel,
		SignBDocRemoveFile,
		SignBDocSaveAs,
		SignBDocSign,
		HomeViewBDoc,
		ViewBDocAddSignature,
		ViewBDocBrowse,
		ViewBDocClose,
		ViewBDocEmail,
		ViewBDocPrint,
		ViewBDocSaveAs,
	};
	void addFile();
	void addFile( const QString &file );
	void dragEnterEvent( QDragEnterEvent *e );
	void dropEvent( QDropEvent *e );
	void loadDocuments( QTreeWidget *view );
	void setCurrentPage( Pages page );

	DigiDoc	*bdoc;
	QTranslator *appTranslator;
	QTranslator *qtTranslator;
};

class SignatureWidget: public QWidget
{
	Q_OBJECT

public:
	SignatureWidget( const DigiDocSignature &s, unsigned int signnum, QWidget *parent = 0 );

Q_SIGNALS:
	void removeSignature( unsigned int num );

private Q_SLOTS:
	void removeSignature();
	void showSignature();

private:
	unsigned int num;
	DigiDocSignature s;
};
