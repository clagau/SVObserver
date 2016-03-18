//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRange.cpp
//* .File Name       : $Workfile:   SVRange.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.8  $
//* .Check In Date   : $Date:   17 Feb 2015 18:14:06  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVRange.h"
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVTool.h"
#include "SVInspectionProcess.h"
#include "RangeClassHelper.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "BasicValueObject.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

struct SVRangeClassCancelData : public SVCancelData	// this does not need to be visible to anyone but this file.
{
	SVDoubleValueObjectClass FailLow;
	SVDoubleValueObjectClass FailHigh;
	SVDoubleValueObjectClass WarnLow;
	SVDoubleValueObjectClass WarnHigh;
};

enum defaults
{
	lowDef = 0, // Warn/Fail Low Default
	highDef = 99999 // Warn/Fail High Default
};

const TCHAR* const ToolSetName = _T( "Tool Set" );

#pragma endregion Declarations

//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVRangeClass
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

SV_IMPLEMENT_CLASS( SVRangeClass, SVRangeClassGuid );

#pragma region Constructor
SVRangeClass::SVRangeClass( SVObjectClass* POwner, int StringResourceID )
	: SVTaskObjectClass( POwner, StringResourceID )
{
	init();
}

SVRangeClass::~SVRangeClass()
{
}
#pragma endregion Constructor

void SVRangeClass::init()
{
	m_bUseOverlays = false;

	// Identify our type in the Output List
	outObjectInfo.ObjectTypeInfo.ObjectType = SVRangeObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &FailHigh, SVRangeClassFailHighObjectGuid, IDS_OBJECTNAME_FAIL_HIGH, false, SVResetItemNone );
	RegisterEmbeddedObject( &WarnHigh, SVRangeClassWarnHighObjectGuid, IDS_OBJECTNAME_WARN_HIGH, false, SVResetItemNone );
	RegisterEmbeddedObject( &FailLow, SVRangeClassFailLowObjectGuid, IDS_OBJECTNAME_FAIL_LOW, false, SVResetItemNone );
	RegisterEmbeddedObject( &WarnLow, SVRangeClassWarnLowObjectGuid, IDS_OBJECTNAME_WARN_LOW, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_ValueIndirect[RangeEnum::ER_FailHigh], SVRangeClassFailHighIndirectObjectGuid, IDS_OBJECTNAME_FAIL_HIGH_INDIRECT, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_ValueIndirect[RangeEnum::ER_WarnHigh], SVRangeClassWarnHighIndirectObjectGuid, IDS_OBJECTNAME_WARN_HIGH_INDIRECT, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_ValueIndirect[RangeEnum::ER_FailLow], SVRangeClassFailLowIndirectObjectGuid, IDS_OBJECTNAME_FAIL_LOW_INDIRECT, false, SVResetItemOwner);
	RegisterEmbeddedObject( &m_ValueIndirect[RangeEnum::ER_WarnLow], SVRangeClassWarnLowIndirectObjectGuid, IDS_OBJECTNAME_WARN_LOW_INDIRECT, false, SVResetItemOwner);

	// Set Embedded defaults
	FailLow.SetDefaultValue( lowDef, TRUE );
	FailHigh.SetDefaultValue( highDef, TRUE );
	WarnLow.SetDefaultValue( lowDef, TRUE );
	WarnHigh.SetDefaultValue( highDef, TRUE );

	for(int i = 0; i < RangeEnum::ER_COUNT; i++)
	{
		m_ValueIndirect[i].SetDefaultValue( _T(""), TRUE );
	}

	m_isValidRange = true;

	isObjectValid.SetDefaultValue(true, TRUE);

	// Setup up the input
	m_inputObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputObjectInfo, _T( "RangeValue" ) );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

BOOL SVRangeClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	isCreated = SVTaskObjectClass::CreateObject( PCreateStructure );

	// Set / Reset Printable Flags
	FailHigh.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;
	WarnHigh.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;
	FailLow.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;
	WarnLow.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;

	for(int i = 0; i < RangeEnum::ER_COUNT; i++)
	{
		m_ValueIndirect[i].ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE;
	}

	return isCreated;
}

bool SVRangeClass::SetReference(LPCTSTR Name, SVObjectReference &ObjectReference)
{
	bool ret = false;

	if(nullptr != Name)
	{
		HRESULT hr = SVObjectManagerClass::Instance().GetObjectByDottedName( Name, ObjectReference );
		if(hr == S_OK)
		{
			ret = true;
		}
	}
	return ret;
}

HRESULT SVRangeClass::ResetObject()
{
	HRESULT hresult = S_OK;
	hresult = InitReferencesAndInputs();
	if(S_OK != hresult)
	{
		SetInvalid();
	}

	HRESULT hres = SVTaskObjectClass::ResetObject();

	if (hresult == S_OK)
	{
		hresult = hres;
	}

	return hresult;
}

BOOL SVRangeClass::CloseObject()
{
	DisconnectAllInputObjects();
	return SVTaskObjectClass::CloseObject();
}

HRESULT SVRangeClass::InitReferencesAndInputs()
{
	HRESULT hResult = S_OK;
	DisconnectAllInputObjects();
	SVValueObjectReference emptyRef;
	CString csValueIndirect;
	CString InspectionName;

	if(GetInspection())
	{
		InspectionName = GetInspection()->GetName();
	}
	InspectionName += _T(".");

	for(int i = 0; i < RangeEnum::ER_COUNT; i++)
	{
		csValueIndirect.Empty();
		m_ValueObjectReferences[i] = emptyRef;
		m_ValueIndirect[i].ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
		m_ValueIndirect[i].GetValue(csValueIndirect);
		if(!csValueIndirect.IsEmpty())
		{
			CString dottedName;
			//If the tool set name is at the start then add the inspection name at the beginning
			if( 0 == csValueIndirect.Find(ToolSetName) )
			{
				dottedName = InspectionName + csValueIndirect;
			}
			else
			{
				dottedName = csValueIndirect;
			}
			if(!SetReference(dottedName,m_ValueObjectReferences[i] ))
			{
				hResult = -SvOi::Err_16025; //invalid Reference;
			}
			//check if we have an valid but disabled input
			else if( FALSE == (m_ValueObjectReferences[i].ObjectAttributesAllowed() & SV_SELECTABLE_FOR_EQUATION ) )
			{
				hResult = -SvOi::Err_16026; //Reference not allowed
			}
			else
			{
				m_ValueIndirect[i].ObjectAttributesAllowedRef() |= SV_PRINTABLE;
			}
		}
	}

	if( S_OK != hResult )
	{
		m_isValidRange = false;
		///SetInvalid();
	}
	else
	{
		m_isValidRange = true;
	}

	if( S_OK == hResult )
	{
		ConnectAllInputObjects();
	}

	return hResult;
}

BOOL SVRangeClass::OnValidate()
{
	BOOL bRetVal = SVTaskObjectClass::OnValidate();

	// check if input is valid
	if( bRetVal && m_inputObjectInfo.IsConnected() &&
		m_inputObjectInfo.GetInputObjectInfo().PObject )
	{
		bRetVal = TRUE;
	}

	if(bRetVal)
	{
		bRetVal = ( InitReferencesAndInputs() == S_OK);
	}

	if(!bRetVal)
	{
		m_isValidRange = false;
		SetInvalid();
	}

	return bRetVal;
}

SVDoubleValueObjectClass&  SVRangeClass::GetRange(RangeEnum::ERange range)
{
	switch (range)
	{
	case RangeEnum::ER_FailHigh:
		return FailHigh;
	case RangeEnum::ER_WarnHigh: 
		return WarnHigh;	
	case 	RangeEnum::ER_FailLow:
		return FailLow;	
	
	case 	RangeEnum::ER_WarnLow:
		return WarnLow;
	
	default:
		ASSERT (FALSE);
		return WarnLow;
	}

}

void   SVRangeClass::UpdateRange(int bucket, RangeEnum::ERange  range )
{

	double dres(0);
	if( SVValueObjectClass* pValueObject = dynamic_cast<SVValueObjectClass*> (m_ValueObjectReferences[range].Object()) )
	{

		if(m_ValueObjectReferences[range].IsIndexPresent())
		{
			int index = m_ValueObjectReferences[range].ArrayIndex();
			int LastSet =  pValueObject->GetLastSetIndex();
			pValueObject->GetValue(LastSet,index,dres);

		}
		else
		{
			pValueObject->GetValue( dres );
		}

		GetRange(range).SetValue( bucket,dres );	

	}
	else if( BasicValueObject* pBasicValueObject = dynamic_cast<BasicValueObject*> (m_ValueObjectReferences[range].Object()) )
	{
		pBasicValueObject->getValue( dres );
		GetRange(range).SetValue( bucket,dres );
	}
}




BOOL SVRangeClass::onRun(SVRunStatusClass& RRunStatus)
{
	BOOL ret = TRUE;
	if(!m_isValidRange)
	{
		RRunStatus.SetInvalid();
		ret = FALSE;
	}

	if(ret)
	{

		double InputValue,failHigh(0), failLow(0),warnLow(0),warnHigh(0);
		getInputValue(InputValue);


		if( nullptr != m_ValueObjectReferences[RangeEnum::ER_FailLow].Object() )
		{
			UpdateRange(RRunStatus.m_lResultDataIndex, RangeEnum::ER_FailLow );
		}
		FailLow.GetValue(failLow);


		if( nullptr != m_ValueObjectReferences[RangeEnum::ER_FailHigh ].Object() )
		{
			UpdateRange(RRunStatus.m_lResultDataIndex, RangeEnum::ER_FailHigh );
		}
		FailHigh.GetValue(failHigh);

		if( nullptr != m_ValueObjectReferences[RangeEnum::ER_WarnLow  ].Object() )
		{
			UpdateRange(RRunStatus.m_lResultDataIndex, RangeEnum::ER_WarnLow );
		}


		WarnLow.GetValue(warnLow);

		if( nullptr != m_ValueObjectReferences[ RangeEnum::ER_WarnHigh  ].Object() )
		{
			UpdateRange(RRunStatus.m_lResultDataIndex, RangeEnum::ER_WarnHigh );
		}

		WarnHigh.GetValue(warnHigh);




		bool isFailed = ( InputValue < failLow || InputValue > failHigh );
		bool isWarned = ( !isFailed && ( InputValue < warnLow || InputValue > warnHigh ) );


		if( isFailed )
		{
			RRunStatus.SetFailed();
		}

		if( isWarned )
		{
			RRunStatus.SetWarned();
		}

		if( !isFailed && !isWarned )
		{
			RRunStatus.SetPassed();
		}
	}

	return ret;
}

DWORD_PTR SVRangeClass::processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = NULL;

	// Try to process message by yourself...
	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;
	switch (dwPureMessageID)
	{
	case SVMSGID_RESET_ALL_OBJECTS:
		{
			HRESULT ResetStatus = ResetObject();
			if( ResetStatus != S_OK )
			{
				BOOL SilentReset = static_cast<BOOL> (DwMessageValue);
				if(!SilentReset && (ResetStatus == -SvOi::Err_16025 || ResetStatus == -SvOi::Err_16026))
				{
					CString ErrorMsg;
					ErrorMsg = GetCompleteObjectNameToObjectType( NULL, SVInspectionObjectType );
					ErrorMsg += _T(": Invalid reference");
					SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
					Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, ErrorMsg, StdMessageParams, SvOi::Err_10186 ); 
				}
				DwResult = SVMR_NO_SUCCESS;
			}
			else
			{
				DwResult = SVMR_SUCCESS;
			}
			break;
		}

		// This Message occurs for two scenarios
		// 1. Some Object is using our outputs and they are no longer needed.
		// 2. We are using some Object's outputs and the outputs are no longer available
	case SVMSGID_DISCONNECT_OBJECT_INPUT:
		{
			m_isValidRange = false;
			//return SVMR_SUCCESS;
		}
		break;

	case SVMSGID_OBJECT_RENAMED:
		{
			SVObjectClass* pObject = reinterpret_cast<SVObjectClass*>(DwMessageValue); // Object with new name
			LPCTSTR orgName = ( LPCTSTR )DwMessageContext;

			if( renameToolSetSymbol(pObject, orgName ) )
			{
				DwResult = SVMR_SUCCESS;
			}
		}
		break;
	}
	return( SVTaskObjectClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
BOOL SVRangeClass::renameToolSetSymbol(const SVObjectClass* pObject, LPCTSTR originalName)
{
	bool Result( false );

	if( nullptr != pObject )
	{
		SVString newPrefix;
		SVString oldPrefix;
		//In this case the inspection name is not part of the saved name so do not rename inspection names
		if( const BasicValueObject* pBasicValueObject = dynamic_cast<const BasicValueObject*> (pObject) )
		{
			newPrefix = pBasicValueObject->GetCompleteObjectNameToObjectType( NULL, SVRootObjectType );
		}
		else
		{
			newPrefix = pObject->GetCompleteObjectNameToObjectType( NULL, SVToolSetObjectType ) + _T( "." );
		}// end else
		oldPrefix = newPrefix;
		SvUl_SF::searchAndReplace( oldPrefix, pObject->GetName(), originalName );

		RangeClassHelper rangeHelper(this);
		rangeHelper.SetRangeTaskObject();
		rangeHelper.GetAllInspectionData();
		if(rangeHelper.RenameIndirectValues( oldPrefix.c_str(), newPrefix.c_str() ))
		{
			rangeHelper.SetInspectionData();
			Result = true;
		}
	}
	return Result;
}

// ISVCancel interface
bool SVRangeClass::CanCancel()
{
	return true;
}

HRESULT SVRangeClass::GetCancelData(SVCancelData*& rpCancelData)
{
	ASSERT(rpCancelData == NULL);
	SVRangeClassCancelData* pData = new SVRangeClassCancelData;
	rpCancelData = pData;

	pData->FailHigh = FailHigh;
	pData->FailLow = FailLow;
	pData->WarnHigh = WarnHigh;
	pData->WarnLow = WarnLow;

	return S_OK;
}

HRESULT SVRangeClass::SetCancelData(SVCancelData* pCancelData)
{
	SVRangeClassCancelData* pData = dynamic_cast<SVRangeClassCancelData*> (pCancelData);
	if ( pData )
	{
		FailHigh = pData->FailHigh;
		FailLow = pData->FailLow;
		WarnHigh = pData->WarnHigh;
		WarnLow = pData->WarnLow;

		return S_OK;
	}
	else
	{
		return S_FALSE;
	}
}

BOOL SVRangeClass::getInputValue( double& RVal )
{
	if( m_inputObjectInfo.IsConnected() && m_inputObjectInfo.GetInputObjectInfo().PObject )
	{
		return ( static_cast<SVValueObjectClass*>( m_inputObjectInfo.GetInputObjectInfo().PObject) )->GetValue( RVal ) == S_OK;
	}
	return FALSE;
}

void SVRangeClass::ConnectAllInputObjects()
{
	for(int i = 0; i < RangeEnum::ER_COUNT; i++)
	{
		m_IsConnectedInput[i] = false;

		if(nullptr != m_ValueObjectReferences[i].Object())
		{
			if( m_ValueObjectReferences[i].Guid() != SVInvalidGUID)
			{
				SVInObjectInfoStruct InObjectInfo;
				InObjectInfo.PObject                    = this;
				InObjectInfo.UniqueObjectID             = GetUniqueObjectID();
				InObjectInfo.ObjectTypeInfo.ObjectType  = SVRangeObjectType;
				DWORD_PTR rc = ::SVSendMessage( m_ValueObjectReferences[i].Guid(), 
					SVM_CONNECT_OBJECT_INPUT, 
					reinterpret_cast<DWORD_PTR>(&InObjectInfo), 
					NULL );

				m_IsConnectedInput[i] = ( rc == SVMR_SUCCESS );
			}
		}
	}
}

void SVRangeClass::DisconnectAllInputObjects()
{
	for(int i = 0; i < RangeEnum::ER_COUNT; i++)
	{
		if(nullptr != m_ValueObjectReferences[i].Object() && m_IsConnectedInput[i] == true)
		{
			if( m_ValueObjectReferences[i].Guid() != SVInvalidGUID )
			{
				SVInObjectInfoStruct InObjectInfo;

				InObjectInfo.PObject                    =this;
				InObjectInfo.UniqueObjectID             = GetUniqueObjectID();
				InObjectInfo.ObjectTypeInfo.ObjectType  = SVRangeObjectType;

				DWORD_PTR rc = ::SVSendMessage(	m_ValueObjectReferences[i].Guid(), 
					SVM_DISCONNECT_OBJECT_INPUT, 
					reinterpret_cast<DWORD_PTR>(&InObjectInfo), 
					NULL );
				m_IsConnectedInput[i] = (rc == SVMR_SUCCESS );
			}
		}
	}
}

HRESULT SVRangeClass::GetIndirectValue(RangeEnum::ERange ra, CString &ref)
{
	return m_ValueIndirect[ra].GetValue(ref);
};

bool SVRangeClass::HasIndirectValue(RangeEnum::ERange ra)
{
	bool res = false;
	CString cs;
	if(S_OK == GetIndirectValue(ra, cs))
	{
		res = !cs.IsEmpty();
	}

	return res;
};

HRESULT SVRangeClass::GetValue(RangeEnum::ERange ra, double &ref)
{
	switch (ra)
	{
	case RangeEnum::ER_FailHigh:
		return FailHigh.GetValue(ref);
		break;

	case RangeEnum::ER_WarnHigh:
		return WarnHigh.GetValue(ref);
		break;

	case RangeEnum::ER_FailLow:
		return FailLow.GetValue(ref);
		break;

	case RangeEnum::ER_WarnLow:
		return WarnLow.GetValue(ref);
		break;
	}
	return E_FAIL;
};

SVStringValueObjectClass* SVRangeClass::GetIndirectObject( RangeEnum::ERange ra )
{
	return &(m_ValueIndirect[ra]);
};

void SVRangeClass::InvalidateRange()
{
	m_isValidRange = false;
}





const SVDoubleValueObjectClass& SVRangeClass::getUpdatedFailLow( int bucket )
{
	if(m_isValidRange)
	{

		if( nullptr != m_ValueObjectReferences[  RangeEnum::ER_FailLow  ].Object() )
		{
			UpdateRange(bucket, RangeEnum::ER_FailLow );
		}


	}
	return FailLow;
}

const SVDoubleValueObjectClass& SVRangeClass::getUpdatedFailHigh( int bucket )
{

	if(m_isValidRange)
	{
		if( nullptr != m_ValueObjectReferences[  RangeEnum::ER_FailHigh ].Object() )
		{
			UpdateRange(bucket, RangeEnum::ER_FailHigh );
		}

	}


	return FailHigh;
}
