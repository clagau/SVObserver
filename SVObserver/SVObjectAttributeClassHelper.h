//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectAttributeClassHelper
//* .File Name       : $Workfile:   SVObjectAttributeClassHelper.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:40:04  $
//******************************************************************************

#ifndef SVOBJECTATTRIBUTECLASSHELPER_H_
#define SVOBJECTATTRIBUTECLASSHELPER_H_

#include <vector>
#include "SVContainerLibrary\SVObjectArrayClassTemplate.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"

class SVObjectAttributeClassHelper
{
public:
	template <typename T>
	static BOOL GetArrayData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector<T>& raData, T defaultValue)
	{
		return pDataObject->GetArrayData(szName, raData, defaultValue);
	}

	template <typename T>
	static BOOL GetAttributeData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector< std::vector<T> >& raaData, T defaultValue)
	{
		return pDataObject->GetAttributeData(szName, raaData, defaultValue);
	}

	template <typename T>
	static BOOL GetAttributeData(SVObjectAttributeClass* pDataObject ,LPCTSTR szName, SvCl::SVObjectArrayClassTemplate<T>& svData)
	{
		return pDataObject->GetAttributeData(szName, svData);
	}
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVObjectAttributeClassHelper.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:40:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   21 Feb 2012 13:32:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated code to fix loading issues associated with POINT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   05 Nov 2010 11:08:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove redundent data objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Dec 2009 12:08:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Oct 2009 11:18:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed Path for SVDPointClass to SVUtilityLibrary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   14 Jul 2005 12:46:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented new array object script parsing functions
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   20 Jun 2005 13:34:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
