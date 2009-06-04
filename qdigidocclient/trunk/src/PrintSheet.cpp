/*
 * QDigiDocCrypt
 *
 * Copyright (C) 2009 Jargo Kster <jargo@innovaatik.ee>
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

#include "PrintSheet.h"

#include "DigiDoc.h"
#include "SslCertificate.h"

#include <digidoc/Document.h>

#include <QDateTime>
#include <QFileInfo>
#include <QTextStream>

PrintSheet::PrintSheet( DigiDoc *d, QWidget *parent )
:	QWebView( parent )
{
	QString html;
	QTextStream s( &html );
	s
	<< "<html>"
	<< "<head>"

	<< "<style type=\"text/css\">"
	<< "#head {"
	<< "font-family: Arial, Helvetica, sans-serif;"
	<< "font-size: 28px;"
	<< "border-bottom: 2px solid #000000;"
	<< "margin-top: 10px;"
	<< "}"
	<< ".sectionHead {"
	<< "font-family: Arial, Helvetica, sans-serif;"
	<< "font-size: 18px;"
	<< "border-bottom: 1px dashed #000000;"
	<< "margin-top: 20px;"
	<< "}"
	<< ".label { font-size: 11px; padding: 2px; }"
	<< ".label, TABLE { margin-top: 5px; }"
	<< ".text, .textborder, .textborderright { font-size: 12px; padding: 3px; }"
	<< ".textborder, .textborderright { border: 1px solid #000000; font-weight: bold; }"
	<< ".textborderright { border-left: 0px; }"
	<< "</style>"

	<< "</head>"
	<< "<body>"

	<< "<div id=\"head\">DIGITAALKIRJADE KINNITUSLEHT</div>"
	<< "<div class=\"sectionHead\">ALLKIRJASTATUD FAILID</div>"

	<< "<table width=\"100%\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">"
	<< "<tr>"
	<< "<td class=\"label\" width=\"300\">FAILI NIMI</td>"
	<< "<td class=\"label\" width=\"200\">FAILI TÜÜP</td>"
	<< "<td class=\"label\" width=\"100\">FAILI SUURUS</td>"
	<< "</tr>";

	Q_FOREACH( const digidoc::Document &doc, d->documents() )
	{
		s
		<< "<tr>"
		<< "<td class=\"textborder\">"
		<< QFileInfo( QString::fromUtf8( doc.getPath().data() ) ).fileName()
		<< "</td>"
		<< "<td class=\"textborderright\">"
		<< QString::fromUtf8( doc.getMediaType().data() )
		<< "</td>"
		<< "<td class=\"textborderright\">"
		<< doc.getSize()
		<< "</td>"
		<< "</tr>";
	}

	s
	<< "</table>"
	<< "<div class=\"sectionHead\">ALLKIRJASTAJAD</div>";

	int i = 1;
	Q_FOREACH( DigiDocSignature sig, d->signatures() )
	{
		const SslCertificate cert = sig.cert();
		s
		<< "<table width=\"100%\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">"
		<< "<tr>"
		<< "<td class=\"label\" width=\"40\">nr.</td>"
		<< "<td class=\"label\">NIMI</td>"
		<< "<td class=\"label\" width=\"200\">ISIKUKOOD</td>"
		<< "<td class=\"label\" width=\"160\">AEG</td>"
		<< "</tr>"
		<< "<tr>"
		<< "<td class=\"textborder\">" << i << "</td>"
		<< "<td class=\"textborderright\">"
		<< SslCertificate::formatName( cert.subjectInfoUtf8( "GN" ) ) << " "
		<< SslCertificate::formatName( cert.subjectInfoUtf8( "SN" ) )
		<< "</td>"
		<< "<td class=\"textborderright\">"
		<< cert.subjectInfo( "serialNumber" )
		<< "</td>"
		<< "<td class=\"textborderright\">"
		<< sig.dateTime().toString( "dd.MM.yyyy hh:mm:ss" )
		<< "</td>"
		<< "</tr>"
		<< "</table>"

		<< "<div class=\"label\">ALLKIRJA KEHTIVUS</div>"
		<< "<div class=\"textborder\">ALLKIRI " << (sig.isValid() ? "KEHTIB" : "EI KEHTI") << "</div>"

		<< "<div class=\"label\">ROLL/RESOLUTSIOON</div>"
		<< "<div class=\"textborder\">" << sig.role() << "</div>"

		<< "<table width=\"100%\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">"
		<< "<tr>"
		<< "<td class=\"label\">ALLKIRJASTAJA ASUKOHT (LINN, MAAKOND, INDEKS, RIIK)</td>"
		<< "<td class=\"label\" width=\"200\">SERTIFIKAADI SEERIANUMBER</td>"
		<< "</tr>"
		<< "<td class=\"textborder\">" << sig.location() << "</td>"
		<< "<td class=\"textborderright\">" << cert.serialNumber() << "</td>"
		<< "</table>"

		<< "<table width=\"100%\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">"
		<< "<tr>"
		<< "<td class=\"label\" width=\"185\">SERTIFIKAADI VÄLJAANDJA NIMI</td>"
		<< "<td class=\"label\">SERTIFIKAADI VÄLJAANDJA AVALIKU VÕTME LÜHEND</td>"
		<< "</tr>"
		<< "<td class=\"textborder\">" << cert.issuerInfo( QSslCertificate::CommonName ) << "</td>"
		<< "<td class=\"textborderright\"></td>"
		<< "</table>"

		<< "<div class=\"label\">KEHTIVUSKINNITUSE SÕNUMILÜHEND</div>"
		<< "<div class=\"textborder\"> </div>";

		++i;
	}

	s
	<< "<div class=\"text\" style=\"margin-top: 10px\">Selle kinnituslehe lahutamatu osa on lõigus "
	<< "<b>Allkirjastatud failid</b> nimetatud failide esitus paberil</div>"
	<< "<div class=\"label\" style=\"margin-top: 20px\">MÄRKUSED</div>"
	<< "<div class=\"textborder\" style=\"height: 100px\"></div>"
	<< "</body>"
	<< "</html>";

	setHtml( html );
}
