#pragma once

#include "stdafx.h"
#include "HashFunction.h"

// State structure
class KeccakBase : public HashFunction
{
public:
	KeccakBase(unsigned int len);
	virtual ~KeccakBase();
	virtual std::vector<unsigned char> digest() = 0;
	virtual void addPadding() = 0;
	void reset();
	void keccakf();
	virtual void addData(uint8_t input);
	virtual void addData(const uint8_t *input, unsigned int off, unsigned int len);
	void processBuffer();
protected:
	uint64_t *A;
	unsigned int blockLen;
	uint8_t *buffer;
	unsigned int bufferLen;
	unsigned int length;
};

class Sha3 : public KeccakBase
{
public:
	Sha3(unsigned int len);
	std::vector<unsigned char> digest();
	void addPadding();
private:
};

class Keccak : public KeccakBase
{
public:
	Keccak(unsigned int len);
	std::vector<unsigned char> digest();
	void addPadding();
private:
};

class Shake : public KeccakBase
{
public:
	Shake(unsigned int len, unsigned int d_);
	std::vector<unsigned char> digest();
	void addPadding();
private:
	unsigned int d;
};
