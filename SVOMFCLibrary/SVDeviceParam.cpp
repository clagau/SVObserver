// ******************************************************************************
// * COPYRIGHT (c) 2013 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVDeviceParam
// * .File Name       : $Workfile:   SVDeviceParam.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 13:01:48  $
// ******************************************************************************

#include "stdafx.h"
#include "SVDeviceParam.h"
#include "SVDeviceParams.h"

SVDeviceParam::SVDeviceParam()
: m_eParam(DeviceParamInvalid)
, m_eDataType(DeviceDataTypeUnknown)
, m_bSupports(true) 
{
	SetOrder(INT_MAX);
	SetDetailLevel(0);
	Init(DeviceParamInvalid);
}

SVDeviceParam::SVDeviceParam(SVDeviceParamEnum typeEnum)
: m_eParam(typeEnum)
, m_eDataType(DeviceDataTypeUnknown)
, m_bSupports(true) 
{
	SetOrder(INT_MAX);
	SetDetailLevel(0);
}

SVDeviceParam::SVDeviceParam(const SVDeviceParam& rhs)
{
	m_eParam = rhs.Type();
	m_eDataType = rhs.DataType();
	m_bSupports = rhs.DeviceSupports();

	SetOrder( rhs.GetOrder() );
	SetDetailLevel( rhs.DetailLevel() );
	SetName( rhs.Name() );
	SetVisualName( rhs.VisualName() );
	SetDescription( rhs.Description() );
	Init( rhs.Type() );
}

SVDeviceParam::~SVDeviceParam()
{
	m_eParam = DeviceParamInvalid; // really?
}

/*static*/CString SVDeviceParam::GetParameterName(SVDeviceParamEnum e)
{
	CString sName;
	TheDeviceParamFactory::InstanceType::const_iterator iter = TheDeviceParamFactory::Instance().PrimaryMap().find(e);
	if (iter != TheDeviceParamFactory::Instance().PrimaryMap().end())
	{
		sName = iter->second;
	}
	return sName;
}

SVDeviceParam* SVDeviceParam::Create(SVDeviceParamEnum eType)
{
	SVDeviceParam* pParam = NULL;

	try
	{
		pParam = TheDeviceParamFactory::Instance().New(eType);
	}
	catch (std::runtime_error&)
	{
		ASSERT(FALSE);
		TRACE(_T("SVDeviceParam::Create unable to create type %d\n"), (long)eType);
		throw;
	}

	ASSERT(pParam);

	if (pParam != NULL)
	{
		pParam->Init(eType);
	}
	return pParam;
}

HRESULT SVDeviceParam::SetName(const SVString& strName)
{
	m_strName = strName;
	return S_OK;
}

HRESULT SVDeviceParam::SetVisualName( const SVString& strName)
{
	m_strVisualName = strName;
	return S_OK;
}

HRESULT SVDeviceParam::SetDetailLevel( long lDetailLevel )
{
	m_lDetailLevel = lDetailLevel;
	return S_OK;
}

HRESULT SVDeviceParam::SetDescription(const SVString& strDescription)
{
	m_strDescription = strDescription;
	return S_OK;
}

HRESULT SVDeviceParam::SetType( SVDeviceParamEnum e )
{
	if ( m_eParam == DeviceDataTypeUnknown )
	{
		m_eParam = e;
		return S_OK;
	}
	return S_FALSE;
}

void SVDeviceParam::Init( SVDeviceParamEnum e )
{
	m_eParam = e;
}

HRESULT SVDeviceParam::GetValue( VARIANT& rv ) const
{
	return S_FALSE;	// derived class needs to implement
}

HRESULT SVDeviceParam::SetValue( const VARIANT& rv )
{
	return S_FALSE;	// derived class needs to implement
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOMFCLibrary\SVDeviceParam.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 13:01:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   01 Apr 2013 13:38:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  820
 * SCR Title:  Baumer Camera Trigger Debounce
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/