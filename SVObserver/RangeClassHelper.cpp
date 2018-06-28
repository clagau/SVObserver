//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRangeClassHelper
//* .File Name       : $Workfile:   RangeClassHelper.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   19 Jan 2015 11:40:44  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVRange.h"
#include "RangeClassHelper.h"
#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "SVCommandLibrary/SVObjectAsynchronousCommandTemplate.h"
#include "SVCommandLibrary/SVObjectSynchronousCommandTemplate.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVInspectionProcess.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "InspectionCommands/BuildSelectableItems.h"
#include "InspectionEngine/SVTaskObjectList.h"
#include "ObjectSelectorLibrary\ObjectTreeGenerator.h"
#include "Definitions/GlobalConst.h"
#include "SVToolSet.h"
#include "InspectionEngine/SVTool.h"
#include "SVPPQObject.h"
#include "SVStatusLibrary\MessageManager.h"
#include "TextDefinesSvO.h"
#include "SVOGui/ValuesAccessor.h"
#include "SVOGui/DataController.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

#pragma region Constructor
RangeClassHelper::RangeClassHelper(SVRangeClass *pRange)
: m_FailHigh(0.0)
, m_WarnHigh(0.0)
, m_WarnLow(0.0)
, m_FailLow(0.0)
, m_pRange(pRange)
{
}

RangeClassHelper::~RangeClassHelper()
{
}
#pragma endregion Constructor

#pragma region Public Methods
void RangeClassHelper::SetRangeTaskObject()
{
}

HRESULT RangeClassHelper::GetInspectionData(RangeEnum::ERange ra)
{
	HRESULT hr = E_FAIL;
	switch (ra)
	{
	case RangeEnum::ER_FailHigh:
		hr = m_pRange->GetValue(ra, m_FailHigh);
		if( S_OK == hr )
		{
			hr = m_pRange->GetIndirectValue(ra, m_FailHighIndirect);
		}
		break;

	case RangeEnum::ER_WarnHigh:
		hr = m_pRange->GetValue(ra, m_WarnHigh);
		if( S_OK == hr )
		{
			hr = m_pRange->GetIndirectValue(ra, m_WarnHighIndirect);
		}
		break;

	case RangeEnum::ER_FailLow:
		hr = m_pRange->GetValue(ra, m_FailLow);
		if( S_OK == hr )
		{
			hr = m_pRange->GetIndirectValue(ra, m_FailLowIndirect);
		}
		break;

	case RangeEnum::ER_WarnLow:
		hr = m_pRange->GetValue(ra, m_WarnLow);
		if( S_OK == hr )
		{
			hr = m_pRange->GetIndirectValue(ra, m_WarnLowIndirect);
		}
		break;

	default:
		// Do nothing.
		break;
	}

	return hr;
}

HRESULT RangeClassHelper::GetAllInspectionData()
{
	HRESULT hr = S_OK;
	for (int i = 0; i < RangeEnum::ER_COUNT; i++)
	{
		if (S_OK == hr)
		{
			hr = GetInspectionData(static_cast<RangeEnum::ERange>(i));
		}
		else
		{
			break;
		}
	}

	return hr;
}

void RangeClassHelper::SetInternalData(SvStl::MessageTextEnum er, LPCTSTR lp)
{
	std::string Text = lp;
	double val = 0.0;
	const double s_RangeMax = 17000000;
	const double s_RangeMin = -17000000;
	int textLength = static_cast<int> (Text.size());
	HINSTANCE resHandle = AfxGetResourceHandle();

	if( 0 == textLength )
	{
		SvDef::StringVector msgList;
		msgList.push_back(SvStl::MessageData::convertId2AddtionalText(er));
		SvStl::MessageMgrStd Exception( SvStl::DataOnly );
		Exception.setMessage( SVMSG_SVO_68_RANGE_VALUE_SET_FAILED, SvStl::Tid_RangeValue_EmptyString, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16022 );
		Exception.Throw();
	}

	bool isNumber = SvUl::Convert2Number<double>( Text, val, true );
	if( isNumber )
	{
		Text = _T("");
		if(val > s_RangeMax || val < s_RangeMin )
		{
			SvDef::StringVector msgList;
			msgList.push_back(SvStl::MessageData::convertId2AddtionalText(er));
			msgList.push_back(SvUl::Format("%d", static_cast<int>(s_RangeMin)));
			msgList.push_back(SvUl::Format("%d", static_cast<int>(s_RangeMax)));
			SvStl::MessageMgrStd Exception( SvStl::DataOnly );
			Exception.setMessage( SVMSG_SVO_68_RANGE_VALUE_SET_FAILED, SvStl::Tid_RangeValue_WrongRange, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16023 );
			Exception.Throw();
		}
	}

	switch (er)
	{
	case SvStl::Tid_FailHigh:
		m_FailHigh = val;
		m_FailHighIndirect = Text;
		break;
	case SvStl::Tid_WarnHigh:
		m_WarnHigh = val;
		m_WarnHighIndirect = Text;
		break;
	case SvStl::Tid_FailLow:
		m_FailLow = val;
		m_FailLowIndirect = Text;
		break;
	case SvStl::Tid_WarnLow:
		m_WarnLow = val;
		m_WarnLowIndirect = Text;
		break;
	default:
		SvStl::MessageMgrStd Exception( SvStl::DataOnly );
		Exception.setMessage( SVMSG_SVO_68_RANGE_VALUE_SET_FAILED, SvStl::Tid_ErrorUnknownEnum, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16024 );
		Exception.Throw();
		break;
	}
}

HRESULT RangeClassHelper::CheckInternalData(SvStl::MessageTextEnum &messageId, SvDef::StringVector &messageList) const
{
	std::string InspectionName;
	if( nullptr != m_pRange)
	{
		SVInspectionProcess* pInspection =  dynamic_cast<SVInspectionProcess*>(m_pRange->GetInspection());
		if( nullptr != pInspection )
		{
			InspectionName = pInspection->GetName();
		}
	}
	InspectionName += _T(".");

	if( m_FailHighIndirect.empty() && m_WarnHighIndirect.empty() && m_FailHigh < m_WarnHigh )
	{
		messageId = SvStl::Tid_IsLessThan;
		messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvStl::Tid_FailHigh));
		messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvStl::Tid_WarnHigh));
		return -SvStl::Err_16012;
	}

	if( m_FailHighIndirect.empty() && m_WarnLowIndirect.empty() && m_FailHigh < m_WarnLow )
	{
		messageId = SvStl::Tid_IsLessThan;
		messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvStl::Tid_FailHigh));
		messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvStl::Tid_WarnLow));
		return -SvStl::Err_16013;
	}

	if( m_FailHighIndirect.empty() && m_FailLowIndirect.empty() && m_FailHigh < m_FailLow )
	{
		messageId = SvStl::Tid_IsLessThan;
		messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvStl::Tid_FailHigh));
		messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvStl::Tid_FailLow));
		return -SvStl::Err_16014;
	}

	if( m_WarnHighIndirect.empty() && m_WarnLowIndirect.empty() && m_WarnHigh < m_WarnLow )
	{
		messageId = SvStl::Tid_IsLessThan;
		messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvStl::Tid_WarnHigh));
		messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvStl::Tid_WarnLow));
		return -SvStl::Err_16015;
	}

	if( m_WarnHighIndirect.empty() && m_FailLowIndirect.empty() && m_WarnHigh < m_FailLow )
	{
		messageId = SvStl::Tid_IsLessThan;
		messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvStl::Tid_WarnHigh));
		messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvStl::Tid_FailLow));
		return -SvStl::Err_16016;
	}

	if( m_WarnLowIndirect.empty() && m_FailLowIndirect.empty() && m_WarnLow < m_FailLow )
	{
		messageId = SvStl::Tid_IsLessThan;
		messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvStl::Tid_WarnLow));
		messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvStl::Tid_FailLow));
		return -SvStl::Err_16017;
	}

	if( !m_FailHighIndirect.empty() )
	{
		if( !isValidReference( InspectionName, m_FailHighIndirect ) )
		{
			messageId = SvStl::Tid_IsInvalidRef;
			messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvStl::Tid_FailHigh));
			messageList.push_back(std::string(m_FailHighIndirect));
			return -SvStl::Err_16018;
		}
	}

	if( !m_WarnHighIndirect.empty() )
	{
		if( !isValidReference( InspectionName, m_WarnHighIndirect ) )
		{
			messageId = SvStl::Tid_IsInvalidRef;
			messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvStl::Tid_WarnHigh));
			messageList.push_back(std::string(m_WarnHighIndirect));
			return -SvStl::Err_16019;
		}
	}

	if( !m_WarnLowIndirect.empty() )
	{
		if( !isValidReference( InspectionName, m_WarnLowIndirect ) )
		{
			messageId = SvStl::Tid_IsInvalidRef;
			messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvStl::Tid_WarnLow));
			messageList.push_back(std::string(m_WarnLowIndirect));
			return -SvStl::Err_16020;
		}
	}

	if(!m_FailLowIndirect.empty())
	{
		if( !isValidReference( InspectionName, m_FailLowIndirect ) )
		{
			messageId = SvStl::Tid_IsInvalidRef;
			messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvStl::Tid_FailLow));
			messageList.push_back(std::string(m_FailLowIndirect));
			return -SvStl::Err_16021;
		}
	}

	return S_OK;
}

HRESULT RangeClassHelper::SetInspectionData()
{
	HRESULT Result{ S_OK };

	if (nullptr != m_pRange)
	{
		typedef SvOg::ValuesAccessor<SvOg::BoundValues> ValueCommand;
		typedef SvOg::DataController<ValueCommand, ValueCommand::value_type> Controller;
		Controller Values{ SvOg::BoundValues{ m_pRange->GetInspection()->GetUniqueObjectID(), m_pRange->GetUniqueObjectID() } };
		Values.Init();

		Values.Set<double>(SVRangeClassFailHighObjectGuid, m_FailHigh);
		Values.Set<double>(SVRangeClassWarnHighObjectGuid, m_WarnHigh);
		Values.Set<double>(SVRangeClassWarnLowObjectGuid, m_WarnLow);
		Values.Set<double>(SVRangeClassFailLowObjectGuid, m_FailLow);

		Values.Set<CString>(SVRangeClassFailHighIndirectObjectGuid, m_FailHighIndirect.c_str());
		Values.Set<CString>(SVRangeClassWarnHighIndirectObjectGuid, m_WarnHighIndirect.c_str());
		Values.Set<CString>(SVRangeClassWarnLowIndirectObjectGuid, m_WarnLowIndirect.c_str());
		Values.Set<CString>(SVRangeClassFailLowIndirectObjectGuid, m_FailLowIndirect.c_str());

		Result = Values.Commit();
	}
	return Result;
}

std::string RangeClassHelper::GetStringFromRange(RangeEnum::ERange ra) const
{
	std::string Result;
	double Value;

	if( nullptr == m_pRange )
	{
		return Result;
	}

	m_pRange->GetIndirectValue(ra, Result);

	if( Result.empty() )
	{
		if( S_OK == m_pRange->GetValue( ra, Value ) )
		{
			Result = SvUl::Format( _T("%lf"), Value );
		}
	}

	return Result;
};

bool RangeClassHelper::IsOwnedByRangeObject(const SVObjectClass& rObject)
{
	bool result = false;

	if(rObject.GetParent())
	{
		if(SvDef::SVRangeObjectType == rObject.GetParent()->GetObjectType())
		{
			result = true;
		}
	}

	return result;
}

bool RangeClassHelper::IsAllowedToSet(const SVObjectClass& ObjectRef, const std::string& rValue, bool bOnline, HRESULT& hres)
{
	if(!IsOwnedByRangeObject(ObjectRef))
	{
		return true;
	}

	bool res = true;
	bool IsReference = false;
	const SVGUID& guid = ObjectRef.GetEmbeddedID();
	if ( guid == SVRangeClassFailHighIndirectObjectGuid ||
		guid == SVRangeClassFailLowIndirectObjectGuid ||
		guid == SVRangeClassWarnHighIndirectObjectGuid ||
		guid == SVRangeClassWarnLowIndirectObjectGuid )
	{
		IsReference = true;
	}

	if(IsReference)
	{
		if(bOnline)
		{
			res = false;
			hres = SVMSG_OBJECT_CANNOT_BE_SET_WHILE_ONLINE;
		}
		else
		{
			SVObjectReference ObjectRefValue;
			HRESULT hrFind = SVObjectManagerClass::Instance().GetObjectByDottedName( rValue.c_str(), ObjectRefValue );

			if( !rValue.empty() && ( S_OK != hrFind || nullptr == ObjectRefValue.getObject() ) )
			{
				//a not empty string and no reference
				hres = SVMSG_OBJECT_CANNOT_BE_SET_INVALID_REFERENCE;
				res = false;
			}
		}
	}

	return res;
}

LPCTSTR RangeClassHelper::GetOwnerName() const
{
	LPCTSTR ret = nullptr;
	if(m_pRange && m_pRange->GetParent())
	{
		ret = m_pRange->GetParent()->GetName();
	}
	return ret;
}

bool RangeClassHelper::RenameIndirectValues(LPCTSTR oldPefix, LPCTSTR newPrefix)
{
	bool result = RenameIndirectValue(m_FailHighIndirect, oldPefix, newPrefix);
	result = RenameIndirectValue(m_WarnHighIndirect, oldPefix, newPrefix) || result;
	result = RenameIndirectValue(m_FailLowIndirect, oldPefix, newPrefix) || result;
	result = RenameIndirectValue(m_WarnLowIndirect, oldPefix, newPrefix) || result;
	return result;
}

std::string RangeClassHelper::GetFailHighString()
{
	return GetValueString(m_FailHighIndirect, m_FailHigh);
}

std::string RangeClassHelper::GetWarnHighString()
{
	return GetValueString(m_WarnHighIndirect, m_WarnHigh);
}

std::string RangeClassHelper::GetFailLowString()
{
	return GetValueString(m_FailLowIndirect, m_FailLow);
}

std::string RangeClassHelper::GetWarnLowString()
{
	return GetValueString(m_WarnLowIndirect, m_WarnLow);
}

bool RangeClassHelper::FillObjectSelector()
{
	bool bRetVal = false;
	if (nullptr != m_pRange)
	{
		SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*>(m_pRange->GetInspection());
		if (nullptr != pInspection)
		{
			SVTaskObjectListClass* pTaskObjectList = dynamic_cast<SVTaskObjectListClass*>(pInspection->GetToolSet());
			if (nullptr != pTaskObjectList)
			{
				std::string PPQName = SvDef::FqnPPQVariables; 
				std::string InspectionName = pInspection->GetName();
				SVPPQObject* pPPQ = pInspection->GetPPQ();
				if( nullptr != pPPQ )
				{
					PPQName = pPPQ->GetName();
				}
				SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterInput, InspectionName, std::string( _T("") ) );
				SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterOutput, InspectionName, std::string( _T("") ) );

				SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterOutput, PPQName, std::string( _T("")  ));
				SvOsl::ObjectTreeGenerator::Instance().setSelectorType( SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeSingleObject );

				SvCmd::SelectorOptions BuildOptions {{SvCmd::ObjectSelectorType::globalConstantItems, SvCmd::ObjectSelectorType::toolsetItems}, 
					pInspection->GetUniqueObjectID(), SvDef::SV_SELECTABLE_FOR_EQUATION, m_pRange->GetUniqueObjectID()};
				SvCl::SelectorItemVector SelectorItems;
				SvCmd::BuildSelectableItems(BuildOptions, std::back_inserter(SelectorItems), SvCmd::RangeSelectorFilterType);
				SvOsl::ObjectTreeGenerator::Instance().insertTreeObjects(SelectorItems);

				bRetVal = true;
			}//@TODO return meaningful error on else
		}//@TODO return meaningful error on else
	}//@TODO return meaningful error on else
	return bRetVal;
}

#pragma endregion Public Methods

#pragma region Private Methods
std::string RangeClassHelper::GetValueString(const std::string& rIndirectString, double directValue)
{
	if( 0 < rIndirectString.size() )
	{
		return rIndirectString;
	}
	else
	{
		std::string Text = SvUl::Format( _T("%lf"), directValue );
		return Text;
	}
}

bool RangeClassHelper::isValidReference( const std::string& rInspectionName, const std::string& rIndirectString ) const
{
	bool Result( true );

	std::string dottedName;
	std::string ToolSetName;
	ToolSetName = SvUl::LoadStdString( IDS_CLASSNAME_SVTOOLSET );
	//If the tool set name is at the start then add the inspection name at the beginning
	if( 0 == rIndirectString.find( ToolSetName ))
	{
		dottedName = rInspectionName + rIndirectString;
	}
	else
	{
		dottedName = rIndirectString;
	}

	SVObjectReference ObjectRef;
	if( !SVRangeClass::SetReference(dottedName.c_str(), ObjectRef) )
	{
		Result = false;
	}
	else if( 0 == ( ObjectRef.ObjectAttributesAllowed() & SvDef::SV_SELECTABLE_FOR_EQUATION ) )
	{
		Result = false;
	}

	return Result;
}

bool RangeClassHelper::RenameIndirectValue(std::string& rIndirectString, LPCTSTR oldPefix, LPCTSTR newPrefix)
{
	bool Result( false );
	std::string oldString(oldPefix);
	if ('.' == oldString[oldString.size()-1])
	{	//check if part of the name (ends with '.') is to replace
		std::string NewIndirectString = rIndirectString;
		SvUl::searchAndReplace( NewIndirectString, oldPefix, newPrefix );
		if( NewIndirectString != rIndirectString )
		{
			rIndirectString = NewIndirectString;
			Result = true;
		}
	}
	else
	{
		std::string indirectTmp = rIndirectString;
		size_t pos = indirectTmp.find('[');
		if (std::string::npos != pos)
		{	//if array ("[x]") in the name, remove it for the check
			indirectTmp = indirectTmp.substr(0, pos);
		}
		//only replace the name if it is the full name. Do NOT replace parts of the name, because then it this a other object with similar name.
		std::string NewIndirectString = rIndirectString;
		SvUl::searchAndReplace( NewIndirectString, oldPefix, newPrefix );
		if( oldString == indirectTmp && NewIndirectString != rIndirectString )
		{	
			rIndirectString = NewIndirectString;
			Result = true;
		}
	}
	return Result;
}
#pragma endregion Private Methods

