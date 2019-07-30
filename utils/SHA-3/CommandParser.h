#pragma once

enum class HashType { Sha3, Keccak, Shake };

struct options
{
	HashType type;
	unsigned int hashWidth;
	unsigned int shakeDigestLength;
};


int doFile(const char *fileName, options &opt);

void usage();

int parseAlg(const char *param, const unsigned int pSize, options &opt);

int parseWidth(const char *param, const unsigned int pSize, options &opt);

int parseDigestWidth(const char *param, const unsigned int pSize, options &opt);

int parseOption(const char *param, const unsigned int pSize, options &opt);

void parseParameter(const char *param, options &opt);

void parseCommandLine(const int argc, char* argv[]);