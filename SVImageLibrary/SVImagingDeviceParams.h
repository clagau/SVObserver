// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
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
#include "SVLut.h"
#include "SVLightReference.h"
#include "SVCameraFormat.h"
#include "CameraLibrary/SVDeviceParams.h"
#include "CameraLibrary/SVStringValueDeviceParam.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

// for now, still need to update the enum in SVDeviceParams.h

class SVLutDeviceParam : public SVDeviceParam
{
public:
	SVLutDeviceParam();
	SVLutDeviceParam(SVDeviceParamEnum typeEnum);
	SVLutDeviceParam(const SVLutDeviceParam& rhs);
	virtual ~SVLutDeviceParam();

	virtual SVClonable* CloneImpl() const override;
	SVLutDeviceParam& operator = (const SVLutDeviceParam& rhs);

	SVLut lut;

	static SVDeviceParam* CreateNew() { return new SVLutDeviceParam; }
	DEFINE_VISITABLE()
};

#if defined(_DEBUG) || defined (_MINDEBUG)
	SVLut& Lut(SVDeviceParamWrapper& w);
#else
	inline SVLut& Lut(SVDeviceParamWrapper& w) {SVLutDeviceParam* p = w.DerivedValue(p); if (nullptr == p) {w = SVLutDeviceParam(); p = w.DerivedValue(p);} assert(p); return p->lut;}
#endif

class SVLightReferenceDeviceParam : public SVDeviceParam
{
public:
	SVLightReferenceDeviceParam();
	SVLightReferenceDeviceParam(SVDeviceParamEnum typeEnum);
	SVLightReferenceDeviceParam( const SVLightReferenceDeviceParam& rhs);
	virtual ~SVLightReferenceDeviceParam();

	virtual SVClonable* CloneImpl() const override;
	SVLightReferenceDeviceParam& operator = (const SVLightReferenceDeviceParam& rhs);

	SVLightReference lr;

	static SVDeviceParam* CreateNew() { return new SVLightReferenceDeviceParam; }
	DEFINE_VISITABLE()
};

#if defined(_DEBUG) || defined (_MINDEBUG)
	SVLightReference& LR(SVDeviceParamWrapper& w);
#else
	inline SVLightReference& LR(SVDeviceParamWrapper& w) {SVLightReferenceDeviceParam* p = w.DerivedValue(p); if (nullptr == p) {w = SVLightReferenceDeviceParam(); p = w.DerivedValue(p);} assert(p); return p->lr;}
#endif

class SVCameraFormatsDeviceParam : public SVStringValueDeviceParam
{
public:
	SVCameraFormatsDeviceParam();
	SVCameraFormatsDeviceParam(SVDeviceParamEnum typeEnum);
	SVCameraFormatsDeviceParam(const SVCameraFormatsDeviceParam& rhs);
	virtual ~SVCameraFormatsDeviceParam();

	virtual SVClonable* CloneImpl() const override;
	const SVCameraFormatsDeviceParam& operator=(const SVCameraFormatsDeviceParam& rhs);
	bool SupportsColor() const;

	typedef std::map<SVString, SVCameraFormat> OptionsType;
	OptionsType options;

	virtual HRESULT SetMetadata(const SVDeviceParam* pParam) override;

	static SVDeviceParam* CreateNew() { return new SVCameraFormatsDeviceParam; }
	DEFINE_VISITABLE()
};

