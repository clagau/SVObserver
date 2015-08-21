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
#include "SVIOLibrary\SVIOConfigurationInterfaceClass.h"
#include "SVObjectLibrary\SVObjectAsynchronousCommandTemplate.h"
#include "SVObjectLibrary\SVObjectSynchronousCommandTemplate.h"
#include "SVObjectLibrary\SVObjectScriptUsage.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVInspectionProcess.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "SVUtilityLibrary\SVString.h"
#include "SVTaskObjectList.h"
#include "ObjectSelectorLibrary\ObjectTreeGenerator.h"
#include "SVObjectLibrary\GlobalConst.h"
#include "SVToolSet.h"
#include "SVTool.h"
#include "SVPPQObject.h"
#include "ObjectNameHelper.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVStatusLibrary\MessageManagerResource.h"
#include "TextDefinesSvO.h"
#include "RootObject.h"
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
void RangeClassHelper::setRangeObject(SVRangeClass* PRange)
{
	m_pRange = PRange;
}

void RangeClassHelper::SetRangeTaskObject()
{
	SetTaskObject(m_pRange);
}

HRESULT RangeClassHelper::GetInspectionData(enum ERange ra)
{
	HRESULT hr = E_FAIL;
	switch (ra)
	{
	case ER_FailHigh:
		hr = m_pRange->GetValue(ER_FailHigh, m_FailHigh);
		if( hr == S_OK )
		{
			hr = m_pRange->GetIndirectValue(ER_FailHigh, m_FailHighIndirect);
		}
		break;

	case ER_WarnHigh:
		hr = m_pRange->GetValue(ER_WarnHigh, m_WarnHigh);
		if( hr == S_OK )
		{
			hr = m_pRange->GetIndirectValue(ER_WarnHigh, m_WarnHighIndirect);
		}
		break;

	case ER_FailLow:
		hr = m_pRange->GetValue(ER_FailLow, m_FailLow);
		if( hr == S_OK )
		{
			hr = m_pRange->GetIndirectValue(ER_FailLow, m_FailLowIndirect);
		}
		break;

	case ER_WarnLow:
		hr = m_pRange->GetValue(ER_WarnLow, m_WarnLow);
		if( hr == S_OK )
		{
			hr = m_pRange->GetIndirectValue(ER_WarnLow, m_WarnLowIndirect);
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
	for(int i = 0; i < ER_COUNT; i++)
	{
		if(hr == S_OK)
		{
			hr = GetInspectionData(ERange(i));
		}
		else
		{
			break;
		}
	}

	return hr;
}

void RangeClassHelper::SetInternalData(ERange er, LPCTSTR lp)
{
	CString csText = lp;
	double val = 0.0;
	const double s_RangeMax = 17000000;
	const double s_RangeMin = -17000000;
	int textLength = csText.GetLength();

	if( textLength == 0)
	{
		CString strText;
		strText.Format(SvO::RangeValue_EmptyString, ERange2String(er).GetString());
		SvStl::MessageMgrNoDisplay Exception( SvStl::DataOnly );
		Exception.setMessage( SVMSG_SVO_68_RANGE_VALUE_SET_FAILED, strText, StdMessageParams, SvOi::Err_16022, MB_OK | MB_ICONERROR );
		Exception.Throw();
	}

	SVString text = lp;
	bool isNumber = text.Convert2Number<double>(val, true);
	if( isNumber )
	{
		csText = _T("");
		if(val > s_RangeMax || val < s_RangeMin )
		{
			CString strText;
			strText.Format(SvO::RangeValue_WrongRange, ERange2String(er).GetString(), static_cast< int >( s_RangeMin ), static_cast< int >( s_RangeMax ) );
			SvStl::MessageMgrNoDisplay Exception( SvStl::DataOnly );
			Exception.setMessage( SVMSG_SVO_68_RANGE_VALUE_SET_FAILED, strText, StdMessageParams, SvOi::Err_16023, MB_OK | MB_ICONERROR );
			Exception.Throw();
		}
	}

	switch (er)
	{
	case ER_FailHigh:
		m_FailHigh = val;
		m_FailHighIndirect = csText;
		break;
	case ER_WarnHigh:
		m_WarnHigh = val;
		m_WarnHighIndirect = csText;
		break;
	case ER_FailLow:
		m_FailLow = val;
		m_FailLowIndirect = csText;
		break;
	case ER_WarnLow:
		m_WarnLow = val;
		m_WarnLowIndirect = csText;
		break;
	default:
		SvStl::MessageMgrNoDisplay Exception( SvStl::DataOnly );
		Exception.setMessage( SVMSG_SVO_68_RANGE_VALUE_SET_FAILED, SvO::ErrorUnknownEnum, StdMessageParams, SvOi::Err_16024, MB_OK | MB_ICONERROR );
		Exception.Throw();
		break;
	}
}

// @TODO:  Better to use SVString here instead of CString.
HRESULT RangeClassHelper::CheckInternalData(CString &csmsg) const
{
	CString InspectionName;
	if( nullptr != m_pRange)
	{
		SVInspectionProcess* pInspection =  m_pRange->GetInspection();
		if( nullptr != pInspection )
		{
			InspectionName = pInspection->GetName();
		}
	}
	InspectionName += _T(".");

	CString csFailHigh, csWarnHigh, csFailLow, csWarnLow;
	csFailHigh.LoadString(IDS_FAIL_HIGH);
	csWarnHigh.LoadString(IDS_WARN_HIGH);
	csFailLow.LoadString(IDS_FAIL_LOW);
	csWarnLow.LoadString(IDS_WARN_LOW);

	if( m_FailHighIndirect.IsEmpty() && m_WarnHighIndirect.IsEmpty() && m_FailHigh < m_WarnHigh )
	{
		//IDS_IS_LESS_THAN			"ERROR:\n%1\nis less than\n%2"
		AfxFormatString2( csmsg, IDS_IS_LESS_THAN, csFailHigh.GetString(), csWarnHigh.GetString() );
		return -SvOi::Err_16012;
	}

	if( m_FailHighIndirect.IsEmpty() && m_WarnLowIndirect.IsEmpty() && m_FailHigh < m_WarnLow )
	{
		//IDS_IS_LESS_THAN			"ERROR:\n%1\nis less than\n%2"
		AfxFormatString2( csmsg, IDS_IS_LESS_THAN, csFailHigh.GetString(), csWarnLow.GetString() );
		return -SvOi::Err_16013;
	}

	if( m_FailHighIndirect.IsEmpty() && m_FailLowIndirect.IsEmpty() && m_FailHigh < m_FailLow )
	{
		//IDS_IS_LESS_THAN			"ERROR:\n%1\nis less than\n%2"
		AfxFormatString2( csmsg, IDS_IS_LESS_THAN, csFailHigh.GetString(), csFailLow.GetString() );
		return -SvOi::Err_16014;
	}

	if( m_WarnHighIndirect.IsEmpty() && m_WarnLowIndirect.IsEmpty() && m_WarnHigh < m_WarnLow )
	{
		//IDS_IS_LESS_THAN			"ERROR:\n%1\nis less than\n%2"
		AfxFormatString2( csmsg, IDS_IS_LESS_THAN, csWarnHigh.GetString(), csWarnLow.GetString() );
		return -SvOi::Err_16015;
	}

	if( m_WarnHighIndirect.IsEmpty() && m_FailLowIndirect.IsEmpty() && m_WarnHigh < m_FailLow )
	{
		//IDS_IS_LESS_THAN			"ERROR:\n%1\nis less than\n%2"
		AfxFormatString2( csmsg, IDS_IS_LESS_THAN, csWarnHigh.GetString(), csFailLow.GetString() );
		return -SvOi::Err_16016;
	}

	if( m_WarnLowIndirect.IsEmpty() && m_FailLowIndirect.IsEmpty() && m_WarnLow < m_FailLow )
	{
		AfxFormatString2( csmsg, IDS_IS_LESS_THAN, csWarnLow.GetString(), csFailLow.GetString() );
		return -SvOi::Err_16017;
	}

	if( !m_FailHighIndirect.IsEmpty() )
	{
		if( !isValidReference( InspectionName, m_FailHighIndirect ) )
		{
			//IDS_ISANINVALID_REFERENCE  "ERROR:\n%1: %2\nis an invalid reference."
			AfxFormatString2( csmsg, IDS_ISANINVALID_REFERENCE, csFailHigh.GetString(), m_FailHighIndirect.GetString() );
			return -SvOi::Err_16018;
		}
	}

	if( !m_WarnHighIndirect.IsEmpty() )
	{
		if( !isValidReference( InspectionName, m_WarnHighIndirect ) )
		{
			//IDS_ISANINVALID_REFERENCE  "ERROR:\n%1: %2\nis an invalid reference."
			AfxFormatString2( csmsg, IDS_ISANINVALID_REFERENCE, csWarnHigh.GetString(), m_WarnHighIndirect.GetString() );
			return -SvOi::Err_16019;
		}
	}

	if( !m_WarnLowIndirect.IsEmpty() )
	{
		if( !isValidReference( InspectionName, m_WarnLowIndirect ) )
		{
			AfxFormatString2( csmsg, IDS_ISANINVALID_REFERENCE, csWarnLow.GetString(), m_WarnLowIndirect.GetString() );
			return -SvOi::Err_16020;
		}
	}

	if(!m_FailLowIndirect.IsEmpty())
	{
		if( !isValidReference( InspectionName, m_FailLowIndirect ) )
		{
			AfxFormatString2( csmsg, IDS_ISANINVALID_REFERENCE, csFailLow.GetString(), m_FailLowIndirect.GetString() );
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

		if( hr != S_OK )
		{
			hr = -SvOi::Err_16001;
		}
		else
		{
			hr = AddInputRequest( &( m_pRange->FailLow ), m_FailLow );
		}

		if(hr != S_OK)
		{
			hr = -SvOi::Err_16002;
		}
		else
		{
			hr = AddInputRequest( &( m_pRange->WarnHigh ), m_WarnHigh );
		}

		if( hr != S_OK )
		{
			hr = -SvOi::Err_16003;
		}
		else
		{
			hr = AddInputRequest( &( m_pRange->WarnLow ), m_WarnLow );
		}

		if( hr != S_OK )
		{
			hr = -SvOi::Err_16004;
		}
		else
		{
			hr = AddInputRequest( m_pRange->GetIndirectObject(ER_FailHigh), m_FailHighIndirect );
		}

		if( hr != S_OK )
		{
			hr = -SvOi::Err_16005;
		}
		else
		{
			hr = AddInputRequest( m_pRange->GetIndirectObject(ER_FailLow), m_FailLowIndirect );
		}

		if( hr != S_OK )
		{
			hr = -SvOi::Err_16006;
		}
		else
		{
			hr = AddInputRequest( m_pRange->GetIndirectObject(ER_WarnHigh), m_WarnHighIndirect );
		}

		if( hr != S_OK )
		{
			hr = -SvOi::Err_16007;
		}
		else
		{
			hr = AddInputRequest( m_pRange->GetIndirectObject(ER_WarnLow), m_WarnLowIndirect );
		}

		if( hr != S_OK )
		{
			hr = -SvOi::Err_16008;
		}
		else
		{
			hr = AddInputRequestMarker();
		}

		if( hr != S_OK )
		{
			hr = -SvOi::Err_16009;
		}
		else
		{
			hr = RunOnce( m_pRange->GetTool() );
		}

		if( hr != S_OK )
		{
			hr = -SvOi::Err_16010;
		}
	}

	return hr;
}

CString RangeClassHelper::ERange2String(ERange range)
{
	CString ret;
	switch (range)
	{
	case ER_FailHigh:
		ret.LoadString(IDS_FAIL_HIGH);
		break;
	case ER_WarnHigh:
		ret.LoadString(IDS_WARN_HIGH);
		break;
	case ER_FailLow:
		ret.LoadString(IDS_FAIL_LOW);
		break;
	case ER_WarnLow:
		ret.LoadStringA(IDS_WARN_LOW);
		break;
	default:
		ret = _T("");
	}
	return ret;
}

CString RangeClassHelper::GetStringFromRange(enum ERange ra) const
{
	CString ret;
	double val;

	if(nullptr == m_pRange)
	{
		return ret;
	}

	m_pRange->GetIndirectValue(ra, ret);

	if(ret.IsEmpty())
	{
		if(m_pRange->GetValue(ra, val) == S_OK)
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

			if( !value.IsEmpty() && ( hrFind != S_OK || ObjectRefValue.Object() == nullptr ) )
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
	bool result = false;

	if(m_FailHighIndirect.Replace(oldPefix,newPrefix) > 0)
	{
		result = true;
	}

	if(m_WarnHighIndirect.Replace(oldPefix,newPrefix) > 0)
	{
		result = true;
	}

	if(m_FailLowIndirect.Replace(oldPefix,newPrefix) > 0)
	{
		result = true;
	}

	if(m_WarnLowIndirect.Replace(oldPefix,newPrefix) > 0)
	{
		result = true;
	}

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
	if (nullptr == m_pRange)
	{
		return false;
	}

	SVInspectionProcess* pInspectionProcess = m_pRange->GetInspection();
	if(nullptr == pInspectionProcess)
	{
		return false; // @TODO:  Better to return a unique error code.
	}

	SVTaskObjectListClass* pTaskObjectList = dynamic_cast<SVTaskObjectListClass*>(pInspectionProcess->GetToolSet());
	if(nullptr == pTaskObjectList)
	{
		return false; // @TODO:  Better to return a unique error code.
	}

	SVToolClass* pTool = m_pRange->GetTool();
	CString csToolCompleteName;
	if(pTool)
	{
		csToolCompleteName = pTool->GetCompleteObjectName();
		csToolCompleteName += _T(".");
	}

	SVStringArray nameArray;
	typedef std::insert_iterator<SVStringArray> Inserter;
	SVString InspectionName;
	SVString PPQName = SvOl::FqnPPQVariables; 

	InspectionName = pInspectionProcess->GetName();
	SVPPQObject* pPPQ = pInspectionProcess->GetPPQ();
	if( nullptr != pPPQ )
	{
		PPQName = pPPQ->GetName();
	}

	SvOsl::ObjectTreeGenerator::Instance().setAttributeFilters( SV_SELECTABLE_FOR_EQUATION );
	SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterInput, InspectionName, SVString( _T("") ) );
	SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterOutput, InspectionName, SVString( _T("") ) );

	SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterOutput, PPQName, SVString( _T("")  ));
	SvOsl::ObjectTreeGenerator::Instance().setSelectorType( SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeSingleObject );

	SVStringArray objectNameList;
	SvOi::getRootChildNameList( objectNameList, _T(""), SV_SELECTABLE_FOR_EQUATION );
	SvOsl::ObjectTreeGenerator::Instance().insertTreeObjects( objectNameList );

	// Insert Tool Set Objects
	ObjectNameHelper::BuildObjectNameList(pTaskObjectList, Inserter(nameArray, nameArray.begin()), csToolCompleteName);
	SvOsl::ObjectTreeGenerator::Instance().insertTreeObjects( nameArray );
	return true;
}
#pragma endregion Public Methods

#pragma region Private Methods
SVString RangeClassHelper::GetValueString(const CString& indirectString, double directValue)
{
	if(indirectString.GetLength() > 0)
	{
		return indirectString;
	}
	else
	{
		SVString csText;
		csText.Format(_T("%lf"), directValue );
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
#pragma endregion Private Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\RangeClassHelper.cpp_v  $
 * 
 *    Rev 1.5   19 Jan 2015 11:40:44   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  979
 * SCR Title:  Provide additional options to input the feature range for the blob analyzer.
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Re-fixed whitespace.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   15 Jan 2015 08:25:38   mEichengruen
 * Project:  SVObserver
 * Change Request (SCR) nbr:  979
 * SCR Title:  Provide additional options to input the feature range for the blob analyzer.
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Test SV_VIEWABLE flag for Range References
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   14 Jan 2015 16:40:42   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  979
 * SCR Title:  Provide additional options to input the feature range for the blob analyzer.
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Moved and renamed Min/Max members.
 *   Changed return values to be negative and unique.
 *   Cleaned up spacing.
 *   Moved method calls out of if conditions in method SetInternalData.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 Jan 2015 13:10:52   mEichengruen
 * Project:  SVObserver
 * Change Request (SCR) nbr:  979
 * SCR Title:  Provide additional options to input the feature range for the blob analyzer.
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Range Indirect name String without inspection Name 
 * Rename Range Indirect name String when a Toolname  is renamed  
 * add function to rename in direct values 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Jan 2015 11:58:12   mEichengruen
 * Project:  SVObserver
 * Change Request (SCR) nbr:  979
 * SCR Title:  Provide additional options to input the feature range for the blob analyzer.
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Check range constant when using constant and indirect values mixed 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 Dec 2014 13:53:42   mEichengruen
 * Project:  SVObserver
 * Change Request (SCR) nbr:  979
 * SCR Title:  Provide additional options to input the feature range for the blob analyzer.
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Initial Check In for SCR 979 Indirect Range Objects 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/