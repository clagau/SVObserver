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
#pragma endregion Includes

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVCustomFilterClass
// -----------------------------------------------------------------------------
// .Description : Base class for all kinds of custom filters
// -----------------------------------------------------------------------------
// .Export
//	 Public Method				Description
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
////////////////////////////////////////////////////////////////////////////////
class SVCustomFilterClass : public SVFilterClass
{
	SV_DECLARE_CLASS( SVCustomFilterClass );

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
// Standard constructor
	SVCustomFilterClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVCUSTOMFILTER );
	virtual ~SVCustomFilterClass();
	
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );

	virtual HRESULT ResetObject();

#pragma region virtual method (IFilter)
	virtual bool shouldResetInspection() const override { return true; }
#pragma region virtual method (IFilter)

private:
	void init();

protected:
	virtual BOOL RebuildKernel();

	virtual BOOL onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus );
	virtual BOOL OnValidate();

private:
	SVMatroxBuffer m_milKernel;
	SVLongValueObjectClass m_lvoCell01;
	SVLongValueObjectClass m_lvoCell02;
	SVLongValueObjectClass m_lvoCell03;
	SVLongValueObjectClass m_lvoCell04;
	SVLongValueObjectClass m_lvoCell05;
	SVLongValueObjectClass m_lvoCell06;
	SVLongValueObjectClass m_lvoCell07;
	SVLongValueObjectClass m_lvoCell08;
	SVLongValueObjectClass m_lvoCell09;
	SVLongValueObjectClass m_lvoCell10;
	SVLongValueObjectClass m_lvoCell11;
	SVLongValueObjectClass m_lvoCell12;
	SVLongValueObjectClass m_lvoCell13;
	SVLongValueObjectClass m_lvoCell14;
	SVLongValueObjectClass m_lvoCell15;
	SVLongValueObjectClass m_lvoCell16;
	SVLongValueObjectClass m_lvoCell17;
	SVLongValueObjectClass m_lvoCell18;
	SVLongValueObjectClass m_lvoCell19;
	SVLongValueObjectClass m_lvoCell20;
	SVLongValueObjectClass m_lvoCell21;
	SVLongValueObjectClass m_lvoCell22;
	SVLongValueObjectClass m_lvoCell23;
	SVLongValueObjectClass m_lvoCell24;
	SVLongValueObjectClass m_lvoCell25;
	SVLongValueObjectClass m_lvoCell26;
	SVLongValueObjectClass m_lvoCell27;
	SVLongValueObjectClass m_lvoCell28;
	SVLongValueObjectClass m_lvoCell29;
	SVLongValueObjectClass m_lvoCell30;
	SVLongValueObjectClass m_lvoCell31;
	SVLongValueObjectClass m_lvoCell32;
	SVLongValueObjectClass m_lvoCell33;
	SVLongValueObjectClass m_lvoCell34;
	SVLongValueObjectClass m_lvoCell35;
	SVLongValueObjectClass m_lvoCell36;
	SVLongValueObjectClass m_lvoCell37;
	SVLongValueObjectClass m_lvoCell38;
	SVLongValueObjectClass m_lvoCell39;
	SVLongValueObjectClass m_lvoCell40;
	SVLongValueObjectClass m_lvoCell41;
	SVLongValueObjectClass m_lvoCell42;
	SVLongValueObjectClass m_lvoCell43;
	SVLongValueObjectClass m_lvoCell44;
	SVLongValueObjectClass m_lvoCell45;
	SVLongValueObjectClass m_lvoCell46;
	SVLongValueObjectClass m_lvoCell47;
	SVLongValueObjectClass m_lvoCell48;
	SVLongValueObjectClass m_lvoCell49;
	static const int cellSize = 49;
	SVLongValueObjectClass *m_plvoKernelCells[cellSize];

	SVLongValueObjectClass m_lvoKernelWidth;
	SVLongValueObjectClass m_lvoKernelHeight;
	SVBoolValueObjectClass m_bvoClippingOn;
	SVBoolValueObjectClass m_bvoAbsoluteValue;	
	SVLongValueObjectClass m_lvoTransformationFactor;
};

