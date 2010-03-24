/*
 * esteid-browser-plugin - a browser plugin for Estonian EID card
 *
 * Copyright (C) 2010  Estonian Informatics Centre
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

#ifndef _WIN32
#include <stdlib.h>
#endif

#include <algorithm>
#include <fstream>

#include "PluginSettings.h"
#include "config.h"

PluginSettings::PluginSettings() :
    allowLocal(true), allowDefaults(true)
{
    FindConfig();
    Load();

    /* Builtin whitelist */
    default_whitelist.clear();
    default_whitelist.push_back("id.swedbank.ee");
    default_whitelist.push_back("id.seb.ee");
    default_whitelist.push_back("id.eesti.ee");
}


void PluginSettings::FindConfig() {
#ifdef _WIN32
    std::string appdata(getenv("APPDATA"));
    fileName = appdata + "\\" FBSTRING_PluginName "plugin.conf";
#else // UNIX
    std::string home(getenv("HOME"));
    fileName = home + "/.config/" FBSTRING_PluginName "plugin.conf";
#endif
}

PluginSettings::~PluginSettings() {
}

void PluginSettings::Load() {
    whitelist.clear();

    std::string line;
    std::ifstream input(fileName.c_str());
    while(input.good()) {
        std::getline(input, line);
        if(line.empty()) continue;

             if(line == "@NODEFAULTS") allowDefaults = false;
        else if(line == "@NOLOCAL")    allowLocal    = false;
        else whitelist.push_back(line);
    }
    input.close();
}

void PluginSettings::Save() {
    std::vector<std::string>::const_iterator i;

    std::ofstream output;
    output.exceptions( std::ofstream::failbit | std::ofstream::badbit );

    output.open(fileName.c_str());
    if(!allowDefaults) output << "@NODEFAULTS" << std::endl;
    if(!allowLocal)    output << "@NOLOCAL"    << std::endl;
    for(i = whitelist.begin(); i != whitelist.end(); ++i)
        output << *i << std::endl;
    output.close();
}

bool PluginSettings::InWhitelist(std::string s) {
    return (allowDefaults && \
            default_whitelist.end() != find(default_whitelist.begin(), default_whitelist.end(), s)) || \
            whitelist.end() != find(whitelist.begin(), whitelist.end(), s);
}
