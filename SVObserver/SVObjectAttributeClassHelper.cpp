//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectAttributeClassHelper
//* .File Name       : $Workfile:   SVObjectAttributeClassHelper.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:39:54  $
//******************************************************************************

#include "stdafx.h"
#include "SVObjectAttributeClassHelper.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"

BOOL SVObjectAttributeClassHelper::GetArrayData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector<BOOL>& raData, BOOL defaultValue)
{
	return pDataObject->GetArrayData(szName, raData, defaultValue);
}

BOOL SVObjectAttributeClassHelper::GetArrayData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector<BYTE>& raData, BYTE defaultValue)
{
	return pDataObject->GetArrayData(szName, raData, defaultValue);
}

BOOL SVObjectAttributeClassHelper::GetArrayData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector<char>& raData, char defaultValue)
{
	return pDataObject->GetArrayData(szName, raData, defaultValue);
}

BOOL SVObjectAttributeClassHelper::GetArrayData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector<CString>& raData, CString defaultValue)
{
	return pDataObject->GetArrayData(szName, raData, defaultValue);
}

BOOL SVObjectAttributeClassHelper::GetArrayData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector<SVString>& raData, SVString defaultValue)
{
	return pDataObject->GetArrayData(szName, raData, defaultValue);
}

BOOL SVObjectAttributeClassHelper::GetArrayData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector<double>& raData, double defaultValue)
{
	return pDataObject->GetArrayData(szName, raData, defaultValue);
}

BOOL SVObjectAttributeClassHelper::GetArrayData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector<DWORD>& raData, DWORD defaultValue)
{
	return pDataObject->GetArrayData(szName, raData, defaultValue);
}

BOOL SVObjectAttributeClassHelper::GetArrayData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector<long>& raData, long defaultValue)
{
	return pDataObject->GetArrayData(szName, raData, defaultValue);
}

BOOL SVObjectAttributeClassHelper::GetArrayData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector<SVPOINT>& raData, SVPOINT defaultValue)
{
	return pDataObject->GetArrayData(szName, raData, defaultValue);
}

BOOL SVObjectAttributeClassHelper::GetArrayData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector<SVDPointClass>& raData, SVDPointClass defaultValue)
{
	return pDataObject->GetArrayData(szName, raData, defaultValue);
}

BOOL SVObjectAttributeClassHelper::GetArrayData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector<_variant_t>& raData, _variant_t defaultValue)
{
	return pDataObject->GetArrayData(szName, raData, defaultValue);
}

BOOL SVObjectAttributeClassHelper::GetArrayData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector<__int64>& raData, __int64 defaultValue)
{
	return pDataObject->GetArrayData(szName, raData, defaultValue);
}

BOOL SVObjectAttributeClassHelper::GetAttributeData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector< std::vector<BOOL> >& raaData, BOOL defaultValue)
{
	return pDataObject->GetAttributeData(szName, raaData, defaultValue);
}

BOOL SVObjectAttributeClassHelper::GetAttributeData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector< std::vector<BYTE> >& raaData, BYTE defaultValue)
{
	return pDataObject->GetAttributeData(szName, raaData, defaultValue);
}

BOOL SVObjectAttributeClassHelper::GetAttributeData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector< std::vector<char> >& raaData, char defaultValue)
{
	return pDataObject->GetAttributeData(szName, raaData, defaultValue);
}

BOOL SVObjectAttributeClassHelper::GetAttributeData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector< std::vector<CString> >& raaData, CString defaultValue)
{
	return pDataObject->GetAttributeData(szName, raaData, defaultValue);
}

BOOL SVObjectAttributeClassHelper::GetAttributeData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector< std::vector<SVString> >& raaData, SVString defaultValue)
{
	return pDataObject->GetAttributeData(szName, raaData, defaultValue);
}

BOOL SVObjectAttributeClassHelper::GetAttributeData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector< std::vector<double> >& raaData, double defaultValue)
{
	return pDataObject->GetAttributeData(szName, raaData, defaultValue);
}

BOOL SVObjectAttributeClassHelper::GetAttributeData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector< std::vector<DWORD> >& raaData, DWORD defaultValue)
{
	return pDataObject->GetAttributeData(szName, raaData, defaultValue);
}

BOOL SVObjectAttributeClassHelper::GetAttributeData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector< std::vector<long> >& raaData, long defaultValue)
{
	return pDataObject->GetAttributeData(szName, raaData, defaultValue);
}

BOOL SVObjectAttributeClassHelper::GetAttributeData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector< std::vector<SVPOINT> >& raaData, SVPOINT defaultValue)
{
	return pDataObject->GetAttributeData(szName, raaData, defaultValue);
}

BOOL SVObjectAttributeClassHelper::GetAttributeData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector< std::vector<SVDPointClass> >& raaData, SVDPointClass defaultValue)
{
	return pDataObject->GetAttributeData(szName, raaData, defaultValue);
}

BOOL SVObjectAttributeClassHelper::GetAttributeData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector< std::vector<_variant_t> >& raaData, _variant_t defaultValue)
{
	return pDataObject->GetAttributeData(szName, raaData, defaultValue);
}

BOOL SVObjectAttributeClassHelper::GetAttributeData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector< std::vector<__int64> >& raaData, __int64 defaultValue)
{
	return pDataObject->GetAttributeData(szName, raaData, defaultValue);
}

BOOL SVObjectAttributeClassHelper::GetAttributeData(SVObjectAttributeClass* pDataObject ,LPCTSTR szName, SVObjectArrayClassTemplate<BOOL>& svData)
{
	return pDataObject->GetAttributeData(szName, svData);
}

BOOL SVObjectAttributeClassHelper::GetAttributeData(SVObjectAttributeClass* pDataObject ,LPCTSTR szName, SVObjectArrayClassTemplate<BYTE>& svData)
{
	return pDataObject->GetAttributeData(szName, svData);
}

BOOL SVObjectAttributeClassHelper::GetAttributeData(SVObjectAttributeClass* pDataObject ,LPCTSTR szName, SVObjectArrayClassTemplate<char>& svData)
{
	return pDataObject->GetAttributeData(szName, svData);
}

BOOL SVObjectAttributeClassHelper::GetAttributeData(SVObjectAttributeClass* pDataObject ,LPCTSTR szName, SVObjectArrayClassTemplate<CString>& svData)
{
	return pDataObject->GetAttributeData(szName, svData);
}

BOOL SVObjectAttributeClassHelper::GetAttributeData(SVObjectAttributeClass* pDataObject ,LPCTSTR szName, SVObjectArrayClassTemplate<SVString>& svData)
{
	return pDataObject->GetAttributeData(szName, svData);
}

BOOL SVObjectAttributeClassHelper::GetAttributeData(SVObjectAttributeClass* pDataObject ,LPCTSTR szName, SVObjectArrayClassTemplate<double>& svData)
{
	return pDataObject->GetAttributeData(szName, svData);
}

BOOL SVObjectAttributeClassHelper::GetAttributeData(SVObjectAttributeClass* pDataObject ,LPCTSTR szName, SVObjectArrayClassTemplate<DWORD>& svData)
{
	return pDataObject->GetAttributeData(szName, svData);
}

BOOL SVObjectAttributeClassHelper::GetAttributeData(SVObjectAttributeClass* pDataObject ,LPCTSTR szName, SVObjectArrayClassTemplate<long>& svData)
{
	return pDataObject->GetAttributeData(szName, svData);
}

BOOL SVObjectAttributeClassHelper::GetAttributeData(SVObjectAttributeClass* pDataObject ,LPCTSTR szName, SVObjectArrayClassTemplate<SVPOINT>& svData)
{
	return pDataObject->GetAttributeData(szName, svData);
}

BOOL SVObjectAttributeClassHelper::GetAttributeData(SVObjectAttributeClass* pDataObject ,LPCTSTR szName, SVObjectArrayClassTemplate<SVDPointClass>& svData)
{
	return pDataObject->GetAttributeData(szName, svData);
}

BOOL SVObjectAttributeClassHelper::GetAttributeData(SVObjectAttributeClass* pDataObject ,LPCTSTR szName, SVObjectArrayClassTemplate<_variant_t>& svData)
{
	return pDataObject->GetAttributeData(szName, svData);
}

BOOL SVObjectAttributeClassHelper::GetAttributeData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, SVObjectArrayClassTemplate<__int64>& svData)
{
	return pDataObject->GetAttributeData(szName, svData);
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVObjectAttributeClassHelper.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:39:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   04 Sep 2012 15:45:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to include 64 bit value object persistance support.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   21 Feb 2012 13:32:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated code to fix loading issues associated with POINT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   05 Nov 2010 11:08:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove redundent data objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Dec 2009 12:08:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
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
 *    Rev 1.0   20 Jun 2005 13:34:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
