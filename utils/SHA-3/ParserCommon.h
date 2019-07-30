#ifndef PARSER_COMMON_H_
#define PARSER_COMMON_H_

// atoun - Version of atoi which takes a limited size buffer and returns an 
//   unsigned int
inline unsigned int atoun(const char *buf, const unsigned int length)
{
	unsigned int i = 0;
	unsigned int noDigits = 0;
	while (i != length)
	{
		if ((buf[i] >= '0') && (buf[i] <= '9'))
		{
			noDigits++;
		}
		else
		{
			break;
		}
		i++;
	}

	unsigned int temp = 0;
	for (i = 0; i != noDigits; i++)
	{
		temp *= 10;
		temp += (buf[i] - '0');
	}

	return temp;
}

inline bool isNumeric(const char a)
{
	if ((a >= '0') && (a <= '9'))
	{
		return true;
	}
	return false;
}

// return true if the buffer range is all numeric.
inline bool isNumeric(const char *buf, const unsigned int size)
{
	for (unsigned int i=0 ; i!=size ; i++)
	{
		if (!isNumeric(buf[i]))
		{
			return false;
		}
	}
	return true;
}


#endif //PARSER_COMMON_H_