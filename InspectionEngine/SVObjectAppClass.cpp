//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectAppClass
//* .File Name       : $Workfile:   SVObjectAppClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   15 May 2014 13:45:42  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVObjectAppClass.h"
#include "Definitions/ObjectDefines.h"
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
#include "SVObjectLibrary/SVClsids.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "ObjectInterfaces/IInspectionProcess.h"
#pragma endregion Includes

namespace SvIe
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( SVObjectAppClass, SVObjectAppClassGuid )

SVObjectAppClass::SVObjectAppClass(LPCSTR ObjectName) : SVObjectClass(ObjectName) 
, m_pInspection(nullptr)
, m_pTool(nullptr)
, m_pAnalyzer(nullptr)
, m_pInspectionInterface(nullptr)
, m_pToolInterface(nullptr)
{
}

SVObjectAppClass::SVObjectAppClass(SVObjectClass* pOwner, int StringResourceID) : SVObjectClass( pOwner, StringResourceID )
, m_pInspection(nullptr)
, m_pTool(nullptr)
, m_pAnalyzer(nullptr)
, m_pInspectionInterface(nullptr)
, m_pToolInterface(nullptr)
{
}

SVObjectAppClass::~SVObjectAppClass()
{
}

bool SVObjectAppClass::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	UpdateConnections(rCreateStructure);
	
	m_isCreated = SVObjectClass::CreateObject(rCreateStructure);
	
	return m_isCreated;
}

////////////////////////////////////////////////////////////////////////////////////////
// This method is here so that this class can obtain access to and hold pointers for the
// Inspection, Tool, and/or Analyzer (via UpdateConnections)
////////////////////////////////////////////////////////////////////////////////////////
void SVObjectAppClass::ConnectObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	if( rCreateStructure.OwnerObjectInfo.getObject() != this && rCreateStructure.OwnerObjectInfo.getUniqueObjectID() != GetUniqueObjectID() )
	{
		UpdateConnections( rCreateStructure );

		SVObjectClass::ConnectObject( rCreateStructure );
	}
}

#pragma region virtual methods (IObjectAppClass)
bool SVObjectAppClass::CreateChildObject(SvOi::IObjectClass& rChildObject, DWORD context)
{
	SVObjectClass* pObject = dynamic_cast<SVObjectClass*> (&rChildObject);
	return CreateChildObject(pObject, context);
}
#pragma endregion virtual methods (IObjectAppClass)

bool SVObjectAppClass::CreateChildObject( SVObjectClass* pChildObject, DWORD context )
{
	if( IsCreated() && nullptr != pChildObject )
	{
		bool Result = createAllObjectsFromChild( *pChildObject );

		if( SvDef::SVMFResetObject == ( context & SvDef::SVMFResetObject ) )
		{
			pChildObject->resetAllObjects();
		}

		if( SvDef::SVMFSetDefaultInputs == ( context & SvDef::SVMFSetDefaultInputs ) )
		{
			m_pInspectionInterface->SetDefaultInputs();
		}

		if( SvDef::SVMFResetInspection == ( context & SvDef::SVMFResetInspection ) )
		{
			m_pInspection->resetAllObjects();
		}

		return Result;
	}

	return false;
}

void SVObjectAppClass::UpdateConnections(const SVObjectLevelCreateStruct& rCreateStructure)
{
	const SVObjectInfoStruct& rInfo = GetObjectInfo();

	if(SvPb::SVAnalyzerObjectType == rInfo.m_ObjectTypeInfo.ObjectType )
	{
		m_pAnalyzer = this;
	}
	else
	{
		m_pAnalyzer = rCreateStructure.m_pAnalyzer;
	}

	if(SvPb::SVToolObjectType == rInfo.m_ObjectTypeInfo.ObjectType )
	{
		m_pTool = this;
		m_pToolInterface = dynamic_cast<SvOi::ITool*> (m_pTool);
	}
	else
	{
		m_pTool = rCreateStructure.m_pTool;
		m_pToolInterface = dynamic_cast<SvOi::ITool*> (m_pTool);
	}

	if(SvPb::SVInspectionObjectType == rInfo.m_ObjectTypeInfo.ObjectType )
	{
		m_pInspection = this;
		m_pInspectionInterface = dynamic_cast<SvOi::IInspectionProcess*> (m_pInspection);
	}
	else
	{
		m_pInspection = rCreateStructure.m_pInspection;
		m_pInspectionInterface = dynamic_cast<SvOi::IInspectionProcess*> (m_pInspection);
	}
}

bool SVObjectAppClass::createAllObjectsFromChild( SVObjectClass& rChildObject )
{
	SVObjectLevelCreateStruct createStruct;
	createStruct.OwnerObjectInfo.SetObject(this);
	createStruct.m_pInspection = GetInspection();
	createStruct.m_pTool = GetTool();
	createStruct.m_pAnalyzer = GetAnalyzer();

	return rChildObject.createAllObjects(createStruct);
}

} //namespace SvIe
