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
//Moved to precompiled header: #include <list>
#include "SVOInspectionObj.h"

#pragma endregion Includes

class SVOInspectionList  
{
public:
	typedef std::list< SVOInspectionObjPtr > SVInspectionList;
	typedef SVInspectionList::iterator iterator;
	typedef SVInspectionList::const_iterator const_iterator;

	SVOInspectionList();
	virtual ~SVOInspectionList();

	bool AddInspectionToList(LPCTSTR InspectionName, bool NewInspection );
	bool SetToolsetImage( LPCTSTR InspectionName,  LPCTSTR Image );
	bool SetColor( LPCTSTR InspectionName, bool Color );
	bool SetNewDisableMethod( LPCTSTR InspectionName,  LPCTSTR Disable );
	bool SetEnableAuxiliaryExtent( LPCTSTR InspectionName, long sEnable );
	bool SetShowAuxExtent( LPCTSTR InspectionName, bool p_bShow=true );
	bool RenameInspection( LPCTSTR InspectionName,  LPCTSTR NewInspectionName );
	bool RemoveInspectionFromList( LPCTSTR InspectionName);

	SVOInspectionObjPtr GetInspectionByName(LPCTSTR InspectionName);
	SVOInspectionObjPtr GetInspectionByOriginalName(LPCTSTR InspectionName);
	SVOInspectionObjPtr GetInspectionByPosition(int iPos);
	bool IsInspectionInList( LPCTSTR InspectionName) const;
	bool IsOriginalInspectionInList(LPCTSTR InspectionName) const;

	void ResetContent();
	int GetInspectionListCount() const;

private:
	SVInspectionList m_InspectionList;
};

