//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOInspectionList
//* .File Name       : $Workfile:   SVOInspectionList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:04:18  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVContainerLibrary\SVList.h"
#include "SVOInspectionObj.h"
#pragma endregion Includes

class SVOInspectionList  
{
public:
	typedef SVList< SVOInspectionObjPtr > SVInspectionList;
	typedef SVInspectionList::iterator iterator;
	typedef SVInspectionList::const_iterator const_iterator;

	SVOInspectionList();
	virtual ~SVOInspectionList();

	BOOL AddInspectionToList(CString sInspectLabel, CString sInspectName, bool NewInspection);
	BOOL SetToolsetImage(CString sInspectLabel, CString sImage);
	BOOL SetColor(CString sInspectLabel, bool Color);
	BOOL SetNewDisableMethod(CString sInspectLabel, CString sDisable);
	BOOL SetEnableAuxiliaryExtent(CString sInspectLabel, long sEnable);
	BOOL SetShowAuxExtent(CString sInspectLabel, bool p_bShow=true);
	BOOL ReNameInspection(CString sInspectLabel, CString sNewInspectName);
	BOOL RemoveInspectionFromList(CString sInspectLabel);

	CString GetInspectionName(CString sInspectLabel);
	CString GetInspectionLabel(CString sInspectName);
	SVOInspectionObjPtr GetInspectionByName(CString sInspectLabel);
	SVOInspectionObjPtr GetInspectionByPosition(int iPos);
	BOOL IsInspectionInList(CString sInspectLabel) const;
	BOOL IsInspectionNameInList(CString sInspectName) const;
	BOOL IsInspectionInList(CString sInspectLabel, CString sInspectName) const;

	void ResetContent();
	int GetInspectionListCount() const;

private:
	SVInspectionList m_InspectionList;

	iterator FindInspectionPosition(CString sInspectLabel);
	iterator FindInspectionPositionFromName(CString sInspectName);


};

