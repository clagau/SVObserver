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

#ifndef SVUNIQUEIDGENERATOR_H
#define SVUNIQUEIDGENERATOR_H

#include <assert.h>

/**
@SVObjectName Unique Identifier Generator

@SVObjectOverview A class to generate ids it just keeps adding to the last id generated

Note: it will roll over and when that happens, we are in big trouble! a better way might be to use a dynamic bitarray and track ids in use/free

@SVObjectOperations

*/
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

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVUtilityLibrary\SVUniqueIDGenerator.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 19:26:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Sep 2008 10:33:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
