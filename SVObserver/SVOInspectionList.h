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

	bool AddInspectionToList( LPCTSTR InspectLabel,  LPCTSTR InspectName, bool NewInspection );
	bool SetToolsetImage( LPCTSTR InspectLabel,  LPCTSTR Image );
	bool SetColor( LPCTSTR InspectLabel, bool Color );
	bool SetNewDisableMethod( LPCTSTR InspectLabel,  LPCTSTR Disable );
	bool SetEnableAuxiliaryExtent( LPCTSTR InspectLabel, long sEnable );
	bool SetShowAuxExtent( LPCTSTR InspectLabel, bool p_bShow=true );
	bool ReNameInspection( LPCTSTR InspectLabel,  LPCTSTR NewInspectName );
	bool RemoveInspectionFromList( LPCTSTR InspectLabel);

	std::string GetInspectionName( LPCTSTR InspectLabel );
	std::string GetInspectionLabel( LPCTSTR InspectName );
	SVOInspectionObjPtr GetInspectionByLabel( LPCTSTR InspectLabel );
	SVOInspectionObjPtr GetInspectionByPosition(int iPos);
	bool IsInspectionInList( LPCTSTR InspectLabel) const;
	bool IsInspectionNameInList( LPCTSTR InspectName) const;
	bool IsInspectionInList( LPCTSTR InspectLabel,  LPCTSTR InspectName ) const;

	void ResetContent();
	int GetInspectionListCount() const;

private:
	SVInspectionList m_InspectionList;
};

