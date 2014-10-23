#include "LDWriter.h"
#include <cassert>
#include <cstring>

int LDWriter::writeUInt8(unsigned char _val) const {
	assert(sizeof(_val)==1);
	fwrite(&_val, 1, 1, out_);
	return 1;
}

int LDWriter::writeInt8(signed char _val) const {
	assert(sizeof(_val)==1);
	fwrite(&_val, 1, 1, out_);
	return 1;
}

int LDWriter::writeUInt16(unsigned short _val) const {
	assert(sizeof(_val)==2);
	fwrite(&_val, 1, 2, out_);
	return 2;
}

int LDWriter::writeInt16(signed short _val) const {
	assert(sizeof(_val)==2);
	fwrite(&_val, 1, 2, out_);
	return 2;
}

int LDWriter::writeUInt32(unsigned int _val) const {
	assert(sizeof(_val)==4);
	fwrite(&_val, 1, 4, out_);
	return 4;
}

int LDWriter::writeInt32(signed int _val) const {
	assert(sizeof(_val)==4);
	fwrite(&_val, 1, 4, out_);
	return 4;
}

int LDWriter::writeFloat32(float _val) const {
	assert(sizeof(_val)==4);
	fwrite(&_val, 1, 4, out_);
	return 4;
}

int LDWriter::writeFloat64(double _val) const {
	assert(sizeof(_val)==8);
	fwrite(&_val, 1, 8, out_);
	return 8;
}

int LDWriter::writeBuffer(unsigned char* _buffer, unsigned int _size) const {
	assert(sizeof(unsigned char)==1);
	fwrite(_buffer, 1, _size, out_);

	return _size;
}


//LDWriter::LDWriter(FILE* _out, int _bufSize) : out_(_out), bufSize_(_bufSize), buf_(0), pos_(0), offset_(0){
//		buf_ = new unsigned char[bufSize_];
//		memset( buf_, '\0', bufSize_ );
//		pos_ = buf_;
//
//		assert(sizeof(char) == 1);
//		assert(sizeof(signed char) == 1);
//		assert(sizeof(unsigned char) == 1);
//		
//		assert(sizeof(int) == 4);
//		assert(sizeof(unsigned int) == 4);
//
//		assert(sizeof(float) == 4);
//		assert(sizeof(double) == 8);
//}
//
//LDWriter::~LDWriter(){
//	delete [] buf_;
//}
//
//int LDWriter::writeBit(bool _val) {
//	unsigned char mask = 1<<(7-offset_);
//	if (_val)	(*pos_)|= mask;
//	else		(*pos_)&= ~mask;
//
//	offset_=(offset_+1)%8;
//	if (offset_==0){
//		++pos_;
//		if (pos_-buf_ == bufSize_) flush();
//	}
//
//	/*if (_val) printf("1\n");
//	else printf("0\n");*/
//	return 1;
//}
//
//int LDWriter::writeUIntK(int _K, unsigned int _val){
//	assert(_K > 0);
//	assert(_K <= 32);
//
//	unsigned int mask = 1<<(_K-1);
//    for (int k=0; k<_K; ++k){
//		writeBit((bool)(_val & mask));
//		mask >>= 1;
//    }
//	return _K;
//}
//
//int LDWriter::writeSIntK(int _K, signed int _val){
//	return writeUIntK(_K, _val);
//}
//
//int LDWriter::writeFloat32(float _val) {
//	union {
//		unsigned char* pattern;
//		float* val;
//	} fl;
//	fl.val = &_val;
//
//	for (int k=0; k<4; ++k){
//		writeUIntK(8, fl.pattern[k]);
//	}
//	return 32;
//
//}
//
//int LDWriter::writeFloat64(double _val){
//	union {
//		unsigned char* pattern;
//		double* val;
//	} db;
//	db.val = &_val;
//
//	for (int k=0; k<8; ++k){
//		writeUIntK(8, db.pattern[k]);
//	}
//	return 64;
//}
//
//int LDWriter::writeBuffer(unsigned char* _buffer, unsigned int _size) {
//	for (int k=0; k<_size; ++k){
//		writeUIntK(8,_buffer[k]);
//	}
//	return _size*8;
//}
//
//void LDWriter::flush(){
//	fwrite(buf_, 1, pos_-buf_, out_);
//	pos_=buf_;
//}
//
//void LDWriter::flushFinal(){
//	if (offset_>0){
//		for (int k=offset_; k<8; ++k) writeBit(false); //fill last byte
//	}
//	flush();
//}



