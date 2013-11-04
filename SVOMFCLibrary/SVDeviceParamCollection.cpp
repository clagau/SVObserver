// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVDeviceParamCollection
// * .File Name       : $Workfile:   SVDeviceParamCollection.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 13:02:20  $
// ******************************************************************************

// SVDeviceParamCollection.cpp: definition for the SVDeviceParamCollection struct.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SVDeviceParamCollection.h"
#include "SVDeviceParams.h"
#include "SVCustomDeviceParam.h"
//#include "SVObserverEnums.h"
#include "SVOMFCLibraryGlobals.h"
#include <algorithm>

SVDeviceParamIndexer::SVDeviceParamIndexer(const SVDeviceParamMap& map) : m_mapIndexed(map)
{
	SVDeviceParamMap::const_iterator iter;
	// fill vector with unsorted enums
	for (iter = map.begin(); iter != map.end(); ++iter)
	{
		push_back(iter->first);
	}
	// now sort
	std::sort(begin(), end(), keycomp(m_mapIndexed));
}

bool SVDeviceParamIndexer::keycomp::operator () (const SVDeviceParamEnum& a, const SVDeviceParamEnum& b) const
{
	SVDeviceParamMap::const_iterator iterA = m_mapIndexed.find(a);
	SVDeviceParamMap::const_iterator iterB = m_mapIndexed.find(b);
	const SVDeviceParam* pA = iterA->second;
	const SVDeviceParam* pB = iterB->second;
	if (pB==NULL)
		return true;
	else if (pA==NULL)
		return false;
	else
		return iterA->second->GetOrder() < iterB->second->GetOrder();
}

SVDeviceParamCollection::SVDeviceParamCollection( const SVDeviceParamCollection& rhs )
{
		mapParameters = rhs.mapParameters;
}

SVDeviceParamCollection& SVDeviceParamCollection::operator = ( const SVDeviceParamCollection& rhs )
{
	if ( this != &rhs )
	{
		mapParameters = rhs.mapParameters;
	}
	return *this;
}

HRESULT SVDeviceParamCollection::Clear()
{
	mapParameters.clear();
	return S_OK;
}

HRESULT SVDeviceParamCollection::CreateParameter( SVDeviceParamEnum e, const VARIANT& rv )
{
	if (e >= DeviceParamGigeCustom1 && e <= DeviceParamGigeCustom30)
	{
		SVDeviceParam* pParam = SVCustomDeviceParam::Create(e, rv);
		if (pParam)
		{
			SetParameter(e, pParam);
			SVDeviceParam* pParam = GetParameter(e);
			if (pParam)
			{
				return pParam->SetValue(rv);
			}
		}
	}
	else
	{
		SetParameter( e, (const SVDeviceParam*) SVDeviceParamTempWrapper(SVDeviceParam::Create( e )) );
		SVDeviceParam* pParam = GetParameter( e );
		if ( pParam )
		{
			return pParam->SetValue( rv );
		}
	}
	return SV_FALSE;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOMFCLibrary\SVDeviceParamCollection.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 13:02:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   01 Apr 2013 13:41:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  820
 * SCR Title:  Baumer Camera Trigger Debounce
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Moved the definition and implenetation for the SVDeviceParam class to SVDeviceParam..h/.cpp
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   04 Oct 2012 09:32:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed from post incrementor to pre incrementor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   18 Feb 2011 09:49:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  700
 * SCR Title:  Remove String Buffer Problems
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to remove duplicate string class, and fixed string conversion issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   20 Jun 2007 14:48:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   11 Feb 2004 17:27:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed compiler warnings
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   03 Feb 2004 17:17:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added support for named device params
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   17 Sep 2003 08:56:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added Clear function and VisualName property
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   29 Aug 2003 13:10:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added support for NullDeviceParam
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   31 Jul 2003 08:27:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added an #include
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Jul 2003 09:03:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed Level to DetailLevel
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Jul 2003 08:41:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   renamed Level to DetailLevel in SVDeviceParam
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Jul 2003 10:19:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   transfered from SVObserver project
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
