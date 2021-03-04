//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCustomFilters
//* .File Name       : $Workfile:   SVCustomFilters.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   13 Aug 2013 09:59:02  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVMatroxLibrary/SVMatroxBuffer.h"
#include "SVFilterClass.h"
#include "SVValueObjectLibrary/SVBoolValueObjectClass.h"
#include "SVValueObjectLibrary/SVLongValueObjectClass.h"
#pragma endregion Includes

////////////////////////////////////////////////////////////////////////////////
// .Title       : CustomFilter
// -----------------------------------------------------------------------------
// .Description : Base class for all kinds of custom filters
////////////////////////////////////////////////////////////////////////////////
class CustomFilter : public SVFilterClass
{
	SV_DECLARE_CLASS

public:
	CustomFilter( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVCUSTOMFILTER );
	virtual ~CustomFilter();
	
	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

#pragma region virtual method (IFilter)
	virtual bool shouldResetInspection() const override { return true; }
#pragma region virtual method (IFilter)

private:
	void init();

protected:
	void RebuildKernel();

	virtual bool onRun( bool First, SvOi::SVImageBufferHandlePtr RInputImageHandle, SvOi::SVImageBufferHandlePtr ROutputImageHandle, RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;
	bool ValidateLocal(SvStl::MessageContainerVector *pErrorMessages=nullptr) const;

private:
	SVMatroxBuffer m_milKernel;
	SvVol::SVLongValueObjectClass m_lvoCell01;
	SvVol::SVLongValueObjectClass m_lvoCell02;
	SvVol::SVLongValueObjectClass m_lvoCell03;
	SvVol::SVLongValueObjectClass m_lvoCell04;
	SvVol::SVLongValueObjectClass m_lvoCell05;
	SvVol::SVLongValueObjectClass m_lvoCell06;
	SvVol::SVLongValueObjectClass m_lvoCell07;
	SvVol::SVLongValueObjectClass m_lvoCell08;
	SvVol::SVLongValueObjectClass m_lvoCell09;
	SvVol::SVLongValueObjectClass m_lvoCell10;
	SvVol::SVLongValueObjectClass m_lvoCell11;
	SvVol::SVLongValueObjectClass m_lvoCell12;
	SvVol::SVLongValueObjectClass m_lvoCell13;
	SvVol::SVLongValueObjectClass m_lvoCell14;
	SvVol::SVLongValueObjectClass m_lvoCell15;
	SvVol::SVLongValueObjectClass m_lvoCell16;
	SvVol::SVLongValueObjectClass m_lvoCell17;
	SvVol::SVLongValueObjectClass m_lvoCell18;
	SvVol::SVLongValueObjectClass m_lvoCell19;
	SvVol::SVLongValueObjectClass m_lvoCell20;
	SvVol::SVLongValueObjectClass m_lvoCell21;
	SvVol::SVLongValueObjectClass m_lvoCell22;
	SvVol::SVLongValueObjectClass m_lvoCell23;
	SvVol::SVLongValueObjectClass m_lvoCell24;
	SvVol::SVLongValueObjectClass m_lvoCell25;
	SvVol::SVLongValueObjectClass m_lvoCell26;
	SvVol::SVLongValueObjectClass m_lvoCell27;
	SvVol::SVLongValueObjectClass m_lvoCell28;
	SvVol::SVLongValueObjectClass m_lvoCell29;
	SvVol::SVLongValueObjectClass m_lvoCell30;
	SvVol::SVLongValueObjectClass m_lvoCell31;
	SvVol::SVLongValueObjectClass m_lvoCell32;
	SvVol::SVLongValueObjectClass m_lvoCell33;
	SvVol::SVLongValueObjectClass m_lvoCell34;
	SvVol::SVLongValueObjectClass m_lvoCell35;
	SvVol::SVLongValueObjectClass m_lvoCell36;
	SvVol::SVLongValueObjectClass m_lvoCell37;
	SvVol::SVLongValueObjectClass m_lvoCell38;
	SvVol::SVLongValueObjectClass m_lvoCell39;
	SvVol::SVLongValueObjectClass m_lvoCell40;
	SvVol::SVLongValueObjectClass m_lvoCell41;
	SvVol::SVLongValueObjectClass m_lvoCell42;
	SvVol::SVLongValueObjectClass m_lvoCell43;
	SvVol::SVLongValueObjectClass m_lvoCell44;
	SvVol::SVLongValueObjectClass m_lvoCell45;
	SvVol::SVLongValueObjectClass m_lvoCell46;
	SvVol::SVLongValueObjectClass m_lvoCell47;
	SvVol::SVLongValueObjectClass m_lvoCell48;
	SvVol::SVLongValueObjectClass m_lvoCell49;
	static const int cellSize = 49;
	SvVol::SVLongValueObjectClass *m_plvoKernelCells[cellSize];

	SvVol::SVLongValueObjectClass m_lvoKernelWidth;
	SvVol::SVLongValueObjectClass m_lvoKernelHeight;
	SvVol::SVBoolValueObjectClass m_bvoClippingOn;
	SvVol::SVBoolValueObjectClass m_bvoAbsoluteValue;
	SvVol::SVLongValueObjectClass m_lvoTransformationFactor;
};

