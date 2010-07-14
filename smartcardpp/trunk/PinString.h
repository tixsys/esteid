#include <smartcardpp/locked_allocator.h>

typedef std::basic_string<char, std::char_traits<char>, locked_allocator<char> > base_str;

/// specialiced class for passing PIN data around
/// use reserve() to always trigger heap allocation, internal stack buffers are 16 bytes
class PinString : public base_str {
	enum {
		ALLOCSZ = 128
	};
public:
	PinString() : base_str() {
		reserve(ALLOCSZ);
		}
	PinString(const PinString & str) : base_str(str) {reserve(ALLOCSZ);}
	PinString(const base_str & str) : base_str(str) {reserve(ALLOCSZ);}
	PinString(const char * s ) : base_str(s) {reserve(ALLOCSZ);}
	PinString(const char * s, size_t n ) : base_str(s,n) {reserve(ALLOCSZ);}
	PinString( size_t n, char c ) : base_str(n,c) {reserve(ALLOCSZ);}
	template<class InputIterator>
	PinString(InputIterator begin,InputIterator end) : base_str(begin,end) {}
	~PinString() {}
};
