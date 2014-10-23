/*
 * =====================================================================================
 * 
 *        Filename:  LDWriter.hh
 * 
 *     Description:  A writer calss
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

#ifndef LD_WRITER_H
#define LD_WRITER_H

#include <cstdio>

class LDWriter{

	FILE* out_;
		
public:
	LDWriter(FILE* _out) : out_(_out){}

	int writeUInt8(unsigned char _val) const;
	int writeInt8(signed char _val) const;
	int writeUInt16(unsigned short _val) const;
	int writeInt16(signed short _val) const;
	int writeUInt32(unsigned int _val) const;
	int writeInt32(signed int _val) const;
	
	int writeFloat32(float _val) const;
	int writeFloat64(double _val) const;

	int writeBuffer(unsigned char* _buffer, unsigned int _size) const;
};


//class LDWriter{
//
//	FILE* out_;
//	const int bufSize_;
//	unsigned char* buf_;
//	unsigned char* pos_;
//	int offset_;
//	
//public:
//	LDWriter(FILE* _out, int _bufSize=1024);
//	~LDWriter();
//
//	int writeBit(bool _val); 
//	int writeUIntK(int _K, unsigned int _val);	//write K bits starting with MSB
//	int writeSIntK(int _K, signed int _val);	//write K bits in 2-complement repr. starting with MSB
//	int writeFloat32(float _val);
//	int writeFloat64(double _val);
//	int writeBuffer(unsigned char* _buffer, unsigned int _size) ;	
//	void flushFinal();
//
//private:
//	void flush();
//};

#endif //LD_WRITER_H

