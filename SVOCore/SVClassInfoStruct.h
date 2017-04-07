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
#include "ObjectInterfaces/SVInterfaceVector.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVUtilityLibrary/SVGUID.h"
#pragma endregion Includes

class SVObjectClass;

struct SVClassInfoStruct
{
	SVClassInfoStruct();
	SVClassInfoStruct( const SVClassInfoStruct& rRhs );
	virtual ~SVClassInfoStruct();
	SVClassInfoStruct operator=( const SVClassInfoStruct& rRhs );

	// Construct Object and set given ClassName...
	SVObjectClass* Construct();

	SVObjectTypeInfoStruct m_ObjectTypeInfo;
	SVString m_ClassName;
	SVGUID m_ClassId;

	// Must be in the same order, like the class defines its input interface!!!
	SvOi::SVInterfaceVector m_DesiredInputInterface;
};

class SVClassInfoStructListClass : public SVVector< SVClassInfoStruct >
{
public:
	SVClassInfoStructListClass();
	virtual ~SVClassInfoStructListClass();

#pragma region public methods
public:
	int Find( const SVGUID& rGuid );

#pragma endregion public methods
};

