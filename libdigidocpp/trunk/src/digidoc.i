// digidoc.i - SWIG interface for DigiDoc C++ library

// TODO: Investigate phppointers.i

// TODO: Add %newobject to stuff that is known to return
//       pointers to specifically allocated objects
//       http://www.swig.org/Doc1.3/Customization.html

// TODO: Look through the code to see if anything needs typemaps.i
//       and %apply to fix functions that use pointer arguments for output

%module digidoc
//%module(directors="1") digidoc
%{
#undef seed // Combat braindead #defines that are present in PERL headers

/* combat PHP braindeadness */
#ifndef zend_error_noreturn
#define zend_error_noreturn zend_error
#endif

#include "BDoc.h"
#include "DDoc.h"
#include "crypto/cert/DirectoryX509CertStore.h"
#include "io/ISerialize.h"
#include "crypto/signer/Signer.h"
#include "util/File.h"
%}

// Handle standard C++ types
%include "std_string.i"
%include "std_vector.i"
%template(StringVector) std::vector<std::string>;
%template(ByteVector) std::vector<unsigned char>;
%apply const std::string& {std::string* foo};
%include "std_except.i"

// Allow variable getters/setters to throw exceptions too
%allowexception;

// Handle STL exceptions from the deep 
// FIXME: Is it actually needed?
%include "exception.i"
%exception {
  try {
    $action
  } catch (const std::exception& e) {
    SWIG_exception(SWIG_RuntimeError, e.what());
  }
}

// Handle DigiDoc Export declarations
#define EXP_DIGIDOC

// Excepton classes are not to be constructed higher up in the food-chain
%ignore digidoc::Exception::Exception;
%ignore digidoc::BDocException::BDocException;
%ignore digidoc::IOException::IOException;
%ignore digidoc::SignatureException::SignatureException;

// There is no use to empty signature objects: hide constructor
%ignore digidoc::Signature::Signature;

// Anything that uses ISserialize should not be visible
%ignore digidoc::BDoc::BDoc(std::auto_ptr<ISerialize>);
%ignore digidoc::DDoc::DDoc(std::auto_ptr<ISerialize>);
%ignore *::saveTo(std::auto_ptr<ISerialize>);

// Hide anything that requires arguments we can't create
%ignore digidoc::Document::calcDigest(digidoc::Digest *);

// Internal hacks are not exposed
%ignore digidoc::SignatureDDOC;
%ignore *::getFileDigest;

// TODO: useful, but broken
%ignore digidoc::Signature::validateOnline;
%ignore *::sign;

%feature("notabstract") digidoc::Signature;  // Breaks PHP if abstract
%rename(ExceptionBase) Exception;            // Too generic, breaks PHP

// Rewrite initialize function
%feature("action") digidoc::initialize() {
    using namespace digidoc;

    // Initialize digidoc library.
    initialize();

    // Init certificate store.
    X509CertStore::init(new DirectoryX509CertStore());
}

// Mark some stuff read-only
// %immutable List::length;

// Expose selected DigiDoc classes
%include "Exception.h"
%include "BDocException.h"
%include "SignatureException.h"
%include "io/IOException.h"
%include "SignatureAttributes.h"
%include "Signature.h"
%include "Document.h"
/* Buggy PHP proxy class generator will fuck up getDocument and getSignature
 * if ADoc.h is known and included into source. So we disable it for PHP and
 * suppress warnings for DDoc and BDoc classes. This is ugly, but what
 * else can we do?!
 */
#ifdef SWIGPHP
namespace digidoc {
    void initialize();
    void terminate();
}

%warnfilter(401) digidoc::DDoc;
%warnfilter(401) digidoc::BDoc;

/* Buggy PHP proxy class generator will fuck up getter functions
 * if there is no constructor with zero arguments for returned class.
 * The following hacks try to work around it.
 */
%extend digidoc::Document {
    Document() {
        throw std::runtime_error("Hacks are NOT callable");
    }
}
#else
%include "ADoc.h"
#endif
%include "DDoc.h"
%include "BDoc.h"

#ifndef SWIGPHP // PHP will use it's own initialization functions
%init %{ %}
#endif

// Target language specific functions
#ifdef SWIGPHP
%minit %{ %}
%mshutdown %{ %}
%rinit %{ %}
%rshutdown %{ %}
%pragma(php) phpinfo="
  zend_printf(\"DigiDoc libraries support\\n\");
  php_info_print_table_start();
  php_info_print_table_header(2, \"Directive\", \"Value\");
  php_info_print_table_row(2, \"DigiDoc support\", \"enabled\");
  php_info_print_table_end();
"
#endif // SWIGPHP
