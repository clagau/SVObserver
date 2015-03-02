//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImagePropertiesClass
//* .File Name       : $Workfile:   SVImagePropertiesClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:51:26  $
//******************************************************************************

#ifndef SVIMAGEPROPERTIESCLASS_H
#define SVIMAGEPROPERTIESCLASS_H

#include "ObjectInterfaces\SVImageEnums.h"

class SVImagePropertiesClass
{
public:
	SVImagePropertiesClass();
	SVImagePropertiesClass( const SVImagePropertiesClass &p_rsvValue );
	virtual ~SVImagePropertiesClass();

	const SVImagePropertiesClass &operator=( const SVImagePropertiesClass &p_rsvValue );

	virtual HRESULT Initialize();

	virtual HRESULT DisableImageProperty( SVImagePropertyEnum p_eProperty );

	virtual HRESULT GetImageProperty( SVImagePropertyEnum p_eProperty, int &p_riValue ) const;
	virtual HRESULT GetImageProperty( SVImagePropertyEnum p_eProperty, long &p_rlValue ) const;

	virtual HRESULT SetImageProperty( SVImagePropertyEnum p_eProperty, long p_lValue );

	bool operator==( const SVImagePropertiesClass &p_rsvValue ) const;
	bool operator!=( const SVImagePropertiesClass &p_rsvValue ) const;

private:

	unsigned long m_ulProperties;

	int m_iFormat;
	int m_iPixelDepth;
	int m_iBandNumber;
	int m_iBandLink;
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageLibrary\SVImagePropertiesClass.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:51:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Mar 2011 13:13:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   25 Feb 2011 11:51:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Feb 2005 14:04:32   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   First Iteration of File
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
