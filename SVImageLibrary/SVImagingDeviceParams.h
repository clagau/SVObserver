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

#ifndef __SVIMAGINGDEVICEPARAMS_H__
#define __SVIMAGINGDEVICEPARAMS_H__

#include <map>
#include "SVLut.h"
#include "SVLightReference.h"
#include "SVCameraFormat.h"

#include "SVOMFCLibrary/SVDeviceParams.h"
#include "SVOMFCLibrary/SVStringValueDeviceParam.h"

// for now, still need to update the enum in SVDeviceParams.h

class SVLutDeviceParam : public SVDeviceParam
{
public:
	SVLutDeviceParam();
	SVLutDeviceParam(SVDeviceParamEnum typeEnum);
	SVLutDeviceParam(const SVLutDeviceParam& rhs);
	virtual SVClonable* CloneImpl() const;
	SVLutDeviceParam& operator = (const SVLutDeviceParam& rhs);

	SVLut lut;

	static SVDeviceParam* CreateNew() { return new SVLutDeviceParam; }
	DEFINE_VISITABLE()
};

#if defined(_DEBUG) || defined (_MINDEBUG)
	SVLut& Lut(SVDeviceParamWrapper& w);
#else
	inline SVLut& Lut(SVDeviceParamWrapper& w) {SVLutDeviceParam* p = w.DerivedValue(p); if (p==NULL) {w = SVLutDeviceParam(); p = w.DerivedValue(p);} ASSERT(p); return p->lut;}
#endif

class SVLightReferenceDeviceParam : public SVDeviceParam
{
public:
	SVLightReferenceDeviceParam();
	SVLightReferenceDeviceParam(SVDeviceParamEnum typeEnum);
	SVLightReferenceDeviceParam( const SVLightReferenceDeviceParam& rhs);
	virtual SVClonable* CloneImpl() const;
	SVLightReferenceDeviceParam& operator = (const SVLightReferenceDeviceParam& rhs);

	SVLightReference lr;

	static SVDeviceParam* CreateNew() { return new SVLightReferenceDeviceParam; }
	DEFINE_VISITABLE()
};

#if defined(_DEBUG) || defined (_MINDEBUG)
	SVLightReference& LR(SVDeviceParamWrapper& w);
#else
	inline SVLightReference& LR(SVDeviceParamWrapper& w) {SVLightReferenceDeviceParam* p = w.DerivedValue(p); if (p==NULL) {w = SVLightReferenceDeviceParam(); p = w.DerivedValue(p);} ASSERT(p); return p->lr;}
#endif


class SVCameraFormatsDeviceParam : public SVStringValueDeviceParam
{
public:
	SVCameraFormatsDeviceParam();
	SVCameraFormatsDeviceParam(SVDeviceParamEnum typeEnum);
	SVCameraFormatsDeviceParam(const SVCameraFormatsDeviceParam& rhs);
	virtual SVClonable* CloneImpl() const;
	const SVCameraFormatsDeviceParam& operator=(const SVCameraFormatsDeviceParam& rhs);
	bool SupportsColor() const;

	typedef std::map<SVString, SVCameraFormat> OptionsType;
	OptionsType options;

	virtual HRESULT SetMetadata(const SVDeviceParam* pParam);

	static SVDeviceParam* CreateNew() { return new SVCameraFormatsDeviceParam; }
	DEFINE_VISITABLE()
};

#endif

