#include "EstEIDIncludes.h"
#include "EstEIDSession.h"

EstEIDSession::EstEIDSession(void)
{
    m_module = NULL;
    m_p11 = NULL;
    m_initialized = 0;
}

CK_RV EstEIDSession::Open(const char *module)
{
    CK_RV rv = ESTEID_ERR;

    m_module = C_LoadModule( module, &m_p11 );
    if( m_module == NULL )
        return( ESTEID_ERR );

    rv = m_p11->C_Initialize( NULL );
    if( rv != CKR_OK )
    {
        m_p11->C_Finalize( NULL );
        C_UnloadModule( m_module );
        m_module = NULL;
        return( rv );
    }

    m_initialized = 1;

    return( rv );
}

CK_RV EstEIDSession::Close()
{
    CK_RV rv = ESTEID_ERR;

    if( m_p11 != NULL )
    {
        rv = m_p11->C_Finalize( NULL );
        C_UnloadModule( m_module );
    }

    m_initialized = 0;

    return( rv );
}
