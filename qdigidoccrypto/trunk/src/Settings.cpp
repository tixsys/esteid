/*
 * QDigiDocCrypto
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

#include "Settings.h"

#include <QDesktopServices>
#include <QFileDialog>

Settings::Settings( QWidget *parent )
:	QDialog( parent )
{
	setupUi( this );

	SettingsValues s;
	s.beginGroup( "Main" );

	defaultSameDir->setChecked( s.value( "SameDir", true ).toBool() );
	defaultDir->setText( s.value( "DefaultDir" ).toString() );
	showIntro->setChecked( s.value( "Intro", true ).toBool() );
	askSaveAs->setChecked( s.value( "AskSaveAs", false ).toBool() );
	s.endGroup();
}

void Settings::on_selectDefaultDir_clicked()
{
	QString dir = SettingsValues().value( "Main/DefaultDir" ).toString();
	if( dir.isEmpty() )
		dir = QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation );
	dir = QFileDialog::getExistingDirectory( this, tr("Select folder"), dir );
	if( !dir.isEmpty() )
	{
		SettingsValues().setValue( "Main/DefaultDir", dir );
		defaultDir->setText( dir );
	}
	defaultSameDir->setChecked( defaultDir->text().isEmpty() );
}

void Settings::save()
{
	SettingsValues s;
	s.beginGroup( "Main" );
	s.setValue( "Intro", showIntro->isChecked() );
	s.setValue( "AskSaveAs", askSaveAs->isChecked() );
	s.setValue( "SameDir", defaultSameDir->isChecked() );
	if( defaultSameDir->isChecked() )
	{
		defaultDir->clear();
		s.remove( "DefaultDir" );
	}
	s.endGroup();
}
