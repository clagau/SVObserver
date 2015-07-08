//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExtentDimensionsClass
//* .File Name       : $Workfile:   SVExtentDimensionsClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:37:02  $
//******************************************************************************

#ifndef SVEXTENTDIMENSIONSCLASS_H
#define SVEXTENTDIMENSIONSCLASS_H

#include "SVExtentEnums.h"

class SVExtentDimensionsClass
{
public:
	SVExtentDimensionsClass();
	SVExtentDimensionsClass( const SVExtentDimensionsClass &p_rsvValue );
	virtual ~SVExtentDimensionsClass();

	const SVExtentDimensionsClass &operator=( const SVExtentDimensionsClass &p_rsvValue );

	virtual HRESULT Initialize();

	virtual HRESULT DisableExtentProperty( SVExtentPropertyEnum p_eProperty );
	bool IsEnabled( SVExtentPropertyEnum p_eProperty ) const;

	virtual HRESULT GetExtentProperty( SVExtentPropertyEnum p_eProperty, long &p_rlValue ) const;
	virtual HRESULT GetExtentProperty( SVExtentPropertyEnum p_eProperty, double &p_rdValue ) const;

	virtual HRESULT SetExtentProperty( SVExtentPropertyEnum p_eProperty, double p_dValue );

	bool operator==( const SVExtentDimensionsClass &p_rsvValue ) const;
	bool operator!=( const SVExtentDimensionsClass &p_rsvValue ) const;

private:
//	currently enabled properties
	unsigned long m_ulProperties;

	double m_dWidth;
	double m_dHeight;
	double m_dStartAngle;
	double m_dEndAngle;
	double m_dInnerRadius;
	double m_dOuterRadius;

	/// m_dWidthScaleFactor - indicates the scale factor to be used size the 
	///  output buffer width from the input image ROI width. Keep in mind that the
	///  input image context is the input to the current task object.
	double m_dWidthScaleFactor;

	/// m_dHeightScaleFactor - indicates the scale factor to be used size the 
	///  output buffer height from the input image ROI height. Keep in mind that the
	///  input image context is the input to the current task object.
	double m_dHeightScaleFactor;

	long m_lOutputWidth;
	long m_lOutputHeight;
};

#endif // _SVEXTENTDIMENTIONSCLASS_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageLibrary\SVExtentDimensionsClass.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:37:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 Mar 2011 13:13:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Feb 2005 14:01:54   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   First Iteration of File
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
