//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStringValueDeviceParam
//* .File Name       : $Workfile:   SVStringValueDeviceParam.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 13:14:34  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVDeviceParam.h"
#include "SVDeviceParams.h"
#pragma endregion Includes
#include "SVStatusLibrary/MessageManagerHelper.h"

class SVStringValueDeviceParam : public SVDeviceParam
{
public:
	SVStringValueDeviceParam();
	explicit SVStringValueDeviceParam(SVDeviceParamEnum typeEnum);
	SVStringValueDeviceParam(const SVStringValueDeviceParam& rRhs);
	explicit SVStringValueDeviceParam(const std::string& rRhs);
	virtual ~SVStringValueDeviceParam();
	virtual SVClonable* CloneImpl() const override;

	// automatic conversions
	SVStringValueDeviceParam& operator=(const SVStringValueDeviceParam& rRhs);
	SVStringValueDeviceParam& operator=(const std::string& rValue);
	virtual HRESULT GetValue(VARIANT& rv) const override;
	virtual HRESULT SetValue(const VARIANT& rv) override;

	std::string strValue {};

	TDeviceParamInfo<std::string> info;
	
	virtual HRESULT SetMetadata(const SVDeviceParam* pParam ) override;

	typedef TDeviceParamInfo<std::string>::OptionType  OptionType;
	typedef TDeviceParamInfo<std::string>::OptionsType OptionsType;

	static SVDeviceParam* CreateNew(SVDeviceParamEnum typeEnum) { return new SVStringValueDeviceParam(typeEnum); }
	DEFINE_VISITABLE()
};

#if defined(_DEBUG)
	std::string& StringValue(SVDeviceParamWrapper& w);
	const std::string& StringValue(const SVDeviceParamWrapper& w);
	SVDeviceParamWrapper DeviceParam(const std::string& s);
#else
	inline std::string& StringValue(SVDeviceParamWrapper& w) {SVStringValueDeviceParam* p = w.DerivedValue(p); if (nullptr == p) {w = SVStringValueDeviceParam(); p = w.DerivedValue(p);} Log_Assert(p); return p->strValue;}
	inline const std::string& StringValue(const SVDeviceParamWrapper& w) {const SVStringValueDeviceParam* p = w.DerivedValue(p); if (p) return p->strValue; else {Log_Assert(false); static std::string s(""); return s;} }
	inline SVDeviceParamWrapper DeviceParam(const std::string& s) {return SVStringValueDeviceParam(s);}
#endif


