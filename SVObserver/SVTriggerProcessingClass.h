//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTriggerProcessingClass
//* .File Name       : $Workfile:   SVTriggerProcessingClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   23 Jul 2013 18:06:38  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <set>
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

class SVTriggerClass;
class SVIOTriggerLoadLibraryClass;

class SVTriggerProcessingClass
{
public:
	static SVTriggerProcessingClass& Instance();

	virtual ~SVTriggerProcessingClass();

	void clear();

	HRESULT UpdateTriggerSubsystem( SVIOTriggerLoadLibraryClass* p_pDLLTrigger );

	SVTriggerClass* GetTrigger( LPCTSTR p_szName ) const;

	// These two (2) methods, Startup, Shutdown are only meant to be called by the main application class and no other
	// They used to be protected and a friend class declaration was used, but that was a bad design as the friend was declares in another project
	// So for now the restriction is made manually, just don't call these methods anywhere else, and described via this comment
	void Startup();		// This method is only meant to be called by the main application class
	void Shutdown();	// This method is only meant to be called by the main application certain class
	
private:
	typedef std::set< SVIOTriggerLoadLibraryClass* > SVTriggerSubsystemSet;
	typedef std::map< SVString, SVTriggerClass* > SVNameTriggerMap;
	typedef std::map< SVString, SVIOTriggerLoadLibraryClass* > SVNameTriggerSubsystemMap;

	SVTriggerProcessingClass();

	HRESULT AddTrigger( LPCTSTR p_szName, SVIOTriggerLoadLibraryClass* p_pTriggerSubsystem, unsigned long p_Handle );

	//Do not implement this method. This definition prevent copy and assignement operations.
	SVTriggerProcessingClass( const SVTriggerProcessingClass* p_rObject );

	//Do not implement this method. This definition prevent copy and assignement operations.
	const SVTriggerProcessingClass& operator=( const SVTriggerProcessingClass* p_rObject );

	SVTriggerSubsystemSet m_Subsystems;

	// these are specific to each board
	SVNameTriggerMap m_Triggers;

	SVNameTriggerSubsystemMap m_TriggerSubsystems;
};
