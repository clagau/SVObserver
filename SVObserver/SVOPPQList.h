//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOPPQList
//* .File Name       : $Workfile:   SVOPPQList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:10:34  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVContainerLibrary\SVList.h"
#include "SVOPPQObj.h"
#pragma endregion Includes

class SVOPPQList  
{
public:
	typedef SVList< SVOPPQObjPtr > SVPPQList;
	typedef SVPPQList::iterator iterator;
	typedef SVPPQList::const_iterator	const_iterator;

	SVOPPQList();
	virtual ~SVOPPQList();

    bool AddPPQToList(LPCTSTR PPQName);
    bool RemovePPQFromList(LPCTSTR PPQName);

    bool AttachCameraToPPQ(LPCTSTR PPQName, LPCTSTR Camera);
    bool AttachInspectToPPQ(LPCTSTR PPQName, LPCTSTR Inspect);
    void AttachTriggerToPPQ(LPCTSTR PPQName, LPCTSTR Trigger);

    int GetPPQListCount() const;
    SVOPPQObjPtr GetPPQObjectByName(LPCTSTR PPQName);
	SVOPPQObjPtr GetPPQObjectByPosition(int iPos);
	const SVOPPQObjPtr GetPPQObjectByPosition(int iPos) const;
    bool IsPPQInList(LPCTSTR PPQName) const;
    void ResetContent();
    
private:
	SVPPQList m_PPQList;
    
	iterator FindPPQPosition(LPCTSTR PPQName);

};

