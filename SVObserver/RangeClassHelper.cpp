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
#include "SVObjectLibrary\SVObjectAsynchronousCommandTemplate.h"
#include "SVObjectLibrary\SVObjectSynchronousCommandTemplate.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVInspectionProcess.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "SVUtilityLibrary\SVString.h"
#include "SVOCore/SVTaskObjectList.h"
#include "ObjectSelectorLibrary\ObjectTreeGenerator.h"
#include "SVObjectLibrary\GlobalConst.h"
#include "SVToolSet.h"
#include "SVTool.h"
#include "SVPPQObject.h"
#include "SVStatusLibrary\MessageManager.h"
#include "TextDefinesSvO.h"
#include "SVOGui/GlobalSelector.h"
#include "SVOGui/NoSelector.h"
#include "SVOGui/ToolSetItemSelector.h"
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
	SetTaskObject(m_pRange);
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

void RangeClassHelper::SetInternalData(SvOi::MessageTextEnum er, LPCTSTR lp)
{
	CString csText = lp;
	double val = 0.0;
	const double s_RangeMax = 17000000;
	const double s_RangeMin = -17000000;
	int textLength = csText.GetLength();
	HINSTANCE resHandle = AfxGetResourceHandle();

	if( textLength == 0)
	{
		SVStringArray msgList;
		msgList.push_back(SvStl::MessageData::convertId2AddtionalText(er));
		SvStl::MessageMgrStd Exception( SvStl::DataOnly );
		Exception.setMessage( SVMSG_SVO_68_RANGE_VALUE_SET_FAILED, SvOi::Tid_RangeValue_EmptyString, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16022 );
		Exception.Throw();
	}

	SVString text = lp;
	bool isNumber = SvUl_SF::Convert2Number<double>(text, val, true);
	if( isNumber )
	{
		csText = _T("");
		if(val > s_RangeMax || val < s_RangeMin )
		{
			SVStringArray msgList;
			msgList.push_back(SvStl::MessageData::convertId2AddtionalText(er));
			msgList.push_back(SvUl_SF::Format("%d", static_cast<int>(s_RangeMin)));
			msgList.push_back(SvUl_SF::Format("%d", static_cast<int>(s_RangeMax)));
			SvStl::MessageMgrStd Exception( SvStl::DataOnly );
			Exception.setMessage( SVMSG_SVO_68_RANGE_VALUE_SET_FAILED, SvOi::Tid_RangeValue_WrongRange, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16023 );
			Exception.Throw();
		}
	}

	switch (er)
	{
	case SvOi::Tid_FailHigh:
		m_FailHigh = val;
		m_FailHighIndirect = csText;
		break;
	case SvOi::Tid_WarnHigh:
		m_WarnHigh = val;
		m_WarnHighIndirect = csText;
		break;
	case SvOi::Tid_FailLow:
		m_FailLow = val;
		m_FailLowIndirect = csText;
		break;
	case SvOi::Tid_WarnLow:
		m_WarnLow = val;
		m_WarnLowIndirect = csText;
		break;
	default:
		SvStl::MessageMgrStd Exception( SvStl::DataOnly );
		Exception.setMessage( SVMSG_SVO_68_RANGE_VALUE_SET_FAILED, SvOi::Tid_ErrorUnknownEnum, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16024 );
		Exception.Throw();
		break;
	}
}

HRESULT RangeClassHelper::CheckInternalData(SvOi::MessageTextEnum &messageId, SVStringArray &messageList) const
{
	CString InspectionName;
	if( nullptr != m_pRange)
	{
		SVInspectionProcess* pInspection =  dynamic_cast<SVInspectionProcess*>(m_pRange->GetInspection());
		if( nullptr != pInspection )
		{
			InspectionName = pInspection->GetName();
		}
	}
	InspectionName += _T(".");

	if( m_FailHighIndirect.IsEmpty() && m_WarnHighIndirect.IsEmpty() && m_FailHigh < m_WarnHigh )
	{
		messageId = SvOi::Tid_IsLessThan;
		messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvOi::Tid_FailHigh));
		messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvOi::Tid_WarnHigh));
		return -SvOi::Err_16012;
	}

	if( m_FailHighIndirect.IsEmpty() && m_WarnLowIndirect.IsEmpty() && m_FailHigh < m_WarnLow )
	{
		messageId = SvOi::Tid_IsLessThan;
		messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvOi::Tid_FailHigh));
		messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvOi::Tid_WarnLow));
		return -SvOi::Err_16013;
	}

	if( m_FailHighIndirect.IsEmpty() && m_FailLowIndirect.IsEmpty() && m_FailHigh < m_FailLow )
	{
		messageId = SvOi::Tid_IsLessThan;
		messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvOi::Tid_FailHigh));
		messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvOi::Tid_FailLow));
		return -SvOi::Err_16014;
	}

	if( m_WarnHighIndirect.IsEmpty() && m_WarnLowIndirect.IsEmpty() && m_WarnHigh < m_WarnLow )
	{
		messageId = SvOi::Tid_IsLessThan;
		messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvOi::Tid_WarnHigh));
		messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvOi::Tid_WarnLow));
		return -SvOi::Err_16015;
	}

	if( m_WarnHighIndirect.IsEmpty() && m_FailLowIndirect.IsEmpty() && m_WarnHigh < m_FailLow )
	{
		messageId = SvOi::Tid_IsLessThan;
		messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvOi::Tid_WarnHigh));
		messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvOi::Tid_FailLow));
		return -SvOi::Err_16016;
	}

	if( m_WarnLowIndirect.IsEmpty() && m_FailLowIndirect.IsEmpty() && m_WarnLow < m_FailLow )
	{
		messageId = SvOi::Tid_IsLessThan;
		messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvOi::Tid_WarnLow));
		messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvOi::Tid_FailLow));
		return -SvOi::Err_16017;
	}

	if( !m_FailHighIndirect.IsEmpty() )
	{
		if( !isValidReference( InspectionName, m_FailHighIndirect ) )
		{
			messageId = SvOi::Tid_IsInvalidRef;
			messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvOi::Tid_FailHigh));
			messageList.push_back(SVString(m_FailHighIndirect));
			return -SvOi::Err_16018;
		}
	}

	if( !m_WarnHighIndirect.IsEmpty() )
	{
		if( !isValidReference( InspectionName, m_WarnHighIndirect ) )
		{
			messageId = SvOi::Tid_IsInvalidRef;
			messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvOi::Tid_WarnHigh));
			messageList.push_back(SVString(m_WarnHighIndirect));
			return -SvOi::Err_16019;
		}
	}

	if( !m_WarnLowIndirect.IsEmpty() )
	{
		if( !isValidReference( InspectionName, m_WarnLowIndirect ) )
		{
			messageId = SvOi::Tid_IsInvalidRef;
			messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvOi::Tid_WarnLow));
			messageList.push_back(SVString(m_WarnLowIndirect));
			return -SvOi::Err_16020;
		}
	}

	if(!m_FailLowIndirect.IsEmpty())
	{
		if( !isValidReference( InspectionName, m_FailLowIndirect ) )
		{
			messageId = SvOi::Tid_IsInvalidRef;
			messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvOi::Tid_FailLow));
			messageList.push_back(SVString(m_FailLowIndirect));
			return -SvOi::Err_16021;
		}
	}

	return S_OK;
}

HRESULT RangeClassHelper::SetInspectionData()
{
	HRESULT hr = -SvOi::Err_16011;
	if( m_pRange )
	{
		hr = AddInputRequest( &( m_pRange->FailHigh ), m_FailHigh );

		if( S_OK != hr )
		{
			hr = -SvOi::Err_16001;
		}
		else
		{
			hr = AddInputRequest( &( m_pRange->FailLow ), m_FailLow );
		}

		if( S_OK != hr )
		{
			hr = -SvOi::Err_16002;
		}
		else
		{
			hr = AddInputRequest( &( m_pRange->WarnHigh ), m_WarnHigh );
		}

		if( S_OK != hr )
		{
			hr = -SvOi::Err_16003;
		}
		else
		{
			hr = AddInputRequest( &( m_pRange->WarnLow ), m_WarnLow );
		}

		if( S_OK != hr )
		{
			hr = -SvOi::Err_16004;
		}
		else
		{
			hr = AddInputRequest( m_pRange->GetIndirectObject(RangeEnum::ER_FailHigh), m_FailHighIndirect );
		}

		if( S_OK != hr )
		{
			hr = -SvOi::Err_16005;
		}
		else
		{
			hr = AddInputRequest( m_pRange->GetIndirectObject(RangeEnum::ER_FailLow), m_FailLowIndirect );
		}

		if( S_OK != hr )
		{
			hr = -SvOi::Err_16006;
		}
		else
		{
			hr = AddInputRequest( m_pRange->GetIndirectObject(RangeEnum::ER_WarnHigh), m_WarnHighIndirect );
		}

		if( S_OK != hr )
		{
			hr = -SvOi::Err_16007;
		}
		else
		{
			hr = AddInputRequest( m_pRange->GetIndirectObject(RangeEnum::ER_WarnLow), m_WarnLowIndirect );
		}

		if( S_OK != hr )
		{
			hr = -SvOi::Err_16008;
		}
		else
		{
			hr = AddInputRequestMarker();
		}

		if( S_OK != hr )
		{
			hr = -SvOi::Err_16009;
		}
		else
		{
			hr = RunOnce( m_pRange->GetTool()->GetUniqueObjectID() );
		}

		if( S_OK != hr )
		{
			hr = -SvOi::Err_16010;
		}
	}

	return hr;
}

CString RangeClassHelper::GetStringFromRange(RangeEnum::ERange ra) const
{
	CString ret;
	double val;

	if( nullptr == m_pRange )
	{
		return ret;
	}

	m_pRange->GetIndirectValue(ra, ret);

	if( ret.IsEmpty() )
	{
		if( S_OK == m_pRange->GetValue(ra, val) )
		{
			ret.Format(_T("%lf"), val);
		}
	}

	return ret;
};

bool RangeClassHelper::IsOwnedByRangeObject(const SVObjectClass& ref)
{
	bool result = false;

	if(ref.GetOwner())
	{
		if(SVRangeObjectType == ref.GetOwner()->GetObjectType())
		{
			result = true;
		}
	}

	return result;
}

bool RangeClassHelper::IsAllowedToSet(const SVObjectClass& ref, const CString& value, bool bOnline, HRESULT& hres)
{
	if(!IsOwnedByRangeObject(ref))
	{
		return true;
	}

	bool res = true;
	bool IsReference = false;
	const SVGUID& guid = ref.GetEmbeddedID();
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
			HRESULT hrFind = SVObjectManagerClass::Instance().GetObjectByDottedName( value.GetString(), ObjectRefValue );

			if( !value.IsEmpty() && ( S_OK != hrFind || nullptr == ObjectRefValue.Object() ) )
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
	if(m_pRange && m_pRange->GetOwner())
	{
		ret = m_pRange->GetOwner()->GetName();
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

SVString RangeClassHelper::GetFailHighString()
{
	return GetValueString(m_FailHighIndirect, m_FailHigh);
}

SVString RangeClassHelper::GetWarnHighString()
{
	return GetValueString(m_WarnHighIndirect, m_WarnHigh);
}

SVString RangeClassHelper::GetFailLowString()
{
	return GetValueString(m_FailLowIndirect, m_FailLow);
}

SVString RangeClassHelper::GetWarnLowString()
{
	return GetValueString(m_WarnLowIndirect, m_WarnLow);
}

bool RangeClassHelper::FillObjectSelector()
{
	bool bRetVal = false;
	if (nullptr != m_pRange)
	{
		SVInspectionProcess* pInspectionProcess = dynamic_cast<SVInspectionProcess*>(m_pRange->GetInspection());
		if (nullptr != pInspectionProcess)
		{
			SVTaskObjectListClass* pTaskObjectList = dynamic_cast<SVTaskObjectListClass*>(pInspectionProcess->GetToolSet());
			if (nullptr != pTaskObjectList)
			{
				SVString PPQName = SvOl::FqnPPQVariables; 
				SVString InspectionName = pInspectionProcess->GetName();
				SVPPQObject* pPPQ = pInspectionProcess->GetPPQ();
				if( nullptr != pPPQ )
				{
					PPQName = pPPQ->GetName();
				}
				SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterInput, InspectionName, SVString( _T("") ) );
				SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterOutput, InspectionName, SVString( _T("") ) );

				SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterOutput, PPQName, SVString( _T("")  ));
				SvOsl::ObjectTreeGenerator::Instance().setSelectorType( SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeSingleObject );

				SvOsl::SelectorOptions BuildOptions( pInspectionProcess->GetUniqueObjectID(), SV_SELECTABLE_FOR_EQUATION, m_pRange->GetUniqueObjectID() );
				SvOsl::ObjectTreeGenerator::Instance().BuildSelectableItems<SvOg::GlobalSelector, SvOg::NoSelector, SvOg::ToolSetItemSelector<GuiCmd::RangeSelectorFilterType>>( BuildOptions );
				bRetVal = true;
			}//@TODO return meaningful error on else
		}//@TODO return meaningful error on else
	}//@TODO return meaningful error on else
	return bRetVal;
}

#pragma endregion Public Methods

#pragma region Private Methods
SVString RangeClassHelper::GetValueString(const CString& indirectString, double directValue)
{
	if(indirectString.GetLength() > 0)
	{
		return SVString(indirectString);
	}
	else
	{
		SVString csText = SvUl_SF::Format(_T("%lf"), directValue );
		return csText;
	}
}

bool RangeClassHelper::isValidReference( const CString& rInspectionName, const CString& rIndirectString ) const
{
	bool Result( true );

	SVValueObjectReference objRef;
	CString dottedName;
	CString ToolSetName;
	ToolSetName.LoadString( IDS_CLASSNAME_SVTOOLSET );
	//If the tool set name is at the start then add the inspection name at the beginning
	if( 0 == rIndirectString.Find( ToolSetName ))
	{
		dottedName = rInspectionName + rIndirectString;
	}
	else
	{
		dottedName = rIndirectString;
	}

	if( !SVRangeClass::SetReference(dottedName, objRef) )
	{
		Result = false;
	}
	else if( 0 == ( objRef.ObjectAttributesAllowed() & SV_SELECTABLE_FOR_EQUATION ) )
	{
		Result = false;
	}

	return Result;
}

bool RangeClassHelper::RenameIndirectValue(CString& rIndirectString, LPCTSTR oldPefix, LPCTSTR newPrefix)
{
	bool Result( false );
	SVString oldString(oldPefix);
	if ('.' == oldString[oldString.size()-1])
	{	//check if part of the name (ends with '.') is to replace
		if(rIndirectString.Replace(oldPefix,newPrefix) > 0)
		{
			Result = true;
		}
	}
	else
	{
		SVString indirectTmp = rIndirectString;
		size_t pos = indirectTmp.find('[');
		if (SVString::npos != pos)
		{	//if array ("[x]") in the name, remove it for the check
			indirectTmp = indirectTmp.substr(0, pos);
		}
		//only replace the name if it is the fully name. Do NOT replace parts of the name, because then it this a other object with similar name.
		if (oldString == indirectTmp && rIndirectString.Replace(oldPefix,newPrefix) > 0)
		{	
			Result = true;
		}
	}
	return Result;
}
#pragma endregion Private Methods

