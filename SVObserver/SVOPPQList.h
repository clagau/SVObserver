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

    BOOL AddPPQToList(CString sPPQName);
    BOOL RemovePPQFromList(CString sPPQName);

    BOOL AttachCameraToPPQ(CString sPPQName, CString sCamera);
    BOOL RemoveCameraFromPPQ(CString sPPQName, CString sCamera);

    BOOL AttachInspectToPPQ(CString sPPQName, CString sInspect);
    BOOL RemoveInspectFromPPQ(CString sPPQName, CString sInspect);
    
    BOOL AttachTriggerToPPQ(CString sPPQName, CString sTrigger);
    BOOL RemoveTriggerFromPPQ(CString sPPQName);

    int GetPPQListCount() const;
    SVOPPQObjPtr GetPPQObjectByName(CString sPPQName);
	SVOPPQObjPtr GetPPQObjectByPosition(int iPos);
	const SVOPPQObjPtr GetPPQObjectByPosition(int iPos) const;
    BOOL IsPPQInList(CString sPPQName) const;
    void ResetContent();
    
private:
	SVPPQList m_PPQList;
    
	iterator FindPPQPosition(CString sPPQName);

};

