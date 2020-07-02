//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCustomDeviceParam
//* .File Name       : $Workfile:   SVCustomDeviceParam.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   28 Feb 2014 09:35:22  $
//******************************************************************************

#pragma once

#include "Stdafx.h"

#include "SVDeviceParam.h"

class SVCustomDeviceParam : public SVDeviceParam
{
private:
	SVDeviceParamWrapper m_holder;

public:
	SVCustomDeviceParam();

	template<typename T>
	explicit SVCustomDeviceParam(const T& value)
	: SVDeviceParam(value->Type())
	, m_holder(value) 
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
	virtual SVClonable* CloneImpl() const override;
	SVCustomDeviceParam& operator=(const SVCustomDeviceParam& rhs);

	virtual HRESULT SetMetadata(const SVDeviceParam* pParam) override;

	HRESULT GetValue(VARIANT& rv) const override;
	HRESULT SetValue(const VARIANT& rv) override;

	SVDeviceParamWrapper& GetHeldParam();
	const SVDeviceParamWrapper& GetHeldParam() const;
	static SVDeviceParam* CreateNew(SVDeviceParamEnum ) { return new SVCustomDeviceParam(); }
	
	static SVDeviceParam* Create(SVDeviceParamEnum typeEnum, const VARIANT& rv);

	DEFINE_VISITABLE()
};
