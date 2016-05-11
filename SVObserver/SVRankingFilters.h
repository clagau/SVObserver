//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRankingFilters
//* .File Name       : $Workfile:   SVRankingFilters.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   13 Aug 2013 09:58:58  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVFilterClass.h"
#include "SVMatroxLibrary/SVMatroxBuffer.h"
#pragma endregion Includes

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVRankingFilterClass
// -----------------------------------------------------------------------------
// .Description : Base class for all kinds of custom filters
////////////////////////////////////////////////////////////////////////////////
class SVRankingFilterClass : public SVFilterClass
{
	SV_DECLARE_CLASS( SVRankingFilterClass );

public:
	SVRankingFilterClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVRANKINGFILTER );
	virtual ~SVRankingFilterClass();
	
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );

	virtual HRESULT ResetObject();

	virtual BOOL RebuildRanking();

#pragma region virtual method (IFilter)
	virtual bool shouldResetInspection() const override { return true; }
#pragma region virtual method (IFilter)

private:
	void init();

protected:
	virtual BOOL onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus );
	virtual BOOL OnValidate();

private:
	SVMatroxBuffer m_milRanking;
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
	SVLongValueObjectClass *m_plvoRankingCells[cellSize];

	SVLongValueObjectClass m_lvoRankingWidth;
	SVLongValueObjectClass m_lvoRankingHeight;
	SVLongValueObjectClass m_lvoRankingRank;
};
