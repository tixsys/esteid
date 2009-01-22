

#include "DateTime.h"

#include <sstream>
#include <iomanip>

namespace digidoc
{
    namespace util
    {
        namespace date
        {
            /// Dedicated helper for converting xml-schema-style DateTyme into a Zulu-string.
            /// 
            /// @param time GMT time as code-synth xml-schema type.
            /// @return a string format of date-time e.g. "2007-12-25T14:06:01Z".
            std::string xsd2string(const xml_schema::DateTime& time)
            {
                std::stringstream stream;

                stream << std::setfill('0') << std::dec
                    << std::setw(4) << time.year()
                    << "-"
                    << std::setw(2) << time.month()
                    << "-"
                    << std::setw(2) << time.day()
                    << "T"
                    << std::setw(2) << time.hours()
                    << ":"
                    << std::setw(2) << time.minutes()
                    << ":"
                    << std::setw(2) << time.seconds()
                    << "Z"
                    ;

                return stream.str();
            }
        }
    }
}
