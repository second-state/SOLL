#ifndef RAII_WRAPPER_H_
#define RAII_WRAPPER_H_

#include "Keccak.h"
#include "stdafx.h"

// RAII wrapper for C standard library FILE pointers.
//  Calls fclose on destruction.
class FileHandleWrapper
{
public:
	FileHandleWrapper(FILE *fh) : fHand(fh) {};
	~FileHandleWrapper() 
	{
		fclose(fHand);
	};
private:
	FILE *fHand;
};


// Array wrapper - simple wrapper to RAII-ise a 
//  array allocation.
//  Considered using std::array, but it seems to
//  want the array size to be set at compile time.
template<typename T1>
class ArrayWrapper
{
public:
	ArrayWrapper(unsigned int s) : size(s)
	{
		data = new T1[s];
	}
	~ArrayWrapper() 
	{
		delete[] data;
	}
	const T1 *data;
private:
	const unsigned int size;
};

#endif //RAII_WRAPPER_H
