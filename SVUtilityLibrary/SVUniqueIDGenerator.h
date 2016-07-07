//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVUniqueIDGenerator
//* .File Name       : $Workfile:   SVUniqueIDGenerator.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:26:00  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <assert.h>
#pragma endregion Includes

template <typename Type, typename InvalidID, class GetNext>
class SVUniqueIdGenerator
{
public:
	static Type GetID() 
	{ 
		GetNext::Next(m_lastID); 
		if (m_lastID == InvalidID())
		{
			m_bOverflow = true;
			assert(0);
		}
		return m_lastID;
	}
	static void Reset() { m_lastID = InvalidID(); m_bOverflow = false; }
	static void Set(const Type& nextID) { m_lastID = nextID; m_bOverflow = false; }

private:
	static Type m_lastID;
	static bool m_bOverflow;
};

///////////////////////////////////////////////////////////////////////
// Initialize Static Members
///////////////////////////////////////////////////////////////////////
template <typename Type, typename InvalidID, class GetNext>
Type SVUniqueIdGenerator<Type, InvalidID, GetNext>::m_lastID = InvalidID();

template <typename Type, typename InvalidID, class GetNext>
bool SVUniqueIdGenerator<Type, InvalidID, GetNext>::m_bOverflow = false;

