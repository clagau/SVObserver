//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVClassInfoStruct.h
//* .File Name       : $Workfile:   SVClassInfoStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:53:34  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVContainerLibrary/SVVector.h"
#include "ObjectInterfaces/SVObjectTypeInfoStruct.h"
#pragma endregion Includes

class SVObjectClass;

class SVInterfaceListClass : public SVVector< SVObjectTypeInfoStruct >
{
public:
	SVInterfaceListClass();
	SVInterfaceListClass( const SVInterfaceListClass& RIn );
	virtual ~SVInterfaceListClass();
	SVInterfaceListClass operator=( const SVInterfaceListClass& RIn );
};

struct SVClassInfoStruct
{
	SVClassInfoStruct();
	SVClassInfoStruct( const SVClassInfoStruct& RClassInfo );
	virtual ~SVClassInfoStruct();
	SVClassInfoStruct operator=( const SVClassInfoStruct& RClassInfo );

	// Construct Object and set given ClassName...
	SVObjectClass* Construct();

	SVObjectTypeInfoStruct	ObjectTypeInfo;
	CString					ClassName;
	GUID					ClassId;

	// Must be in the same order, like the class defines its input interface!!!
	SVInterfaceListClass	DesiredInputInterface;
};

class SVClassInfoStructListClass : public SVVector< SVClassInfoStruct >
{
public:
	SVClassInfoStructListClass();
	virtual ~SVClassInfoStructListClass();

#pragma region public methods
public:
	virtual int Find( const GUID& ClassID ); 

#pragma endregion public methods
};

