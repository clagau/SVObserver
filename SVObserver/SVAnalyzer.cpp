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
#include "SVInspectionProcess.h"
#include "SVTool.h"
#include "SVResult.h"
#include "SVImageLibrary/SVImageInfoClass.h"
#include "SVImageClass.h"
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"
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

SVAnalyzerClass::SVAnalyzerClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
: SVTaskObjectListClass( BCreateDefaultTaskList, POwner, StringResourceID ) 
{
	init();
}

// Initialization of newly Instantiated Object
void SVAnalyzerClass::init()
{
	pAnalyzerResult	= nullptr;

	// Indentify our type in the Output List
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;

	// Set up the Defualt Inputs/Outputs
	addDefaultInputObjects();
}

SVAnalyzerClass::~SVAnalyzerClass()
{
}

BOOL SVAnalyzerClass::CloseObject()
{
	return SVTaskObjectListClass::CloseObject();
}

BOOL SVAnalyzerClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = false;

	if( SVTaskObjectClass::CreateObject( PCreateStructure ) )
	{
		if( GetInspection() && GetTool() )
		{
			bOk = true;
		}
	}
	m_isCreated = bOk;

	return bOk;
}

SVResultClass* SVAnalyzerClass::GetResultObject()
{
	SVObjectTypeInfoStruct info;
	info.ObjectType = SVResultObjectType;
	SVResultClass* pResult = dynamic_cast<SVResultClass*>( getFirstObject( info ) );
	return pResult;
}

bool SVAnalyzerClass::createAllObjectsFromChild( SVObjectClass& rChildObject )
{
	SVAnalyzerLevelCreateStruct createStruct;
	createStruct.OwnerObjectInfo        = this;
	createStruct.AnalyzerObjectInfo		= this;
	createStruct.ToolObjectInfo			= GetTool();
	createStruct.InspectionObjectInfo	= GetInspection();
	
	return rChildObject.createAllObjects(createStruct);
}

void SVAnalyzerClass::connectChildObject( SVTaskObjectClass& rChildObject )
{
	SVAnalyzerLevelCreateStruct createStruct;
	createStruct.OwnerObjectInfo        = this;
	createStruct.AnalyzerObjectInfo		= this;
	createStruct.ToolObjectInfo			= GetTool();
	createStruct.InspectionObjectInfo	= GetInspection();

	rChildObject.ConnectObject(createStruct);
}
