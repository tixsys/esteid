// Copyright (C) 2005-2008 Code Synthesis Tools CC
//
// This program was generated by CodeSynthesis XSD, an XML Schema to
// C++ data binding compiler.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
//
// In addition, as a special exception, Code Synthesis Tools CC gives
// permission to link this program with the Xerces-C++ library (or with
// modified versions of Xerces-C++ that use the same license as Xerces-C++),
// and distribute linked combinations including the two. You must obey
// the GNU General Public License version 2 in all respects for all of
// the code used other than Xerces-C++. If you modify this copy of the
// program, you may extend this exception to your version of the program,
// but you are not obligated to do so. If you do not wish to do so, delete
// this exception statement from your version.
//
// Furthermore, Code Synthesis Tools CC makes a special exception for
// the Free/Libre and Open Source Software (FLOSS) which is described
// in the accompanying FLOSSE file.
//

/**
 * @file
 * @brief Generated from conf.xsd.
 */

#ifndef CONF_HXX
#define CONF_HXX

// Begin prologue.
//
//
// End prologue.

#include <xsd/cxx/config.hxx>

#if (XSD_INT_VERSION != 3020000L)
#error XSD runtime version mismatch
#endif

#include <xsd/cxx/pre.hxx>

#ifndef XSD_USE_CHAR
#define XSD_USE_CHAR
#endif

#ifndef XSD_CXX_TREE_USE_CHAR
#define XSD_CXX_TREE_USE_CHAR
#endif

#include <xsd/cxx/tree/exceptions.hxx>
#include <xsd/cxx/tree/elements.hxx>
#include <xsd/cxx/tree/types.hxx>

#include <xsd/cxx/xml/error-handler.hxx>

#include <xsd/cxx/xml/dom/auto-ptr.hxx>

#include <xsd/cxx/tree/parsing.hxx>
#include <xsd/cxx/tree/parsing/byte.hxx>
#include <xsd/cxx/tree/parsing/unsigned-byte.hxx>
#include <xsd/cxx/tree/parsing/short.hxx>
#include <xsd/cxx/tree/parsing/unsigned-short.hxx>
#include <xsd/cxx/tree/parsing/int.hxx>
#include <xsd/cxx/tree/parsing/unsigned-int.hxx>
#include <xsd/cxx/tree/parsing/long.hxx>
#include <xsd/cxx/tree/parsing/unsigned-long.hxx>
#include <xsd/cxx/tree/parsing/boolean.hxx>
#include <xsd/cxx/tree/parsing/float.hxx>
#include <xsd/cxx/tree/parsing/double.hxx>
#include <xsd/cxx/tree/parsing/decimal.hxx>

/**
 * @brief C++ namespace for the %http://www.w3.org/2001/XMLSchema
 * schema namespace.
 */
namespace xml_schema
{
  // anyType and anySimpleType.
  //

  /**
   * @brief C++ type corresponding to the anyType XML Schema
   * built-in type.
   */
  typedef ::xsd::cxx::tree::type Type;

  /**
   * @brief C++ type corresponding to the anySimpleType XML Schema
   * built-in type.
   */
  typedef ::xsd::cxx::tree::simple_type< Type > SimpleType;

  /**
   * @brief Alias for the anyType type.
   */
  typedef ::xsd::cxx::tree::type Container;


  // 8-bit
  //

  /**
   * @brief C++ type corresponding to the byte XML Schema
   * built-in type.
   */
  typedef signed char Byte;

  /**
   * @brief C++ type corresponding to the unsignedByte XML Schema
   * built-in type.
   */
  typedef unsigned char UnsignedByte;


  // 16-bit
  //

  /**
   * @brief C++ type corresponding to the short XML Schema
   * built-in type.
   */
  typedef short Short;

  /**
   * @brief C++ type corresponding to the unsignedShort XML Schema
   * built-in type.
   */
  typedef unsigned short UnsignedShort;


  // 32-bit
  //

  /**
   * @brief C++ type corresponding to the int XML Schema
   * built-in type.
   */
  typedef int Int;

  /**
   * @brief C++ type corresponding to the unsignedInt XML Schema
   * built-in type.
   */
  typedef unsigned int UnsignedInt;


  // 64-bit
  //

  /**
   * @brief C++ type corresponding to the long XML Schema
   * built-in type.
   */
  typedef long long Long;

  /**
   * @brief C++ type corresponding to the unsignedLong XML Schema
   * built-in type.
   */
  typedef unsigned long long UnsignedLong;


  // Supposed to be arbitrary-length integral types.
  //

  /**
   * @brief C++ type corresponding to the integer XML Schema
   * built-in type.
   */
  typedef long long Integer;

  /**
   * @brief C++ type corresponding to the nonPositiveInteger XML Schema
   * built-in type.
   */
  typedef long long NonPositiveInteger;

  /**
   * @brief C++ type corresponding to the nonNegativeInteger XML Schema
   * built-in type.
   */
  typedef unsigned long long NonNegativeInteger;

  /**
   * @brief C++ type corresponding to the positiveInteger XML Schema
   * built-in type.
   */
  typedef unsigned long long PositiveInteger;

  /**
   * @brief C++ type corresponding to the negativeInteger XML Schema
   * built-in type.
   */
  typedef long long NegativeInteger;


  // Boolean.
  //

  /**
   * @brief C++ type corresponding to the boolean XML Schema
   * built-in type.
   */
  typedef bool Boolean;


  // Floating-point types.
  //

  /**
   * @brief C++ type corresponding to the float XML Schema
   * built-in type.
   */
  typedef float Float;

  /**
   * @brief C++ type corresponding to the double XML Schema
   * built-in type.
   */
  typedef double Double;

  /**
   * @brief C++ type corresponding to the decimal XML Schema
   * built-in type.
   */
  typedef double Decimal;


  // String types.
  //

  /**
   * @brief C++ type corresponding to the string XML Schema
   * built-in type.
   */
  typedef ::xsd::cxx::tree::string< char, SimpleType > String;

  /**
   * @brief C++ type corresponding to the normalizedString XML Schema
   * built-in type.
   */
  typedef ::xsd::cxx::tree::normalized_string< char, String > NormalizedString;

  /**
   * @brief C++ type corresponding to the token XML Schema
   * built-in type.
   */
  typedef ::xsd::cxx::tree::token< char, NormalizedString > Token;

  /**
   * @brief C++ type corresponding to the Name XML Schema
   * built-in type.
   */
  typedef ::xsd::cxx::tree::name< char, Token > Name;

  /**
   * @brief C++ type corresponding to the NMTOKEN XML Schema
   * built-in type.
   */
  typedef ::xsd::cxx::tree::nmtoken< char, Token > Nmtoken;

  /**
   * @brief C++ type corresponding to the NMTOKENS XML Schema
   * built-in type.
   */
  typedef ::xsd::cxx::tree::nmtokens< char, SimpleType, Nmtoken > Nmtokens;

  /**
   * @brief C++ type corresponding to the NCName XML Schema
   * built-in type.
   */
  typedef ::xsd::cxx::tree::ncname< char, Name > Ncname;

  /**
   * @brief C++ type corresponding to the language XML Schema
   * built-in type.
   */
  typedef ::xsd::cxx::tree::language< char, Token > Language;


  // ID/IDREF.
  //

  /**
   * @brief C++ type corresponding to the ID XML Schema
   * built-in type.
   */
  typedef ::xsd::cxx::tree::id< char, Ncname > Id;

  /**
   * @brief C++ type corresponding to the IDREF XML Schema
   * built-in type.
   */
  typedef ::xsd::cxx::tree::idref< Type, char, Ncname > Idref;

  /**
   * @brief C++ type corresponding to the IDREFS XML Schema
   * built-in type.
   */
  typedef ::xsd::cxx::tree::idrefs< char, SimpleType, Idref > Idrefs;


  // URI.
  //

  /**
   * @brief C++ type corresponding to the anyURI XML Schema
   * built-in type.
   */
  typedef ::xsd::cxx::tree::uri< char, SimpleType > Uri;


  // Qualified name.
  //

  /**
   * @brief C++ type corresponding to the QName XML Schema
   * built-in type.
   */
  typedef ::xsd::cxx::tree::qname< char, SimpleType, Uri, Ncname > Qname;


  // Binary.
  //

  /**
   * @brief Binary buffer type.
   */
  typedef ::xsd::cxx::tree::buffer< char > Buffer;

  /**
   * @brief C++ type corresponding to the base64Binary XML Schema
   * built-in type.
   */
  typedef ::xsd::cxx::tree::base64_binary< char, SimpleType > Base64Binary;

  /**
   * @brief C++ type corresponding to the hexBinary XML Schema
   * built-in type.
   */
  typedef ::xsd::cxx::tree::hex_binary< char, SimpleType > HexBinary;


  // Date/time.
  //

  /**
   * @brief Time zone type.
   */
  typedef ::xsd::cxx::tree::time_zone TimeZone;

  /**
   * @brief C++ type corresponding to the date XML Schema
   * built-in type.
   */
  typedef ::xsd::cxx::tree::date< char, SimpleType > Date;

  /**
   * @brief C++ type corresponding to the dateTime XML Schema
   * built-in type.
   */
  typedef ::xsd::cxx::tree::date_time< char, SimpleType > DateTime;

  /**
   * @brief C++ type corresponding to the duration XML Schema
   * built-in type.
   */
  typedef ::xsd::cxx::tree::duration< char, SimpleType > Duration;

  /**
   * @brief C++ type corresponding to the gDay XML Schema
   * built-in type.
   */
  typedef ::xsd::cxx::tree::gday< char, SimpleType > Gday;

  /**
   * @brief C++ type corresponding to the gMonth XML Schema
   * built-in type.
   */
  typedef ::xsd::cxx::tree::gmonth< char, SimpleType > Gmonth;

  /**
   * @brief C++ type corresponding to the gMonthDay XML Schema
   * built-in type.
   */
  typedef ::xsd::cxx::tree::gmonth_day< char, SimpleType > GmonthDay;

  /**
   * @brief C++ type corresponding to the gYear XML Schema
   * built-in type.
   */
  typedef ::xsd::cxx::tree::gyear< char, SimpleType > Gyear;

  /**
   * @brief C++ type corresponding to the gYearMonth XML Schema
   * built-in type.
   */
  typedef ::xsd::cxx::tree::gyear_month< char, SimpleType > GyearMonth;

  /**
   * @brief C++ type corresponding to the time XML Schema
   * built-in type.
   */
  typedef ::xsd::cxx::tree::time< char, SimpleType > Time;


  // Entity.
  //

  /**
   * @brief C++ type corresponding to the ENTITY XML Schema
   * built-in type.
   */
  typedef ::xsd::cxx::tree::entity< char, Ncname > Entity;

  /**
   * @brief C++ type corresponding to the ENTITIES XML Schema
   * built-in type.
   */
  typedef ::xsd::cxx::tree::entities< char, SimpleType, Entity > Entities;


  // Flags and properties.
  //

  /**
   * @brief Parsing and serialization flags.
   */
  typedef ::xsd::cxx::tree::flags Flags;

  /**
   * @brief Parsing properties.
   */
  typedef ::xsd::cxx::tree::properties< char > Properties;

  // Exceptions.
  //

  /**
   * @brief Root of the C++/Tree %exception hierarchy.
   */
  typedef ::xsd::cxx::tree::exception< char > Exception;

  /**
   * @brief Exception indicating that the size argument exceeds
   * the capacity argument.
   */
  typedef ::xsd::cxx::tree::bounds< char > Bounds;

  /**
   * @brief Exception indicating that a duplicate ID value
   * was encountered in the object model.
   */
  typedef ::xsd::cxx::tree::duplicate_id< char > DuplicateId;

  /**
   * @brief Exception indicating a parsing failure.
   */
  typedef ::xsd::cxx::tree::parsing< char > Parsing;

  /**
   * @brief Exception indicating that an expected element
   * was not encountered.
   */
  typedef ::xsd::cxx::tree::expected_element< char > ExpectedElement;

  /**
   * @brief Exception indicating that an unexpected element
   * was encountered.
   */
  typedef ::xsd::cxx::tree::unexpected_element< char > UnexpectedElement;

  /**
   * @brief Exception indicating that an expected attribute
   * was not encountered.
   */
  typedef ::xsd::cxx::tree::expected_attribute< char > ExpectedAttribute;

  /**
   * @brief Exception indicating that an unexpected enumerator
   * was encountered.
   */
  typedef ::xsd::cxx::tree::unexpected_enumerator< char > UnexpectedEnumerator;

  /**
   * @brief Exception indicating that the text content was
   * expected for an element.
   */
  typedef ::xsd::cxx::tree::expected_text_content< char > ExpectedTextContent;

  /**
   * @brief Exception indicating that a prefix-namespace
   * mapping was not provided.
   */
  typedef ::xsd::cxx::tree::no_prefix_mapping< char > NoPrefixMapping;

  // Parsing/serialization diagnostics.
  //

  /**
   * @brief Error severity.
   */
  typedef ::xsd::cxx::tree::severity Severity;

  /**
   * @brief Error condition.
   */
  typedef ::xsd::cxx::tree::error< char > Error;

  /**
   * @brief List of %error conditions.
   */
  typedef ::xsd::cxx::tree::diagnostics< char > Diagnostics;

  /**
   * @brief Error handler callback interface.
   */
  typedef ::xsd::cxx::xml::error_handler< char > ErrorHandler;

  /**
   * @brief DOM interaction.
   */
  namespace dom
  {
    /**
     * @brief Automatic pointer for DOMDocument.
     */
    using ::xsd::cxx::xml::dom::auto_ptr;

#ifndef XSD_CXX_TREE_TREE_NODE_KEY_IN___XML_SCHEMA
#define XSD_CXX_TREE_TREE_NODE_KEY_IN___XML_SCHEMA
    /**
     * @brief DOM user data key for back pointers to tree nodes.
     */
    const XMLCh* const treeNodeKey = ::xsd::cxx::tree::user_data_keys::node;
#endif
  }
}

// Forward declarations.
//
class Configuration;
class Ocsp;
class Param;

#include <memory>    // std::auto_ptr
#include <algorithm> // std::binary_search

#include <xsd/cxx/tree/exceptions.hxx>
#include <xsd/cxx/tree/elements.hxx>
#include <xsd/cxx/tree/containers.hxx>
#include <xsd/cxx/tree/list.hxx>

#include <xsd/cxx/xml/dom/parsing-header.hxx>

/**
 * @brief Class corresponding to the %configuration schema type.
 *
 * @nosubgrouping
 */
class Configuration: public ::xml_schema::Type
{
  public:
  /**
   * @name param
   *
   * @brief Accessor and modifier functions for the %param
   * sequence element.
   */
  //@{

  /**
   * @brief Element type.
   */
  typedef ::Param ParamType;

  /**
   * @brief Element sequence container type.
   */
  typedef ::xsd::cxx::tree::sequence< ParamType > ParamSequence;

  /**
   * @brief Element iterator type.
   */
  typedef xsd::cxx::tree::sequence< ParamType >::iterator ParamIterator;

  /**
   * @brief Element constant iterator type.
   */
  typedef xsd::cxx::tree::sequence< ParamType >::const_iterator ParamConstIterator;

  /**
   * @brief Element traits type.
   */
  typedef ::xsd::cxx::tree::traits< ParamType, char > ParamTraits;

  /**
   * @brief Return a read-only (constant) reference to the element
   * sequence.
   *
   * @return A constant reference to the sequence container.
   */
  const ParamSequence&
  param () const;

  /**
   * @brief Return a read-write reference to the element sequence.
   *
   * @return A reference to the sequence container.
   */
  ParamSequence&
  param ();

  /**
   * @brief Copy elements from a given sequence.
   *
   * @param s A sequence to copy elements from.
   *
   * For each element in @a s this function makes a copy and adds it 
   * to the sequence. Note that this operation completely changes the 
   * sequence and all old elements will be lost.
   */
  void
  param (const ParamSequence& s);

  //@}

  /**
   * @name ocsp
   *
   * @brief Accessor and modifier functions for the %ocsp
   * sequence element.
   */
  //@{

  /**
   * @brief Element type.
   */
  typedef ::Ocsp OcspType;

  /**
   * @brief Element sequence container type.
   */
  typedef ::xsd::cxx::tree::sequence< OcspType > OcspSequence;

  /**
   * @brief Element iterator type.
   */
  typedef xsd::cxx::tree::sequence< OcspType >::iterator OcspIterator;

  /**
   * @brief Element constant iterator type.
   */
  typedef xsd::cxx::tree::sequence< OcspType >::const_iterator OcspConstIterator;

  /**
   * @brief Element traits type.
   */
  typedef ::xsd::cxx::tree::traits< OcspType, char > OcspTraits;

  /**
   * @brief Return a read-only (constant) reference to the element
   * sequence.
   *
   * @return A constant reference to the sequence container.
   */
  const OcspSequence&
  ocsp () const;

  /**
   * @brief Return a read-write reference to the element sequence.
   *
   * @return A reference to the sequence container.
   */
  OcspSequence&
  ocsp ();

  /**
   * @brief Copy elements from a given sequence.
   *
   * @param s A sequence to copy elements from.
   *
   * For each element in @a s this function makes a copy and adds it 
   * to the sequence. Note that this operation completely changes the 
   * sequence and all old elements will be lost.
   */
  void
  ocsp (const OcspSequence& s);

  //@}

  /**
   * @name Constructors
   */
  //@{

  /**
   * @brief Create an instance from the ultimate base and
   * initializers for required elements and attributes.
   */
  Configuration ();

  /**
   * @brief Create an instance from a DOM element.
   *
   * @param e A DOM element to extract the data from.
   * @param f Flags to create the new instance with.
   * @param c A pointer to the object that will contain the new
   * instance.
   */
  Configuration (const xercesc::DOMElement& e,
                 ::xml_schema::Flags f = 0,
                 ::xml_schema::Container* c = 0);

  /**
   * @brief Copy constructor.
   *
   * @param x An instance to make a copy of.
   * @param f Flags to create the copy with.
   * @param c A pointer to the object that will contain the copy.
   *
   * For polymorphic object models use the @c _clone function instead.
   */
  Configuration (const Configuration& x,
                 ::xml_schema::Flags f = 0,
                 ::xml_schema::Container* c = 0);

  /**
   * @brief Copy the instance polymorphically.
   *
   * @param f Flags to create the copy with.
   * @param c A pointer to the object that will contain the copy.
   * @return A pointer to the dynamically allocated copy.
   *
   * This function ensures that the dynamic type of the instance is
   * used for copying and should be used for polymorphic object
   * models instead of the copy constructor.
   */
  virtual Configuration*
  _clone (::xml_schema::Flags f = 0,
          ::xml_schema::Container* c = 0) const;

  //@}

  /**
   * @brief Destructor.
   */
  virtual 
  ~Configuration ();

  // Implementation.
  //

  //@cond

  protected:
  void
  parse (::xsd::cxx::xml::dom::parser< char >&,
         ::xml_schema::Flags);

  protected:
  ParamSequence param_;
  OcspSequence ocsp_;

  //@endcond
};

/**
 * @brief Class corresponding to the %ocsp schema type.
 *
 * @nosubgrouping
 */
class Ocsp: public ::xml_schema::Type
{
  public:
  /**
   * @name url
   *
   * @brief Accessor and modifier functions for the %url
   * required element.
   */
  //@{

  /**
   * @brief Element type.
   */
  typedef ::xml_schema::String UrlType;

  /**
   * @brief Element traits type.
   */
  typedef ::xsd::cxx::tree::traits< UrlType, char > UrlTraits;

  /**
   * @brief Return a read-only (constant) reference to the element.
   *
   * @return A constant reference to the element.
   */
  const UrlType&
  url () const;

  /**
   * @brief Return a read-write reference to the element.
   *
   * @return A reference to the element.
   */
  UrlType&
  url ();

  /**
   * @brief Set the element value.
   *
   * @param x A new value to set.
   *
   * This function makes a copy of its argument and sets it as
   * the new value of the element.
   */
  void
  url (const UrlType& x);

  /**
   * @brief Set the element value without copying.
   *
   * @param p A new value to use.
   *
   * This function will try to use the passed value directly instead
   * of making a copy.
   */
  void
  url (::std::auto_ptr< UrlType > p);

  //@}

  /**
   * @name cert
   *
   * @brief Accessor and modifier functions for the %cert
   * required element.
   */
  //@{

  /**
   * @brief Element type.
   */
  typedef ::xml_schema::String CertType;

  /**
   * @brief Element traits type.
   */
  typedef ::xsd::cxx::tree::traits< CertType, char > CertTraits;

  /**
   * @brief Return a read-only (constant) reference to the element.
   *
   * @return A constant reference to the element.
   */
  const CertType&
  cert () const;

  /**
   * @brief Return a read-write reference to the element.
   *
   * @return A reference to the element.
   */
  CertType&
  cert ();

  /**
   * @brief Set the element value.
   *
   * @param x A new value to set.
   *
   * This function makes a copy of its argument and sets it as
   * the new value of the element.
   */
  void
  cert (const CertType& x);

  /**
   * @brief Set the element value without copying.
   *
   * @param p A new value to use.
   *
   * This function will try to use the passed value directly instead
   * of making a copy.
   */
  void
  cert (::std::auto_ptr< CertType > p);

  //@}

  /**
   * @name issuer
   *
   * @brief Accessor and modifier functions for the %issuer
   * required attribute.
   */
  //@{

  /**
   * @brief Attribute type.
   */
  typedef ::xml_schema::String IssuerType;

  /**
   * @brief Attribute traits type.
   */
  typedef ::xsd::cxx::tree::traits< IssuerType, char > IssuerTraits;

  /**
   * @brief Return a read-only (constant) reference to the attribute.
   *
   * @return A constant reference to the attribute.
   */
  const IssuerType&
  issuer () const;

  /**
   * @brief Return a read-write reference to the attribute.
   *
   * @return A reference to the attribute.
   */
  IssuerType&
  issuer ();

  /**
   * @brief Set the attribute value.
   *
   * @param x A new value to set.
   *
   * This function makes a copy of its argument and sets it as
   * the new value of the attribute.
   */
  void
  issuer (const IssuerType& x);

  /**
   * @brief Set the attribute value without copying.
   *
   * @param p A new value to use.
   *
   * This function will try to use the passed value directly instead
   * of making a copy.
   */
  void
  issuer (::std::auto_ptr< IssuerType > p);

  //@}

  /**
   * @name Constructors
   */
  //@{

  /**
   * @brief Create an instance from the ultimate base and
   * initializers for required elements and attributes.
   */
  Ocsp (const UrlType&,
        const CertType&,
        const IssuerType&);

  /**
   * @brief Create an instance from a DOM element.
   *
   * @param e A DOM element to extract the data from.
   * @param f Flags to create the new instance with.
   * @param c A pointer to the object that will contain the new
   * instance.
   */
  Ocsp (const xercesc::DOMElement& e,
        ::xml_schema::Flags f = 0,
        ::xml_schema::Container* c = 0);

  /**
   * @brief Copy constructor.
   *
   * @param x An instance to make a copy of.
   * @param f Flags to create the copy with.
   * @param c A pointer to the object that will contain the copy.
   *
   * For polymorphic object models use the @c _clone function instead.
   */
  Ocsp (const Ocsp& x,
        ::xml_schema::Flags f = 0,
        ::xml_schema::Container* c = 0);

  /**
   * @brief Copy the instance polymorphically.
   *
   * @param f Flags to create the copy with.
   * @param c A pointer to the object that will contain the copy.
   * @return A pointer to the dynamically allocated copy.
   *
   * This function ensures that the dynamic type of the instance is
   * used for copying and should be used for polymorphic object
   * models instead of the copy constructor.
   */
  virtual Ocsp*
  _clone (::xml_schema::Flags f = 0,
          ::xml_schema::Container* c = 0) const;

  //@}

  /**
   * @brief Destructor.
   */
  virtual 
  ~Ocsp ();

  // Implementation.
  //

  //@cond

  protected:
  void
  parse (::xsd::cxx::xml::dom::parser< char >&,
         ::xml_schema::Flags);

  protected:
  ::xsd::cxx::tree::one< UrlType > url_;
  ::xsd::cxx::tree::one< CertType > cert_;
  ::xsd::cxx::tree::one< IssuerType > issuer_;

  //@endcond
};

/**
 * @brief Class corresponding to the %param schema type.
 *
 * @nosubgrouping
 */
class Param: public ::xml_schema::String
{
  public:
  /**
   * @name name
   *
   * @brief Accessor and modifier functions for the %name
   * required attribute.
   */
  //@{

  /**
   * @brief Attribute type.
   */
  typedef ::xml_schema::String NameType;

  /**
   * @brief Attribute traits type.
   */
  typedef ::xsd::cxx::tree::traits< NameType, char > NameTraits;

  /**
   * @brief Return a read-only (constant) reference to the attribute.
   *
   * @return A constant reference to the attribute.
   */
  const NameType&
  name () const;

  /**
   * @brief Return a read-write reference to the attribute.
   *
   * @return A reference to the attribute.
   */
  NameType&
  name ();

  /**
   * @brief Set the attribute value.
   *
   * @param x A new value to set.
   *
   * This function makes a copy of its argument and sets it as
   * the new value of the attribute.
   */
  void
  name (const NameType& x);

  /**
   * @brief Set the attribute value without copying.
   *
   * @param p A new value to use.
   *
   * This function will try to use the passed value directly instead
   * of making a copy.
   */
  void
  name (::std::auto_ptr< NameType > p);

  //@}

  /**
   * @name Constructors
   */
  //@{

  /**
   * @brief Create an instance from initializers for required 
   * elements and attributes.
   */
  Param (const NameType&);

  /**
   * @brief Create an instance from a C string and initializers
   * for required elements and attributes.
   */
  Param (const char*,
         const NameType&);

  /**
   * @brief Create an instance from a string andinitializers
   * for required elements and attributes.
   */
  Param (const ::std::string&,
         const NameType&);

  /**
   * @brief Create an instance from the ultimate base and
   * initializers for required elements and attributes.
   */
  Param (const ::xml_schema::String&,
         const NameType&);

  /**
   * @brief Create an instance from a DOM element.
   *
   * @param e A DOM element to extract the data from.
   * @param f Flags to create the new instance with.
   * @param c A pointer to the object that will contain the new
   * instance.
   */
  Param (const xercesc::DOMElement& e,
         ::xml_schema::Flags f = 0,
         ::xml_schema::Container* c = 0);

  /**
   * @brief Copy constructor.
   *
   * @param x An instance to make a copy of.
   * @param f Flags to create the copy with.
   * @param c A pointer to the object that will contain the copy.
   *
   * For polymorphic object models use the @c _clone function instead.
   */
  Param (const Param& x,
         ::xml_schema::Flags f = 0,
         ::xml_schema::Container* c = 0);

  /**
   * @brief Copy the instance polymorphically.
   *
   * @param f Flags to create the copy with.
   * @param c A pointer to the object that will contain the copy.
   * @return A pointer to the dynamically allocated copy.
   *
   * This function ensures that the dynamic type of the instance is
   * used for copying and should be used for polymorphic object
   * models instead of the copy constructor.
   */
  virtual Param*
  _clone (::xml_schema::Flags f = 0,
          ::xml_schema::Container* c = 0) const;

  //@}

  /**
   * @brief Destructor.
   */
  virtual 
  ~Param ();

  // Implementation.
  //

  //@cond

  protected:
  void
  parse (::xsd::cxx::xml::dom::parser< char >&,
         ::xml_schema::Flags);

  protected:
  ::xsd::cxx::tree::one< NameType > name_;

  //@endcond
};

#include <iosfwd>

#include <xercesc/sax/InputSource.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMErrorHandler.hpp>

/**
 * @name Parsing functions for the %configuration document root.
 */
//@{

/**
 * @brief Parse a URI or a local file.
 *
 * @param uri A URI or a local file name.
 * @param f Parsing flags.
 * @param p Parsing properties. 
 * @return A pointer to the root of the object model.
 *
 * This function uses exceptions to report parsing errors.
 */
::std::auto_ptr< ::Configuration >
configuration (const ::std::string& uri,
               ::xml_schema::Flags f = 0,
               const ::xml_schema::Properties& p = ::xml_schema::Properties ());

/**
 * @brief Parse a URI or a local file with an error handler.
 *
 * @param uri A URI or a local file name.
 * @param eh An error handler.
 * @param f Parsing flags.
 * @param p Parsing properties. 
 * @return A pointer to the root of the object model.
 *
 * This function reports parsing errors by calling the error handler.
 */
::std::auto_ptr< ::Configuration >
configuration (const ::std::string& uri,
               ::xml_schema::ErrorHandler& eh,
               ::xml_schema::Flags f = 0,
               const ::xml_schema::Properties& p = ::xml_schema::Properties ());

/**
 * @brief Parse a URI or a local file with a Xerces-C++ DOM error
 * handler.
 *
 * @param uri A URI or a local file name.
 * @param eh A Xerces-C++ DOM error handler.
 * @param f Parsing flags.
 * @param p Parsing properties. 
 * @return A pointer to the root of the object model.
 *
 * This function reports parsing errors by calling the error handler.
 */
::std::auto_ptr< ::Configuration >
configuration (const ::std::string& uri,
               xercesc::DOMErrorHandler& eh,
               ::xml_schema::Flags f = 0,
               const ::xml_schema::Properties& p = ::xml_schema::Properties ());

/**
 * @brief Parse a standard input stream.
 *
 * @param is A standrad input stream.
 * @param f Parsing flags.
 * @param p Parsing properties. 
 * @return A pointer to the root of the object model.
 *
 * This function uses exceptions to report parsing errors.
 */
::std::auto_ptr< ::Configuration >
configuration (::std::istream& is,
               ::xml_schema::Flags f = 0,
               const ::xml_schema::Properties& p = ::xml_schema::Properties ());

/**
 * @brief Parse a standard input stream with an error handler.
 *
 * @param is A standrad input stream.
 * @param eh An error handler.
 * @param f Parsing flags.
 * @param p Parsing properties. 
 * @return A pointer to the root of the object model.
 *
 * This function reports parsing errors by calling the error handler.
 */
::std::auto_ptr< ::Configuration >
configuration (::std::istream& is,
               ::xml_schema::ErrorHandler& eh,
               ::xml_schema::Flags f = 0,
               const ::xml_schema::Properties& p = ::xml_schema::Properties ());

/**
 * @brief Parse a standard input stream with a Xerces-C++ DOM error
 * handler.
 *
 * @param is A standrad input stream.
 * @param eh A Xerces-C++ DOM error handler.
 * @param f Parsing flags.
 * @param p Parsing properties. 
 * @return A pointer to the root of the object model.
 *
 * This function reports parsing errors by calling the error handler.
 */
::std::auto_ptr< ::Configuration >
configuration (::std::istream& is,
               xercesc::DOMErrorHandler& eh,
               ::xml_schema::Flags f = 0,
               const ::xml_schema::Properties& p = ::xml_schema::Properties ());

/**
 * @brief Parse a standard input stream with a resource id.
 *
 * @param is A standrad input stream.
 * @param id A resource id.
 * @param f Parsing flags.
 * @param p Parsing properties. 
 * @return A pointer to the root of the object model.
 *
 * The resource id is used to identify the document being parsed in
 * diagnostics as well as to resolve relative paths.
 *
 * This function uses exceptions to report parsing errors.
 */
::std::auto_ptr< ::Configuration >
configuration (::std::istream& is,
               const ::std::string& id,
               ::xml_schema::Flags f = 0,
               const ::xml_schema::Properties& p = ::xml_schema::Properties ());

/**
 * @brief Parse a standard input stream with a resource id and an
 * error handler.
 *
 * @param is A standrad input stream.
 * @param id A resource id.
 * @param eh An error handler.
 * @param f Parsing flags.
 * @param p Parsing properties. 
 * @return A pointer to the root of the object model.
 *
 * The resource id is used to identify the document being parsed in
 * diagnostics as well as to resolve relative paths.
 *
 * This function reports parsing errors by calling the error handler.
 */
::std::auto_ptr< ::Configuration >
configuration (::std::istream& is,
               const ::std::string& id,
               ::xml_schema::ErrorHandler& eh,
               ::xml_schema::Flags f = 0,
               const ::xml_schema::Properties& p = ::xml_schema::Properties ());

/**
 * @brief Parse a standard input stream with a resource id and a
 * Xerces-C++ DOM error handler.
 *
 * @param is A standrad input stream.
 * @param id A resource id.
 * @param eh A Xerces-C++ DOM error handler.
 * @param f Parsing flags.
 * @param p Parsing properties. 
 * @return A pointer to the root of the object model.
 *
 * The resource id is used to identify the document being parsed in
 * diagnostics as well as to resolve relative paths.
 *
 * This function reports parsing errors by calling the error handler.
 */
::std::auto_ptr< ::Configuration >
configuration (::std::istream& is,
               const ::std::string& id,
               xercesc::DOMErrorHandler& eh,
               ::xml_schema::Flags f = 0,
               const ::xml_schema::Properties& p = ::xml_schema::Properties ());

/**
 * @brief Parse a Xerces-C++ input source.
 *
 * @param is A Xerces-C++ input source.
 * @param f Parsing flags.
 * @param p Parsing properties. 
 * @return A pointer to the root of the object model.
 *
 * This function uses exceptions to report parsing errors.
 */
::std::auto_ptr< ::Configuration >
configuration (xercesc::InputSource& is,
               ::xml_schema::Flags f = 0,
               const ::xml_schema::Properties& p = ::xml_schema::Properties ());

/**
 * @brief Parse a Xerces-C++ input source with an error handler.
 *
 * @param is A Xerces-C++ input source.
 * @param eh An error handler.
 * @param f Parsing flags.
 * @param p Parsing properties. 
 * @return A pointer to the root of the object model.
 *
 * This function reports parsing errors by calling the error handler.
 */
::std::auto_ptr< ::Configuration >
configuration (xercesc::InputSource& is,
               ::xml_schema::ErrorHandler& eh,
               ::xml_schema::Flags f = 0,
               const ::xml_schema::Properties& p = ::xml_schema::Properties ());

/**
 * @brief Parse a Xerces-C++ input source with a Xerces-C++ DOM
 * error handler.
 *
 * @param is A Xerces-C++ input source.
 * @param eh A Xerces-C++ DOM error handler.
 * @param f Parsing flags.
 * @param p Parsing properties. 
 * @return A pointer to the root of the object model.
 *
 * This function reports parsing errors by calling the error handler.
 */
::std::auto_ptr< ::Configuration >
configuration (xercesc::InputSource& is,
               xercesc::DOMErrorHandler& eh,
               ::xml_schema::Flags f = 0,
               const ::xml_schema::Properties& p = ::xml_schema::Properties ());

/**
 * @brief Parse a Xerces-C++ DOM document.
 *
 * @param d A Xerces-C++ DOM document.
 * @param f Parsing flags.
 * @param p Parsing properties. 
 * @return A pointer to the root of the object model.
 */
::std::auto_ptr< ::Configuration >
configuration (const xercesc::DOMDocument& d,
               ::xml_schema::Flags f = 0,
               const ::xml_schema::Properties& p = ::xml_schema::Properties ());

/**
 * @brief Parse a Xerces-C++ DOM document.
 *
 * @param d A pointer to the Xerces-C++ DOM document.
 * @param f Parsing flags.
 * @param p Parsing properties. 
 * @return A pointer to the root of the object model.
 *
 * This function is normally used together with the keep_dom and
 * own_dom parsing flags to assign ownership of the DOM document
 * to the object model.
 */
::std::auto_ptr< ::Configuration >
configuration (::xml_schema::dom::auto_ptr< xercesc::DOMDocument >& d,
               ::xml_schema::Flags f = 0,
               const ::xml_schema::Properties& p = ::xml_schema::Properties ());

//@}

#include <xsd/cxx/post.hxx>

// Begin epilogue.
//
//
// End epilogue.

#endif // CONF_HXX
