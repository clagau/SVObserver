//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
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
#include "SVDeviceParams.h"
#pragma endregion Includes

class SVi64ValueDeviceParam : public SVDeviceParam
{
public:
	SVi64ValueDeviceParam();
	SVi64ValueDeviceParam(SVDeviceParamEnum typeEnum);
	SVi64ValueDeviceParam(const SVi64ValueDeviceParam& rhs);
	virtual ~SVi64ValueDeviceParam();
	virtual SVClonable* CloneImpl() const override;

	// automatic conversions
	SVi64ValueDeviceParam(const __int64& rhs);
	SVi64ValueDeviceParam(const LARGE_INTEGER& rhs);
	operator __int64() const;
	SVi64ValueDeviceParam& operator=(const SVi64ValueDeviceParam& rhs);
	virtual HRESULT GetValue(VARIANT& rv) const override;
	virtual HRESULT SetValue(const VARIANT& rv) override;

	__int64 iValue;

	TDeviceParamInfo<__int64> info;
	typedef TDeviceOption<__int64> OptionType;
	typedef std::vector<OptionType> OptionsType;

	virtual HRESULT SetMetadata(const SVDeviceParam* pParam) override;

	typedef TDeviceParamInfo<__int64>::OptionType  OptionType;
	typedef TDeviceParamInfo<__int64>::OptionsType OptionsType;

	static SVDeviceParam* CreateNew() { return new SVi64ValueDeviceParam; }
	DEFINE_VISITABLE()

private:
	void init();
};

#if defined(_DEBUG) || defined (_MINDEBUG)
	__int64& i64Value(SVDeviceParamWrapper& w);
	__int64 i64Value(const SVDeviceParamWrapper& w);
	SVDeviceParamWrapper DeviceParam(__int64 i);
#else
	inline __int64& i64Value(SVDeviceParamWrapper& w) {SVi64ValueDeviceParam* p = w.DerivedValue(p); if (nullptr == p) {w = SVi64ValueDeviceParam(); p = w.DerivedValue(p);} ASSERT(p); return p->iValue;}
	inline __int64 i64Value(const SVDeviceParamWrapper& w) {const SVi64ValueDeviceParam* p = w.DerivedValue(p); if (p) return p->iValue; else {ASSERT(FALSE); return 0;} }
	inline SVDeviceParamWrapper DeviceParam(__int64 i) {return SVi64ValueDeviceParam(i);}
#endif


