#if !defined(__DATETIME_H_INCLUDED__)
#define __DATETIME_H_INCLUDED__

#include <string>

#include "../xml/xmldsig-core-schema.hxx"

namespace digidoc
{
    namespace util
    {

        /// Implements some date and time things specifically dedicated for BDoc.
        /// 
        /// @author Andrus Randveer, Vambola Kotkas
        namespace date
        {
            std::string xsd2string(const xml_schema::DateTime& time);
        }

    }
}

#endif // !defined(__DATETIME_H_INCLUDED__)
