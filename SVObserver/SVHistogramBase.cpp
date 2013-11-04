//******************************************************************************
//* COPYRIGHT (c) 2009 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVHistogramBase
//* .File Name       : $Workfile:   SVHistogramBase.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:46:54  $
//******************************************************************************

#include "stdafx.h"
#include "SVHistogramBase.h"

SVHistogramBase::SVHistogramBase():
	scale(1.0),
	m_scale(histogram::linear),
	m_axis(histogram::percent_all),
	m_colors(histogram::config),
	m_clip(histogram::no_clip),
	m_disp(histogram::normal),
	m_placement(histogram::elsewhere),
	m_disabled(0),
	m_start(0),
	m_end(256),
	m_clipCount(256),
	m_lowPeak(0),
	m_highPeak(0),
	valley_low(0),
	valley_high(255),
	valley_default(0),
	needs_repaint(true)
{
	m_func = histogram::Scaler<histogram::linear>();
	m_values.resize(256);
	m_accumulated.resize(256);
}

SVHistogramBase::~SVHistogramBase()
{
}


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVHistogramBase.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:46:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   27 Jan 2011 11:12:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
