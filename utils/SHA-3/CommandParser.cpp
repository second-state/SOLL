#include "stdafx.h"
#include "CommandParser.h"
#include "Keccak.h"
#include "ParserCommon.h"
#include "RAII_Wrapper.h"
#include <cstdlib>
#include "Hex.h"


unsigned int sha3widths[] = {224, 256, 384, 512, 0};
unsigned int keccakwidths[] = {224, 256, 384, 512, 0};
unsigned int shakewidths[] = {128, 256, 0};

unsigned int bufferSize = 1024 * 4;


template<typename F>
int readFileIntoFunc(const char *fileName, F f) 
{
	FILE *fHand = fopen(fileName, "rb");
	if (!fHand)
	{
		std::cerr << "Unable to open input file: " << fileName << "\n";
		return 0;
	}
	FileHandleWrapper fhw(fHand);

	fseek(fHand, 0, SEEK_SET);
	ArrayWrapper<char> buf(bufferSize);
	while (true)
	{
		unsigned int bytesRead = fread((void *)buf.data, 1, bufferSize, fHand);

		f((uint8_t*)buf.data, bytesRead);
		if (bytesRead < bufferSize)
		{
			break;
		}
	}

	return 1;
};

template<typename F1>
int hashFile(const char *fileName, const std::string &hashName, F1 &hashObj)
{
	//unsigned int hashSize = hashWidth;

	if (readFileIntoFunc(fileName, [&hashObj](uint8_t* buf, unsigned int bLength){ hashObj.addData(buf, 0, bLength); }) == 0)
	{
		return 0;
	}

	std::vector<unsigned char> op = hashObj.digest();

	std::ostringstream b;
	std::cout << hashName << fileName << ": ";
	for (auto &oi : op)
	{
		Hex(oi, [&b](unsigned char a) { b << a; });
	}
	std::cout << b.str() << "\n";
	return 1;
}

int doFile(const char *fileName, options &opt)
{
	if(opt.type == HashType::Sha3)
	{
		//  SHA-3
		Sha3 h(opt.hashWidth);
		std::ostringstream description;
		description << "SHA-3-" << opt.hashWidth;
		return hashFile(fileName, description.str(), h);
	}
	else if (opt.type == HashType::Keccak)
	{
		// Keccak
		Keccak h(opt.hashWidth);
		std::ostringstream description;
		description << "Keccak-" << opt.hashWidth;
		return hashFile(fileName, description.str(), h);
	}
	else if (opt.type == HashType::Shake)
	{
		// SHAKE
		Shake h(opt.hashWidth, opt.shakeDigestLength);
		std::ostringstream description;
		description << "SHAKE-" << opt.shakeDigestLength;
		return hashFile(fileName, description.str(), h);

	}
	return 0;
}


void usage()
{
	puts("\n\nUsage: sha3sum [command]* file*\n"
	"\n"
	" where command is an optional parameter that can set either the algorithm, as\n"
	" there is a slight difference between the bare keccak function and the SHA-3\n"
	" variant.\n"
	"\n" 
	" Algorithm \n"
	"\n" 
	" -a=s   :  Set algorithm to SHA-3 (default).\n"
	" -a=k   :  Set algotithm to Keccak.\n"
	" -a=h   :  Set algotithm to SHAKE.\n"
	"\n" 
	" Size\n"
	" \n"
	" -w=224 :  Set width to 224 bits.\n"
	" -w=256 :  Set width to 256 bits.\n"
	" -w=384 :  Set width to 384 bits.\n"
	" -w=512 :  Set width to 512 bits (default).\n"
	"\n"
	" Digest size (SHAKE)\n"
	"\n"
	" -d=number : Set the SHAKE digest size. Should be less than or equal to the hash size.\n"
	"		should be multiple of 8.\n"
	"       Only relevant for SHAKE - For SHA-3 and keccak, digest size is equal to sponge size.\n"
	"\n"
	"Any number of files can be specified. Files will be processed with the most\n"
	"recently specified options - for example:\n"
	"\n"
	"  sha3sum test.txt -a=k -w=384 test.txt -a=s -w=256 text.txt\n"
	"\n"
	"will hash \"test.txt\" three times - First with 512-bit SHA-3, then with 384-bit\n"
	"keccak, then finally with 256-bit SHA-3.\n");

}

int parseAlg(const char *param, const unsigned int pSize, options &opt)
{
	unsigned int index = 0;
	if(param[index] == '=')
	{
		index++;
	}

	if(index + 1 == pSize)
	{
		const char algInitial = param[index];
		if(algInitial == 'k')
		{
			opt.type = HashType::Keccak;
			return 1;
		}
		else if(algInitial == 's')
		{
			opt.type = HashType::Sha3;
			return 1;
		}
		else if (algInitial == 'h')
		{
			opt.type = HashType::Shake;
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

int parseWidth(const char *param, const unsigned int pSize, options &opt)
{
	unsigned int index = 0;
	if(param[index] == '=')
	{
		index++;
	}

	if(index+3 == pSize)
	{
		if(strncmp(&param[index], "224", pSize-index)==0)
		{
			opt.hashWidth = 224;
			return 1;
		}
		else if(strncmp(&param[index], "256", pSize-index)==0)
		{
			opt.hashWidth = 256;
			return 1;	
		}
		else if(strncmp(&param[index], "384", pSize-index)==0)
		{
			opt.hashWidth = 384;
			return 1;
		}
		else if(strncmp(&param[index], "512", pSize-index)==0)
		{
			opt.hashWidth = 512;
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}

}

int parseDigestWidth(const char *param, const unsigned int pSize, options &opt)
{
	unsigned int index = 0;
	if (param[index] == '=')
	{
		index++;
	}

	if (index + 3 == pSize)
	{
		if (isNumeric(&param[index], pSize - index))
		{
			unsigned int sdl = atoun(&param[index], pSize - index);
			if (sdl % 8 != 0)
			{
				std::cerr << "Error - param: " << param + index << " is not divisible by 8.\n";
				return 0;
			}
			if (sdl > opt.hashWidth)
			{
				std::cerr << "Error - param: " << param + index << " is greater than the hash size.\n";
				return 0;
			}
			opt.shakeDigestLength = sdl;
			return 1;
		}
		else
		{
			std::cerr << "Error - param: " << param + index << " is not numeric. \n";
			return 0;
		}
	}
	else
	{
		std::cerr << "Error - param: " << param + index << " - expecting a three digit number.\n";
		return 0;
	}

}

int parseOption(const char *param, const unsigned int pSize, options &opt)
{
	unsigned int index = 1;

	if(index != pSize)
	{
		const char commandInitial = param[index];
		if(commandInitial == 'h')
		{
			if((index + 1) == pSize)
			{
				usage();
				return 0;
			}
			else
			{
				return 0;
			}
		}
		else if(commandInitial == 'a')
		{
			return parseAlg(&param[index+1], pSize-(index+1), opt);	
		}
		else if(commandInitial == 'w')
		{
			return parseWidth(&param[index+1], pSize-(index+1), opt);
		}
		else if (commandInitial == 'd')
		{
			return parseDigestWidth(&param[index + 1], pSize - (index + 1), opt);
		}
		else
		{
			std::cerr << "Error - Unrecognised option " << param << "\n";
			return 0;	
		}
	}
	else 
	{
		std::cerr << "Error - malformed option.\n";
		return 0;
	}
}

void parseParameter(const char *param, options &opt)
{
	unsigned int index = 0;
	unsigned int paramSize = 0;

	paramSize = strlen(param);

	// Eat leading whitespace
	for(unsigned int i = index ; i != paramSize ; i++)
	{
		const char posI = param[i];
		if((posI != ' ') && (posI != '\t'))
		{
			index = i;
			break;
		}
	}

	if(index != paramSize)
	{
		if(param[index] != '-')
		{
			doFile(&param[index], opt);
		}
		else
		{
			parseOption(&param[index], paramSize-index, opt);	
		}
	}
}

void parseCommandLine(const int argc, char* argv[])
{
	if(argc > 1)
	{

		options opt;
		// Default options
		opt.type = HashType::Sha3;
		opt.hashWidth = 512;
		opt.shakeDigestLength = 512;

		for(unsigned int i = 1 ; i != argc ; i++)
		{
			parseParameter(argv[i], opt);
		}	
	}
}
