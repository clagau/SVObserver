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
#include "Definitions/ObjectDefines.h"
#pragma endregion Includes


namespace SvIe
{

class SVIPResultItemDefinition
{
public:
	SVIPResultItemDefinition();
	explicit SVIPResultItemDefinition(uint32_t objectID );
	SVIPResultItemDefinition(uint32_t objectID, int Index );
	SVIPResultItemDefinition( const SVIPResultItemDefinition& p_rObject );

	bool operator<( const SVIPResultItemDefinition& p_rObject ) const;
	bool operator==( const SVIPResultItemDefinition& p_rObject ) const;

	uint32_t GetObjectID() const;
	bool GetIndexPresent() const;
	
	int GetIndex() const;

protected:
	uint32_t m_ObjectID = SvDef::InvalidObjectId;

	bool m_IndexPresent;
	int m_Index;
};
} //namespace SvIe

namespace std
{
	template<> struct hash<SvIe::SVIPResultItemDefinition>
	{
		std::size_t operator()(SvIe::SVIPResultItemDefinition const& s) const noexcept
		{
			return std::hash<uint32_t> {}(s.GetObjectID());
		}
	};
}