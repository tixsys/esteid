/*
 * QEstEidUtil
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

#include "SettingsDialog.h"
#include "common/Settings.h"

SettingsDialog::SettingsDialog( QWidget *parent )
:	QDialog( parent )
{
	setupUi( this );
	setAttribute( Qt::WA_DeleteOnClose, true );

	Settings s;
	s.beginGroup( "Util" );
	sessionTime->setValue( s.value( "sessionTime", 0 ).toInt() );

#ifdef WIN32
	int interval = updateInterval->findText( s.value( "updateInterval" ).toString() );
	if ( interval == -1 )
		interval = 0;
	updateInterval->setCurrentIndex( interval );
	autoUpdate->setChecked( s.value( "autoUpdate", true ).toBool() );
#else
	updateInterval->hide();
	updateIntervalLabel->hide();
	autoUpdate->hide();
	autoUpdateLabel->hide();
#endif
}

void SettingsDialog::accept()
{
	Settings s;
	s.beginGroup( "Util" );
	s.setValue( "sessionTime", sessionTime->value() );
	
	s.setValue( "updateInterval", updateInterval->currentText() );
	s.setValue( "autoUpdate", autoUpdate->isChecked() );

	done( 1 );
}
