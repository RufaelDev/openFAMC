/*!
 ***************************************************************************
 * \file
 *    cabac.h
 *
 * \brief
 *    Headerfile for entropy coding routines
 *
 * \author
 *    Detlev Marpe                                                         \n
 *    Copyright (C) 2000 HEINRICH HERTZ INSTITUTE All Rights Reserved.
 *
 * \date
 *    21. Oct 2000 (Changes by Tobias Oelbaum 28.08.2001)
 ***************************************************************************
 */

#ifndef _CABAC_H_INCLUDED__
#define _CABAC_H_INCLUDED__

// #pragma once
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>


#include "FAMCCommon.h"

#include<vector>
#include<math.h>
#include<algorithm>

/***********************************************************************
 * D a t a    t y p e s   f o r  C A B A C
 ***********************************************************************
 */
typedef unsigned char byte;    //!< byte type definition

//! struct to characterize the state of the arithmetic coding engine
typedef struct
{
  unsigned int  Elow, Erange;
  unsigned int  Ebuffer;
  unsigned int  Ebits_to_go;
  unsigned int  Ebits_to_follow;
  byte          *Ecodestrm;
  int           *Ecodestrm_len;
  int           C;
  int           E;

} EncodingEnvironment;

typedef EncodingEnvironment *EncodingEnvironmentPtr;

//! struct for context management
typedef struct
{
  unsigned short state;         // index into state-table CP  
  unsigned char  MPS;           // Least Probable Symbol 0/1 CP
} BiContextType;

typedef BiContextType *BiContextTypePtr;

//! struct to characterize the state of the arithmetic coding engine
typedef struct
{
  unsigned int    Dlow, Drange;
  unsigned int    Dvalue;
  unsigned int    Dbuffer;
  int             Dbits_to_go;
  byte            *Dcodestrm;
  int             *Dcodestrm_len;
} DecodingEnvironment;

typedef DecodingEnvironment *DecodingEnvironmentPtr;

/************************************************************************
 * D e f i n i t i o n s
 ***********************************************************************
 */

// some definitions to increase the readability of the source code
//endocding
#define Elow                      (eep->Elow)
#define Erange                    (eep->Erange)
#define Ebits_to_follow           (eep->Ebits_to_follow)
#define Ebuffer                   (eep->Ebuffer)
#define Ebits_to_go               (eep->Ebits_to_go)
#define Ecodestrm                 (eep->Ecodestrm)
#define Ecodestrm_len             (eep->Ecodestrm_len)
#define Ecodestrm_laststartcode   (eep->Ecodestrm_laststartcode)
//decoding
#define Dbuffer         (dep->Dbuffer)
#define Dbits_to_go     (dep->Dbits_to_go)
#define Dcodestrm       (dep->Dcodestrm)
#define Dcodestrm_len   (dep->Dcodestrm_len)
//both
#define B_BITS                    10 // Number of bits to represent the whole coding interval
#define ONE                       (1 << B_BITS)
#define HALF                      (1 << (B_BITS-1))
#define QUARTER                   (1 << (B_BITS-2))

/* Range table for LPS */ 
const byte rLPS_table_64x4[64][4]=
{
        { 128, 176, 208, 240},
        { 128, 167, 197, 227},
        { 128, 158, 187, 216},
        { 123, 150, 178, 205},
        { 116, 142, 169, 195},
        { 111, 135, 160, 185},
        { 105, 128, 152, 175},
        { 100, 122, 144, 166},
        {  95, 116, 137, 158},
        {  90, 110, 130, 150},
        {  85, 104, 123, 142},
        {  81,  99, 117, 135},
        {  77,  94, 111, 128},
        {  73,  89, 105, 122},
        {  69,  85, 100, 116},
        {  66,  80,  95, 110},
        {  62,  76,  90, 104},
        {  59,  72,  86,  99},
        {  56,  69,  81,  94},
        {  53,  65,  77,  89},
        {  51,  62,  73,  85},
        {  48,  59,  69,  80},
        {  46,  56,  66,  76},
        {  43,  53,  63,  72},
        {  41,  50,  59,  69},
        {  39,  48,  56,  65},
        {  37,  45,  54,  62},
        {  35,  43,  51,  59},
        {  33,  41,  48,  56},
        {  32,  39,  46,  53},
        {  30,  37,  43,  50},
        {  29,  35,  41,  48},
        {  27,  33,  39,  45},
        {  26,  31,  37,  43},
        {  24,  30,  35,  41},
        {  23,  28,  33,  39},
        {  22,  27,  32,  37},
        {  21,  26,  30,  35},
        {  20,  24,  29,  33},
        {  19,  23,  27,  31},
        {  18,  22,  26,  30},
        {  17,  21,  25,  28},
        {  16,  20,  23,  27},
        {  15,  19,  22,  25},
        {  14,  18,  21,  24},
        {  14,  17,  20,  23},
        {  13,  16,  19,  22},
        {  12,  15,  18,  21},
        {  12,  14,  17,  20},
        {  11,  14,  16,  19},
        {  11,  13,  15,  18},
        {  10,  12,  15,  17},
        {  10,  12,  14,  16},
        {   9,  11,  13,  15},
        {   9,  11,  12,  14},
        {   8,  10,  12,  14},
        {   8,   9,  11,  13},
        {   7,   9,  11,  12},
        {   7,   9,  10,  12},
        {   7,   8,  10,  11},
        {   6,   8,   9,  11},
        {   6,   7,   9,  10},
        {   6,   7,   8,   9},
        {   2,   2,   2,   2}
};

const unsigned short AC_next_state_MPS_64[64] =    
{
                1,2,3,4,5,6,7,8,9,10,
                11,12,13,14,15,16,17,18,19,20,
                21,22,23,24,25,26,27,28,29,30,
                31,32,33,34,35,36,37,38,39,40,
                41,42,43,44,45,46,47,48,49,50,
                51,52,53,54,55,56,57,58,59,60,
                61,62,62,63
};      

const unsigned short AC_next_state_LPS_64[64] =    
{
                 0, 0, 1, 2, 2, 4, 4, 5, 6, 7,
                 8, 9, 9,11,11,12,13,13,15,15, 
                 16,16,18,18,19,19,21,21,22,22,
                 23,24,24,25,26,26,27,27,28,29,
                 29,30,30,30,31,32,32,33,33,33,
                 34,34,35,35,35,36,36,36,37,37, 
                 37,38,38,63 
};

/*!
 ************************************************************************
 * Macro for writing bytes of code
 ***********************************************************************
 */


#define put_byte() { \
                     Ecodestrm[(*Ecodestrm_len)++] = Ebuffer; \
                     Ebits_to_go = 8; \
                     while (eep->C > 7) { \
                       eep->C-=8; \
                       eep->E++; \
                     } \
                    } 

#define put_one_bit(b) { \
                         Ebuffer <<= 1; Ebuffer |= (b); \
                         if (--Ebits_to_go == 0) \
                           put_byte(); \
                       }

#define put_one_bit_plus_outstanding(b) { \
                                          put_one_bit(b); \
                                          while (Ebits_to_follow > 0) \
                                          { \
                                            Ebits_to_follow--; \
                                            put_one_bit(!(b)); \
                                          } \
                                         }

#define get_byte(){                                         \
                    Dbuffer = Dcodestrm[(*Dcodestrm_len)++];\
                    Dbits_to_go = 7;                        \
                  }


class CABAC
{
public:
	int _pic_bin_count;
	EncodingEnvironmentPtr _eep;
	DecodingEnvironmentPtr _dep;
	BiContextTypePtr _ctx;


public:
	CABAC(void);
	~CABAC(void);

//	Encoding
	inline void reset_pic_bin_count(){ _pic_bin_count = 0;}
	inline int get_pic_bin_count(){return _pic_bin_count;}
	static void biari_init_context (BiContextTypePtr ctx, int ini);
	void arienco_start_encoding(EncodingEnvironmentPtr eep, unsigned char *code_buffer, int *code_len);
#if TRACE_SIG_MAP
  static
#endif
	int  arienco_bits_written(EncodingEnvironmentPtr eep);
	void arienco_done_encoding(EncodingEnvironmentPtr eep);
	void rescale_cum_freq(BiContextTypePtr bi_ct);

	static void biari_encode_symbol(EncodingEnvironmentPtr eep, signed short symbol, BiContextTypePtr bi_ct );
	static void biari_encode_symbol_eq_prob(EncodingEnvironmentPtr eep, signed short symbol);
	void biari_encode_symbol_final(EncodingEnvironmentPtr eep, signed short symbol);
	
	void unary_bin_encode(EncodingEnvironmentPtr eep_dp, unsigned int symbol, BiContextTypePtr ctx, int ctx_offset);
	void unary_bin_max_encode(EncodingEnvironmentPtr eep_dp, unsigned int symbol, BiContextTypePtr ctx, int ctx_offset, unsigned int max_symbol);
	static void exp_golomb_encode_eq_prob( EncodingEnvironmentPtr eep_dp, unsigned int symbol, int k);
	static void unary_exp_golomb_encode( EncodingEnvironmentPtr eep_dp, unsigned int symbol, BiContextTypePtr ctx, unsigned int exp_start);

// decoding
	void arideco_start_decoding(DecodingEnvironmentPtr eep, unsigned char *code_buffer, int firstbyte, int *code_len);
	int  arideco_bits_read(DecodingEnvironmentPtr dep);
	void arideco_done_decoding(DecodingEnvironmentPtr dep);
	static	unsigned int biari_decode_symbol(DecodingEnvironmentPtr dep, BiContextTypePtr bi_ct );
	static	unsigned int biari_decode_symbol_eq_prob(DecodingEnvironmentPtr dep);
	unsigned int biari_decode_final(DecodingEnvironmentPtr dep);

	unsigned int unary_bin_decode(DecodingEnvironmentPtr dep_dp, BiContextTypePtr ctx, int ctx_offset);
	unsigned int unary_bin_max_decode(DecodingEnvironmentPtr dep_dp, BiContextTypePtr ctx, int ctx_offset, unsigned int max_symbol);
	static unsigned int exp_golomb_decode_eq_prob( DecodingEnvironmentPtr dep_dp, int k);
	static unsigned int unary_exp_golomb_decode( DecodingEnvironmentPtr dep_dp, BiContextTypePtr ctx, unsigned int exp_start);
};





template<class CodEnv>
class BinModelIf
{
public:
  BinModelIf() {}
  virtual ~BinModelIf() {}
  virtual void   init( CodEnv *pCodEnv, int iInit ) = 0;
  virtual int    processBin( int iBin )             = 0;
  virtual int    processBinEqProb( int iBin )       = 0;
  virtual double getBits()                          = 0;
};

template<class Environment>
class CabacBinModel
  : public BinModelIf<Environment>
{
public:
  CabacBinModel() : m_pEP( NULL ) {}

  void init( Environment *pCodEnv, int iInit )
  {
    m_pEP = pCodEnv;
    AOF( m_pEP );
    CABAC::biari_init_context( &m_cCtx, iInit );
  }

  int    processBin( int iBin )       { CAOT( true ); }
  int    processBinEqProb( int iBin ) { CAOT( true ); }
  double getBits()                    { assert( 0 ); return 0.0; }

private:
  BiContextType  m_cCtx;
  Environment   *m_pEP;
};

template<>
int CabacBinModel<EncodingEnvironment>::processBin( int iBin )
{
  CABAC::biari_encode_symbol( m_pEP, iBin, &m_cCtx );
  return 0;
}

template<>
int CabacBinModel<DecodingEnvironment>::processBin( int iBin )
{
  return CABAC::biari_decode_symbol( m_pEP, &m_cCtx );
}

template<>
int CabacBinModel<EncodingEnvironment>::processBinEqProb( int iBin )
{
  CABAC::biari_encode_symbol_eq_prob( m_pEP, iBin );
  return 0;
}

template<>
int CabacBinModel<DecodingEnvironment>::processBinEqProb( int iBin )
{
  return CABAC::biari_decode_symbol_eq_prob( m_pEP );
}



class CabacRateBinModel
  : public BinModelIf<void>
{
public:
  CabacRateBinModel() {}

  void init( void *pvDummy, int iInit )
  {
    m_iRange = HALF - 2;
    CABAC::biari_init_context( &m_cCtx, iInit );
    m_iEPSym = 0;
  }

  int processBin( int iBin )
  {
    unsigned int rLPS = rLPS_table_64x4[m_cCtx.state][(m_iRange>>6) & 3];
    if( iBin != m_cCtx.MPS ) 
    {
      m_iRange = rLPS;
      if (!m_cCtx.state)
      {
        m_cCtx.MPS = (unsigned char) (m_cCtx.MPS ^ 0x01);
      }
      m_cCtx.state = AC_next_state_LPS_64[m_cCtx.state];
    }
    else 
    {
      m_iRange -= rLPS;  
      m_cCtx.state = AC_next_state_MPS_64[m_cCtx.state];
    }

    while( m_iRange < QUARTER )
    {
      m_iEPSym++;
      m_iRange <<= 1;
    }
    return 0;
  }

  int processBinEqProb( int iBin ) { m_iEPSym++; return 0; }

  double getBits()
  {
    if( m_iRange == HALF-2 )
    {
      return (double)m_iEPSym;
    }
    // maybe the following should be replaced by a lookup-table
    static const double dFirst = ( log( (double)(HALF-2) ) / log( 2.0 ) );
    static const double dLog2  = log( 2.0 );
    double dSecond = ( log( (double)m_iRange ) / dLog2 );
    return (double)m_iEPSym + dFirst - dSecond;
  }

public:
  BiContextType  m_cCtx;
  int            m_iRange;
  int            m_iEPSym;
};

#endif
