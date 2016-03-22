//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLutTransform
//* .File Name       : $Workfile:   SVLutTransform.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:51:24  $
//******************************************************************************

#pragma once

#include "SVContainerLibrary/SVVector.h"

enum SVLutTransformOperationEnum	// for legacy conversion
{
	LutTransformTypeNormal = 0,
	LutTransformTypeInversion = 1,
	LutTransformTypeSign = 2,
	LutTransformTypeClip = 3,
	//LutTransformTypeFormula = 4,
	LutTransformTypeFreeform = 5,
	LutTransformTypeTwoKnee = 6,

	MAX_LUT_TRANSFORM_TYPE,
	LutTransformTypeUnknown
};

class SVLutTransformOperation;
class SVLutTransform;

class SVLutTransformOperationMap
{
public:
	struct SVLutTransformTypeInfo
	{
		SVLutTransformOperationEnum eType;
		SVLutTransformOperation* pType;
		CString sType;
		SVLutTransformTypeInfo() {eType = LutTransformTypeUnknown; pType = NULL;}
		SVLutTransformTypeInfo(SVLutTransformOperationEnum e_, SVLutTransformOperation* pType_, const CString& sType_) {eType = e_; pType = pType_; sType = sType_;}
	};
	typedef SVVector< SVLutTransformTypeInfo > SVLutTransformTypeInfoArray;
public:
	SVLutTransformOperationMap();
	~SVLutTransformOperationMap();
	SVLutTransformOperation* GetType(SVLutTransformOperationEnum e);
	SVLutTransformOperationEnum GetType(const SVLutTransformOperation* pType);
	const SVLutTransformTypeInfo* GetInfo(SVLutTransformOperationEnum e);
	const SVLutTransformTypeInfo* GetInfo(const SVLutTransformOperation* pType);
	const SVLutTransformTypeInfo* GetInfo(const CString& sType);
	const SVLutTransformTypeInfoArray& GetTypes() {return maTypeInfo;}
private:
	SVLutTransformTypeInfoArray maTypeInfo;
};

class SVLutTransformParameters : public CDWordArray
{
};

class SVLutBand;

class SVLutTransformOperation;
class SVLutTransformOperationNormal;
class SVLutTransformOperationInvert;
class SVLutTransformOperationSign;
class SVLutTransformOperationClip;
class SVLutTransformOperationFreeform;
class SVLutTransformOperationTwoKnee;

class SVLutTransform
{
public:
	SVLutTransform();
	SVLutTransform(const SVLutTransformOperation& rType);
	SVLutTransform(const SVLutTransform& rhs);
	virtual ~SVLutTransform();
	virtual SVLutTransform* Clone() const = 0;
	SVLutTransform& operator = (const SVLutTransform& rhs);

	bool operator == (const SVLutTransform& rhs) const;

	HRESULT Transform(SVLutBand& data, const SVLutTransformParameters& param);

	SVLutTransform& operator = (const SVLutTransformOperation& rhs);

	bool SetOperation(const SVLutTransformOperation& rType);
	const SVLutTransformOperation* GetOperation() const;
	
	bool SetOperation(SVLutTransformOperationEnum eType);
	void GetOperation(SVLutTransformOperationEnum& eType) const;

	bool GetParameters(SVLutTransformParameters& rParam) const;
	bool SetParameters(const SVLutTransformParameters& rParam);

	virtual HRESULT Transform(SVLutTransformOperationNormal& t, SVLutBand& data, const SVLutTransformParameters& param) = 0;
	virtual HRESULT Transform(SVLutTransformOperationInvert& t, SVLutBand& data, const SVLutTransformParameters& param) = 0;
	virtual HRESULT Transform(SVLutTransformOperationSign& t, SVLutBand& data, const SVLutTransformParameters& param) = 0;
	virtual HRESULT Transform(SVLutTransformOperationClip& t, SVLutBand& data, const SVLutTransformParameters& param) = 0;
	virtual HRESULT Transform(SVLutTransformOperationFreeform& t, SVLutBand& data, const SVLutTransformParameters& param) = 0;
	virtual HRESULT Transform(SVLutTransformOperationTwoKnee& t, SVLutBand& data, const SVLutTransformParameters& param) = 0;

	static const SVLutTransformOperation* GetEquivalentType(SVLutTransformOperationEnum e) {return mMapTypes.GetType(e);}
	static SVLutTransformOperationEnum GetEquivalentType(const SVLutTransformOperation* pType) {return mMapTypes.GetType(pType);}


protected:
	SVLutTransformOperation* mpType;
	bool mbOwnType;
	static SVLutTransformOperationMap mMapTypes;
	SVLutTransformParameters mParam;

	void Cleanup();

};

class SVLutTransformOperation
{
public:
	virtual ~SVLutTransformOperation();
	virtual HRESULT Transform(SVLutTransform& transform, SVLutBand& data, const SVLutTransformParameters& param) = 0;
	virtual SVLutTransformOperation* Clone() const = 0;

	bool operator == ( const SVLutTransformOperation& rhs ) const;
};

class SVLutTransformOperationNormal : public SVLutTransformOperation
{
	virtual HRESULT Transform(SVLutTransform& transform, SVLutBand& data, const SVLutTransformParameters& param) {return transform.Transform(*this, data, param);}
	virtual SVLutTransformOperation* Clone() const {return new SVLutTransformOperationNormal;}
};
class SVLutTransformOperationInvert : public SVLutTransformOperation
{
	virtual HRESULT Transform(SVLutTransform& transform, SVLutBand& data, const SVLutTransformParameters& param) {return transform.Transform(*this, data, param);}
	virtual SVLutTransformOperation* Clone() const {return new SVLutTransformOperationInvert;}
};
class SVLutTransformOperationSign : public SVLutTransformOperation
{
	virtual HRESULT Transform(SVLutTransform& transform, SVLutBand& data, const SVLutTransformParameters& param) {return transform.Transform(*this, data, param);}
	virtual SVLutTransformOperation* Clone() const {return new SVLutTransformOperationSign;}
};
class SVLutTransformOperationClip : public SVLutTransformOperation
{
	virtual HRESULT Transform(SVLutTransform& transform, SVLutBand& data, const SVLutTransformParameters& param) {return transform.Transform(*this, data, param);}
	virtual SVLutTransformOperation* Clone() const {return new SVLutTransformOperationClip;}
};
class SVLutTransformOperationFreeform : public SVLutTransformOperation
{
	virtual HRESULT Transform(SVLutTransform& transform, SVLutBand& data, const SVLutTransformParameters& param) {return transform.Transform(*this, data, param);}
	virtual SVLutTransformOperation* Clone() const {return new SVLutTransformOperationFreeform;}
};
class SVLutTransformOperationTwoKnee : public SVLutTransformOperation
{
	virtual HRESULT Transform(SVLutTransform& transform, SVLutBand& data, const SVLutTransformParameters& param) {return transform.Transform(*this, data, param);}
	virtual SVLutTransformOperation* Clone() const {return new SVLutTransformOperationTwoKnee;}
};





class SVDefaultLutTransform : public SVLutTransform
{
public:
	SVDefaultLutTransform(){};
	SVDefaultLutTransform(const SVLutTransformOperation& rType) : SVLutTransform(rType) {}
	SVDefaultLutTransform(const SVDefaultLutTransform& rhs) : SVLutTransform(rhs) {}
	virtual SVLutTransform* Clone() const {return new SVDefaultLutTransform(*this);}
	virtual HRESULT Transform(SVLutTransformOperationNormal& t, SVLutBand& data, const SVLutTransformParameters& param);
	virtual HRESULT Transform(SVLutTransformOperationInvert& t, SVLutBand& data, const SVLutTransformParameters& param);
	virtual HRESULT Transform(SVLutTransformOperationSign& t, SVLutBand& data, const SVLutTransformParameters& param);
	virtual HRESULT Transform(SVLutTransformOperationClip& t, SVLutBand& data, const SVLutTransformParameters& param);
	virtual HRESULT Transform(SVLutTransformOperationFreeform& t, SVLutBand& data, const SVLutTransformParameters& param);
	virtual HRESULT Transform(SVLutTransformOperationTwoKnee& t, SVLutBand& data, const SVLutTransformParameters& param);
};
