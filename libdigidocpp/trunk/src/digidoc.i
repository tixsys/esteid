// digidoc.i - SWIG interface for DigiDoc C++ library

// TODO: Find a way to tap into PHP-s RINIT and RSHUTDOWN
//       (request init/shutdown), MSHUTDOWN and MINFO would
//       be nice too. Also investigate phppointers.i

// TODO: Add %newobject to stuff that is known to return
//       pointers to specifically allocated objects
//       http://www.swig.org/Doc1.3/Customization.html

// TODO: Look through the code to see if anything needs typemaps.i
//       and %apply to fix functions that use pointer arguments for output

%module digidoc
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
%}

// Handle standard C++ types
%include "std_string.i"
%include "std_vector.i"
%apply const std::string& {std::string* foo};

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

// Internal hacks are not exposed
%ignore digidoc::SignatureDDOC;
%ignore *::getFileDigest;

// Broken in PHP :(
%ignore digidoc::Document::Document;
%ignore digidoc::SignerRole::SignerRole;
%ignore digidoc::SignatureProductionPlace::SignatureProductionPlace;

// TODO: useful, but broken
%ignore digidoc::Signature::validateOnline;
%ignore *::sign;

%feature("notabstract") digidoc::Signature;  // Breaks PHP if abstract
%rename(ExceptionBase) Exception;            // Too generic, breaks PHP

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
%include "ADoc.h"
%include "DDoc.h"
%include "BDoc.h"

#ifndef SWIGPHP // PHP will use it's own initialization functions
%init %{
    using namespace digidoc;

    // Initialize digidoc library.
    initialize();

    // Init certificate store.
    X509CertStore::init(new DirectoryX509CertStore());
%}
#endif

// Target language specific functions
#ifdef SWIGPHP
%minit %{ %}
%mshutdown %{ %}
%rinit %{
    using namespace digidoc;

    // Initialize digidoc library.
    initialize();

    // Init certificate store.
    X509CertStore::init(new DirectoryX509CertStore());
%}
%rshutdown %{
    digidoc::terminate();
%}
%pragma(php) phpinfo="
  zend_printf(\"DigiDoc libraries support\\n\");
  php_info_print_table_start();
  php_info_print_table_header(2, \"Directive\", \"Value\");
  php_info_print_table_row(2, \"DigiDoc support\", \"enabled\");
  php_info_print_table_end();
"
#endif // SWIGPHP
