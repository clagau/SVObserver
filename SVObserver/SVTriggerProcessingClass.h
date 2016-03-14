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

#ifndef SVTRIGGERPROCESSINGCLASS_H
#define SVTRIGGERPROCESSINGCLASS_H

#include <map>
#include <set>
#include "SVUtilityLibrary/SVString.h"

class SVTriggerClass;
class SVIOTriggerLoadLibraryClass;

class SVTriggerProcessingClass
{
public:
	friend class SVObserverApp;

	static SVTriggerProcessingClass& Instance();

	virtual ~SVTriggerProcessingClass();

	void clear();

	HRESULT UpdateTriggerSubsystem( SVIOTriggerLoadLibraryClass* p_pDLLTrigger );

	SVTriggerClass* GetTrigger( LPCTSTR p_szName ) const;

protected:
	SVTriggerProcessingClass();

	void Startup();
	void Shutdown();

private:
	typedef std::set< SVIOTriggerLoadLibraryClass* > SVTriggerSubsystemSet;
	typedef std::map< SVString, SVTriggerClass* > SVNameTriggerMap;
	typedef std::map< SVString, SVIOTriggerLoadLibraryClass* > SVNameTriggerSubsystemMap;

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

#endif

