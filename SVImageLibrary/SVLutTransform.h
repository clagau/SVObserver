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

#pragma region Includes
#include "SVContainerLibrary/SVVector.h"
#pragma endregion Includes

enum SVLutTransformOperationEnum	// for legacy conversion
{
	LutTransformTypeNormal = 0,
	LutTransformTypeInversion = 1,
	LutTransformTypeSign = 2,
	LutTransformTypeClip = 3,
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
		SVLutTransformTypeInfo() {eType = LutTransformTypeUnknown; pType = nullptr;}
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

typedef CDWordArray SVLutTransformParameters;

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
public:
	virtual ~SVLutTransformOperationNormal() {}
private:
	virtual HRESULT Transform(SVLutTransform& transform, SVLutBand& data, const SVLutTransformParameters& param) override {return transform.Transform(*this, data, param);}
	virtual SVLutTransformOperation* Clone() const override {return new SVLutTransformOperationNormal;}
};

class SVLutTransformOperationInvert : public SVLutTransformOperation
{
public:
	virtual ~SVLutTransformOperationInvert() {}
private:
	virtual HRESULT Transform(SVLutTransform& transform, SVLutBand& data, const SVLutTransformParameters& param) override {return transform.Transform(*this, data, param);}
	virtual SVLutTransformOperation* Clone() const override {return new SVLutTransformOperationInvert;}
};

class SVLutTransformOperationSign : public SVLutTransformOperation
{
public:
	virtual ~SVLutTransformOperationSign() {}
private:
	virtual HRESULT Transform(SVLutTransform& transform, SVLutBand& data, const SVLutTransformParameters& param) override {return transform.Transform(*this, data, param);}
	virtual SVLutTransformOperation* Clone() const override {return new SVLutTransformOperationSign;}
};

class SVLutTransformOperationClip : public SVLutTransformOperation
{
public:
	virtual ~SVLutTransformOperationClip() {}
private:
	virtual HRESULT Transform(SVLutTransform& transform, SVLutBand& data, const SVLutTransformParameters& param) override {return transform.Transform(*this, data, param);}
	virtual SVLutTransformOperation* Clone() const override {return new SVLutTransformOperationClip;}
};

class SVLutTransformOperationFreeform : public SVLutTransformOperation
{
public:
	virtual ~SVLutTransformOperationFreeform() {}
private:
	virtual HRESULT Transform(SVLutTransform& transform, SVLutBand& data, const SVLutTransformParameters& param) override {return transform.Transform(*this, data, param);}
	virtual SVLutTransformOperation* Clone() const override {return new SVLutTransformOperationFreeform;}
};

class SVLutTransformOperationTwoKnee : public SVLutTransformOperation
{
public:
	virtual ~SVLutTransformOperationTwoKnee() {}
private:
	virtual HRESULT Transform(SVLutTransform& transform, SVLutBand& data, const SVLutTransformParameters& param) override {return transform.Transform(*this, data, param);}
	virtual SVLutTransformOperation* Clone() const override {return new SVLutTransformOperationTwoKnee;}
};

class SVDefaultLutTransform : public SVLutTransform
{
public:
	SVDefaultLutTransform() {}
	SVDefaultLutTransform(const SVLutTransformOperation& rType) : SVLutTransform(rType) {}
	SVDefaultLutTransform(const SVDefaultLutTransform& rhs) : SVLutTransform(rhs) {}
	virtual ~SVDefaultLutTransform() {}
	virtual SVLutTransform* Clone() const override {return new SVDefaultLutTransform(*this);}
	virtual HRESULT Transform(SVLutTransformOperationNormal& t, SVLutBand& data, const SVLutTransformParameters& param) override;
	virtual HRESULT Transform(SVLutTransformOperationInvert& t, SVLutBand& data, const SVLutTransformParameters& param) override;
	virtual HRESULT Transform(SVLutTransformOperationSign& t, SVLutBand& data, const SVLutTransformParameters& param) override;
	virtual HRESULT Transform(SVLutTransformOperationClip& t, SVLutBand& data, const SVLutTransformParameters& param) override;
	virtual HRESULT Transform(SVLutTransformOperationFreeform& t, SVLutBand& data, const SVLutTransformParameters& param) override;
	virtual HRESULT Transform(SVLutTransformOperationTwoKnee& t, SVLutBand& data, const SVLutTransformParameters& param) override;
};
