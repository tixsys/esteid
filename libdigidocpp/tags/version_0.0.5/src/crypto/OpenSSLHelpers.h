#if !defined(__OPENSSLHELPERS_H_INCLUDED__)
#define __OPENSSLHELPERS_H_INCLUDED__


#define DECLARE_OPENSSL_RELEASE_STRUCT(type) \
struct type##_scope \
{ \
    type** p; \
    type##_scope(type** p) { this->p = p; } \
    ~type##_scope() { if(p && *p) { type##_free(*p); *p = NULL; } } \
};

#define DECLARE_OPENSSL_RELEASE_STACK(type) \
struct type##Stack_scope \
{ \
    STACK_OF(type)** p; \
    type##Stack_scope(STACK_OF(type)** p) { this->p = p; } \
    ~type##Stack_scope() { if(p && *p) { sk_##type##_free(*p); *p = NULL; } } \
};

DECLARE_OPENSSL_RELEASE_STRUCT(BIO)
DECLARE_OPENSSL_RELEASE_STRUCT(X509)
DECLARE_OPENSSL_RELEASE_STACK(X509)


#endif // !defined(__OPENSSLHELPERS_H_INCLUDED__)
