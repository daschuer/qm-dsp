/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*-  vi:set ts=8 sts=4 sw=4: */

/*
    QM DSP Library

    Centre for Digital Music, Queen Mary, University of London.
    This file copyright 2005-2006 Christian Landone.
    All rights reserved.
*/

// PeakPicking.h: interface for the PeakPicking class.
//
//////////////////////////////////////////////////////////////////////

#ifndef PEAKPICKING_H
#define PEAKPICKING_H

#include "dsp/maths/MathUtilities.h"
#include "dsp/maths/MathAliases.h"
#include "dsp/signalconditioning/DFProcess.h"


struct WinThresh
{
    unsigned int pre;
    unsigned int  post;
};

struct QFitThresh
{
    double a;
    double b;
    double c;
};

struct PPickParams
{
    unsigned int length; //Detection FunctionLength
    double tau; // time resolution of the detection function:
    unsigned int alpha; //alpha-norm parameter
    double cutoff;//low-pass Filter cutoff freq
    unsigned int LPOrd; // low-pass Filter order
    double* LPACoeffs; //low pass Filter den coefficients
    double* LPBCoeffs; //low pass Filter num coefficients
    WinThresh WinT;//window size in frames for adaptive thresholding [pre post]:
    QFitThresh QuadThresh;
};

class PeakPicking  
{
public:
    PeakPicking( PPickParams Config );
    virtual ~PeakPicking();
	
    void process( double* src, unsigned int len, vector<int> &onsets  );


private:
    void initialise( PPickParams Config  );
    void deInitialise();
    int  quadEval( vector<double> &src, vector<int> &idx );
	
    DFProcConfig m_DFProcessingParams;

    unsigned int m_DFLength ;
    double m_alphaNormParam ;
    double Qfilta ;
    double Qfiltb;
    double Qfiltc;


    double* m_workBuffer;
	
    DFProcess*	m_DFSmoothing;
};

#endif
