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
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"
#include "SVObjectLibrary/SVClsids.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "ObjectInterfaces/IInspectionProcess.h"
#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS( SVObjectAppClass, SVObjectAppClassGuid )

SVObjectAppClass::SVObjectAppClass(LPCSTR ObjectName)
: SVObjectClass(ObjectName) 
{
	m_psvInspection = nullptr;
	m_psvTool = nullptr;
	m_psvAnalyzer = nullptr;
}

SVObjectAppClass::SVObjectAppClass(SVObjectClass* pOwner, int StringResourceID)
: SVObjectClass( pOwner, StringResourceID ) 
{
	m_psvInspection = nullptr;
	m_psvTool = nullptr;
	m_psvAnalyzer = nullptr;
}

SVObjectAppClass::~SVObjectAppClass()
{
}

////////////////////////////////////////////////////////////////////////////////////////
// This method is here so that this class can obtain access to and hold pointers for the
// Inspection, Tool, and/or Analyzer (via UpdateConnections)
////////////////////////////////////////////////////////////////////////////////////////
bool SVObjectAppClass::CreateObject(SVObjectLevelCreateStruct* pCreateStructure)
{
	UpdateConnections( pCreateStructure );
	
	m_isCreated = SVObjectClass::CreateObject( pCreateStructure );
	return m_isCreated;
}

////////////////////////////////////////////////////////////////////////////////////////
// This method is here so that this class can obtain access to and hold pointers for the
// Inspection, Tool, and/or Analyzer (via UpdateConnections)
////////////////////////////////////////////////////////////////////////////////////////
void SVObjectAppClass::ConnectObject(const SVObjectLevelCreateStruct& rCreateStruct)
{
	if( rCreateStruct.OwnerObjectInfo.m_pObject != this && rCreateStruct.OwnerObjectInfo.m_UniqueObjectID != GetUniqueObjectID() )
	{
		UpdateConnections( &rCreateStruct );

		SVObjectClass::ConnectObject( rCreateStruct );
	}
}


SVObjectClass* SVObjectAppClass::GetInspection() const
{
	return m_psvInspection;
}


SvOi::IInspectionProcess* SVObjectAppClass::GetInspectionInterface() const
{
	return dynamic_cast<SvOi::IInspectionProcess*>(m_psvInspection);
}


SVObjectClass* SVObjectAppClass::GetTool() const
{
	return m_psvTool;
}

SVObjectClass* SVObjectAppClass::GetAnalyzer() const
{
	return m_psvAnalyzer;
}


#pragma region virtual methods (IObjectAppClass)
bool SVObjectAppClass::CreateChildObject(SvOi::IObjectClass& rChildObject, DWORD context)
{
	SVObjectClass* pObject = dynamic_cast<SVObjectClass*>(&rChildObject);
	return CreateChildObject(pObject, context);
}
#pragma endregion virtual methods (IObjectAppClass)

bool SVObjectAppClass::CreateChildObject( SVObjectClass* pChildObject, DWORD context )
{
	if( IsCreated() && nullptr != pChildObject )
	{
		long l_LastIndex = 0;
		SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess*> (GetInspection());

		if( nullptr != pInspection )
		{
			l_LastIndex = pInspection->GetLastIndex();
		}

		// Set first object depth...
		pChildObject->SetObjectDepthWithIndex( m_objectDepth, l_LastIndex );
		pChildObject->SetImageDepth( m_lImageDepth );

		bool Result = createAllObjectsFromChild( *pChildObject );

		if( SvOi::SVMFResetObject == ( context & SvOi::SVMFResetObject ) )
		{
			pChildObject->resetAllObjects();
		}

		if( SvOi::SVMFSetDefaultInputs == ( context & SvOi::SVMFSetDefaultInputs ) )
		{
			pInspection->SetDefaultInputs();
		}

		if( SvOi::SVMFResetInspection == ( context & SvOi::SVMFResetInspection ) )
		{
			GetInspection()->resetAllObjects();
		}

		return Result;
	}

	return false;
}

void SVObjectAppClass::UpdateConnections( const SVObjectLevelCreateStruct* pCreateStruct )
{
	const SVObjectInfoStruct& rInfo = GetObjectInfo();

	if( SVAnalyzerObjectType == rInfo.m_ObjectTypeInfo.ObjectType )
	{
		m_psvAnalyzer = this;
	}
	else
	{
		const SVAnalyzerLevelCreateStruct* l_psvTemp = dynamic_cast<const SVAnalyzerLevelCreateStruct *>(pCreateStruct);

		if( nullptr != l_psvTemp )
		{
			m_psvAnalyzer = l_psvTemp->AnalyzerObjectInfo.m_pObject;
		}
	}

	if( SVToolObjectType == rInfo.m_ObjectTypeInfo.ObjectType )
	{
		m_psvTool = this;
	}
	else
	{
		const SVToolLevelCreateStruct* l_psvTemp = dynamic_cast<const SVToolLevelCreateStruct *>(pCreateStruct);

		if( nullptr != l_psvTemp )
		{
			m_psvTool = l_psvTemp->ToolObjectInfo.m_pObject;
		}
	}

	if( SVInspectionObjectType == rInfo.m_ObjectTypeInfo.ObjectType )
	{
		m_psvInspection = this;
	}
	else
	{
		const SVInspectionLevelCreateStruct* l_psvTemp = dynamic_cast<const SVInspectionLevelCreateStruct *>(pCreateStruct);

		if( nullptr != l_psvTemp )
		{
			m_psvInspection = l_psvTemp->InspectionObjectInfo.m_pObject;
		}
	}
}

bool SVObjectAppClass::createAllObjectsFromChild( SVObjectClass& rChildObject )
{
	SVAnalyzerLevelCreateStruct createStruct;
	createStruct.OwnerObjectInfo = this;
	createStruct.AnalyzerObjectInfo = GetAnalyzer();
	createStruct.ToolObjectInfo	= GetTool();
	createStruct.InspectionObjectInfo	= GetInspection();

	return rChildObject.createAllObjects(createStruct);
}


