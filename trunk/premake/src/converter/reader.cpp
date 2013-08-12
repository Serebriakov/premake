#include <fstream>

#include "reader.h"


Reader::Reader(const std::string &Path) : m_Offset(0), m_Finished(true)
{
	if( Path != "" )
	{
		ReadFile(Path);
	}
}

/*
Check if reached the end
*/
void Reader::CheckOffset()
{
	m_Finished = (size_t)m_Offset >= m_Array.size();
}

/*
Read the contents of a file into m_Array
*/
Reader::ERROR_CODE Reader::ReadFile(const std::string &Path)
{
	// open and read the file
    std::streampos file_size;
	std::ifstream file(Path.c_str(), std::ios::binary);
	if( file.is_open() == false )
	{
		return CANNOT_OPEN_FILE;
	}
    file.seekg(0, std::ios::end);
    file_size = file.tellg();
    file.seekg(0, std::ios::beg);
	m_Array.resize((size_t)file_size);
    file.read((char*)&m_Array[0], file_size);
	file.close();

	// reset the offset
	m_Offset = 0;
	m_Finished = false;

	return OK;
}

/*
Get a raw pointer to the data
*/
const unsigned char *Reader::GetPointer() const
{
	return &m_Array[0];
}

/*
Get the size of file in bytes
*/
unsigned Reader::GetSize() const
{
	return m_Array.size();
}

/*
Set the offset to an absolute value
*/
Reader::ERROR_CODE Reader::JumpTo(int Offset)
{
	if( ((size_t)Offset >= m_Array.size()) || (Offset < 0) )
	{
		return INVALID_OFFSET;
	}

	m_Offset = Offset;
	return OK;
}

/*
Modify the offset with a relative value
*/
Reader::ERROR_CODE Reader::JumpWith(int DeltaOffset)
{
	if( ((size_t)(m_Offset + DeltaOffset) >= m_Array.size()) || (m_Offset + DeltaOffset < 0) )
	{
		return INVALID_OFFSET;
	}

	m_Offset += DeltaOffset;
	return OK;
}

/*
Get the current value of the offset
*/
int Reader::GetCurrentOffset() const
{
	return m_Offset;
}

/*
Read a single byte
*/
Reader::ERROR_CODE Reader::GetByte(unsigned char *Dst)
{
	if( m_Finished )
	{
		return ALREADY_FINISHED;
	}

	*Dst = m_Array[m_Offset];
	m_Offset++;
	CheckOffset();

	return OK;
}

/*
Read a 2-byte integer
*/
Reader::ERROR_CODE Reader::GetUnsignedShort(unsigned short *Dst)
{
	if( m_Finished )
	{
		return ALREADY_FINISHED;
	}

	unsigned short value = 0;
	for( int i = 0; i < sizeof(unsigned short); i++ )
	{
		value |= ((unsigned short)m_Array[m_Offset + i]) << (i * 8);
	}
	*Dst = value;

	m_Offset += sizeof(unsigned short);
	CheckOffset();

	return OK;
}

/*
Read n bytes into an array
*/
Reader::ERROR_CODE Reader::GetBytes(void *Dst, unsigned Count)
{
	if( m_Finished )
	{
		return ALREADY_FINISHED;
	}

	memcpy(Dst, &m_Array[m_Offset], Count);
	m_Offset += Count;
	CheckOffset();

	return OK;
}
