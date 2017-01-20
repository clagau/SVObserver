//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAccessPointNode
//* .File Name       : $Workfile:   SVAccessPointNode.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:00:38  $
//******************************************************************************

#pragma once

#pragma region Includes 
#include "SVUtilityLibrary\SVString.h"
#pragma endregion Includes


class SVAccessPointNode  
{
public:
	SVAccessPointNode();
	SVAccessPointNode(const SVAccessPointNode& rRhs);
	SVAccessPointNode(long lID, LPCTSTR Name, LPCTSTR NTGroup, bool bForce );
	~SVAccessPointNode();
	const SVAccessPointNode& operator=( const SVAccessPointNode& rRhs );
	const SVAccessPointNode& CopyData( const SVAccessPointNode& rRhs );
	bool m_bHasData;
	long m_lID;
	SVString m_Name;
	SVString m_NTGroup;
	bool m_bForcePrompt;
	bool m_bDataCannotChange;
};
