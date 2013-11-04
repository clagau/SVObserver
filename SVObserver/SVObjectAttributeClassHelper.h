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
#include <comdef.h>
#include "SVObjectLibrary/SVObjectArrayClassTemplate.h"
#include "SVUtilityLibrary/SVDPointClass.h"
#include "SVUtilityLibrary/SVPOINT.h"
class SVObjectAttributeClass;

class SVObjectAttributeClassHelper
{
public:
	static BOOL GetArrayData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector<BOOL>& raaData, BOOL defaultValue);
	static BOOL GetArrayData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector<BYTE>& raaData, BYTE defaultValue);
	static BOOL GetArrayData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector<char>& raaData, char defaultValue);
	static BOOL GetArrayData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector<CString>& raaData, CString defaultValue);
	static BOOL GetArrayData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector<SVString>& raaData, SVString defaultValue);
	static BOOL GetArrayData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector<double>& raaData, double defaultValue);
	static BOOL GetArrayData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector<DWORD>& raaData, DWORD defaultValue);
	static BOOL GetArrayData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector<long>& raaData, long defaultValue);
	static BOOL GetArrayData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector<SVPOINT>& raaData, SVPOINT defaultValue);
	static BOOL GetArrayData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector<SVDPointClass>& raaData, SVDPointClass defaultValue);
	static BOOL GetArrayData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector<_variant_t>& raaData, _variant_t defaultValue);
	static BOOL GetArrayData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector<__int64>& raaData, __int64 defaultValue);

	static BOOL GetAttributeData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector< std::vector<BOOL> >& raaData, BOOL defaultValue);
	static BOOL GetAttributeData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector< std::vector<BYTE> >& raaData, BYTE defaultValue);
	static BOOL GetAttributeData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector< std::vector<char> >& raaData, char defaultValue);
	static BOOL GetAttributeData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector< std::vector<CString> >& raaData, CString defaultValue);
	static BOOL GetAttributeData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector< std::vector<SVString> >& raaData, SVString defaultValue);
	static BOOL GetAttributeData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector< std::vector<double> >& raaData, double defaultValue);
	static BOOL GetAttributeData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector< std::vector<DWORD> >& raaData, DWORD defaultValue);
	static BOOL GetAttributeData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector< std::vector<long> >& raaData, long defaultValue);
	static BOOL GetAttributeData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector< std::vector<SVPOINT> >& raaData, SVPOINT defaultValue);
	static BOOL GetAttributeData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector< std::vector<SVDPointClass> >& raaData, SVDPointClass defaultValue);
	static BOOL GetAttributeData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector< std::vector<_variant_t> >& raaData, _variant_t defaultValue);
	static BOOL GetAttributeData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector< std::vector<__int64> >& raaData, __int64 defaultValue);

	static BOOL GetAttributeData(SVObjectAttributeClass* pDataObject ,LPCTSTR szName, SVObjectArrayClassTemplate<BOOL>& svData);
	static BOOL GetAttributeData(SVObjectAttributeClass* pDataObject ,LPCTSTR szName, SVObjectArrayClassTemplate<BYTE>& svData);
	static BOOL GetAttributeData(SVObjectAttributeClass* pDataObject ,LPCTSTR szName, SVObjectArrayClassTemplate<char>& svData);
	static BOOL GetAttributeData(SVObjectAttributeClass* pDataObject ,LPCTSTR szName, SVObjectArrayClassTemplate<CString>& svData);
	static BOOL GetAttributeData(SVObjectAttributeClass* pDataObject ,LPCTSTR szName, SVObjectArrayClassTemplate<SVString>& svData);
	static BOOL GetAttributeData(SVObjectAttributeClass* pDataObject ,LPCTSTR szName, SVObjectArrayClassTemplate<double>& svData);
	static BOOL GetAttributeData(SVObjectAttributeClass* pDataObject ,LPCTSTR szName, SVObjectArrayClassTemplate<DWORD>& svData);
	static BOOL GetAttributeData(SVObjectAttributeClass* pDataObject ,LPCTSTR szName, SVObjectArrayClassTemplate<long>& svData);
	static BOOL GetAttributeData(SVObjectAttributeClass* pDataObject ,LPCTSTR szName, SVObjectArrayClassTemplate<SVPOINT>& svData);
	static BOOL GetAttributeData(SVObjectAttributeClass* pDataObject ,LPCTSTR szName, SVObjectArrayClassTemplate<SVDPointClass>& svData);
	static BOOL GetAttributeData(SVObjectAttributeClass* pDataObject ,LPCTSTR szName, SVObjectArrayClassTemplate<_variant_t>& svData);
	static BOOL GetAttributeData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, SVObjectArrayClassTemplate<__int64>& raaData);
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
