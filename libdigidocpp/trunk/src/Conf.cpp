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

#include "Conf.h"
#include "log.h"

//std::auto_ptr<digidoc::Conf> digidoc::Conf::INSTANCE = std::auto_ptr<digidoc::Conf>(new digidoc::Conf());
digidoc::Conf* digidoc::Conf::INSTANCE = NULL;

digidoc::Conf* digidoc::Conf::getInstance() throw(IOException)
{
    if (INSTANCE == NULL)
    {
        THROW_IOEXCEPTION("Conf is not initialized");
    }
	return INSTANCE;
}

///
/// Init global Conf with conf
/// @param conf implementation to use
///
void digidoc::Conf::init(Conf* conf)
{
    //INSTANCE = std::auto_ptr<Conf>(conf);
    INSTANCE = conf;
}

bool digidoc::Conf::isInitialized()
{
    return INSTANCE != NULL;
}

void digidoc::Conf::destroy()
{
    delete INSTANCE;
    INSTANCE = NULL;
}
