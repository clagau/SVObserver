//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVi64ValueDeviceParam
//* .File Name       : $Workfile:   SVi64ValueDeviceParam.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 13:07:02  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVDeviceParam.h"
#include "SVDeviceParams.h"
#pragma endregion Includes

class SVi64ValueDeviceParam : public SVDeviceParam
{
public:
	SVi64ValueDeviceParam();
	explicit SVi64ValueDeviceParam(SVDeviceParamEnum typeEnum);
	SVi64ValueDeviceParam(const SVi64ValueDeviceParam& rhs);
	virtual ~SVi64ValueDeviceParam();
	virtual SVClonable* CloneImpl() const override;

	// automatic conversions
	explicit SVi64ValueDeviceParam(const __int64& rhs);
	explicit SVi64ValueDeviceParam(const LARGE_INTEGER& rhs);
	operator __int64() const;
	SVi64ValueDeviceParam& operator=(const SVi64ValueDeviceParam& rhs);
	virtual HRESULT GetValue(VARIANT& rv) const override;
	virtual HRESULT SetValue(const VARIANT& rv) override;

	__int64 iValue {0LL};

	TDeviceParamInfo<__int64> info;
	typedef TDeviceOption<__int64> OptionType;
	typedef std::vector<OptionType> OptionsType;

	virtual HRESULT SetMetadata(const SVDeviceParam* pParam) override;

	typedef TDeviceParamInfo<__int64>::OptionType  OptionType;
	typedef TDeviceParamInfo<__int64>::OptionsType OptionsType;

	static SVDeviceParam* CreateNew(SVDeviceParamEnum typeEnum) { return new SVi64ValueDeviceParam(typeEnum); }
	DEFINE_VISITABLE()

private:
	void init();
};
