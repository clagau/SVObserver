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

	BOOL AddInspectionToList( LPCTSTR InspectLabel,  LPCTSTR InspectName, bool NewInspection );
	BOOL SetToolsetImage( LPCTSTR InspectLabel,  LPCTSTR Image );
	BOOL SetColor( LPCTSTR InspectLabel, bool Color );
	BOOL SetNewDisableMethod( LPCTSTR InspectLabel,  LPCTSTR Disable );
	BOOL SetEnableAuxiliaryExtent( LPCTSTR InspectLabel, long sEnable );
	BOOL SetShowAuxExtent( LPCTSTR InspectLabel, bool p_bShow=true );
	BOOL ReNameInspection( LPCTSTR InspectLabel,  LPCTSTR NewInspectName );
	BOOL RemoveInspectionFromList( LPCTSTR InspectLabel);

	std::string GetInspectionName( LPCTSTR InspectLabel );
	std::string GetInspectionLabel( LPCTSTR InspectName );
	SVOInspectionObjPtr GetInspectionByName( LPCTSTR InspectLabel );
	SVOInspectionObjPtr GetInspectionByPosition(int iPos);
	BOOL IsInspectionInList( LPCTSTR InspectLabel) const;
	BOOL IsInspectionNameInList( LPCTSTR InspectName) const;
	BOOL IsInspectionInList( LPCTSTR InspectLabel,  LPCTSTR InspectName ) const;

	void ResetContent();
	int GetInspectionListCount() const;

private:
	SVInspectionList m_InspectionList;

	iterator FindInspectionPosition( LPCTSTR InspectLabel );
	iterator FindInspectionPositionFromName( LPCTSTR InspectName );
};

