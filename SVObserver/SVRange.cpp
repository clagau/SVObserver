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
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVOCore/SVTool.h"
#include "SVInspectionProcess.h"
#include "RangeClassHelper.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVValueObjectLibrary/BasicValueObject.h"
#include "Definitions/StringTypeDef.h"
#include "SVUtilityLibrary/StringHelper.h"
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
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvDef::SVRangeObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &FailHigh, SVRangeClassFailHighObjectGuid, IDS_OBJECTNAME_FAIL_HIGH, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &WarnHigh, SVRangeClassWarnHighObjectGuid, IDS_OBJECTNAME_WARN_HIGH, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &FailLow, SVRangeClassFailLowObjectGuid, IDS_OBJECTNAME_FAIL_LOW, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &WarnLow, SVRangeClassWarnLowObjectGuid, IDS_OBJECTNAME_WARN_LOW, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_ValueIndirect[RangeEnum::ER_FailHigh], SVRangeClassFailHighIndirectObjectGuid, IDS_OBJECTNAME_FAIL_HIGH_INDIRECT, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject( &m_ValueIndirect[RangeEnum::ER_WarnHigh], SVRangeClassWarnHighIndirectObjectGuid, IDS_OBJECTNAME_WARN_HIGH_INDIRECT, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject( &m_ValueIndirect[RangeEnum::ER_FailLow], SVRangeClassFailLowIndirectObjectGuid, IDS_OBJECTNAME_FAIL_LOW_INDIRECT, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject( &m_ValueIndirect[RangeEnum::ER_WarnLow], SVRangeClassWarnLowIndirectObjectGuid, IDS_OBJECTNAME_WARN_LOW_INDIRECT, false, SvOi::SVResetItemOwner);

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

bool SVRangeClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	m_isCreated = SVTaskObjectClass::CreateObject(rCreateStructure);

	// Set / Reset Printable Flags
	const UINT cAttributes = SvDef::SV_PRINTABLE | SvDef::SV_SETABLE_ONLINE | SvDef::SV_REMOTELY_SETABLE;
	FailHigh.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	WarnHigh.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	FailLow.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	WarnLow.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );

	for(int i = 0; i < RangeEnum::ER_COUNT; i++)
	{
		m_ValueIndirect[i].SetObjectAttributesAllowed( SvDef::SV_PRINTABLE | SvDef::SV_REMOTELY_SETABLE, SvOi::SetAttributeType::AddAttribute );
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
	if( !m_inputObjectInfo.IsConnected() || nullptr == m_inputObjectInfo.GetInputObjectInfo().m_pObject )
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
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

bool SVRangeClass::CloseObject()
{
	DisconnectAllInputObjects();
	return SVTaskObjectClass::CloseObject();
}

bool SVRangeClass::InitReferencesAndInputs(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = true;
	DisconnectAllInputObjects();
	std::string ValueIndirect;
	std::string InspectionName;

	if( nullptr != GetInspection() )
	{
		InspectionName = GetInspection()->GetName();
	}
	InspectionName += _T(".");

	for(int i = 0; i < RangeEnum::ER_COUNT; i++)
	{
		ValueIndirect.clear();
		m_ValueObjectReferences[i] = SVObjectReference();
		m_ValueIndirect[i].SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
		m_ValueIndirect[i].GetValue( ValueIndirect );
		if( !ValueIndirect.empty() )
		{
			std::string dottedName;
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
					SvDef::StringVector msgList;
					msgList.push_back(std::string(GetCompleteObjectNameToObjectType( nullptr, SvDef::SVInspectionObjectType )));
					message.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_InvalidReference, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16025 ); 
					pErrorMessages->push_back( message );
			}
			}
			//check if we have an valid but disabled input
			else if( 0 == (m_ValueObjectReferences[i].ObjectAttributesAllowed() & SvDef::SV_SELECTABLE_FOR_EQUATION ) )
			{
				Result = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer message;
					SvDef::StringVector msgList;
					msgList.push_back(std::string(GetCompleteObjectNameToObjectType( nullptr, SvDef::SVInspectionObjectType )));
					message.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_InvalidReference, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16026 ); 
					pErrorMessages->push_back( message );
			}
			}
			else
			{
				m_ValueIndirect[i].SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
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
		ASSERT (false);
		return WarnLow;
	}
}

void   SVRangeClass::UpdateRange(RangeEnum::ERange  range )
{
	SVObjectClass* pObject = m_ValueObjectReferences[range].getObject();

	if(nullptr != pObject)
	{
		double Value(0.0);
		if(m_ValueObjectReferences[range].isArray())
		{
			int index = m_ValueObjectReferences[range].ArrayIndex();
			pObject->getValue( Value, index );
		}
		else
		{
			pObject->getValue( Value );
		}

		GetRange(range).SetValue(Value);	
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

void SVRangeClass::OnObjectRenamed(const SVObjectClass& rRenamedObject, const std::string& rOldName)
{
	std::string newPrefix;
	std::string oldPrefix;
	//In this case the inspection name is not part of the saved name so do not rename inspection names
	if( nullptr != dynamic_cast<const BasicValueObject*> (&rRenamedObject) )
	{
		newPrefix = rRenamedObject.GetCompleteObjectNameToObjectType( nullptr, SvDef::SVRootObjectType );
	}
	else if( nullptr != dynamic_cast<const SvOi::IValueObject*> (&rRenamedObject) )
	{
		newPrefix = rRenamedObject.GetCompleteObjectNameToObjectType( nullptr, SvDef::SVToolSetObjectType );
	}
	else
	{
		newPrefix = rRenamedObject.GetCompleteObjectNameToObjectType( nullptr, SvDef::SVToolSetObjectType ) + _T( "." );
	}// end else
	oldPrefix = newPrefix;
	SvUl::searchAndReplace( oldPrefix, rRenamedObject.GetName(), rOldName.c_str() );

	RangeClassHelper rangeHelper(this);
	rangeHelper.SetRangeTaskObject();
	rangeHelper.GetAllInspectionData();
	if(rangeHelper.RenameIndirectValues( oldPrefix.c_str(), newPrefix.c_str() ))
	{
		rangeHelper.SetInspectionData();
	}
	__super::OnObjectRenamed(rRenamedObject, rOldName);
}

bool SVRangeClass::onRun(SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
{
	bool ret = true;
	if(!m_isValidRange)
	{
		rRunStatus.SetInvalid();
		ret = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidRange, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	if(ret)
	{
		double InputValue,failHigh(0), failLow(0),warnLow(0),warnHigh(0);
		getInputValue(InputValue);


		if( nullptr != m_ValueObjectReferences[RangeEnum::ER_FailLow].getObject() )
		{
			UpdateRange(RangeEnum::ER_FailLow);
		}
		FailLow.GetValue(failLow);

		if( nullptr != m_ValueObjectReferences[RangeEnum::ER_FailHigh ].getObject() )
		{
			UpdateRange(RangeEnum::ER_FailHigh);
		}
		FailHigh.GetValue(failHigh);

		if( nullptr != m_ValueObjectReferences[RangeEnum::ER_WarnLow  ].getObject() )
		{
			UpdateRange(RangeEnum::ER_WarnLow);
		}

		WarnLow.GetValue(warnLow);

		if( nullptr != m_ValueObjectReferences[ RangeEnum::ER_WarnHigh  ].getObject() )
		{
			UpdateRange(RangeEnum::ER_WarnHigh);
		}

		WarnHigh.GetValue(warnHigh);

		bool isFailed = ( InputValue < failLow || InputValue > failHigh );
		bool isWarned = ( !isFailed && ( InputValue < warnLow || InputValue > warnHigh ) );

		if( isFailed )
		{
			rRunStatus.SetFailed();
		}

		if( isWarned )
		{
			rRunStatus.SetWarned();
		}

		if( !isFailed && !isWarned )
		{
			rRunStatus.SetPassed();
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

bool SVRangeClass::getInputValue( double& rValue )
{
	if( m_inputObjectInfo.IsConnected() && m_inputObjectInfo.GetInputObjectInfo().m_pObject )
	{
		if( nullptr != m_inputObjectInfo.GetInputObjectInfo().m_pObject)
		{
			return S_OK == m_inputObjectInfo.GetInputObjectInfo().m_pObject->getValue(rValue);
		}
	}
	return false;
}

void SVRangeClass::ConnectAllInputObjects()
{
	for(int i = 0; i < RangeEnum::ER_COUNT; i++)
	{
		m_IsConnectedInput[i] = false;

		if(nullptr != m_ValueObjectReferences[i].getObject())
		{
			if( SV_GUID_NULL != m_ValueObjectReferences[i].Guid() )
			{
				SVInObjectInfoStruct InObjectInfo;
				InObjectInfo.m_pObject                    = this;
				InObjectInfo.m_UniqueObjectID             = GetUniqueObjectID();
				InObjectInfo.m_ObjectTypeInfo.ObjectType  = SvDef::SVRangeObjectType;
				m_IsConnectedInput[i] = SVObjectManagerClass::Instance().ConnectObjectInput( m_ValueObjectReferences[i].Guid(), &InObjectInfo );
			}
		}
	}
}

void SVRangeClass::DisconnectAllInputObjects()
{
	for(int i = 0; i < RangeEnum::ER_COUNT; i++)
	{
		if(nullptr != m_ValueObjectReferences[i].getObject() && m_IsConnectedInput[i] == true)
		{
			if( SV_GUID_NULL != m_ValueObjectReferences[i].Guid() )
			{
				SVInObjectInfoStruct InObjectInfo;

				InObjectInfo.m_pObject                    =this;
				InObjectInfo.m_UniqueObjectID             = GetUniqueObjectID();
				InObjectInfo.m_ObjectTypeInfo.ObjectType  = SvDef::SVRangeObjectType;

				SVObjectManagerClass::Instance().DisconnectObjectInput(m_ValueObjectReferences[i].Guid(), &InObjectInfo);
				m_IsConnectedInput[i] = false;
			}
		}
	}
}

HRESULT SVRangeClass::GetIndirectValue(RangeEnum::ERange ra, std::string& rValue )
{
	return m_ValueIndirect[ra].GetValue( rValue );
};

bool SVRangeClass::HasIndirectValue(RangeEnum::ERange ra)
{
	bool res = false;
	std::string Temp;
	if(S_OK == GetIndirectValue(ra, Temp))
	{
		res = !Temp.empty();
	}

	return res;
};

HRESULT SVRangeClass::GetValue(RangeEnum::ERange ra, double &rValue)
{
	switch (ra)
	{
	case RangeEnum::ER_FailHigh:
		return FailHigh.GetValue(rValue);
		break;

	case RangeEnum::ER_WarnHigh:
		return WarnHigh.GetValue(rValue);
		break;

	case RangeEnum::ER_FailLow:
		return FailLow.GetValue(rValue);
		break;

	case RangeEnum::ER_WarnLow:
		return WarnLow.GetValue(rValue);
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

const SVDoubleValueObjectClass& SVRangeClass::getUpdatedRange( RangeEnum::ERange ra)
{
	if(m_isValidRange)
	{
		if( nullptr != m_ValueObjectReferences[  ra ].getObject() )
		{
			UpdateRange(ra );
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

const SVDoubleValueObjectClass& SVRangeClass::getUpdatedFailLow()
{
	if(m_isValidRange)
	{
		if( nullptr != m_ValueObjectReferences[  RangeEnum::ER_FailLow  ].getObject() )
		{
			UpdateRange(RangeEnum::ER_FailLow );
		}
	}
	return FailLow;
}

const SVDoubleValueObjectClass& SVRangeClass::getUpdatedFailHigh()
{
	if(m_isValidRange)
	{
		if( nullptr != m_ValueObjectReferences[  RangeEnum::ER_FailHigh ].getObject() )
		{
			UpdateRange(RangeEnum::ER_FailHigh );
		}
	}
	return FailHigh;
}
