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

#ifdef WIN32
  #ifdef digidocpp_EXPORTS
    #define EXP_DIGIDOC __declspec(dllexport)
  #else
    #define EXP_DIGIDOC __declspec(dllimport)
  #endif
  #pragma warning( disable: 4251 ) // shut up std::vector warnings
#else
  #if __GNUC__ >= 4
    #define EXP_DIGIDOC __attribute__ ((visibility("default")))
  #else
    #define EXP_DIGIDOC
  #endif
#endif
