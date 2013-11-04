//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExtentPointStruct
//* .File Name       : $Workfile:   SVExtentPointStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:40:06  $
//******************************************************************************

#ifndef SVEXTENTPOINTSTRUCT_H
#define SVEXTENTPOINTSTRUCT_H

#include "SVContainerLibrary/SVVector.h"

struct SVExtentPointStruct  
{
	SVExtentPointStruct();
	SVExtentPointStruct( const SVExtentPointStruct &p_rsvValue );
	SVExtentPointStruct( const POINT &p_roValue );
	SVExtentPointStruct( double l_dValueX, double l_dValueY );
	virtual ~SVExtentPointStruct();

	const SVExtentPointStruct &operator=( const SVExtentPointStruct &p_rsvValue );
	const SVExtentPointStruct &operator=( const POINT &p_roValue );

	virtual HRESULT Initialize();

	bool operator==( const SVExtentPointStruct &p_rsvValue ) const;
	bool operator!=( const SVExtentPointStruct &p_rsvValue ) const;

	const SVExtentPointStruct& operator -= (const SVExtentPointStruct& p_rPoint);
	const SVExtentPointStruct& operator += (const SVExtentPointStruct& p_rPoint);
	friend SVExtentPointStruct operator - (const SVExtentPointStruct& p_rPoint1, const SVExtentPointStruct& p_rPoint2);
	friend SVExtentPointStruct operator + (const SVExtentPointStruct& p_rPoint1, const SVExtentPointStruct& p_rPoint2);
	friend SVExtentPointStruct operator / (const SVExtentPointStruct& p_rPoint1, double p_Constant);

	operator POINT() const;

	double m_dPositionX;
	double m_dPositionY;
};

typedef SVVector< SVExtentPointStruct > SVExtentPointStructCArray;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageLibrary\SVExtentPointStruct.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:40:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   30 Jul 2012 12:19:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent structures to provide a hit test methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 Nov 2010 15:41:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   19 May 2005 14:37:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added operators
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Feb 2005 14:03:34   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
