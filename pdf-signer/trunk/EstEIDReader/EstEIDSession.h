/*
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

#ifndef EST_EID_SESSION_H_INCLUDED
#define EST_EID_SESSION_H_INCLUDED

#pragma once

class EstEIDSession
{
public:
    EstEIDSession(void);
    CK_RV Open(const char *module);
    CK_RV Close();
    bool Initialized() { return m_initialized != 0; } 
protected:    
    void *m_module;
    CK_FUNCTION_LIST_PTR m_p11;
    char m_initialized;
};


#endif // #ifndef EST_EID_SESSION_H_INCLUDED