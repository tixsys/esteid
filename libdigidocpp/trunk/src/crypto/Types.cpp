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

#include "Types.h"

#include <algorithm>
#include <sstream>
#include <iomanip>
#include <stdexcept>

/* Helper functions lifted from
 * esteid-browser-plugin/trunk/utility/converters.cpp
 *
 * TODO: Make more generic converter namespace out of those,
 *       review all code for places where these could be useful
 *       and move these under util 
 */
typedef std::vector<unsigned char> ByteVec;

static bool getOneChar(ByteVec &hexStr,int &c) {
        if (hexStr.empty()) return false;
        int ch = hexStr.back();
        hexStr.pop_back();
        while (ch == ' ' || ch == '\r' || ch == '\t' || ch == '\n') {
                if (hexStr.empty()) return false;
                ch = hexStr.back();
                hexStr.pop_back();
        }
        if (ch >= '0' && ch <= '9')
                c = ch - '0';
        else if (ch >= 'A' && ch <= 'F')
                c = ch - 'A' + 10;
        else if (ch >= 'a' && ch <= 'f')
                c = ch - 'a' + 10;
        return true;
}

static ByteVec fromHex(const std::string &hexStr) {
        ByteVec retVal,inpVal(hexStr.length(),'\0');
        copy(hexStr.begin(),hexStr.end(),inpVal.begin());
        reverse(inpVal.begin(),inpVal.end());
        int c1,c2;
        while(getOneChar(inpVal,c1) && getOneChar(inpVal,c2)) {
                retVal.push_back(       (c1 << 4)  | c2 );
        }
        return retVal;
}

static std::string toHex(const std::vector<unsigned char> &byteVec) {
        std::ostringstream buf;
        for(ByteVec::const_iterator it = byteVec.begin();it!=byteVec.end();it++)
                buf << std::setfill('0') << std::setw(2) <<std::hex <<
                (short) *it;
        return buf.str();
}

digidoc::SignatureValue::SignatureValue(const std::string& sigHex)
{
    // FIXME: Support other signature types in format {OpenSSL SN}hexStr
    // TODO: Find proper exception to throw from here
    method = NID_sha1WithRSAEncryption;
    std::string hexStr = sigHex;

    value = fromHex(sigHex);
}

std::string digidoc::DigestValue::toHexString()
{
    std::string prefix = "";
    if(type != NID_sha1) {
        const char *s = OBJ_nid2sn(type);
        if(!s)
            throw std::runtime_error("Unknown digest type: " + type);
        else {
            prefix += "[";
            prefix += s;
            prefix += "]";
        }
    }
    return toHex(value);
}
