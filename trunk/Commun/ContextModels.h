


#ifndef _CONTEXT_MODELS_INCLUDED__
#define _CONTEXT_MODELS_INCLUDED__

#include "FAMCCommon.h"
#include "CABAC.h"


template<class BinModel>
class EncBinarizer
{
public:
  static void encodeExpGolombEqProb( BinModel *pcBinModel, unsigned int uiLevel, int iExpGolombPar )
  {
    while(1)
    {
      if( uiLevel >= (unsigned int)(1<<iExpGolombPar) )   
      {
        pcBinModel->processBinEqProb( 1 );
        uiLevel = uiLevel - (1<<iExpGolombPar);
        iExpGolombPar++;
      }
      else                  
      {
        pcBinModel->processBinEqProb( 0 );
        while( iExpGolombPar-- )
        {
          pcBinModel->processBinEqProb( ( uiLevel >> iExpGolombPar ) & 1 );
        }
        break;
      }
    }
  }

  static void unaryExpGolombEncode( unsigned int symbol, BinModel &rcModel, unsigned int exp_start )
  {
    unsigned int l,k;

    if (symbol==0)
    {
      rcModel.processBin( 0 );
      return;
    }
    else
    {
      rcModel.processBin( 1 );
      l=symbol;
      k=1;
      while (((--l)>0) && (++k <= exp_start))
      {
        rcModel.processBin( 1 );
      }
      if (symbol < exp_start)
      {
        rcModel.processBin( 0 );
      }
      else
      {
        encodeExpGolombEqProb( &rcModel, symbol-exp_start, 0 );
      }
    }
    return;
  }

};



template<class BinModel>
class DecBinarizer
{
public:
  static unsigned int decodeExpGolombEqProb( CabacBinModel<DecodingEnvironment> *pcBinModel, int iExpGolombPar )
  {
    unsigned int l;
    int symbol = 0;
    int binary_symbol = 0;

    do
    {
      l = pcBinModel->processBinEqProb(0);
      if (l==1) 
      {
        symbol += (1<<iExpGolombPar); 
        iExpGolombPar++;
      }
    }
    while (l!=0);

    while (iExpGolombPar--)
      if( pcBinModel->processBinEqProb(0) ) 
        binary_symbol |= (1<<iExpGolombPar);

    return (unsigned int) (symbol+binary_symbol);
  }

  static unsigned int unaryExpGolombDecode( BinModel &rcModel, unsigned int exp_start )
  {
    unsigned int l,k;
    unsigned int symbol;

    symbol = rcModel.processBin(0);

    if (symbol==0)
    {
      return 0;
    }
    else
    {
      symbol=0;
      k=1;
      do
      {
        l=rcModel.processBin(0);
        symbol++;
        k++;
      }
      while((l!=0) && (k!=exp_start));
      if (l!=0)
        symbol += decodeExpGolombEqProb( &rcModel, 0 ) + 1;
      return symbol;
    }
  }

};


template<class BinModel, class CodEnv, int t_iNumCells = 64>
class SigLastCtx
{
public:
  SigLastCtx( int iLen )
    : m_iLen     ( iLen )
    , m_iCellSize( iLen/t_iNumCells + ( iLen % t_iNumCells ? 1 : 0 ) )
  {
  }

  virtual ~SigLastCtx() {}

  double getBits()
  {
    double dBits = m_cCbp.getBits();
    for( int i = 0; i < t_iNumCells; i++ )
    {
      dBits += m_acSig[i].getBits();
      dBits += m_acLast[i].getBits();
    }
    return dBits;
  }

  void init( CodEnv *pcEnv, int iInit )
  {
    for( int i = 0; i < t_iNumCells; i++ )
    {
      m_acSig[i] .init( pcEnv, iInit );
      m_acLast[i].init( pcEnv, iInit );
    }
    m_cCbp.init( pcEnv, iInit );
  }

protected:
  BinModel* getSigCtx ( int iIdx ) { return &m_acSig [iIdx/m_iCellSize]; }
  BinModel* getLastCtx( int iIdx ) { return &m_acLast[iIdx/m_iCellSize]; }
  BinModel* getCbpCtx ()           { return &m_cCbp; }

  int       m_iLen;

private:
  BinModel  m_acSig[t_iNumCells];
  BinModel  m_acLast[t_iNumCells];
  BinModel  m_cCbp;
  int       m_iCellSize;
};



template<class BinModel>
class SigLastEncoder
  : public SigLastCtx<BinModel, EncodingEnvironment>
{
public:
  SigLastEncoder( int iLen ) : SigLastCtx( iLen ) {}

  __inline void encodeOne( Vec3D<int> *pacRes, int c )
  {
    int k, iSig = 0;
    for( k = 0; k < m_iLen; k++ )
    {
      if( pacRes[k].data[c] )
      {
        iSig = 1;
        break;
      }
    }
    if( iSig == 0 )
    {
      getCbpCtx()->processBin( 0 );
    }
    else
    {
      getCbpCtx()->processBin( 1 );
      for( k = 0; k < m_iLen; k++ )
      {
        int sym = pacRes[k].data[c] != 0 ? 1 : 0;
        getSigCtx( k)->processBin( sym );
        if( sym && k+1 < m_iLen )
        {
          sym = 1;
          // coding of last-flag
          for( int ll = k+1; ll < m_iLen; ll++ )
          {
            if( pacRes[ll].data[c] != 0 )
            {
              sym = 0;
              break;
            }
          }
          getLastCtx( k )->processBin( sym );

          if( k+2 == m_iLen )
          {
            break;
          }

          if( sym )
          {
            break;
          }
        }
      }
    }
  }

  void encode( Vec3D<int> *pacRes, int iStride, int iNbr )
  {
    Vec3D<int> *pacCurr = pacRes;
    for (int v = 0; v < iNbr; v++, pacCurr += iStride )
    {
      for( int c = 0; c < 3; c++ )
      {
        encodeOne( pacCurr, c );
      }
    }
  }

};


class SigLastDecoder
  : public SigLastCtx< CabacBinModel<DecodingEnvironment>, DecodingEnvironment>
{
public:
  SigLastDecoder( int iLen ) : SigLastCtx( iLen ) {}

  void decode( Vec3D<int> *pacRes, int iStride, int iNbr )
  {
    Vec3D<int> *pacCurr = pacRes;
    for (int v = 0; v < iNbr; v++, pacCurr += iStride )
    {
      for( int c = 0; c < 3; c++ )
      {
        if( getCbpCtx()->processBin( 0 ) )
        {
          for( int k = 0; k < m_iLen; k++ )
          {
            pacCurr[k].data[c] = getSigCtx( k )->processBin( 0 );
            if( pacCurr[k].data[c] && k+1 < m_iLen )
            {
              int iLast = getLastCtx( k )->processBin( 0 );
              if( iLast )
              {
                for( int ll = k+1; ll < m_iLen; ll++ )
                {
                  pacCurr[ll].data[c] = 0;
                }
                break;
              }
              else if( k+2 == m_iLen )
              {
                pacCurr[k+1].data[c] = iLast != 0 ? 0 : 1;
                break;
              }
            }
          }
        }
        else
        {
          for (int k = 0; k < m_iLen; k++)
          {
            pacCurr[k].data[c] = 0;
          }
        }
      }
    }
  }

};


class UEGPar
{
public:
  UEGPar() {}
  UEGPar( int iULen, int iUCutOff, int iGolombParameter ) : iUnaryLength( iULen ), iCutOff( iUCutOff ), iGolPar( iGolombParameter ) {}

  int iUnaryLength;
  int iCutOff;
  int iGolPar;

  double dRate;
};


template<class BinModel, class CodEnv, int t_iMaxUnaryCutOff = 2>
class ResLevelCtx
{
public:
  ResLevelCtx() {}
  virtual ~ResLevelCtx() {} 

  void init( CodEnv *pEP, int iInit )
  {
    m_pEP  = pEP;
    m_iInit = iInit;
  }

  void setPar( UEGPar par )
  {
    AOF( par.iGolPar >= 0 );
    AOF( par.iCutOff <= t_iMaxUnaryCutOff );
    m_cPar = par;
    for( int i = 0; i < par.iCutOff; i++ )
    {
      m_acUnaryPart[i].init( m_pEP, m_iInit );
    }
    m_cEPModel.init( m_pEP, m_iInit );
  }

protected:
  BinModel  m_acUnaryPart[t_iMaxUnaryCutOff];
  BinModel  m_cEPModel;
  UEGPar    m_cPar;
  CodEnv   *m_pEP;
  int       m_iInit;

};



template<class BinModel>
class ResLevelEncoder
  : public ResLevelCtx< BinModel, EncodingEnvironment >
  , private EncBinarizer<BinModel>
{
public:
  ResLevelEncoder() : ResLevelCtx() {}

  __inline void encode( int iLevel )
  {
    if( iLevel )
    {
      iLevel = abs( iLevel ) - 1;
      for( int i = 0; i < m_cPar.iUnaryLength; i++ )
      {
        int iSym = iLevel > i ? 1 : 0;
        m_acUnaryPart[ MIN_HHI( i, m_cPar.iCutOff - 1 ) ].processBin( iSym );
        if( iSym == 0 )
          return;
      }
      encodeExpGolombEqProb( &m_cEPModel, iLevel - m_cPar.iUnaryLength, m_cPar.iGolPar );
    }
  }

  double getBits()
  {
    double dBits = m_cEPModel.getBits();
    for( int i = 0; i < m_cPar.iCutOff; i++ )
    {
      dBits += m_acUnaryPart[i].getBits();
    }
    return dBits;
  }
};



class ResLevelDecoder
  : public ResLevelCtx< CabacBinModel<DecodingEnvironment>, DecodingEnvironment >
  , private DecBinarizer<CabacBinModel<DecodingEnvironment> >
{
public:
  ResLevelDecoder() : ResLevelCtx() {}

  int decode( int iSig )
  {
    if( iSig )
    {
      for( int i = 0; i < m_cPar.iUnaryLength; i++ )
      {
        int iSym = m_acUnaryPart[MIN_HHI(i, m_cPar.iCutOff - 1)].processBin(0);
        if( iSym == 0 )
          return 1 + i;
      }
      return 1 + m_cPar.iUnaryLength + decodeExpGolombEqProb( &m_cEPModel, m_cPar.iGolPar );
    }
    return 0;
  }
};



template<class BinModel, class CodEnv>
class SignCtx
{
public:
  SignCtx( int iLen ) : m_iLen ( iLen ) {}
  virtual ~SignCtx() {}

  void init( CodEnv *pEP, int iInit )
  {
    m_cCtx.init( pEP, iInit );
  }

  BinModel* getCtx() { return &m_cCtx; }

protected:
  int       m_iLen;
private:
  BinModel m_cCtx;
};



template<class BinModel>
class SignEncoder
  : public SignCtx< BinModel, EncodingEnvironment >
{
public:
  SignEncoder( int iLen ) : SignCtx( iLen ) {}

  void encode( Vec3D<int> *pacRes, int iStride, int iNbr )
  {
    Vec3D<int> *pacCurr = pacRes;
    for (int v = 0; v < iNbr; v++, pacCurr += iStride )
    {
      for( int c = 0; c < 3; c++ )
      {
        for( int k = 0; k < m_iLen; k++ )
        {
          int iLevel = pacCurr[k].data[c];
          if( iLevel )
          {
            getCtx()->processBinEqProb( iLevel < 0 ? 1 : 0 );
          }
        }
      }
    }
  }
};



class SignDecoder
  : public SignCtx< CabacBinModel<DecodingEnvironment>, DecodingEnvironment >
{
public:
  SignDecoder( int iLen ) : SignCtx( iLen ) {}

  void decode( Vec3D<int> *pacRes, int iStride, int iNbr )
  {
    Vec3D<int> *pacCurr = pacRes;
    for (int v = 0; v < iNbr; v++, pacCurr += iStride )
    {
      for( int c = 0; c < 3; c++ )
      {
        for( int k = 0; k < m_iLen; k++ )
        {
          if( pacCurr[k].data[c] )
          {
            if( getCtx()->processBinEqProb(0) )
            {
              pacCurr[k].data[c] = -pacCurr[k].data[c];
            }
          }
        }
      }
    }
  }
};



template<class BinModel, class CodEnv>
class PredCtx
{
public:

  PredCtx( int iLen ) : m_rcEqProb( m_cSkip ), m_iLen( iLen )
  {
    m_apiPrevDim[0] = &m_aiPreviousDim[0];
    m_apiPrevDim[1] = &m_aiPreviousDim[1];
    m_apiPrevDim[2] = &m_aiPreviousDim[2];
  }

  virtual ~PredCtx() {} 

  void init( CodEnv *pEP, int iInit )
  {
    m_cSkip.init( pEP, iInit );
    m_cPred.init( pEP, iInit );
    m_cPredDim.init( pEP, iInit );
    *m_apiPrevDim[0] = 1;
    *m_apiPrevDim[1] = 1;
    *m_apiPrevDim[2] = 1;
  }

protected:
  BinModel  m_cSkip;
  BinModel  m_cPred;
  BinModel  m_cPredDim;
  BinModel &m_rcEqProb;
  int      *m_apiPrevDim[3];
  int       m_iLen;

  void xCopyFrom( const PredCtx &rcSrc )
  {
    m_cSkip          = rcSrc.m_cSkip;
    m_cPred          = rcSrc.m_cPred;
    m_cPredDim       = rcSrc.m_cPredDim;
    *m_apiPrevDim[0] = *rcSrc.m_apiPrevDim[0];
    *m_apiPrevDim[1] = *rcSrc.m_apiPrevDim[1];
    *m_apiPrevDim[2] = *rcSrc.m_apiPrevDim[2];
    m_iLen           = rcSrc.m_iLen;
  }
private:
  int m_aiPreviousDim[3];
};


template<class BinModel>
class PredEncoder
  : public PredCtx< BinModel, EncodingEnvironment >
  , private EncBinarizer<BinModel>
{
public:
  PredEncoder( int iLen ) : PredCtx( iLen ) {}
  
  PredEncoder& operator=( const PredEncoder &rcSrc ) { xCopyFrom( rcSrc ); return *this; }

  __inline void encode( int iPred, int iDim, int iPrevPred, int iPrevDim, int c )
  {
    bool bSkipFlag = iPred == iPrevPred && ( iPred == 0 || iDim == iPrevDim );
    m_cSkip.processBin( bSkipFlag ? 1 : 0 );
    if( !bSkipFlag )
    {
      unaryExpGolombEncode( iPred, m_cPred, 2 );
      if( iPred )
      {
        int iDimRes = ((iDim-1)>>PRED_QUANT_BITS) - ((*m_apiPrevDim[c]-1)>>PRED_QUANT_BITS);
        unaryExpGolombEncode( abs( iDimRes ), m_cPredDim, 2 );
        if( iDimRes )
        {
          int iRec = abs( iDimRes ) << PRED_QUANT_BITS;
          if( *m_apiPrevDim[c] + iRec < m_iLen + (1<<PRED_QUANT_BITS) && *m_apiPrevDim[c] - iRec >= 0 )
          {
            m_rcEqProb.processBinEqProb( iDimRes < 0 ? 1 : 0 );
          }
        }
        *m_apiPrevDim[c] = iDim;
      }
    }
  }

  double getBits()
  {
    double dBits = m_cSkip.getBits() + m_cPred.getBits() + m_cPredDim.getBits();
    return dBits;
  }
};



class PredDecoder
  : public PredCtx< CabacBinModel<DecodingEnvironment>, DecodingEnvironment >
  , public DecBinarizer< CabacBinModel<DecodingEnvironment> >
{
public:
  PredDecoder( int iLen ) : PredCtx( iLen ) {}

  __inline void decode( int &riPred, int &riDim, int iPrevPred, int iPrevDim, int c )
  {
    if( m_cSkip.processBin(0) )
    {
      riPred = iPrevPred;
      if( riPred )
      {
        riDim = iPrevDim;
      }
      else
      {
        riDim = 0;
      }
    }
    else
    {
      riPred = unaryExpGolombDecode( m_cPred, 2 );
      if( riPred )
      {
        int iDimRes = unaryExpGolombDecode( m_cPredDim, 2 ) << PRED_QUANT_BITS;
        if( iDimRes )
        {
          if( *m_apiPrevDim[c] + iDimRes >= m_iLen + (1<<PRED_QUANT_BITS) )
          {
            iDimRes = -iDimRes;
          }
          else if( *m_apiPrevDim[c] - iDimRes >= 0 )
          {
            if( m_rcEqProb.processBinEqProb(0) )
            {
              iDimRes = -iDimRes;
            }
          }
        }
        riDim = iDimRes + *m_apiPrevDim[c];
        *m_apiPrevDim[c] = riDim;
      }
      else
      {
        riDim = 0;
      }
    }
  }
};

#endif
