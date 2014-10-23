#include "LDReader.h"
#include <cassert>

void LDReader::readUInt8(unsigned char& _val) const {
	assert(sizeof(_val)==1);
	fread(&_val, 1, 1, in_);
}

void LDReader::readInt8(signed char& _val) const {
	assert(sizeof(_val)==1);
	fread(&_val, 1, 1, in_); //two complement
}

void LDReader::readUInt16(unsigned short& _val) const {
	assert(sizeof(_val)==2);
	fread(&_val, 1, 2, in_);
}

void LDReader::readInt16(signed short& _val) const {
	assert(sizeof(_val)==2);
	fread(&_val, 1, 2, in_); //two complement
}

void LDReader::readUInt32(unsigned int& _val) const {
	assert(sizeof(_val)==4);
	fread(&_val, 1, 4, in_);
}

void LDReader::readInt32(signed int& _val) const {
	assert(sizeof(_val)==4);
	fread(&_val, 1, 4, in_); //two complement
}

void LDReader::readFloat32(float& _val) const {
	assert(sizeof(_val)==4);
	fread(&_val, 1, 4, in_); 
}

void LDReader::readFloat64(double& _val) const {
	assert(sizeof(_val)==8);
	fread(&_val, 1, 8, in_); 
}

void LDReader::readBuffer(unsigned char* _buffer, unsigned int _size) const {
	assert(sizeof(unsigned char)==1);
	fread(_buffer, 1, _size, in_);
}
//
//
//LDReader::LDReader(FILE* _in, int _streamSize, int _bufSize) : in_(_in), bufSize_(_bufSize), streamSize_(_streamSize), nBytesRead_(0), buf_(0), pos_(0), offset_(0){
//		buf_ = new unsigned char[bufSize_];
//		fill();
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
//LDReader::~LDReader(){
//	delete [] buf_;
//}
//
//bool LDReader::readBit() {
//	unsigned char mask = 1<<(7-offset_);
//	bool bit = (bool) ((*pos_)& mask);
//
//	offset_=(offset_+1)%8;
//	if (offset_==0){
//		++pos_;
//		if (pos_-buf_ == bufSize_) fill();
//	}
//
//	return bit;
//}
//
//int LDReader::readUIntK(int _K, unsigned int& _val){
//	assert(_K > 0);
//	assert(_K <= 32);
//
//	_val = 0;
//	unsigned int mask = 1<<(_K-1);
//    for (int k=0; k<_K; ++k){
//		if (readBit()) _val |= mask;
//		mask >>= 1;
//    }
//	return _K;
//}
//
//int LDReader::readSIntK(int _K, signed int& _val){
//	unsigned int val = _val;
//	readUIntK(_K, val);
//	_val = val;
//	return _K;
//}
//
//int LDReader::fill(){
//	int nBytesToRead = (streamSize_- nBytesRead_ >= bufSize_) ? bufSize_ : streamSize_- nBytesRead_;
//	nBytesRead_ += fread(buf_, 1, nBytesToRead, in_);
//	assert(!feof(in_));//check if wrong streamSize, i.e. no bytes left for reading in file
//	pos_=buf_;
//	return nBytesRead_;
//}
//
//int LDReader::readFloat32(float& _val) {
//	union {
//		unsigned char* pattern;
//		float* val;
//	} fl;
//	fl.val = &_val;
//
//	unsigned int tmp = 0;
//	for (int k=0; k<4; ++k){
//		readUIntK(8, tmp);
//		fl.pattern[k] = tmp;
//	}
//	return 32;
//}
//
//int LDReader::readFloat64(double& _val) {
//	union {
//		unsigned char* pattern;
//		double* val;
//	} db;
//	db.val = &_val;
//
//	unsigned int tmp = 0;
//	for (int k=0; k<8; ++k){
//		readUIntK(8, tmp);
//		db.pattern[k] = tmp;
//	}
//	return 64;
//}
//
//int LDReader::readBuffer(unsigned char* _buffer, unsigned int _size) {
//	unsigned int val = 0;
//	for (int k=0; k<_size; ++k){
//		readUIntK(8, val);
//		_buffer[k] = val;
//	}
//
//	return _size*8;
//}
//
//
