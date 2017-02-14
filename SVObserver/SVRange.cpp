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
#include "SVValueObjectLibrary/BasicValueObject.h"
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
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVRangeObjectType;

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
	FailLow.SetDefaultValue( lowDef, true );
	FailHigh.SetDefaultValue( highDef, true );
	WarnLow.SetDefaultValue( lowDef, true );
	WarnHigh.SetDefaultValue( highDef, true );

	for(int i = 0; i < RangeEnum::ER_COUNT; i++)
	{
		m_ValueIndirect[i].SetDefaultValue( _T(""), true );
	}

	m_isValidRange = true;

	// Setup up the input
	m_inputObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputObjectInfo, _T( "RangeValue" ) );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

BOOL SVRangeClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	m_isCreated = SVTaskObjectClass::CreateObject( PCreateStructure );

	// Set / Reset Printable Flags
	FailHigh.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;
	WarnHigh.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;
	FailLow.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;
	WarnLow.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;

	for(int i = 0; i < RangeEnum::ER_COUNT; i++)
	{
		m_ValueIndirect[i].ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE;
	}

	return m_isCreated;
}

bool SVRangeClass::SetReference(LPCTSTR Name, SVObjectReference &ObjectReference)
{
	bool ret = false;

	if(nullptr != Name)
	{
		HRESULT hr = SVObjectManagerClass::Instance().GetObjectByDottedName( Name, ObjectReference );
		if( S_OK == hr )
		{
			ret = true;
		}
	}
	return ret;
}

bool SVRangeClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = InitReferencesAndInputs(pErrorMessages);

	// check if input is valid
	if( !m_inputObjectInfo.IsConnected() || nullptr == m_inputObjectInfo.GetInputObjectInfo().PObject )
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	if(!Result)
	{
		SetInvalid();
	}

	Result = SVTaskObjectClass::ResetObject(pErrorMessages) && Result;
	return Result;
}

BOOL SVRangeClass::CloseObject()
{
	DisconnectAllInputObjects();
	return SVTaskObjectClass::CloseObject();
}

bool SVRangeClass::InitReferencesAndInputs(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = true;
	DisconnectAllInputObjects();
	SVValueObjectReference emptyRef;
	SVString ValueIndirect;
	SVString InspectionName;

	if( nullptr != GetInspection() )
	{
		InspectionName = GetInspection()->GetName();
	}
	InspectionName += _T(".");

	for(int i = 0; i < RangeEnum::ER_COUNT; i++)
	{
		ValueIndirect.clear();
		m_ValueObjectReferences[i] = emptyRef;
		m_ValueIndirect[i].ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
		m_ValueIndirect[i].GetValue( ValueIndirect );
		if( !ValueIndirect.empty() )
		{
			SVString dottedName;
			//If the tool set name is at the start then add the inspection name at the beginning
			if( 0 == ValueIndirect.find(ToolSetName) )
			{
				dottedName = InspectionName + ValueIndirect;
			}
			else
			{
				dottedName = ValueIndirect;
			}
			if(!SetReference( dottedName.c_str(), m_ValueObjectReferences[i] ))
			{
				Result = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer message;
					SVStringVector msgList;
					msgList.push_back(SVString(GetCompleteObjectNameToObjectType( nullptr, SVInspectionObjectType )));
					message.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_InvalidReference, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16025 ); 
					pErrorMessages->push_back( message );
				}
			}
			//check if we have an valid but disabled input
			else if( FALSE == (m_ValueObjectReferences[i].ObjectAttributesAllowed() & SV_SELECTABLE_FOR_EQUATION ) )
			{
				Result = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer message;
					SVStringVector msgList;
					msgList.push_back(SVString(GetCompleteObjectNameToObjectType( nullptr, SVInspectionObjectType )));
					message.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_InvalidReference, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16026 ); 
					pErrorMessages->push_back( message );
				}
			}
			else
			{
				m_ValueIndirect[i].ObjectAttributesAllowedRef() |= SV_PRINTABLE;
			}
		}
	}

	if( !Result )
	{
		m_isValidRange = false;
		///SetInvalid();
	}
	else
	{
		m_isValidRange = true;
	}

	if( Result )
	{
		ConnectAllInputObjects();
	}

	return Result;
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

bool SVRangeClass::DisconnectObjectInput( SVInObjectInfoStruct* pObjectInInfo )
{
// This Message occurs for two scenarios
// 1. Some Object is using our outputs and they are no longer needed.
// 2. We are using some Object's outputs and the outputs are no longer available
	m_isValidRange = false;
	return __super::DisconnectObjectInput(pObjectInInfo);
}

void SVRangeClass::OnObjectRenamed(const SVObjectClass& rRenamedObject, const SVString& rOldName)
{
	SVString newPrefix;
	SVString oldPrefix;
	//In this case the inspection name is not part of the saved name so do not rename inspection names
	if( const BasicValueObject* pBasicValueObject = dynamic_cast<const BasicValueObject*> (&rRenamedObject) )
	{
		newPrefix = pBasicValueObject->GetCompleteObjectNameToObjectType( nullptr, SVRootObjectType );
	}
	else if( const SVValueObjectClass* pValueObject = dynamic_cast<const SVValueObjectClass*> (&rRenamedObject) )
	{
		newPrefix = pValueObject->GetCompleteObjectNameToObjectType( nullptr, SVToolSetObjectType );
	}
	else
	{
		newPrefix = rRenamedObject.GetCompleteObjectNameToObjectType( nullptr, SVToolSetObjectType ) + _T( "." );
	}// end else
	oldPrefix = newPrefix;
	SvUl_SF::searchAndReplace( oldPrefix, rRenamedObject.GetName(), rOldName.c_str() );

	RangeClassHelper rangeHelper(this);
	rangeHelper.SetRangeTaskObject();
	rangeHelper.GetAllInspectionData();
	if(rangeHelper.RenameIndirectValues( oldPrefix.c_str(), newPrefix.c_str() ))
	{
		rangeHelper.SetInspectionData();
	}
	__super::OnObjectRenamed(rRenamedObject, rOldName);
}

BOOL SVRangeClass::onRun(SVRunStatusClass& RRunStatus)
{
	BOOL ret = true;
	if(!m_isValidRange)
	{
		RRunStatus.SetInvalid();
		ret = false;
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

// ISVCancel interface
bool SVRangeClass::CanCancel()
{
	return true;
}

HRESULT SVRangeClass::GetCancelData(SVCancelData*& rpCancelData)
{
	ASSERT(nullptr == rpCancelData);
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
		return S_OK == ( static_cast<SVValueObjectClass*>( m_inputObjectInfo.GetInputObjectInfo().PObject) )->GetValue( RVal );
	}
	return false;
}

void SVRangeClass::ConnectAllInputObjects()
{
	for(int i = 0; i < RangeEnum::ER_COUNT; i++)
	{
		m_IsConnectedInput[i] = false;

		if(nullptr != m_ValueObjectReferences[i].Object())
		{
			if( SV_GUID_NULL != m_ValueObjectReferences[i].Guid() )
			{
				SVInObjectInfoStruct InObjectInfo;
				InObjectInfo.PObject                    = this;
				InObjectInfo.UniqueObjectID             = GetUniqueObjectID();
				InObjectInfo.ObjectTypeInfo.ObjectType  = SVRangeObjectType;
				m_IsConnectedInput[i] = SVObjectManagerClass::Instance().ConnectObjectInput( m_ValueObjectReferences[i].Guid(), &InObjectInfo );
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
			if( SV_GUID_NULL != m_ValueObjectReferences[i].Guid() )
			{
				SVInObjectInfoStruct InObjectInfo;

				InObjectInfo.PObject                    =this;
				InObjectInfo.UniqueObjectID             = GetUniqueObjectID();
				InObjectInfo.ObjectTypeInfo.ObjectType  = SVRangeObjectType;

				SVObjectManagerClass::Instance().DisconnectObjectInput(m_ValueObjectReferences[i].Guid(), &InObjectInfo);
				m_IsConnectedInput[i] = false;
			}
		}
	}
}

HRESULT SVRangeClass::GetIndirectValue(RangeEnum::ERange ra, SVString& rValue )
{
	return m_ValueIndirect[ra].GetValue( rValue );
};

bool SVRangeClass::HasIndirectValue(RangeEnum::ERange ra)
{
	bool res = false;
	SVString Temp;
	if(S_OK == GetIndirectValue(ra, Temp))
	{
		res = !Temp.empty();
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

const SVDoubleValueObjectClass& SVRangeClass::getUpdatedRange( RangeEnum::ERange ra  ,int bucket )
{
	if(m_isValidRange)
	{
		if( nullptr != m_ValueObjectReferences[  ra ].Object() )
		{
			UpdateRange(bucket, ra );
		}
	}

	switch (ra)
	{
	case RangeEnum::ER_FailHigh:
		return FailHigh;
		break;

	case RangeEnum::ER_WarnHigh:
		return WarnHigh;
		break;

	case RangeEnum::ER_FailLow:
		return FailLow;
		break;

	case RangeEnum::ER_WarnLow:
		return WarnLow;
		break;
	default:
		return FailLow;
		break;
	}
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
