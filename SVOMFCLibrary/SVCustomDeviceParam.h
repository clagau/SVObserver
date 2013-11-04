//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCustomDeviceParam
//* .File Name       : $Workfile:   SVCustomDeviceParam.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   13 May 2013 11:10:18  $
//******************************************************************************
#ifndef INCL_SVCUSTOMDEVICEPARAM_H
#define INCL_SVCUSTOMDEVICEPARAM_H

#include "Stdafx.h"

#include "SVBoolValueDeviceParam.h"
#include "SVLongValueDeviceParam.h"
#include "SVStringValueDeviceParam.h"
#include "SVI64ValueDeviceParam.h"
#include "SVNullDeviceParam.h"

class SVCustomDeviceParam : public SVDeviceParam
{
private:
	SVDeviceParamWrapper m_holder;

public:
	SVCustomDeviceParam();

	template<typename T>
	SVCustomDeviceParam(const T& value) 
	: SVDeviceParam(value->Type())
	, m_holder(SVDeviceParamWrapper(value))
	{
		m_eDataType = DeviceDataTypeCustom;
		// copy attributes from holder
		SetName(m_holder->Name());
		SetVisualName(m_holder->VisualName());
		SetDescription(m_holder->Description());
		SetDetailLevel(m_holder->DetailLevel());
		SetOrder(m_holder->GetOrder());
	}
	
	SVCustomDeviceParam(const SVCustomDeviceParam& rhs);

	virtual ~SVCustomDeviceParam();
	virtual SVClonable* CloneImpl() const;
	SVCustomDeviceParam& operator=(const SVCustomDeviceParam& rhs);

	virtual HRESULT SetMetadata(const SVDeviceParam* pParam);

	HRESULT GetValue(VARIANT& rv) const;
	HRESULT SetValue(const VARIANT& rv);

	SVDeviceParamWrapper& GetHeldParam();
	const SVDeviceParamWrapper& GetHeldParam() const;
	static SVDeviceParam* CreateNew() { return new SVCustomDeviceParam; }
	
	static SVDeviceParam* Create(SVDeviceParamEnum typeEnum, const VARIANT& rv);

	DEFINE_VISITABLE()
};

#endif
//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOMFCLibrary\SVCustomDeviceParam.h_v  $
 * 
 *    Rev 1.1   13 May 2013 11:10:18   bWalter
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
 *    Rev 1.0   25 Apr 2013 13:01:32   bWalter
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
