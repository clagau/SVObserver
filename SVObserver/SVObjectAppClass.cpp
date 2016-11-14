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

#include "stdafx.h"
#include "SVObjectAppClass.h"
#include "SVAnalyzer.h"
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"
#include "SVInspectionProcess.h"
#include "SVTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS( SVObjectAppClass, SVObjectAppClassGuid )

SVObjectAppClass::SVObjectAppClass(LPCSTR ObjectName)
				  :SVObjectClass(ObjectName) 
{
	m_psvInspection = nullptr;
	m_psvTool = nullptr;
	m_psvAnalyzer = nullptr;
}

SVObjectAppClass::SVObjectAppClass(SVObjectClass* POwner, int StringResourceID)
: SVObjectClass( POwner, StringResourceID ) 
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
BOOL SVObjectAppClass::CreateObject(SVObjectLevelCreateStruct* PCreateStruct)
{
	UpdateConnections( PCreateStruct );
	
	BOOL l_bOk = SVObjectClass::CreateObject( PCreateStruct );
	
	m_isCreated = l_bOk;
	
	return l_bOk;	
}

////////////////////////////////////////////////////////////////////////////////////////
// This method is here so that this class can obtain access to and hold pointers for the
// Inspection, Tool, and/or Analyzer (via UpdateConnections)
////////////////////////////////////////////////////////////////////////////////////////
void SVObjectAppClass::ConnectObject(const SVObjectLevelCreateStruct& rCreateStruct)
{
	if( rCreateStruct.OwnerObjectInfo.PObject != this && rCreateStruct.OwnerObjectInfo.UniqueObjectID != GetUniqueObjectID() )
	{
		UpdateConnections( &rCreateStruct );
	
		SVObjectClass::ConnectObject( rCreateStruct );
	}
}

SVInspectionProcess* SVObjectAppClass::GetInspection() const
{
	return m_psvInspection;
}

SVToolClass *SVObjectAppClass::GetTool() const
{
	return m_psvTool;
}

SVAnalyzerClass *SVObjectAppClass::GetAnalyzer() const
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

bool SVObjectAppClass::CreateChildObject(SVObjectClass* pChildObject, DWORD context)
{
	if( IsCreated() && nullptr != pChildObject )
	{
		long l_LastIndex = 1;
		SVInspectionProcess* pInspection = GetInspection();

		if( nullptr != pInspection )
		{
			SVProductInfoStruct l_Product = pInspection->LastProductGet( SV_INSPECTION );

			if( !( l_Product.empty() ) )
			{
				SVDataManagerHandle l_Handle;
				l_Product.GetResultDataIndex( l_Handle );
				l_LastIndex = l_Handle.GetIndex();
			}
		}

		// Set first object depth...
		pChildObject->SetObjectDepthWithIndex( m_objectDepth, l_LastIndex );
		pChildObject->SetImageDepth( m_lImageDepth );

		bool l_Return = createAllObjectsFromChild( *pChildObject );

		if( ( context & SVMFResetObject ) == SVMFResetObject )
		{
			pChildObject->resetAllObjects(true, false);
		}

		if( ( context & SVMFSetDefaultInputs ) == SVMFSetDefaultInputs )
		{
			GetInspection()->SetDefaultInputs();
		}

		if( ( context & SVMFResetInspection ) == SVMFResetInspection )
		{
			GetInspection()->resetAllObjects(true, false);
		}

		return l_Return;
	}

	return false;
}

void SVObjectAppClass::UpdateConnections( const SVObjectLevelCreateStruct* PCreateStruct )
{
	m_psvAnalyzer = dynamic_cast<SVAnalyzerClass *>(this);

	if( nullptr == m_psvAnalyzer )
	{
		const SVAnalyzerLevelCreateStruct *l_psvTemp = dynamic_cast<const SVAnalyzerLevelCreateStruct *>(PCreateStruct);

		if( nullptr != l_psvTemp )
		{
			m_psvAnalyzer = dynamic_cast<SVAnalyzerClass *>(l_psvTemp->AnalyzerObjectInfo.PObject);
		}
	}
	
	m_psvTool = dynamic_cast<SVToolClass *>(this);

	if( nullptr == m_psvTool )
	{
		const SVToolLevelCreateStruct *l_psvTemp = dynamic_cast<const SVToolLevelCreateStruct *>(PCreateStruct);

		if( nullptr != l_psvTemp )
		{
			m_psvTool = dynamic_cast<SVToolClass *>(l_psvTemp->ToolObjectInfo.PObject);
		}
	}
	
	m_psvInspection = dynamic_cast<SVInspectionProcess*> (this);

	if( nullptr == m_psvInspection )
	{
		const SVInspectionLevelCreateStruct *l_psvTemp = dynamic_cast<const SVInspectionLevelCreateStruct *>(PCreateStruct);

		if( nullptr != l_psvTemp )
		{
			m_psvInspection = dynamic_cast<SVInspectionProcess*> (l_psvTemp->InspectionObjectInfo.PObject);
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

