/*!
 ***************************************************************************
 * \file
 *    cabac.cpp
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
#include "CABAC.h"

CABAC::CABAC(void)
{
	_pic_bin_count = 0;

	// Allocates memory 
	_eep = new EncodingEnvironment;

	if ( _eep == NULL) {
		printf("arienco_create_encoding_environment: eep");
		exit(0);
	}

	_dep = new DecodingEnvironment;
	if ( _dep == NULL) {
		printf("arienco_create_decoding_environment: dep");
		exit(0);
	}

	_ctx = new BiContextType;
	if ( _ctx == NULL) {
		printf("arienco_create_context: ctx");
		exit(0);
	}
}

CABAC::~CABAC(void)
{
	// Frees memory
	if (_eep != NULL)    delete _eep ;
	if (_dep != NULL)    delete _dep ;
	if (_ctx != NULL)    delete _ctx ;
}

/*!
 ************************************************************************
 * \brief
 *    Initializes the EncodingEnvironment for the arithmetic coder
 ************************************************************************
 */
void CABAC::arienco_start_encoding(EncodingEnvironmentPtr eep,
                            unsigned char *code_buffer,
                            int *code_len )
{
  Elow = 0;
  Ebits_to_follow = 0;
  Ebuffer = 0;
  Ebits_to_go = 9; // to swallow first redundant bit

  Ecodestrm = code_buffer;
  Ecodestrm_len = code_len;

  Erange = HALF-2;

  eep->C = 0;
  eep->E = 0;

}

/*!
 ************************************************************************
 * \brief
 *    Returns the number of currently written bits
 ************************************************************************
 */
int CABAC::arienco_bits_written(EncodingEnvironmentPtr eep)
{
   return (8 * (*Ecodestrm_len) + Ebits_to_follow + 8  - Ebits_to_go);
}


/*!
 ************************************************************************
 * \brief
 *    Terminates the arithmetic codeword, writes stop bit and stuffing bytes (if any)
 ************************************************************************
 */
void CABAC::arienco_done_encoding(EncodingEnvironmentPtr eep)
{
  put_one_bit_plus_outstanding((unsigned char) ((Elow >> (B_BITS-1)) & 1));
  put_one_bit((unsigned char) (Elow >> (B_BITS-2))&1);
  put_one_bit((unsigned char) 1);

  while (Ebits_to_go != 8)
    put_one_bit(0);

  _pic_bin_count += eep->E*8 + eep->C; // no of processed bins
}

/*!
 ************************************************************************
 * \brief
 *    Actually arithmetic encoding of one binary symbol by using
 *    the probability estimate of its associated context model
 ************************************************************************
 */
void CABAC::biari_encode_symbol(EncodingEnvironmentPtr eep, signed short symbol, BiContextTypePtr bi_ct )
{
  register unsigned int range = Erange;
  register unsigned int low = Elow;
  unsigned int rLPS = rLPS_table_64x4[bi_ct->state][(range>>6) & 3];
   
  range -= rLPS;  
//  bi_ct->count += _cabac_encoding;

  /* covers all cases where code does not bother to shift down symbol to be 
   * either 0 or 1, e.g. in some cases for cbp, mb_Type etc the code simply 
   * masks off the bit position and passes in the resulting value */
  symbol = (short) (symbol != 0);

  if (symbol != bi_ct->MPS) 
  {
    low += range;
    range = rLPS;
    
    if (!bi_ct->state)
      bi_ct->MPS = (unsigned char) (bi_ct->MPS ^ 0x01);               // switch LPS if necessary
    bi_ct->state = AC_next_state_LPS_64[bi_ct->state]; // next state
  } 
  else 
    bi_ct->state = AC_next_state_MPS_64[bi_ct->state]; // next state
 
  /* renormalisation */    
  while (range < QUARTER)
  {
    if (low >= HALF)
    {
      put_one_bit_plus_outstanding(1);
      low -= HALF;
    }
    else if (low < QUARTER)
    {
      put_one_bit_plus_outstanding(0);
    }
    else
    {
      Ebits_to_follow++;
      low -= QUARTER;
    }
    low <<= 1;
    range <<= 1;
  }
  Erange = range;
  Elow = low;
  eep->C++;
}

/*!
 ************************************************************************
 * \brief
 *    Arithmetic encoding of one binary symbol assuming 
 *    a fixed prob. distribution with p(symbol) = 0.5
 ************************************************************************
 */
void CABAC::biari_encode_symbol_eq_prob(EncodingEnvironmentPtr eep, signed short symbol)
{
  register unsigned int low = (Elow<<1);
   
  if (symbol != 0)
    low += Erange;

  /* renormalisation as for biari_encode_symbol; 
     note that low has already been doubled */ 
  if (low >= ONE)
  {
    put_one_bit_plus_outstanding(1);
    low -= ONE;
  }
  else 
    if (low < HALF)
    {
      put_one_bit_plus_outstanding(0);
    }
    else
    {
      Ebits_to_follow++;
      low -= HALF;
    }
    Elow = low;
    eep->C++;    
}

/*!
 ************************************************************************
 * \brief
 *    Arithmetic encoding for last symbol before termination
 ************************************************************************
 */
void CABAC::biari_encode_symbol_final(EncodingEnvironmentPtr eep, signed short symbol)
{
  register unsigned int range = Erange-2;
  register unsigned int low = Elow;
   
  if (symbol) {
    low += range;
    range = 2;
  }
  
  while (range < QUARTER)
  {
    if (low >= HALF)
    {
      put_one_bit_plus_outstanding(1);
      low -= HALF;
    }
    else 
      if (low < QUARTER)
      {
        put_one_bit_plus_outstanding(0);
      }
      else
      {
        Ebits_to_follow++;
        low -= QUARTER;
      }
      low <<= 1;
      range <<= 1;
  }
  Erange = range;
  Elow = low;
  eep->C++;
}


/*!
 ************************************************************************
 * \brief
 *    Initializes a given context with some pre-defined probability state
 ************************************************************************
 */
void CABAC::biari_init_context (BiContextTypePtr ctx, int ini)
{
  int pstate = ini; //iClip3 ( 1, 126, ((ini[0]* imax(0, img->currentSlice->qp)) >> 4) + ini[1]);

  if ( pstate >= 64 )
  {
    ctx->state  = (unsigned short) (pstate - 64);
    ctx->MPS    = 1;
  }
  else
  {
    ctx->state  = (unsigned short) (63 - pstate);
    ctx->MPS    = 0;
  }
}
/*!
 ************************************************************************
 * \brief
 *    Unary binarization and encoding of a symbol by using
 *    one or two distinct models for the first two and all
 *    remaining bins
*
************************************************************************/
void CABAC::unary_bin_encode(EncodingEnvironmentPtr eep_dp,
                      unsigned int symbol,
                      BiContextTypePtr ctx,
                      int ctx_offset)
{
  unsigned int l;
  BiContextTypePtr ictx;

  if (symbol==0)
  {
    biari_encode_symbol(eep_dp, 0, ctx );
    return;
  }
  else
  {
    biari_encode_symbol(eep_dp, 1, ctx );
    l = symbol;
    ictx = ctx+ctx_offset;
    while ((--l)>0)
      biari_encode_symbol(eep_dp, 1, ictx);
    biari_encode_symbol(eep_dp, 0, ictx);
  }
  return;
}

/*!
 ************************************************************************
 * \brief
 *    Unary binarization and encoding of a symbol by using
 *    one or two distinct models for the first two and all
 *    remaining bins; no terminating "0" for max_symbol
 *    (finite symbol alphabet)
 ************************************************************************
 */
void CABAC::unary_bin_max_encode(EncodingEnvironmentPtr eep_dp,
                          unsigned int symbol,
                          BiContextTypePtr ctx,
                          int ctx_offset,
                          unsigned int max_symbol)
{
  unsigned int l;
  BiContextTypePtr ictx;

  if (symbol==0)
  {
    biari_encode_symbol(eep_dp, 0, ctx );
    return;
  }
  else
  {
    biari_encode_symbol(eep_dp, 1, ctx );
    l=symbol;
    ictx=ctx+ctx_offset;
    while ((--l)>0)
      biari_encode_symbol(eep_dp, 1, ictx);
    if (symbol<max_symbol)
      biari_encode_symbol(eep_dp, 0, ictx);
  }
  return;
}

/*!
 ************************************************************************
 * \brief
 *    Exp Golomb binarization and encoding
 ************************************************************************
 */
void CABAC::exp_golomb_encode_eq_prob( EncodingEnvironmentPtr eep_dp,
                                unsigned int symbol,
                                int k) 
{
  while(1)
  {
    if (symbol >= (unsigned int)(1<<k))   
    {
      biari_encode_symbol_eq_prob(eep_dp, 1);   //first unary part
      symbol = symbol - (1<<k);
      k++;
    }
    else                  
    {
      biari_encode_symbol_eq_prob(eep_dp, 0);   //now terminated zero of unary part
      while (k--)                               //next binary part
        biari_encode_symbol_eq_prob(eep_dp, (signed short)((symbol>>k)&1)); 
      break;
    }
  }

  return;
}

/*!
 ************************************************************************
 * \brief
 *    Exp-Golomb Encoding
*
************************************************************************/
void CABAC::unary_exp_golomb_encode( EncodingEnvironmentPtr eep_dp,
                                    unsigned int symbol,
                                    BiContextTypePtr ctx, unsigned int exp_start)
{
  unsigned int l,k;
  
  if (symbol==0)
  {
    biari_encode_symbol(eep_dp, 0, ctx );
    return;
  }
  else
  {
    biari_encode_symbol(eep_dp, 1, ctx );
    l=symbol;
    k=1;
    while (((--l)>0) && (++k <= exp_start))
      biari_encode_symbol(eep_dp, 1, ctx);
    if (symbol < exp_start) biari_encode_symbol(eep_dp, 0, ctx);
    else exp_golomb_encode_eq_prob(eep_dp,symbol-exp_start,0);
  }
  return;
}


//----------------------------------------------------------decoding-------------------------------------------//

/*!
 ************************************************************************
 * \brief
 *    Initializes the DecodingEnvironment for the arithmetic coder
 ************************************************************************
 */
void CABAC::arideco_start_decoding(DecodingEnvironmentPtr dep, unsigned char *cpixcode, int firstbyte, int *cpixcode_len)
{
 
  int value = 0;

  Dcodestrm = cpixcode;
  Dcodestrm_len = cpixcode_len;
  *Dcodestrm_len = firstbyte;
 
  {
    int i;
    Dbits_to_go = 0;
    for (i = 0; i < B_BITS -1 ; i++) // insertion of redundant bit
    {
      if (--Dbits_to_go < 0)
        get_byte();
      value = (value<<1)  | ((Dbuffer >> Dbits_to_go) & 0x01);
    }
  }
  dep->Drange = HALF-2;
  dep->Dvalue = value;
}


/*!
 ************************************************************************
 * \brief
 *    arideco_bits_read
 ************************************************************************
 */
int CABAC::arideco_bits_read(DecodingEnvironmentPtr dep)
{
  return 8 * ((*Dcodestrm_len)-1) + (8 - Dbits_to_go) - 16;
}


/*!
 ************************************************************************
 * \brief
 *    arideco_done_decoding():
 ************************************************************************
 */
void CABAC::arideco_done_decoding(DecodingEnvironmentPtr dep)
{
  (*Dcodestrm_len)++;
}


/*!
 ************************************************************************
 * \brief
 *    biari_decode_symbol():
 * \return
 *    the decoded symbol
 ************************************************************************
 */
unsigned int CABAC::biari_decode_symbol(DecodingEnvironmentPtr dep, BiContextTypePtr bi_ct )
{
  register unsigned int bit = bi_ct->MPS;
  register unsigned int value = dep->Dvalue;
  register unsigned int range = dep->Drange;
  register unsigned int rLPS = (unsigned int) rLPS_table_64x4[bi_ct->state][(range>>6) & 0x03];

  range -= rLPS;

  if (value < range) /* MPS */ 
    bi_ct->state = AC_next_state_MPS_64[bi_ct->state]; // next state
  else              /* LPS */
  {
    value -= range;
    range = rLPS;
    bit = !bit;
    if (!bi_ct->state)       // switch meaning of MPS if necessary  
      bi_ct->MPS ^= 0x01;              
    bi_ct->state = AC_next_state_LPS_64[bi_ct->state]; // next state 
  }
  
  while (range < QUARTER)
  {
    /* Double range */
    range <<= 1;
    if (--Dbits_to_go < 0) 
      get_byte();   
    /* Shift in next bit and add to value */
    value = (value << 1) | ((Dbuffer >> Dbits_to_go) & 0x01);

  }
  
  dep->Drange = range;
  dep->Dvalue = value;

  return(bit);
}


/*!
 ************************************************************************
 * \brief
 *    biari_decode_symbol_eq_prob():
 * \return
 *    the decoded symbol
 ************************************************************************
 */
unsigned int CABAC::biari_decode_symbol_eq_prob(DecodingEnvironmentPtr dep)
{
  register unsigned int bit = 0;
  register unsigned int value  = (dep->Dvalue<<1);
  if (--Dbits_to_go < 0) 
    get_byte(); 
  /* Shift in next bit and add to value */  
  value |= (Dbuffer >> Dbits_to_go) &  0x01;  
  if (value >= dep->Drange) 
  {
    bit = 1;
    value -= dep->Drange;
  }

  dep->Dvalue = value;

  return(bit);
}

/*!
 ************************************************************************
 * \brief
 *    biari_decode_symbol_final():
 * \return
 *    the decoded symbol
 ************************************************************************
 */
unsigned int CABAC::biari_decode_final(DecodingEnvironmentPtr dep)
{
  register unsigned int value  = dep->Dvalue;
  register unsigned int range  = dep->Drange - 2;
   
  if (value >= range) 
  {
    return 1;
  }
  else
  {
    while (range < QUARTER)
    {
    /* Double range */
      range <<= 1;
      if (--Dbits_to_go < 0) 
        get_byte();   
      /* Shift in next bit and add to value */
      value = (value << 1) | ((Dbuffer >> Dbits_to_go) & 0x01);
    } 
    dep->Dvalue = value;
    dep->Drange = range;
    return 0;
  }
}

/*!
 ************************************************************************
 * \brief
 *    decoding of unary binarization using one or 2 distinct
 *    models for the first and all remaining bins; no terminating
 *    "0" for max_symbol
 ***********************************************************************
 */
unsigned int CABAC::unary_bin_max_decode(DecodingEnvironmentPtr dep_dp,
                                  BiContextTypePtr ctx,
                                  int ctx_offset,
                                  unsigned int max_symbol)
{
  unsigned int l;
  unsigned int symbol;
  BiContextTypePtr ictx;

  symbol =  biari_decode_symbol(dep_dp, ctx );

  if (symbol==0)
    return 0;
  else
  {
    if (max_symbol == 1)
    return symbol;
    symbol=0;
    ictx=ctx+ctx_offset;
    do
    {
      l=biari_decode_symbol(dep_dp, ictx);
      symbol++;
    }
    while( (l!=0) && (symbol<max_symbol-1) );
    if ((l!=0) && (symbol==max_symbol-1))
      symbol++;
    return symbol;
  }
}


/*!
 ************************************************************************
 * \brief
 *    decoding of unary binarization using one or 2 distinct
 *    models for the first and all remaining bins
 ***********************************************************************
 */
unsigned int CABAC::unary_bin_decode(DecodingEnvironmentPtr dep_dp,
                              BiContextTypePtr ctx,
                              int ctx_offset)
{
  unsigned int l;
  unsigned int symbol;
  BiContextTypePtr ictx;

  symbol = biari_decode_symbol(dep_dp, ctx );

  if (symbol==0)
    return 0;
  else
  {
    symbol=0;
    ictx=ctx+ctx_offset;
    do
    {
      l=biari_decode_symbol(dep_dp, ictx);
      symbol++;
    }
    while( l!=0 );
    return symbol;
  }
}

/*!
 ************************************************************************
 * \brief
 *    Exp Golomb binarization and decoding of a symbol
 *    with prob. of 0.5
 ************************************************************************
 */
unsigned int CABAC::exp_golomb_decode_eq_prob( DecodingEnvironmentPtr dep_dp,
                                        int k)
{
  unsigned int l;
  int symbol = 0;
  int binary_symbol = 0;

  do
  {
    l=biari_decode_symbol_eq_prob(dep_dp);
    if (l==1) 
    {
      symbol += (1<<k); 
      k++;
    }
  }
  while (l!=0);

  while (k--)                             //next binary part
    if (biari_decode_symbol_eq_prob(dep_dp)==1) 
      binary_symbol |= (1<<k);

  return (unsigned int) (symbol+binary_symbol);
}


/*!
 ************************************************************************
 * \brief
 *    Unary Exp-Golomb decoding
 ***********************************************************************
 */
unsigned int CABAC::unary_exp_golomb_decode( DecodingEnvironmentPtr dep_dp,
                                            BiContextTypePtr ctx, unsigned int exp_start)
{
  unsigned int l,k;
  unsigned int symbol;

  symbol = biari_decode_symbol(dep_dp, ctx );

  if (symbol==0)
    return 0;
  else
  {
    symbol=0;
    k=1;
    do
    {
      l=biari_decode_symbol(dep_dp, ctx);
      symbol++;
      k++;
    }
    while((l!=0) && (k!=exp_start));
    if (l!=0)
      symbol += exp_golomb_decode_eq_prob(dep_dp,0)+1;
    return symbol;
  }
}
