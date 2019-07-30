#ifndef HEX_H_
#define HEX_H_

static inline char Hex(const unsigned char a)
{
	const char *palette = "0123456789abcdef";
	if (a <= 0x0F)
	{
		return palette[a];
	}
	return '-';
}

template <typename F>
static inline void Hex(const unsigned char a, F func)
{
	unsigned char b = a & 0x0F;
	unsigned char c = a & 0xF0;
	c >>= 4;
	func(Hex(b));
	func(Hex(c));
}

#endif //HEX_H_
