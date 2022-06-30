//******************************************************************************
//* COPYRIGHT (c) 2005 by Körber Pharma Inspection GmbH. All Rights Reserved
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

#pragma endregion Includes


class SVAccessPointNode  
{
public:
	SVAccessPointNode() = default;
	SVAccessPointNode(const SVAccessPointNode& rRhs);
	SVAccessPointNode(long lID, LPCTSTR Name, LPCTSTR NTGroup, bool bForce );
	~SVAccessPointNode() =default;
	const SVAccessPointNode& operator=( const SVAccessPointNode& rRhs );
	const SVAccessPointNode& CopyData( const SVAccessPointNode& rRhs );
	bool m_bHasData {false};
	long m_lID {0L};
	std::string m_Name;
	std::string m_NTGroup {_T("Administrators")};
	bool m_bForcePrompt {true};
	bool m_bDataCannotChange {false};
};
