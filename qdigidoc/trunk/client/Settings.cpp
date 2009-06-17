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

#include "Settings.h"

#include <digidocpp/XmlConf.h>

#include "version.h"

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

	const QString type = s.value( "type", "bdoc" ).toString();
	typeBDoc->setChecked( type == "bdoc" );
	typeDDoc->setChecked( type == "ddoc" );

	signRoleInput->setText( s.value( "Role" ).toString() );
	signResolutionInput->setText( s.value( "Resolution" ).toString() );
	signCityInput->setText( s.value( "City" ).toString() );
	signStateInput->setText( s.value( "State" ).toString() );
	signCountryInput->setText( s.value( "Country" ).toString() );
	signZipInput->setText( s.value( "Zip" ).toString() );

	signOverwrite->setChecked( s.value( "Overwrite", false ).toBool() );

	proxyHost->setText( s.value( "proxyHost" ).toString() );
	proxyPort->setText( s.value( "proxyPort" ).toString() );
	p12Cert->setText( s.value( "pkcs12Cert" ).toString() );
	p12Pass->setText( s.value( "pkcs12Pass" ).toString() );

	s.endGroup();
}

void Settings::on_p12Button_clicked()
{
	QString cert = SettingsValues().value( "Main/pkcs12Cert" ).toString();
	if( cert.isEmpty() )
		cert = QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation );
	cert = QFileDialog::getOpenFileName( this, tr("Select PKCS#12 certificate"), cert,
		tr("PKCS#12 Certificates (*.p12 *.p12d)") );
	if( !cert.isEmpty() )
	{
		SettingsValues().setValue( "Main/pkcs12Cert", cert );
		p12Cert->setText( cert );
	}
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
	s.setValue( "Overwrite", signOverwrite->isChecked() );
	s.setValue( "AskSaveAs", askSaveAs->isChecked() );
	s.setValue( "SameDir", defaultSameDir->isChecked() );
	s.setValue( "type", typeBDoc->isChecked() ? "bdoc" : "ddoc" );
	if( defaultSameDir->isChecked() )
	{
		defaultDir->clear();
		s.remove( "DefaultDir" );
	}
	s.setValue( "proxyHost", proxyHost->text() );
	s.setValue( "proxyPort", proxyPort->text() );
	s.setValue( "pkcs12Cert", p12Cert->text() );
	s.setValue( "pkcs12Pass", p12Pass->text() );
	digidoc::XmlConf::getInstance()->setProxyPort( s.value( "proxyPort" ).toString().toStdString() );
	digidoc::XmlConf::getInstance()->setProxyHost( s.value( "proxyHost" ).toString().toStdString() );
	digidoc::XmlConf::getInstance()->setPKCS12Cert( s.value( "pkcs12Cert" ).toString().toStdString() );
	digidoc::XmlConf::getInstance()->setPKCS12Pass( s.value( "pkcs12Pass" ).toString().toStdString() );

	s.endGroup();

	saveSignatureInfo(
		signRoleInput->text(),
		signResolutionInput->text(),
		signCityInput->text(),
		signStateInput->text(),
		signCountryInput->text(),
		signZipInput->text(),
		true );
}

void Settings::saveSignatureInfo(
		const QString &role,
		const QString &resolution,
		const QString &city,
		const QString &state,
		const QString &country,
		const QString &zip,
		bool force )
{
	SettingsValues s;
	s.beginGroup( "Main" );
	if( force || s.value( "Overwrite", "false" ).toBool() )
	{
		s.setValue( "Role", role );
		s.setValue( "Resolution", resolution );
		s.setValue( "City", city );
		s.setValue( "State", state ),
		s.setValue( "Country", country );
		s.setValue( "Zip", zip );
	}
	s.endGroup();
}

void Settings::saveMobileInfo( const QString &code, const QString &number )
{
	SettingsValues s;
	s.beginGroup( "Main" );
	if( s.value( "Overwrite", "false" ).toBool() )
	{
		s.setValue( "MobileCode", code );
		s.setValue( "MobileNumber", number );
	}
	s.endGroup();
}

SettingsValues::SettingsValues( QObject *parent )
:	QSettings( QSettings::NativeFormat, QSettings::UserScope, ORG, APP, parent )
{}
