//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLut
//* .File Name       : $Workfile:   SVLut.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:51:44  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <vector>
#include "SVUtilityLibrary/SVUtilityGlobals.h"
#include "SVLutTransform.h"

enum SVLutFormatEnum
{
	SVLUT_FORMAT_NEUTRAL = 987654321,
};

class SVLutInfo
{
public:
	SVLutInfo();
	SVLutInfo( UINT p_Bands, UINT p_BandSize, UINT p_BandMaxValue, const SVLutTransform& p_rTransform );
	SVLutInfo(const SVLutInfo& rhs);
	const SVLutInfo& operator = (const SVLutInfo& rhs);
	~SVLutInfo();
	void CopyNoTransform(const SVLutInfo& rhs);
	UINT Bands() const;
	UINT Format() const;
	UINT BandSize() const;
	UINT MaxValue() const;
	const SVLutTransform* GetTransform() const;
	SVLutTransform*& GetTransform();
	bool SetTransform(const SVLutTransform& pTransform);
	const SVLutTransformOperation* GetTransformOperation() const;
	bool SetTransformOperation(const SVLutTransformOperation& pType);
	bool GetTransformParameters(SVLutTransformParameters& rParam) const;
	bool GetTransformParameters(SAFEARRAY*& rpsaParam) const;
	void SetBands(UINT uiBands);
	void SetFormat(UINT uiFormat);
	void SetBandSize(UINT uiBandSize);
	void SetMaxValue(UINT uiMaxValue);
	bool IsNullHandle() const;
	SVHANDLE GetHandle() const;
	void Cleanup();
private:
	SVHANDLE        mHandle;        // handle to Acquisition Device

	UINT            muiBands;       // number of LUT bands to create
	UINT            muiBandSize;    // Size of one band in neutral format
	UINT            muiFormat;      // color format of the LUT
	UINT            muiMaxValue;    // max LUT value
	SVLutTransform* mpTransform;	// transform object;
};

typedef ULONG SVLutValueType;

template <typename T> class SVLutValue
{
public:
	SVLutValue();
	SVLutValue(const T& val);
	operator T() const;
	const T& operator = (const T& rhs);

	T value;
	typedef T ValueType;
};

typedef SVLutValue<UINT> SVLutEntry;   // this is needed so the specialization can be called.
                                       // there is still an automatic conversion to/from SVLutValueType. (!)

class SVLutBand
{
public:
	SVLutBand();
	SVLutBand( const SVLutBand& rhs );
	~SVLutBand();
	const SVLutBand& operator = ( const SVLutBand& rhs );
	void CopyNoTransform( const SVLutBand& rhs );

	bool operator == ( const SVLutBand& rhs ) const;

	SVLutEntry& operator () (UINT nIndex);
	const SVLutEntry& operator () (UINT nIndex) const;

	ULONG Band() const;
	bool Create(const SVLutInfo& info, UINT uiBand);
	bool Destroy();
	bool CopyBandData(const SVLutBand& lutband);
	bool SetBandData(SAFEARRAY* psaBandData);
	bool GetBandData(SAFEARRAY*& rpsaBandData) const;

	UINT Size() const;

	const SVLutTransformOperation* GetTransformOperation() const;
	bool SetTransformOperation(const SVLutTransformOperation& pType);
	bool Transform(const SVLutTransformParameters& param = SVLutTransformParameters());

	const SVLutInfo& Info() const;

private:
	std::vector < SVLutEntry > maTable;
	ULONG mulBand;
	SVLutInfo mInfo;

friend class SVLut;
};



class SVLut
{
public:
	SVLut();
	SVLut( const SVLut& rhs );
	~SVLut();
	const SVLut& operator = ( const SVLut& rhs );
	void CopyNoTransform( const SVLut& rhs );

	bool IsCreated() const;
	bool Create(const SVLutInfo& info);
	bool Destroy();
	bool CopyBandData(const SVLut& lut, int iBand=-1);
	bool CopyBandData(const SVLutBand& lutband);
	bool SetBandData(SAFEARRAY* psaBands);
	bool GetBandData(SAFEARRAY*& rpsaBands) const;

	const SVLutTransformOperation* GetTransformOperation() const;
	bool SetTransformOperation(const SVLutTransformOperation& pType);
	bool SetTransform(const SVLutTransform& pTransform);
	bool Transform(const SVLutTransformParameters& param = SVLutTransformParameters());

	SVLutBand& operator () (UINT nBand);
	const SVLutBand& operator () (UINT nBand) const;
	SVLutEntry& operator () (UINT nBand, UINT nIndex);
	const SVLutEntry& operator () (UINT nBand, UINT nIndex) const;

	ULONG NumBands() const;
	const SVLutInfo& Info() const;

private:
	std::vector< SVLutBand > maBands;
	SVLutInfo mInfo;
};


template <typename T>
inline SVLutValue<T>::SVLutValue()
{
}

// specialize for UINT so can init to 0
inline SVLutValue<UINT>::SVLutValue()
{
	value = 0;
}

template <typename T>
inline SVLutValue<T>::SVLutValue(const T& rVal)
{
	value = rVal;
}

template <typename T>
inline SVLutValue<T>::operator T() const
{
	return value;
}

template <typename T>
inline const T& SVLutValue<T>::operator = (const T& rhs)
{
	value = rhs;
	return value;
}

class SVLutTestCases
{
public:
	SVLutTestCases();
};


