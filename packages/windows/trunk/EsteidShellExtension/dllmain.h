/*
 * EsteidShellExtension - Estonian EID card Windows Explorer integration
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

// dllmain.h : Declaration of module class.

class CEsteidShellExtensionModule : public CAtlDllModuleT< CEsteidShellExtensionModule >
{
public :
	DECLARE_LIBID(LIBID_EsteidShellExtensionLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_ESTEIDEXT, "{08BD50C2-0AFC-4ED0-BC78-78488EDF9E58}")
};

extern class CEsteidShellExtensionModule _AtlModule;
