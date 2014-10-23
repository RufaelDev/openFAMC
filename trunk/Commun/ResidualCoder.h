

#ifndef _RESIDUAL_CODER_INCLUDED__
#define _RESIDUAL_CODER_INCLUDED__

#include "FAMCCommon.h"

#include <stdio.h>
#include "ContextModels.h"
#include "CABAC.h"

class ResidualCoder
{
public:

  class Clustering
  {
	public:
    Clustering( int kk, int vv, int cc ) : k( kk ), v( vv ), c( cc ) {}

    int k; // frequency index
    int v; // vector index
    int c; // coordinate index
  };

  ResidualCoder( int iLen, int iNbr )
    : m_iLen      ( iLen                      )
    , m_iNbr      ( iNbr                      )
    , m_paiRes    ( new Vec3D<int>[iLen*iNbr] )
    , m_paiPred   ( new Vec3D<int>[iNbr]      )
    , m_paiPredDim( new Vec3D<int>[iNbr]      )
  {
    assert( m_paiRes );
    assert( m_paiPred );
    assert( m_paiPredDim );
    for( int c = 0; c < 3; c++ )
    {
      for( int v = 0; v < iNbr; v++ )
      {
        m_paiPred[v].data[c] = 0;
        m_paiPredDim[v].data[c] = 0;
        for( int k = 0; k < iLen; k++ )
        {
          m_paiRes[v*iLen+k].data[c] = 0;
        }
      }
    }
  }

  virtual ~ResidualCoder()
  {
   // delete[] m_paiRes;
   // delete[] m_paiPred;
   // delete[] m_paiPredDim;
  }

  int getStride() const { return m_iLen; }

  Vec3D<int>           *m_paiRes;
  Vec3D<int>           *m_paiPred;
  Vec3D<int>           *m_paiPredDim;
  Vec3D<float>          m_fMax;
  Vec3D<float>          m_fMin;
  unsigned char         m_ucNbits;

protected:
  int                   m_iLen;
  int                   m_iNbr;
public:
  void xTraceRes( char *pcFN )
  {
    FILE *pcFile = fopen( pcFN, "wb" );
    AOF( pcFile );
    for( int v = 0; v < m_iNbr; v++ )
    {
      for( int k = 0; k < m_iLen; k++ )
      {
        fprintf( pcFile, "{%4d,%4d,%4d}, ", m_paiRes[v*getStride()+k].data[0], m_paiRes[v*getStride()+k].data[1], m_paiRes[v*getStride()+k].data[2] );
      }
      fprintf( pcFile, "\n" );
    }
    fclose( pcFile );
  }

  void xTracePred( char *pcFN )
  {
    FILE *pcFile = fopen( pcFN, "wb" );
    AOF( pcFile );
    for( int v = 0; v < m_iNbr; v++ )
    {
      fprintf( pcFile, "Pred:{%5d,%5d,%5d} Dim:{%3d,%3d,%3d}\n", m_paiPred[v].data[0], m_paiPred[v].data[1], m_paiPred[v].data[2],
        m_paiPred[v].data[0] ? m_paiPredDim[v].data[0] : -1,
        m_paiPred[v].data[1] ? m_paiPredDim[v].data[1] : -1,
        m_paiPred[v].data[2] ? m_paiPredDim[v].data[2] : -1 );
    }
    fclose( pcFile );
  }

};


class ResidualEncoder
  : public ResidualCoder
{
public:
  ResidualEncoder( EncodingEnvironment *pEEP, int iLen, int iNbr )
    : ResidualCoder( iLen, iNbr )
    , m_pEEP( pEEP )
  {
  }

  void encode();

  inline void SetEncodingEnvironment( EncodingEnvironment *pEEP){m_pEEP = pEEP;}
  inline EncodingEnvironment * GetEncodingEnvironment( ){ return m_pEEP;}

private:
  EncodingEnvironment *m_pEEP;

  template<class ResLevEnc>
  __inline void xEncode( ResLevEnc &rcEnc, UEGPar cPar, Clustering cClustering )
  {
    rcEnc.setPar( cPar );
    int k_start = cClustering.k == -1 ? 0      : cClustering.k;
    int k_end   = cClustering.k == -1 ? m_iLen : cClustering.k + 1;
    int v_start = cClustering.v == -1 ? 0      : cClustering.v;
    int v_end   = cClustering.v == -1 ? m_iNbr : cClustering.v + 1;
    int c_start = cClustering.c == -1 ? 0      : cClustering.c;
    int c_end   = cClustering.c == -1 ? 3      : cClustering.c + 1;
    for( int v = v_start; v < v_end; v++ )
    {
      for( int c = c_start; c < c_end; c++ )
      {
        for( int k = k_start; k < k_end; k++ )
        {
          rcEnc.encode( m_paiRes[v * getStride() + k].data[c] );
        }
      }
    }
  }

  template<class ResLevEnc>
  __inline double xEstimatePar( ResLevEnc &rcEnc, UEGPar &rcPar, Clustering cClustering, UEGPar cFrom, UEGPar cTo )
  {
    double dMinRate = 1e30;
    UEGPar cPar;
    for( cPar.iGolPar = cFrom.iGolPar; cPar.iGolPar <= cTo.iGolPar; cPar.iGolPar++ )
    {
      for( cPar.iUnaryLength = cFrom.iUnaryLength; cPar.iUnaryLength <= cTo.iUnaryLength; cPar.iUnaryLength+=4 )
      {
        for( cPar.iCutOff = cFrom.iCutOff; cPar.iCutOff <= cTo.iCutOff; cPar.iCutOff++ )
        {
          if( cPar.iUnaryLength != 0 && cPar.iCutOff > cPar.iUnaryLength )
          {
            continue;
          }
          xEncode( rcEnc, cPar, cClustering );
          double dRate = rcEnc.getBits();
          if( dRate < dMinRate )
          {
            dMinRate   = dRate;
            cPar.dRate = dMinRate;
            rcPar      = cPar;
          }
        }
      }
    }
    return dMinRate;
  }

  template<class ResLevEnc>
  __inline double xEstimateAllPar( ResLevEnc &rcEnc, Clustering cClustering, std::vector<UEGPar> &racPar, UEGPar cFrom, UEGPar cTo )
  {
    double dBits = 0.0;
    int k_start = cClustering.k == -1 ? 0      : -1;
    int k_end   = cClustering.k == -1 ? m_iLen :  0;
    int v_start = cClustering.v == -1 ? 0      : -1;
    int v_end   = cClustering.v == -1 ? m_iNbr :  0;
    int c_start = cClustering.c == -1 ? 0      : -1;
    int c_end   = cClustering.c == -1 ? 3      :  0;
    for( int v = v_start; v < v_end; v++ )
    {
      for( int c = c_start; c < c_end; c++ )
      {
        for( int k = k_start; k < k_end; k++ )
        {
          Clustering cClus( k, v, c );
          UEGPar cPar;
          dBits += xEstimatePar( rcEnc, cPar, cClus, cFrom, cTo );
          racPar.push_back( cPar );
        }
      }
    }
    return dBits;
  }

  template<class BinModel>
  __inline void xEncodeAll( ResLevelEncoder<BinModel> &rcEnc, Clustering cClustering, std::vector<UEGPar> &racPar )
  {
    int idx = 0;
    double dBits = 0.0;
    int k_start = cClustering.k == -1 ? 0      : -1;
    int k_end   = cClustering.k == -1 ? m_iLen :  0;
    int v_start = cClustering.v == -1 ? 0      : -1;
    int v_end   = cClustering.v == -1 ? m_iNbr :  0;
    int c_start = cClustering.c == -1 ? 0      : -1;
    int c_end   = cClustering.c == -1 ? 3      :  0;
    for( int v = v_start; v < v_end; v++ )
    {
      for( int c = c_start; c < c_end; c++ )
      {
        for( int k = k_start; k < k_end; k++ )
        {
          Clustering cClus( k, v, c );
          xEncode( rcEnc, racPar.at( idx++ ), cClus );
        }
      }
    }
  }

  void xWriteAllPar( std::vector<UEGPar> &rcVec )
  {
    AOF( rcVec.size() == 1 );
//    CABAC::exp_golomb_encode_eq_prob( m_pEEP, (unsigned int)rcVec.size(), 2 );
    std::vector<UEGPar>::iterator cIter = rcVec.begin();
    for( ; cIter < rcVec.end(); cIter++ )
    {
//       printf( "ul:%2d co:%2d gp:%2d min-rate:%d\n", (*cIter).iUnaryLength, (*cIter).iCutOff, (*cIter).iGolPar, (int)floor((*cIter).dRate + 0.5) );
      xWritePar( *cIter );
    }
  }

  void xWritePar( UEGPar cPar )
  {
    int iPos;
    AOF( cPar.iUnaryLength == 16 );
//     for( iPos = 0; iPos < 8; iPos++ )
//     {
//       CABAC::biari_encode_symbol_eq_prob( m_pEEP, (cPar.iUnaryLength>>iPos)&1 );
//     }
    AOF( cPar.iGolPar > 0 && cPar.iGolPar < 9 );
    for( iPos = 0; iPos < 3; iPos++ )
    {
      CABAC::biari_encode_symbol_eq_prob( m_pEEP, ((cPar.iGolPar-1)>>iPos)&1 );
    }
    AOF( cPar.iCutOff == 1 || cPar.iCutOff == 2 );
    for( iPos = 0; iPos < 1; iPos++ )
    {
      CABAC::biari_encode_symbol_eq_prob( m_pEEP, ((cPar.iCutOff-1)>>iPos)&1 );
    }
  }


  void xEstimatePred( UEGPar cEstPar )
  {
	int v = 0;
	int c = 0;
	int d = 0;
    // compute prediction
    PredEncoder<CabacRateBinModel> cCurrPred( m_iLen ),    cBestPred( m_iLen ),    cStartPred( m_iLen );
    SigLastEncoder<CabacRateBinModel> cCurrSigLast( m_iLen ), cBestSigLast( m_iLen ), cStartSigLast( m_iLen );

    cCurrSigLast.init( NULL, 64 );
    cBestSigLast.init( NULL, 64 );
    cStartSigLast.init( NULL, 64 );
    ResLevelEncoder<CabacRateBinModel> cCurrLev, cBestLev, cStartLev;

    cCurrPred.init( NULL, 64 );
    cBestPred.init( NULL, 64 );
    cStartPred.init( NULL, 64 );

    cCurrLev.init( NULL, 64 );
    cBestLev.init( NULL, 64 );
    cStartLev.init( NULL, 64 );

    cCurrLev.setPar( cEstPar );
    cBestLev.setPar( cEstPar );
    cStartLev.setPar( cEstPar );

    int iSumSigns = 0;
    Vec3D<int> *paiVec = new Vec3D<int>[m_iLen];

    m_paiPred[0].data[0] = 0;
    m_paiPred[0].data[1] = 0;
    m_paiPred[0].data[2] = 0;
    for( v = 0; v < m_iNbr; v++ )
    {
      for( c = 0; c < 3; c++ )
      {
        if( v == 0 )
        {
          cBestSigLast.encodeOne( &m_paiRes[v*getStride()], c );
          int k;
          for( k = 0; k < m_iLen; k++ )
          {
            int iLev = m_paiRes[v*getStride()+k].data[c];
            if( iLev )
            {
              cBestLev.encode( iLev );
            }
            if( iLev )
            {
              iSumSigns++;
            }
          }
          continue;
        }
        cStartLev     = cBestLev;
        cStartPred    = cBestPred;
        cStartSigLast = cBestSigLast;
        double dStartBits = cStartPred.getBits() + cStartLev.getBits();
        dStartBits += cStartSigLast.getBits();
        double dBestBits = 1e30;
        int iBestSigns = 0;
        for( int p = 1; v-p >= 0 && p < 64; p++ )
        {
          for( d = 0; d < m_iLen + (1<<PRED_QUANT_BITS); d += (d ? (1<<PRED_QUANT_BITS) : 1) )
          {
            cCurrLev  = cStartLev;
            cCurrPred = cStartPred;
            cCurrPred.encode( d?p:0, d, m_paiPred[v-1].data[c], m_paiPredDim[v-1].data[c], c );

            int iSignBits = 0;
            for( int k = 0; k < m_iLen; k++ )
            {
              int iLev = m_paiRes[v*getStride()+k].data[c];
              if( k < d )
              {
                iLev -= m_paiRes[(v-p)*getStride()+k].data[c];
              }
              paiVec[k].data[c] = iLev;
              if( iLev )
              {
                cCurrLev.encode( iLev );
              }
              if( iLev )
              {
                iSignBits++;
              }
            }

            cCurrSigLast = cStartSigLast;
            cCurrSigLast.encodeOne( paiVec, c );

            double dBits = cCurrPred.getBits() + cCurrLev.getBits() - dStartBits;
            dBits += (double)iSignBits;
            dBits += cCurrSigLast.getBits();
            if( dBits < dBestBits )
            {
              dBestBits               = dBits;
              m_paiPred[v].data[c]    = d ? p : 0;
              m_paiPredDim[v].data[c] = d;
              cBestPred               = cCurrPred;
              cBestLev                = cCurrLev;
              iBestSigns              = iSignBits;
              cBestSigLast            = cCurrSigLast;
            }
          }
        }
        iSumSigns += iBestSigns;
      }
    }

    delete[] paiVec;

    printf( "\n---\n" );
    printf( "EstSigLastBits:%d\n", (int)floor(cBestSigLast.getBits()+0.5 ) );
    printf( "EstLevBits:%d\n", (int)floor(cBestLev.getBits()+0.5 ) );
    printf( "EstPredBits:%d\n", (int)floor(cBestPred.getBits()+0.5 ) );
    printf( "EstSignBits:%d\n", iSumSigns );
    printf( "---\n" );

    // apply new prediction
    for( v = m_iNbr - 1; v > 0; v-- )
    {
      for( c = 0; c < 3; c++ )
      {
        if( m_paiPred[v].data[c] )
        {
          for( d = 0; d < MIN_HHI( m_iLen, m_paiPredDim[v].data[c]); d++ )
          {
            m_paiRes[v * getStride() + d].data[c] -= m_paiRes[(v-m_paiPred[v].data[c]) * getStride() + d].data[c];
          }
        }
      }
    }
  }

};


class ResidualDecoder
  : public ResidualCoder
{
public:
  ResidualDecoder(DecodingEnvironment *pDEP, int iLen, int iNbr )
    : ResidualCoder( iLen, iNbr )
    , m_pDEP( pDEP )
  {
  }

  inline void SetDecodingEnvironment( DecodingEnvironment *pDEP){m_pDEP = pDEP;}
  inline DecodingEnvironment * GetDecodingEnvironment( ){ return m_pDEP;}


  void decode();

private:
  DecodingEnvironment *m_pDEP;

  void xReadAllPar( std::vector<UEGPar> &rcVec )
  {
//    int iSize = CABAC::exp_golomb_decode_eq_prob( m_pDEP, 2 );
    int iSize = 1;
    for( int i = 0; i < iSize; i++ )
    {
      UEGPar cPar;
      xReadPar( cPar );
      rcVec.push_back( cPar );
    }
  }

  void xReadPar( UEGPar &rcPar )
  {
    int iPos;
    rcPar.iUnaryLength = 0;
    rcPar.iCutOff      = 0;
    rcPar.iGolPar      = 0;
//     for( iPos = 0; iPos < 8; iPos++ )
//     {
//       if( CABAC::biari_decode_symbol_eq_prob( m_pDEP ) )
//       {
//         rcPar.iUnaryLength |= 1<<iPos;
//       }
//     }
    rcPar.iUnaryLength = 16;
    for( iPos = 0; iPos < 3; iPos++ )
    {
      if( CABAC::biari_decode_symbol_eq_prob( m_pDEP ) )
      {
        rcPar.iGolPar |= 1<<iPos;
      }
    }
    rcPar.iGolPar++;
    for( iPos = 0; iPos < 1; iPos++ )
    {
      if( CABAC::biari_decode_symbol_eq_prob( m_pDEP ) )
      {
        rcPar.iCutOff |= 1<<iPos;
      }
    }
    rcPar.iCutOff++;
  }

  template<class ResLevDec>
  __inline void xDecode( ResLevDec &rcDec, UEGPar cPar, Clustering cClustering )
  {
    rcDec.setPar( cPar );
    int k_start = cClustering.k == -1 ? 0      : cClustering.k;
    int k_end   = cClustering.k == -1 ? m_iLen : cClustering.k + 1;
    int v_start = cClustering.v == -1 ? 0      : cClustering.v;
    int v_end   = cClustering.v == -1 ? m_iNbr : cClustering.v + 1;
    int c_start = cClustering.c == -1 ? 0      : cClustering.c;
    int c_end   = cClustering.c == -1 ? 3      : cClustering.c + 1;
    for( int v = v_start; v < v_end; v++ )
    {
      for( int c = c_start; c < c_end; c++ )
      {
        for( int k = k_start; k < k_end; k++ )
        {
          m_paiRes[v * getStride() + k].data[c] = rcDec.decode( m_paiRes[v * getStride() + k].data[c] );
        }
      }
    }
  }


  template<class ResLevDec>
  __inline void xDecodeAll( ResLevDec &rcDec, Clustering cClustering, std::vector<UEGPar> &racPar )
  {
    int idx = 0;
    double dBits = 0.0;
    int k_start = cClustering.k == -1 ? 0      : -1;
    int k_end   = cClustering.k == -1 ? m_iLen :  0;
    int v_start = cClustering.v == -1 ? 0      : -1;
    int v_end   = cClustering.v == -1 ? m_iNbr :  0;
    int c_start = cClustering.c == -1 ? 0      : -1;
    int c_end   = cClustering.c == -1 ? 3      :  0;
    for( int v = v_start; v < v_end; v++ )
    {
      for( int c = c_start; c < c_end; c++ )
      {
        for( int k = k_start; k < k_end; k++ )
        {
          Clustering cClus( k, v, c );
          xDecode( rcDec, racPar.at( idx++ ), cClus );
        }
      }
    }
  }

};

#endif
