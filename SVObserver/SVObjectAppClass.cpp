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
HRESULT SVObjectAppClass::ConnectObject(SVObjectLevelCreateStruct* PCreateStruct)
{
	HRESULT l_Status = S_OK;

	if( nullptr != PCreateStruct && PCreateStruct->OwnerObjectInfo.PObject != this && PCreateStruct->OwnerObjectInfo.UniqueObjectID != GetUniqueObjectID() )
	{
		UpdateConnections( PCreateStruct );
	
		l_Status = SVObjectClass::ConnectObject( PCreateStruct );
	}
	
	return l_Status;	
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
DWORD_PTR SVObjectAppClass::CreateChildObject(SvOi::IObjectClass& rChildObject, DWORD context)
{
	SVObjectClass* pObject = dynamic_cast<SVObjectClass*>(&rChildObject);
	return CreateChildObject(pObject, context);
}
#pragma endregion virtual methods (IObjectAppClass)

DWORD_PTR SVObjectAppClass::processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = SVMR_NOT_PROCESSED;

	// Try to process message by yourself...
	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;
	switch( dwPureMessageID )
	{
		case SVMSGID_CREATE_CHILD_OBJECT:
		{
			SVObjectClass* pChildObject = ( SVObjectClass* ) DwMessageValue;
			return CreateChildObject(pChildObject, static_cast<DWORD>(DwMessageContext));
		}

		case SVMSGID_CONNECT_CHILD_OBJECT:
		{
			// ...use second message parameter ( DwMessageValue ) as SVObjectClass* of the child object
			SVObjectClass* pChildObject = reinterpret_cast<SVObjectClass*>(DwMessageValue);

			SVAnalyzerLevelCreateStruct createStruct;

			createStruct.OwnerObjectInfo = this;
			createStruct.AnalyzerObjectInfo = GetAnalyzer();
			createStruct.ToolObjectInfo	= GetTool();
			createStruct.InspectionObjectInfo	= GetInspection();

			return SVSendMessage( pChildObject, SVM_CONNECT_ALL_OBJECTS, reinterpret_cast<DWORD_PTR>(&createStruct), 0 );
		}
	}

	return( SVObjectClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );
}

void SVObjectAppClass::UpdateConnections( SVObjectLevelCreateStruct* PCreateStruct )
{
	m_psvAnalyzer = dynamic_cast<SVAnalyzerClass *>(this);

	if( nullptr == m_psvAnalyzer )
	{
		SVAnalyzerLevelCreateStruct *l_psvTemp = dynamic_cast<SVAnalyzerLevelCreateStruct *>(PCreateStruct);

		if( nullptr != l_psvTemp )
		{
			m_psvAnalyzer = dynamic_cast<SVAnalyzerClass *>(l_psvTemp->AnalyzerObjectInfo.PObject);
		}
	}
	
	m_psvTool = dynamic_cast<SVToolClass *>(this);

	if( nullptr == m_psvTool )
	{
		SVToolLevelCreateStruct *l_psvTemp = dynamic_cast<SVToolLevelCreateStruct *>(PCreateStruct);

		if( nullptr != l_psvTemp )
		{
			m_psvTool = dynamic_cast<SVToolClass *>(l_psvTemp->ToolObjectInfo.PObject);
		}
	}
	
	m_psvInspection = dynamic_cast<SVInspectionProcess*> (this);

	if( nullptr == m_psvInspection )
	{
		SVInspectionLevelCreateStruct *l_psvTemp = dynamic_cast<SVInspectionLevelCreateStruct *>(PCreateStruct);

		if( nullptr != l_psvTemp )
		{
			m_psvInspection = dynamic_cast<SVInspectionProcess*> (l_psvTemp->InspectionObjectInfo.PObject);
		}
	}
}

DWORD_PTR SVObjectAppClass::createAllObjectsFromChild( SVObjectClass* pChildObject )
{
	SVAnalyzerLevelCreateStruct createStruct;
	createStruct.OwnerObjectInfo = this;
	createStruct.AnalyzerObjectInfo = GetAnalyzer();
	createStruct.ToolObjectInfo	= GetTool();
	createStruct.InspectionObjectInfo	= GetInspection();

	return SVSendMessage( pChildObject, SVM_CREATE_ALL_OBJECTS, reinterpret_cast<DWORD_PTR>(&createStruct), 0 );
}

DWORD_PTR SVObjectAppClass::CreateChildObject( SVObjectClass* pChildObject, DWORD context )
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

		DWORD_PTR l_Return = createAllObjectsFromChild( pChildObject );

		if( ( context & SVMFResetObject ) == SVMFResetObject )
		{
			::SVSendMessage( pChildObject, SVM_RESET_ALL_OBJECTS, 0, 0 );
		}

		if( ( context & SVMFSetDefaultInputs ) == SVMFSetDefaultInputs )
		{
			GetInspection()->SetDefaultInputs();
		}

		if( ( context & SVMFResetInspection ) == SVMFResetInspection )
		{
			::SVSendMessage( GetInspection(), SVM_RESET_ALL_OBJECTS, 0, 0 );
		}

		return l_Return;
	}

	return SVMR_NOT_PROCESSED;
}

