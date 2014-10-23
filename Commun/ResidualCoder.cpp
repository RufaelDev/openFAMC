


#include "ResidualCoder.h"

#define INIT_FREQ  0
#define INIT_VEC   0
#define INIT_COMP  0


void ResidualEncoder::encode()
{
//  FILE *pcStatFile = fopen( "BitStatNewNP.txt", "ab" );
//  AOF( pcStatFile );
//  fprintf( pcStatFile, "VecLen:\t%d\tNumVec:\t%d\t", m_iLen, m_iNbr );
//  printf("\n");
  int iBefore = CABAC::arienco_bits_written( m_pEEP );


  // compute best predictors
  int iEstGol = (m_ucNbits - 2) / 2;
  iEstGol = MIN_HHI( 1, iEstGol );
  printf("Calculating new prediction...");
  UEGPar cEstPar( 16, 2, iEstGol );
  xEstimatePred( cEstPar );
  printf("Done!\n");

  // significance map
  SigLastEncoder< CabacBinModel<EncodingEnvironment> > cSigLast( m_iLen );
  cSigLast.init( m_pEEP, 64 );
  cSigLast.encode( m_paiRes, getStride(), m_iNbr );
//  fprintf( pcStatFile, "SigLast(64cells):\t%10d\t", CABAC::arienco_bits_written( m_pEEP ) - iBefore );
  printf( "SigMap: %d\n", CABAC::arienco_bits_written( m_pEEP ) - iBefore );
  iBefore = CABAC::arienco_bits_written( m_pEEP );
  
  // absolute values
  ResLevelEncoder<CabacRateBinModel> cResLevelRate;
  cResLevelRate.init( NULL, 64 );
  std::vector<UEGPar> acPar;
  Clustering cClus( INIT_FREQ, INIT_VEC, INIT_COMP );

  printf("Calculating UnaryExpGolomb parameters...");
  int iEstBits = (int)floor( xEstimateAllPar( cResLevelRate, cClus, acPar, UEGPar( 16, 1,  1 ), UEGPar( 16, 2, 8 ) ) + 0.5 );
  printf("Done!\n");

  ResLevelEncoder< CabacBinModel<EncodingEnvironment> > cResLevelEnc;
  cResLevelEnc.init( m_pEEP, 64 );
  xWriteAllPar( acPar );

  iBefore = CABAC::arienco_bits_written( m_pEEP );
  xEncodeAll(cResLevelEnc, cClus, acPar );

//  fprintf( pcStatFile, "AbsLevel:\t%10d\t", CABAC::arienco_bits_written( m_pEEP ) - iBefore );
  printf( "AbsLevel: %d\n", CABAC::arienco_bits_written( m_pEEP ) - iBefore );
  iBefore = CABAC::arienco_bits_written( m_pEEP );

  // sign
  SignEncoder< CabacBinModel<EncodingEnvironment> > cSignEnc( m_iLen );
  cSignEnc.init( m_pEEP, 64 );
  cSignEnc.encode( m_paiRes, getStride(), m_iNbr );

//  fprintf( pcStatFile, "Signs:\t%10d\t",  CABAC::arienco_bits_written( m_pEEP ) - iBefore );
  printf( "Signs: %d\n", CABAC::arienco_bits_written( m_pEEP ) - iBefore );
  iBefore = CABAC::arienco_bits_written( m_pEEP );

  // prediction information
  PredEncoder<CabacBinModel<EncodingEnvironment> > cPredEnc( m_iLen );
  cPredEnc.init( m_pEEP, 64 );
  m_paiPred[0].data[0] = 0;
  m_paiPred[0].data[1] = 0;
  m_paiPred[0].data[2] = 0;
  for (int v = 1; v < m_iNbr; v++)
  {
    for( int c = 0; c < 3; c++ )
    {
      cPredEnc.encode( m_paiPred[v].data[c], m_paiPredDim[v].data[c], m_paiPred[v-1].data[c], m_paiPredDim[v-1].data[c], c );
    }
  }
  int iPredBits = CABAC::arienco_bits_written( m_pEEP ) - iBefore;
//  fprintf( pcStatFile, "Pred:\t%10d\t", iPredBits );
  printf( "Pred: %d\n", iPredBits );
 

//  fprintf( pcStatFile, "UnLen:\t%d\tUnCutOff:\t%d\tGolPar:\t%d\t", acPar[0].iUnaryLength, acPar[0].iCutOff, acPar[0].iGolPar );
//  fprintf( pcStatFile, "\n" );
//  fclose( pcStatFile );
}


void ResidualDecoder::decode()
{
  // significance map
  SigLastDecoder cSigLast( m_iLen );
  cSigLast.init( m_pDEP, 64 );
  cSigLast.decode( m_paiRes, getStride(), m_iNbr );

  // absolute values
  std::vector<UEGPar> acPar;
  xReadAllPar( acPar );
  ResLevelDecoder cResLevelDec;
  cResLevelDec.init( m_pDEP, 64 );
  Clustering cClus( INIT_FREQ, INIT_VEC, INIT_COMP );
  xDecodeAll( cResLevelDec, cClus, acPar );

  // sign
  SignDecoder cSignDec( m_iLen );
  cSignDec.init( m_pDEP, 64 );
  cSignDec.decode( m_paiRes, getStride(), m_iNbr );

  // prediction information
  PredDecoder cPredDec( m_iLen );
  cPredDec.init( m_pDEP, 64 );
  m_paiPred[0].data[0] = 0;
  m_paiPred[0].data[1] = 0;
  m_paiPred[0].data[2] = 0;
  for (int v = 1; v < m_iNbr; v++)
  {
    for( int c = 0; c < 3; c++ )
    {
      cPredDec.decode( m_paiPred[v].data[c], m_paiPredDim[v].data[c], m_paiPred[v-1].data[c], m_paiPredDim[v-1].data[c], c );
    }
  }

  // apply new prediction
  for(int  v = 0; v < m_iNbr; v++ ){
	for(int  c = 0; c < 3; c++ ){
		if( m_paiPred[v].data[c] ){
			for(int d = 0; d < MIN_HHI( m_iLen, m_paiPredDim[v].data[c]); d++ ){
				m_paiRes[v * getStride() + d].data[c] += m_paiRes[(v-m_paiPred[v].data[c]) * getStride() + d].data[c];
			}
		}
	}
  }
}