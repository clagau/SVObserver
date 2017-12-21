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
#include "SVObjectLibrary/SVClsIds.h"
#include "SVImageLibrary/SVImageInfoClass.h"
#include "SVImageClass.h"
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS( SVAnalyzerClass, SVAnalyzerClassGuid );

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
	pAnalyzerResult	= nullptr;

	// Indentify our type in the Output List
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvDef::SVAnalyzerObjectType;

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
	SvDef::SVObjectTypeInfoStruct info;
	info.ObjectType = SvDef::SVResultObjectType;
	SvOi::IObjectClass* pResult = getFirstObject( info );
	return pResult;
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
