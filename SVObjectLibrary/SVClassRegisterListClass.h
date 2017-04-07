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

#pragma region Includes
#include "SVContainerLibrary/SVMap.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVClassRegisterClass.h"
#pragma endregion Includes

class SVObjectClass;

class SVClassRegisterListClass
{
public:
	static SVClassRegisterListClass& Instance();

	virtual ~SVClassRegisterListClass();

	SVObjectClass* ConstructNewObject( const SVGUID& rUniqueClassID );

	void Add( SVClassRegisterClass* pClass);

	// This method, Shutdown, is only meant to be called by the main application class and no other
	// It used to be protected and a friend class declaration was used, but that was a bad design as the friend was declared in another project
	// So for now the restriction is made manually, just don't call this method anywhere else, as described via this comment
	void Shutdown();	// This method is only meant to be called by the main application class

protected:
	typedef SVMap< SVGUID, SVClassRegisterClass* > SVRegisterClasses;
	typedef std::map<SVGUID, SVGUID>  ExchangeClassMap;

	SVClassRegisterListClass();

	ExchangeClassMap m_ExchangeClassID;			//This map is used to exchange a class ID with another, can construct another object when read from a confguration
	SVRegisterClasses m_Registers;

private:
	// Do not Implement
	SVClassRegisterListClass( const SVClassRegisterListClass& rObject );

	// Do not Implement
	const SVClassRegisterListClass& operator=( const SVClassRegisterListClass& rObject );
};

