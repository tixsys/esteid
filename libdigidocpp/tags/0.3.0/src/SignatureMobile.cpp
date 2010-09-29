/*
 * libdigidocpp - a C++ library for creating and validating BDOC-1.0 documents
 *
 * Copyright (C) 2009-2010  Estonian Informatics Centre
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
 */

#include "SignatureMobile.h"

#include "log.h"

using namespace digidoc;

SignatureMobile::SignatureMobile(const std::string &p, BDoc& _bdoc)
: SignatureTM(p,_bdoc)
{}

SignatureMobile::~SignatureMobile()
{}

OCSP::CertStatus SignatureMobile::validateOnline() const throw(SignatureException)
{
	return OCSP::GOOD;
}
