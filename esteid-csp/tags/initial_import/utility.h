template<class E,
         class T = std::char_traits<E>,
         class A = std::allocator<E> >
class Widen : public std::unary_function<
    const std::string&, std::basic_string<E, T, A> >
{
    std::locale loc_;
    const std::ctype<E>* pCType_;

    // No copy-constructor, no assignment operator...
    Widen(const Widen&);
    Widen& operator= (const Widen&);
public:
    // Constructor...
    Widen(const std::locale& loc = std::locale()) : loc_(loc)
    {
#if defined(_MSC_VER) && (_MSC_VER < 1300)  // VC++ 6.0...
        using namespace std;
        pCType_ = &_USE(loc, ctype<E>);
#else
        pCType_ = &std::use_facet<std::ctype<E> >(loc);
#endif
    }

    // Conversion...
    std::basic_string<E, T, A> operator() (const std::string& str) const
    {
        typename std::basic_string<E, T, A>::size_type srcLen = str.length();
		if (!srcLen) 
			return std::basic_string<E , T, A>();
        const char* pSrcBeg = str.c_str();
        std::vector<E> tmp(srcLen);

        pCType_->widen(pSrcBeg, pSrcBeg + srcLen, &tmp[0]);
        return std::basic_string<E, T, A>(&tmp[0], srcLen);
    }
}; 

inline std::string narrow(std::wstring source)
{
    std::string result(source.size(), char(0));
    typedef std::ctype<wchar_t> ctype_t;
    const ctype_t& ct = std::use_facet<ctype_t>(std::locale());
    ct.narrow(source.data(), source.data() + source.size(), '\u00B6',&(*result.begin()));
    return result;

} 