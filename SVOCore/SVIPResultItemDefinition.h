//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIPResultItemDefinition
//* .File Name       : $Workfile:   SVIPResultItemDefinition.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:45:54  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

class SVIPResultItemDefinition
{
public:
	SVIPResultItemDefinition();
	SVIPResultItemDefinition( const SVGUID& p_rObjectID );
	SVIPResultItemDefinition( const SVGUID& p_rObjectID, int Index );
	SVIPResultItemDefinition( const SVIPResultItemDefinition& p_rObject );

	bool operator<( const SVIPResultItemDefinition& p_rObject ) const;
	bool operator==( const SVIPResultItemDefinition& p_rObject ) const;

	const SVGUID& GetObjectID() const;
	bool GetIndexPresent() const;
	
	int GetIndex() const;

protected:
	SVGUID m_ObjectID;

	bool m_IndexPresent;
	int m_Index;
};

