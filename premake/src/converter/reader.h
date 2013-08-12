#ifndef _READER_H_
#define _READER_H_

#include <vector>
#include <string>


/*
Creates a byte sequence from a binary file
*/
class Reader
{
protected :
	std::vector<unsigned char>	m_Array;										// array of bytes read from the file
	int							m_Offset;										// current position in m_Array
	bool						m_Finished;										// already reached the end of the array

	// check if reached the end
	void CheckOffset();


public :
	// type for return values
	enum ERROR_CODE
	{
		OK,
		CANNOT_OPEN_FILE,
		INVALID_OFFSET,
		ALREADY_FINISHED
	};


	Reader(const std::string &Path = "");

	// read the contents of a file
	ERROR_CODE ReadFile(const std::string &Path);

	// get a raw pointer to the data
	const unsigned char *GetPointer() const;

	// get the size of file in bytes
	unsigned GetSize() const;

	// set the offset to an absolute value
	ERROR_CODE JumpTo(int Offset);

	// modify the offset with a relative value
	ERROR_CODE JumpWith(int DeltaOffset);

	// get the current position of the offset
	int GetCurrentOffset() const;

	// read a single byte
	ERROR_CODE GetByte(unsigned char *Dst);

	// read a 2-byte integer
	ERROR_CODE GetUnsignedShort(unsigned short *Dst);

	// read n bytes into an array
	ERROR_CODE GetBytes(void *Dst, unsigned Count);
};

#endif
