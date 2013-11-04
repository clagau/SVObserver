// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : ISVCancel
// * .File Name       : $Workfile:   ISVCancel.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 09:13:38  $
// ******************************************************************************

#ifndef ISVCANCEL_H
#define ISVCANCEL_H
#pragma once

#include <map>

#include "SVTaskObjectInterfaceInputRequestStruct.h"

struct SVCancelData
{
	SVCancelData() {}
	virtual ~SVCancelData() {}
};

class ISVCancel
{
public:
	virtual bool CanCancel() = 0;
	virtual HRESULT GetCancelData(SVCancelData*& rpData) = 0;
	virtual HRESULT SetCancelData(SVCancelData* pData) = 0;
};

// new style using the input request queue
// the caller of GetCancelData is responsible for sending it to the input request queue
// (via SVTaskObjectInterfaceClass)
class ISVCancel2
{
public:
	virtual HRESULT GetCancelData(SVInputRequestStructMap& rMap) = 0;
};

struct SVMultiCancelData : public SVCancelData
{
	typedef std::map<ISVCancel*, SVCancelData*> MapType;
	MapType mapData;

	SVMultiCancelData() {}
	inline const SVMultiCancelData& operator = (const SVMultiCancelData& rhs);
	inline ~SVMultiCancelData();
};

// this class assumes that when one is assigned to another, only one will be full when deleted.
// ownership of the SVCancelData (via mapData.clear() on nonowners) is left up to the client code.

inline SVMultiCancelData::~SVMultiCancelData()
{
	MapType::iterator iter;
	for ( iter = mapData.begin(); iter != mapData.end(); ++iter)
	{
		delete iter->second;
	}
	mapData.clear();
}

inline const SVMultiCancelData& SVMultiCancelData::operator = (const SVMultiCancelData& rhs)
{
	mapData = rhs.mapData;
	return *this;
}

#endif  //#ifndef ISVCANCEL_H
// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\ISVCancel.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:13:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   04 Oct 2012 10:36:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed from post incrementor to pre incrementor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Aug 2005 10:28:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   implemented ISVCancel2 for new-style InputRequest data
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   08 Jul 2004 11:54:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
