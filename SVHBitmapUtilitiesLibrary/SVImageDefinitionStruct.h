// ******************************************************************************
// * COPYRIGHT (c) 2008 by Seidenader Vistion Inc., Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVImageDefinitionStruct
// * .File Name       : $Workfile:   SVImageDefinitionStruct.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 10:11:12  $
// ******************************************************************************

#ifndef SVIMAGE_DEFINITION_STRUCT_H
#define SVIMAGE_DEFINITION_STRUCT_H

#include <vector>
#include <comdef.h>
#include "SVImageFormatEnum.h"

#pragma pack (push, 1)
struct SVImageDefinitionStruct
{
	BSTR bstrDisplayName;
	long lHeight;
	long lWidth;
	SVImageFormatEnum eImageFormat;
public:
	SVImageDefinitionStruct( );
	~SVImageDefinitionStruct( );
	SVImageDefinitionStruct(const SVImageDefinitionStruct& rhs );
	const SVImageDefinitionStruct& operator = (const SVImageDefinitionStruct& rhs );
private:
	void Clear();
	void Init();
};
#pragma pack (pop)

typedef std::vector<SVImageDefinitionStruct> SVImageDefinitionStructArray;

#endif // SVIMAGE_DEFINITION_STRUCT_H

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVHBitmapUtilitiesLibrary\SVImageDefinitionStruct.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:11:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   24 May 2010 13:48:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include file due to changes in the data manager include path update.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   03 Mar 2010 11:01:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  682
 * SCR Title:  Upgrade SVObserver version for 5.31 release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to remove MFC from the project.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   20 Oct 2008 11:26:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  636
 * SCR Title:  Modify compiler settings to be consistent in SVObserver projects
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added pragma pack 1 to Image definition struct to force structure member alignment to 1 byte.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/