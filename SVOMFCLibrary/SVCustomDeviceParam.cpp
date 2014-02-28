//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCustomDeviceParam
//* .File Name       : $Workfile:   SVCustomDeviceParam.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   28 Feb 2014 09:31:46  $
//******************************************************************************
#include "Stdafx.h"
#include "SVCustomDeviceParam.h"

// Not particularly useful... (just here to statisfy factory signature)
SVCustomDeviceParam::SVCustomDeviceParam()
: m_holder(SVDeviceParamWrapper(SVNullDeviceParam()))
{
}

SVCustomDeviceParam::SVCustomDeviceParam(const SVCustomDeviceParam& rhs) 
: SVDeviceParam( rhs )
, m_holder(rhs.m_holder)
{
}

SVCustomDeviceParam::~SVCustomDeviceParam()
{
	m_holder.Clear();
}

SVClonable* SVCustomDeviceParam::CloneImpl() const
{
	return new SVCustomDeviceParam(*this);
}

SVCustomDeviceParam& SVCustomDeviceParam::operator=(const SVCustomDeviceParam& rhs)
{
	if (this != &rhs)
	{
		m_holder = rhs.m_holder;
	}
	return *this;
}

HRESULT SVCustomDeviceParam::GetValue(VARIANT& rv) const
{
	return m_holder->GetValue(rv);
}

HRESULT SVCustomDeviceParam::SetValue(const VARIANT& rv)
{
	return m_holder->SetValue(rv);
}

HRESULT SVCustomDeviceParam::SetMetadata(const SVDeviceParam* pBaseParam)
{
	HRESULT hr = S_FALSE;
	if (const SVCustomDeviceParam* pParam = dynamic_cast<const SVCustomDeviceParam*>(pBaseParam))
	{
		m_holder->SetMetadata(pBaseParam);
	}
	return hr;
}

SVDeviceParam* SVCustomDeviceParam::Create(SVDeviceParamEnum typeEnum, const VARIANT& rv)
{
	SVCustomDeviceParam* pParam(NULL);

	switch (rv.vt)
	{
		case VT_BSTR:
		{
			SVStringValueDeviceParam* pStringValueParam = new SVStringValueDeviceParam(typeEnum);
			pStringValueParam->SetValue(rv);
			pParam = new SVCustomDeviceParam(pStringValueParam);
			delete pStringValueParam;
		}
		break;
			
		case VT_I4:
		{
			SVLongValueDeviceParam* pLongValueParam = new SVLongValueDeviceParam(typeEnum);
			pLongValueParam->SetValue(rv);
			pParam = new SVCustomDeviceParam(pLongValueParam);
			delete pLongValueParam;
		}
		break;
			
		case VT_BOOL:
		{
			SVBoolValueDeviceParam* pBoolValueParam = new SVBoolValueDeviceParam(typeEnum);
			pBoolValueParam->SetValue(rv);
			pParam = new SVCustomDeviceParam(pBoolValueParam);
			delete pBoolValueParam;
		}
		break;
			
		case VT_I8:
		{
			SVi64ValueDeviceParam* pI64ValueParam = new SVi64ValueDeviceParam(typeEnum);
			pI64ValueParam->SetValue(rv);
			pParam = new SVCustomDeviceParam(pI64ValueParam);
			delete pI64ValueParam;
		}
		break;
	}
	return pParam;
}

SVDeviceParamWrapper& SVCustomDeviceParam::GetHeldParam()
{
	return m_holder;
}

const SVDeviceParamWrapper& SVCustomDeviceParam::GetHeldParam() const
{
	return m_holder;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVOMFCLibrary\SVCustomDeviceParam.cpp_v  $
 * 
 *    Rev 1.2   28 Feb 2014 09:31:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  890
 * SCR Title:  Fix SVObserver Memory Leaks
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   delete device param leaks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 May 2013 11:10:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 May 2013 15:51:12   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  820
 * SCR Title:  Baumer Camera Trigger Debounce
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added destructor to correct memory leak
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 13:01:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   01 Apr 2013 13:38:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  820
 * SCR Title:  Baumer Camera Trigger Debounce
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
