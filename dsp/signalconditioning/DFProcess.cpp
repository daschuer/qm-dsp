/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*-  vi:set ts=8 sts=4 sw=4: */

/*
    QM DSP Library

    Centre for Digital Music, Queen Mary, University of London.
    This file copyright 2005-2006 Christian Landone.
    All rights reserved.
*/

#include "DFProcess.h"
#include "dsp/maths/MathUtilities.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DFProcess::DFProcess( DFProcConfig Config )
{
    filtSrc = NULL;
    filtDst = NULL;	
    m_filtScratchIn = NULL;
    m_filtScratchOut = NULL;

    m_FFOrd = 0;

    initialise( Config );
}

DFProcess::~DFProcess()
{
    deInitialise();
}

void DFProcess::initialise( DFProcConfig Config )
{
    m_length = Config.length;
    m_winPre = Config.winPre;
    m_winPost = Config.winPost;
    m_alphaNormParam = Config.AlphaNormParam;

    m_isMedianPositive = Config.isMedianPositive;

    filtSrc = new double[ m_length ];
    filtDst = new double[ m_length ];

	
    //Low Pass Smoothing Filter Config
    m_FilterConfigParams.ord = Config.LPOrd;
    m_FilterConfigParams.ACoeffs = Config.LPACoeffs;
    m_FilterConfigParams.BCoeffs = Config.LPBCoeffs;
	
    m_FiltFilt = new FiltFilt( m_FilterConfigParams );	
}

void DFProcess::deInitialise()
{
    delete [] filtSrc;

    delete [] filtDst;

    delete [] m_filtScratchIn;

    delete [] m_filtScratchOut;

    delete m_FiltFilt;
}

void DFProcess::process(double *src, double* dst)
{
    removeDCNormalize( src, filtSrc );

    m_FiltFilt->process( filtSrc, filtDst, m_length );

    medianFilter( filtDst, dst );
}


void DFProcess::medianFilter(double *src, double *dst)
{
    unsigned int i,k,j,l;
    unsigned int index = 0;

    double val = 0;

    double* y = new double[ m_winPost + m_winPre + 1];
    memset( y, 0, sizeof( double ) * ( m_winPost + m_winPre + 1) );

    double* scratch = new double[ m_length ];

    for( i = 0; i < m_winPre; i++)
    {
	k = i + m_winPost + 1;

	for( j = 0; j < k; j++)
	{
	    y[ j ] = src[ j ];
	}
	scratch[ index ] = MathUtilities::median( y, k );
	index++;
    }

    for(  i = 0; i < ( m_length - ( m_winPost + m_winPre ) ); i ++)
    {
			 
	l = 0;
	for(  j  = i; j < ( i + m_winPost + m_winPre + 1); j++)
	{
	    y[ l ] = src[ j ];
	    l++;
	}

	scratch[ index++ ] = MathUtilities::median( y, (m_winPost + m_winPre + 1 ));
    }

    for( i = std::max( m_length - m_winPost, (unsigned)1); i < m_length; i++)
    {
	k = std::max( i - m_winPre, (unsigned)1);

	l = 0;
	for( j = k; j < m_length; j++)
	{
	    y[ l ] = src[ j ];

	    l++;
	}
		
	scratch[ index++ ] = MathUtilities::median( y, l); 
    }


    for( i = 0; i < m_length; i++ )
    {
	val = src[ i ] - scratch[ i ];// - 0.033;
		
	if( m_isMedianPositive )
	{
	    if( val > 0 )
	    {
		dst[ i ]  = val;
	    }
	    else
	    {
		dst[ i ]  = 0;
	    }
	}
	else
	{
	    dst[ i ]  = val;
	}
    }
	
    delete [] y;
    delete [] scratch;
}


void DFProcess::removeDCNormalize( double *src, double*dst )
{
    double DFmax = 0;
    double DFMin = 0;
    double DFAlphaNorm = 0;

    MathUtilities::getFrameMinMax( src, m_length, &DFMin, &DFmax );

    MathUtilities::getAlphaNorm( src, m_length, m_alphaNormParam, &DFAlphaNorm );

    for( unsigned int i = 0; i< m_length; i++)
    {
	dst[ i ] = ( src[ i ] - DFMin ) / DFAlphaNorm; 
    }
}
