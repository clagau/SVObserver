//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIPImageDataElement
//* .File Name       : $Workfile:   SVIPImageDataElement.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:17:24  $
//******************************************************************************

#ifndef SVIPIMAGEDATAELEMENT_H
#define SVIPIMAGEDATAELEMENT_H

#include <map>

#include "SVImageLibrary/SVExtentMultiLineStruct.h"
#include "SVUtilityLibrary/SVUtilityGlobals.h"
#include "SVUtilityLibrary/SVGUID.h"

struct SVIPImageDataElement
{
	SVByteVector m_ImageDIB;
	SVExtentMultiLineStructCArray m_OverlayData;

	SVIPImageDataElement() : m_ImageDIB(), m_OverlayData() {}
};

typedef std::map< SVGUID, SVIPImageDataElement > SVIPImageDataElementMap;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVIPImageDataElement.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 11:17:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Jul 2012 16:58:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/