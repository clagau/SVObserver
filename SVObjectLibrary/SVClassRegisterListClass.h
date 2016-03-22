//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVClassRegisterListClass
//* .File Name       : $Workfile:   SVClassRegisterListClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:35:36  $
//******************************************************************************

#pragma once

#include "SVContainerLibrary/SVMap.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVClassRegisterClass.h"

class SVObjectClass;

class SVClassRegisterListClass
{
public:
	friend class SVObserverApp;

	static SVClassRegisterListClass& Instance();

	virtual ~SVClassRegisterListClass();

	SVObjectClass* ConstructNewObject( const SVGUID& RUniqueClassID );

	void Add( SVClassRegisterClass* pClass);

protected:
	typedef SVMap< SVGUID, SVClassRegisterClass* > SVRegisterClasses;

	SVClassRegisterListClass();

	void Shutdown();

	SVRegisterClasses m_Registers;

private:
	// Do not Implement
	SVClassRegisterListClass( const SVClassRegisterListClass& p_rObject );

	// Do not Implement
	const SVClassRegisterListClass& operator=( const SVClassRegisterListClass& p_rObject );

};

