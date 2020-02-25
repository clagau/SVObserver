//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAnalyzer.cpp
//* .File Name       : $Workfile:   SVAnalyzer.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 10:09:10  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVAnalyzer.h"
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
#pragma endregion Includes

namespace SvAo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

///For this class it is not necessary to call SV_IMPLEMENT_CLASS as it is a base class and only derived classes are instantiated.
//SV_IMPLEMENT_CLASS( SVAnalyzerClass, SVAnalyzerClassGuid );

SVAnalyzerClass::SVAnalyzerClass( LPCSTR ObjectName )
: SVTaskObjectListClass( ObjectName ) 
{
	init();
}

SVAnalyzerClass::SVAnalyzerClass( SVObjectClass* POwner, int StringResourceID )
: SVTaskObjectListClass( POwner, StringResourceID ) 
{
	init();
}

// Initialization of newly Instantiated Object
void SVAnalyzerClass::init()
{
	// Indentify our type in the Output List
	m_outObjectInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVAnalyzerObjectType;

	// Set up the Defualt Inputs/Outputs
	addDefaultInputObjects();
}

SVAnalyzerClass::~SVAnalyzerClass()
{
}

bool SVAnalyzerClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	m_isCreated = SVTaskObjectClass::CreateObject(rCreateStructure) && nullptr != GetInspection() && nullptr != GetTool();

	return m_isCreated;
}

SvOi::IObjectClass* SVAnalyzerClass::GetResultObject()
{
	SvDef::SVObjectTypeInfoStruct TypeInfo{SvPb::SVResultObjectType};
	return getFirstObject(TypeInfo);
}

bool SVAnalyzerClass::createAllObjectsFromChild( SVObjectClass& rChildObject )
{
	SVObjectLevelCreateStruct createStruct;
	createStruct.OwnerObjectInfo.SetObject(this);
	createStruct.m_pInspection = GetInspection();
	createStruct.m_pTool = GetTool();
	createStruct.m_pAnalyzer = this;
	
	return rChildObject.createAllObjects(createStruct);
}

void SVAnalyzerClass::connectChildObject( SVTaskObjectClass& rChildObject )
{
	SVObjectLevelCreateStruct createStruct;
	createStruct.OwnerObjectInfo.SetObject(this);
	createStruct.m_pInspection = GetInspection();
	createStruct.m_pTool = GetTool();
	createStruct.m_pAnalyzer = this;

	rChildObject.ConnectObject(createStruct);
}

} //namespace SvAo
