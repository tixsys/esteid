#include "DummySigner.h"

namespace digidoc
{
DummySigner::DummySigner():publicCert(NULL){}
DummySigner::DummySigner(X509* cert):publicCert(cert){}

DummySigner::~DummySigner(){}

X509* DummySigner::getCert() throw(SignException)
{
    return publicCert;
};

}
