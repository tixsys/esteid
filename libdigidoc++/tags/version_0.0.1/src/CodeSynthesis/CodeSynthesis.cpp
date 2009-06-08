// CodeSynthesis.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "XAdES.h"

using namespace std;

int _tmain( int argc, _TCHAR* argv[] )
{
	try
	{
		xml_schema::Properties p;
		p.schema_location( ds::XMLNS, "xmldsig-core-schema.xsd" );
		p.schema_location( xades::XMLNS, "XAdES.xsd" );

		auto_ptr<ds::Signature> h( ds::signature( "C:/develop/Smartlink/Docs/References/example_signature1.xml", 0, p ) );
		// do here something with parsed h
		int i = 0;
	}
	catch ( const xml_schema::Exception& e )
	{
		cerr << e << endl;
		return 1;
	}
	return 0;
}

