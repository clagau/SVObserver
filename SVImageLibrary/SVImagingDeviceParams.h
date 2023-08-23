// ******************************************************************************
// * COPYRIGHT (c) 2002 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVImagingDeviceParams
// * .File Name       : $Workfile:   SVImagingDeviceParams.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 10:51:32  $ 
// ******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
#include "SVOLibrary/SVLut.h"
#include "SVLightReference.h"
#include "SVCameraFormat.h"
#include "CameraLibrary/SVDeviceParams.h"
#include "CameraLibrary/SVStringValueDeviceParam.h"

#pragma endregion Includes

// for now, still need to update the enum in SVDeviceParams.h

class SVLutDeviceParam : public SVDeviceParam
{
public:
	SVLutDeviceParam();
	explicit SVLutDeviceParam(SVDeviceParamEnum typeEnum);
	SVLutDeviceParam(const SVLutDeviceParam& rhs);
	virtual ~SVLutDeviceParam();

	virtual SVClonable* CloneImpl() const override;
	SVLutDeviceParam& operator = (const SVLutDeviceParam& rhs);

	SVLut lut;

	static SVDeviceParam* CreateNew(SVDeviceParamEnum typeEnum) { return new SVLutDeviceParam(typeEnum); }
	DEFINE_VISITABLE()
};

#if defined(_DEBUG)
	SVLut& Lut(SVDeviceParamWrapper& w);
#else
	inline SVLut& Lut(SVDeviceParamWrapper& w) {SVLutDeviceParam* p = w.DerivedValue(p); if (nullptr == p) {w = SVLutDeviceParam(); p = w.DerivedValue(p);} Log_Assert(p); return p->lut;}
#endif

class SVLightReferenceDeviceParam : public SVDeviceParam
{
public:
	SVLightReferenceDeviceParam();
	explicit SVLightReferenceDeviceParam(SVDeviceParamEnum typeEnum);
	SVLightReferenceDeviceParam( const SVLightReferenceDeviceParam& rhs);
	virtual ~SVLightReferenceDeviceParam();

	virtual SVClonable* CloneImpl() const override;
	SVLightReferenceDeviceParam& operator = (const SVLightReferenceDeviceParam& rhs);

	SVLightReference lr;

	static SVDeviceParam* CreateNew(SVDeviceParamEnum typeEnum) { return new SVLightReferenceDeviceParam(typeEnum); }
	DEFINE_VISITABLE()
};

#if defined(_DEBUG)
	SVLightReference& LR(SVDeviceParamWrapper& w);
#else
	inline SVLightReference& LR(SVDeviceParamWrapper& w) {SVLightReferenceDeviceParam* p = w.DerivedValue(p); if (nullptr == p) {w = SVLightReferenceDeviceParam(); p = w.DerivedValue(p);} Log_Assert(p); return p->lr;}
#endif

class SVCameraFormatsDeviceParam : public SVStringValueDeviceParam
{
public:
	SVCameraFormatsDeviceParam();
	explicit SVCameraFormatsDeviceParam(SVDeviceParamEnum typeEnum);
	SVCameraFormatsDeviceParam(const SVCameraFormatsDeviceParam& rhs);
	virtual ~SVCameraFormatsDeviceParam();

	virtual SVClonable* CloneImpl() const override;
	const SVCameraFormatsDeviceParam& operator=(const SVCameraFormatsDeviceParam& rhs);
	bool SupportsColor() const;
	bool SupportsMono() const;

	typedef std::map<std::string, SVCameraFormat> OptionsType;
	OptionsType options;

	virtual HRESULT SetMetadata(const SVDeviceParam* pParam) override;

	static SVDeviceParam* CreateNew(SVDeviceParamEnum typeEnum) { return new SVCameraFormatsDeviceParam(typeEnum); }
	DEFINE_VISITABLE()
};

