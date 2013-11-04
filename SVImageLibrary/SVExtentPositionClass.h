//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExtentPositionClass
//* .File Name       : $Workfile:   SVExtentPositionClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:39:38  $
//******************************************************************************

#ifndef SVEXTENTPOSITIONCLASS_H
#define SVEXTENTPOSITIONCLASS_H

#include "SVExtentEnums.h"
#include "SVExtentPointStruct.h"

class SVExtentPositionClass
{
public:
	SVExtentPositionClass();
	SVExtentPositionClass( const SVExtentPositionClass &p_rsvValue );
	virtual ~SVExtentPositionClass();

	const SVExtentPositionClass &operator=( const SVExtentPositionClass &p_rsvValue );

	virtual HRESULT Initialize();

	virtual HRESULT DisableExtentProperty( SVExtentPropertyEnum p_eProperty );
	bool IsEnabled( SVExtentPropertyEnum p_eProperty ) const;

	virtual HRESULT GetExtentProperty( SVExtentPropertyEnum p_eProperty, long &p_rlValue ) const;
	virtual HRESULT GetExtentProperty( SVExtentPropertyEnum p_eProperty, double &p_rdValue ) const;
	virtual HRESULT GetExtentProperty( SVExtentPropertyEnum p_eProperty, POINT &p_roValue ) const;
	virtual HRESULT GetExtentProperty( SVExtentPropertyEnum p_eProperty, SVExtentPointStruct &p_rsvValue ) const;

	virtual HRESULT SetExtentProperty( SVExtentPropertyEnum p_eProperty, double p_dValue );
	virtual HRESULT SetExtentProperty( SVExtentPropertyEnum p_eProperty, SVExtentPointStruct p_svValue );

	bool operator==( const SVExtentPositionClass &p_rsvValue ) const;
	bool operator!=( const SVExtentPositionClass &p_rsvValue ) const;

private:
	unsigned long m_ulProperties;

	// New Extent Data
	double m_dRotationAngle;

	SVExtentPointStruct m_svPositionPoint;
	SVExtentPointStruct m_svPositionPointEndOfLine;
	SVExtentPointStruct m_svTranslationOffset;
	SVExtentPointStruct m_svOutputPositionPoint;

	// Old Extent Data
	double m_dOldRotationAngle;

	SVExtentPointStruct m_svOldPositionPoint;
	SVExtentPointStruct m_svOldAbsPositionPoint;
	SVExtentPointStruct m_svOldRotationPoint;
	SVExtentPointStruct m_svOldTranslationOffset;
};

#endif // _SVEXTENTPOSITIONCLASS_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageLibrary\SVExtentPositionClass.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:39:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 Mar 2011 13:13:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Feb 2005 14:03:32   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
