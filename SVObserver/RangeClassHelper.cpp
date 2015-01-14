//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRangeClassHelper
//* .File Name       : $Workfile:   RangeClassHelper.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   13 Jan 2015 13:10:52  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVRange.h"
#include "RangeClassHelper.h"
#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "SVObjectLibrary/SVObjectAsynchronousCommandTemplate.h"
#include "SVObjectLibrary/SVObjectSynchronousCommandTemplate.h"
#include "SVObjectLibrary/SVObjectScriptUsage.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVInspectionProcess.h"
#include "ErrorNumbers.h"
#pragma endregion Includes

double RangeClassHelper::s_MaxValue = 16000000;
double RangeClassHelper::s_MinValue = -16000000;

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

bool RangeClassHelper::Convert2Number(double &Value, LPCTSTR str) const
{
	// @TODO:  Better to use SVString here instead of CString.  (Apply this principle throughout the class.)
	CString number = str;
	int point =0;
	number.Trim();
	number.TrimLeft( _T("+-."));


	int len = number.GetLength();
	if(len == 0) 
	{ 
		return false; 
	}

	for(int i = 0; i < len ; i++)
	{
		TCHAR tchar = number.GetAt(i);
		if( tchar == _T('.'))
		{
			if(++point > 1) 
			{ 
				return false; 
			}
		}

		else if( !_istdigit( tchar ) ) 
		{ 
			return false; 
		}
	}

	Value = _ttof(str);
	return true;
}


HRESULT  RangeClassHelper::SetInternalData(ERange er, LPCTSTR lp, CString &Errorsmsg)
{
	HRESULT hresult = S_OK;
	CString csText = lp;
	double val =0.0;
	if(csText.GetLength() == 0)
	{
		//IDS_IS_MISSING            "ERROR:\n%1 is missing."
		AfxFormatString1(Errorsmsg,IDS_IS_MISSING,ERange2String(er).GetString());
		
		return Err_16019;

	}

	if( Convert2Number( val,lp ) )
	{
		csText = _T("");
		if(val > s_MaxValue || val < s_MinValue)
		{
			//IDS_MUST_BETWEEN           "ERROR:\n%1 must between "
			AfxFormatString1(Errorsmsg,IDS_MUST_BETWEEN,ERange2String(er).GetString());
			CString csValues;
			csValues.Format(_T(" %i and %i"), int(s_MinValue), int(s_MaxValue));
			Errorsmsg += csValues;
			
			return Err_16020;
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
		hresult = Err_16020;
		break;
	}


	return hresult;
}

// @TODO:  Better to use SVString here instead of CString.
// @TODO:  Better to use specific error number here instead of bool as a return.
HRESULT RangeClassHelper::CheckInternalData(CString &csmsg) const
{

	CString InspectionName ;
	if(m_pRange )
	{
		SVInspectionProcess* pInspectionProc =  m_pRange->GetInspection();
		if(pInspectionProc)
		{
			InspectionName = pInspectionProc->GetName();
		}
		
	}
	InspectionName += _T(".");


	CString csFailHigh,csWarnHigh,csFailLow, csWarnLow; 
	csFailHigh.LoadString(IDS_FAIL_HIGH);
	csWarnHigh.LoadString(IDS_WARN_HIGH);
	csFailLow.LoadString(IDS_FAIL_LOW);
	csWarnLow.LoadString(IDS_WARN_LOW);

	
	
	if( m_FailHighIndirect.IsEmpty() && m_WarnHighIndirect.IsEmpty() && m_FailHigh < m_WarnHigh)
	{
		//IDS_IS_LESS_THAN			"ERROR:\n%1\nis less than\n%2"
		AfxFormatString2(csmsg,IDS_IS_LESS_THAN, csFailHigh.GetString(),csWarnHigh.GetString());
		return Err_16012;
	}

	if( m_FailHighIndirect.IsEmpty() && m_WarnLowIndirect.IsEmpty() && m_FailHigh < m_WarnLow)
	{
		//IDS_IS_LESS_THAN			"ERROR:\n%1\nis less than\n%2"
		AfxFormatString2(csmsg,IDS_IS_LESS_THAN, csFailHigh.GetString(),csWarnLow.GetString());
		return Err_16012;
	}
	if( m_FailHighIndirect.IsEmpty() && m_FailLowIndirect.IsEmpty() && m_FailHigh < m_FailLow)
	{
		//IDS_IS_LESS_THAN			"ERROR:\n%1\nis less than\n%2"
		AfxFormatString2(csmsg,IDS_IS_LESS_THAN, csFailHigh.GetString(),csFailLow.GetString());
		return Err_16012;
	}

	if( m_WarnHighIndirect.IsEmpty() && m_WarnLowIndirect.IsEmpty() && m_WarnHigh < m_WarnLow)
	{
		
		//IDS_IS_LESS_THAN			"ERROR:\n%1\nis less than\n%2"
		AfxFormatString2(csmsg,IDS_IS_LESS_THAN, csWarnHigh.GetString(),csWarnLow.GetString());
		return  Err_16013;
	}

	if( m_WarnHighIndirect.IsEmpty() && m_FailLowIndirect.IsEmpty() && m_WarnHigh < m_FailLow)
	{

		//IDS_IS_LESS_THAN			"ERROR:\n%1\nis less than\n%2"
		AfxFormatString2(csmsg,IDS_IS_LESS_THAN, csWarnHigh.GetString(),csFailLow.GetString());
		return  Err_16013;
	}

	if( m_WarnLowIndirect.IsEmpty() && m_FailLowIndirect.IsEmpty() && m_WarnLow < m_FailLow)
	{
		
		AfxFormatString2(csmsg,IDS_IS_LESS_THAN,csWarnLow.GetString(),csFailLow.GetString());
		return  Err_16014;
	}

	if( !m_FailHighIndirect.IsEmpty() )
	{
		SVValueObjectReference objRef;
		CString dottetName = InspectionName + m_FailHighIndirect;
		if( false == SVRangeClass::SetReference(dottetName, objRef) )
		{
			//IDS_ISANINVALID_REFERENCE  "ERROR:\n%1: %2\nis an invalid reference."
			AfxFormatString2(csmsg,IDS_ISANINVALID_REFERENCE,csFailHigh.GetString(),m_FailHighIndirect.GetString() );
			return Err_16015;
		}
	}

	if( !m_WarnHighIndirect.IsEmpty() )
	{
		SVValueObjectReference objRef;
		CString dottetName = InspectionName + m_WarnHighIndirect;
		if( false == SVRangeClass::SetReference(dottetName, objRef) )
		{
			//IDS_ISANINVALID_REFERENCE  "ERROR:\n%1: %2\nis an invalid reference."
			AfxFormatString2(csmsg,IDS_ISANINVALID_REFERENCE,csWarnHigh.GetString(),m_WarnHighIndirect.GetString() );
			return  Err_16016;
		}
	}

	if( !m_WarnLowIndirect.IsEmpty() )
	{
		SVValueObjectReference objRef;
		CString dottetName = InspectionName + m_WarnLowIndirect;
		if( false == SVRangeClass::SetReference(dottetName,objRef) )
		{
			//IDS_ISANINVALID_REFERENCE  "ERROR:\n%1: %2\nis an invalid reference."
			AfxFormatString2(csmsg,IDS_ISANINVALID_REFERENCE,csWarnLow.GetString(),m_WarnLowIndirect.GetString() );
			return Err_16017;
		}
	}

	if(!m_FailLowIndirect.IsEmpty())
	{
		SVValueObjectReference objRef;
		CString dottetName = InspectionName + m_FailLowIndirect;
		if(false == SVRangeClass::SetReference(dottetName,objRef ))
		{
			AfxFormatString2(csmsg,IDS_ISANINVALID_REFERENCE,csFailLow.GetString(),m_FailLowIndirect.GetString() );
			return Err_16018;
		}
	}
	
	return S_OK;
}

HRESULT RangeClassHelper::SetInspectionData()
{
	HRESULT hr = Err_16011;
	if( m_pRange )
	{
		hr = AddInputRequest( &( m_pRange->FailHigh ), m_FailHigh );

		if( hr != S_OK )
		{
			hr = Err_16001; 
		}
		else
		{
			hr = AddInputRequest( &( m_pRange->FailLow ),  m_FailLow );
		}


		if(hr !=  S_OK)
		{
			hr = Err_16002;

		}
		else
		{
			hr = AddInputRequest( &( m_pRange->WarnHigh ), m_WarnHigh );
		}
		if( hr != S_OK )
		{
			hr = Err_16003; 
		}
		else
		{
			hr = AddInputRequest( &( m_pRange->WarnLow ),  m_WarnLow );
		}

		if( hr != S_OK )
		{
			hr = Err_16004; 
		}
		else
		{
			hr = AddInputRequest( m_pRange->GetIndirectObject(ER_FailHigh) , m_FailHighIndirect  );
		}
		if( hr != S_OK )
		{
			hr = Err_16005; 
		}
		else
		{
			hr = AddInputRequest(  m_pRange->GetIndirectObject(ER_FailLow) ,  m_FailLowIndirect  );
		}
		if( hr != S_OK )
		{
			hr = Err_16006; 
		}
		else
		{
			hr = AddInputRequest(  m_pRange->GetIndirectObject(ER_WarnHigh), m_WarnHighIndirect  );
		}

		if( hr != S_OK )
		{
			hr = Err_16007; 
		}
		else

		{
			hr = AddInputRequest(m_pRange->GetIndirectObject(ER_WarnLow),  m_WarnLowIndirect  );
		}

		if( hr != S_OK )
		{
			hr = Err_16008; 
		}
		else
		{
			hr = AddInputRequestMarker();
		}
		if( hr != S_OK )
		{
			hr = Err_16009; 
		}
		else
		{
			hr = RunOnce( m_pRange->GetTool() );
		}
		if( hr != S_OK )
		{
			hr = Err_16010; 
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
		//ret = _T("Fail High");
		ret.LoadString(IDS_FAIL_HIGH);
		break;
	case ER_WarnHigh:
		//ret = _T("Warn High");
		ret.LoadString(IDS_WARN_HIGH);
		break;
	case ER_FailLow:
		//ret = _T("Fail Low");
		ret.LoadString(IDS_FAIL_LOW);
		break;
	case ER_WarnLow:
		//ret = _T("Warn Low");
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

	m_pRange->GetIndirectValue(ra ,ret);

	if(ret.IsEmpty())
	{
		if(m_pRange->GetValue(ra ,val)== S_OK)
		{
			ret.Format(_T("%lf"), val);
		}
	}

	return ret;
};

bool RangeClassHelper::IsOwnedByRangeObject(const SVObjectClass& ref)
{
	bool result = false; 

	// @TODO:  Keep the owner and avoid calling GetOwner twice.
	// GetOwner is inline and returns a pointer to the owner 
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

bool RangeClassHelper::RenameIndirectValues(LPCTSTR oldPefix , LPCTSTR newPrefix )
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

#pragma endregion Public Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSARCH65\PROJECTFILES\ARCHIVES\SVOBSERVER_SRC\SVObserver\RangeClassHelper.cpp_v  $
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