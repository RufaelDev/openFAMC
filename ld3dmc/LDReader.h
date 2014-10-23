/*
 * =====================================================================================
 * 
 *        Filename:  LDReader.hh
 * 
 *     Description:  A reader class
 * 
 *         Version:  1.0
 *         Created:  07/25/07 17:15:18 CEST
 *        Revision:  none
 *        Compiler:  gcc
 * 
 *          Author:  Nikolce Stefanoski (nst)
 *         Company:  Institut für Informationsverarbeitung
 *           Email:  stefanos@tnt.uni-hannover.de
 * 
 * =====================================================================================
 */

#ifndef LD_READER_H
#define LD_READER_H

#include <stdio.h>

class LDReader{

	FILE* in_;
	
public:
	LDReader(FILE* _in) : in_(_in){}

	void readUInt8(unsigned char& _val) const;
	void readInt8(signed char& _val) const;
	void readUInt16(unsigned short& _val) const;
	void readInt16(signed short& _val) const;
	void readUInt32(unsigned int& _val) const;
	void readInt32(signed int& _val) const;
	
	void readFloat32(float& _val) const;
	void readFloat64(double& _val) const;

	void readBuffer(unsigned char* _buffer, unsigned int _size) const;
};

//class LDReader{
//
//	FILE* in_;
//	const int bufSize_;
//	const int streamSize_;
//	int nBytesRead_;
//	unsigned char* buf_;
//	unsigned char* pos_;
//	
//	int offset_;
//	
//public:
//	LDReader(FILE* _in, int _streamSize, int _bufSize=1024);
//	~LDReader();
//
//	bool readBit();
//	int readUIntK(int _K, unsigned int& _val);
//	int readSIntK(int _K,   signed int& _val);
//	int readFloat32(float& _val);
//	int readFloat64(double& _val);
//	int readBuffer(unsigned char* _buffer, unsigned int _size);
//
//	private:
//		int fill();
//};

#endif //LD_READER_H

