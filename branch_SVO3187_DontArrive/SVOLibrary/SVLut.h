//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
	SVLutInfo& operator = (const SVLutInfo& rhs);
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
	void Cleanup();
private:
	UINT            muiBands;       // number of LUT bands to create
	UINT            muiBandSize;    // Size of one band in neutral format
	UINT            muiFormat;      // color format of the LUT
	UINT            muiMaxValue;    // max LUT value
	SVLutTransform* mpTransform;	// transform object;
};

class SVLutBand
{
public:
	SVLutBand();
	SVLutBand( const SVLutBand& rhs );
	~SVLutBand();
	SVLutBand& operator = ( const SVLutBand& rhs );
	void CopyNoTransform( const SVLutBand& rhs );

	bool operator == ( const SVLutBand& rhs ) const;

	UINT& operator () (UINT nIndex);
	const UINT& operator () (UINT nIndex) const;

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
	std::vector < UINT > maTable;
	ULONG mulBand = 0;
	SVLutInfo mInfo;

friend class SVLut;
};



class SVLut
{
public:
	SVLut();
	SVLut( const SVLut& rhs );
	~SVLut();
	SVLut& operator = ( const SVLut& rhs );
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
	UINT& operator () (UINT nBand, UINT nIndex);
	const UINT& operator () (UINT nBand, UINT nIndex) const;

	int NumBands() const;
	const SVLutInfo& Info() const;

private:
	std::vector<SVLutBand> m_Bands;
	SVLutInfo mInfo;
};



class SVLutTestCases
{
public:
	SVLutTestCases();
};


