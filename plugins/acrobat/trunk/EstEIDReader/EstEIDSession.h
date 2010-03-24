#ifndef EST_EID_SESSION_H_INCLUDED
#define EST_EID_SESSION_H_INCLUDED

#pragma once

class EstEIDSession
{
public:
    EstEIDSession(void);
    CK_RV Open(const char *module);
    CK_RV Close();
    bool Initialized() { return m_initialized != 0; } 
protected:    
    void *m_module;
    CK_FUNCTION_LIST_PTR m_p11;
    char m_initialized;
};


#endif // #ifndef EST_EID_SESSION_H_INCLUDED