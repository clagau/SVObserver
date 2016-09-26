//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRankingFilters
//* .File Name       : $Workfile:   SVRankingFilters.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:39:36  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVRankingFilters.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVImageProcessingClass.h"
#include "SVOMFCLibrary/SVDeviceParams.h" //Arvid added to avoid VS2015 compile Error

#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define RANKING_BIT_DEPTH SV32BitSigned

SV_IMPLEMENT_CLASS( SVRankingFilterClass, SVRankingFilterClassGuid )

SVRankingFilterClass::SVRankingFilterClass( SVObjectClass* POwner, int StringResourceID )
					: SVFilterClass( POwner, StringResourceID )
{
	init();
}

SVRankingFilterClass::~SVRankingFilterClass()
{
	m_milRanking.clear();
}

void SVRankingFilterClass::init()
{
	long l( 0 );

	m_outObjectInfo.ObjectTypeInfo.SubType = SVRankingFilterObjectType;

	RegisterEmbeddedObject( &m_lvoCell01, SVRankingFilterCell01Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL01, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell02, SVRankingFilterCell02Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL02, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell03, SVRankingFilterCell03Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL03, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell04, SVRankingFilterCell04Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL04, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell05, SVRankingFilterCell05Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL05, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell06, SVRankingFilterCell06Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL06, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell07, SVRankingFilterCell07Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL07, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell08, SVRankingFilterCell08Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL08, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell09, SVRankingFilterCell09Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL09, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell10, SVRankingFilterCell10Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL10, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell11, SVRankingFilterCell11Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL11, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell12, SVRankingFilterCell12Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL12, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell13, SVRankingFilterCell13Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL13, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell14, SVRankingFilterCell14Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL14, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell15, SVRankingFilterCell15Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL15, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell16, SVRankingFilterCell16Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL16, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell17, SVRankingFilterCell17Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL17, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell18, SVRankingFilterCell18Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL18, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell19, SVRankingFilterCell19Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL19, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell20, SVRankingFilterCell20Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL20, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell21, SVRankingFilterCell21Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL21, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell22, SVRankingFilterCell22Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL22, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell23, SVRankingFilterCell23Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL23, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell24, SVRankingFilterCell24Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL24, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell25, SVRankingFilterCell25Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL25, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell26, SVRankingFilterCell26Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL26, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell27, SVRankingFilterCell27Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL27, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell28, SVRankingFilterCell28Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL28, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell29, SVRankingFilterCell29Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL29, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell30, SVRankingFilterCell30Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL30, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell31, SVRankingFilterCell31Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL31, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell32, SVRankingFilterCell32Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL32, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell33, SVRankingFilterCell33Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL33, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell34, SVRankingFilterCell34Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL34, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell35, SVRankingFilterCell35Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL35, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell36, SVRankingFilterCell36Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL36, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell37, SVRankingFilterCell37Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL37, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell38, SVRankingFilterCell38Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL38, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell39, SVRankingFilterCell39Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL39, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell40, SVRankingFilterCell40Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL40, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell41, SVRankingFilterCell41Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL41, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell42, SVRankingFilterCell42Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL42, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell43, SVRankingFilterCell43Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL43, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell44, SVRankingFilterCell44Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL44, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell45, SVRankingFilterCell45Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL45, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell46, SVRankingFilterCell46Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL46, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell47, SVRankingFilterCell47Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL47, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell48, SVRankingFilterCell48Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL48, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell49, SVRankingFilterCell49Guid, IDS_OBJECTNAME_RANKINGFILTER_CELL49, false, SVResetItemOwner );

	RegisterEmbeddedObject( &m_lvoRankingWidth,  SVRankingFilterRankingWidthGuid, IDS_OBJECTNAME_RANKINGFILTER_RANKINGWIDTH, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoRankingHeight, SVRankingFilterRankingHeightGuid, IDS_OBJECTNAME_RANKINGFILTER_RANKINGHEIGHT, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoRankingRank,  SVRankingFilterRankingRankGuid, IDS_OBJECTNAME_RANKINGFILTER_RANKINGRANK, false, SVResetItemOwner );

	m_plvoRankingCells[ 0] = &m_lvoCell01;
	m_plvoRankingCells[ 1] = &m_lvoCell02;
	m_plvoRankingCells[ 2] = &m_lvoCell03;
	m_plvoRankingCells[ 3] = &m_lvoCell04;
	m_plvoRankingCells[ 4] = &m_lvoCell05;
	m_plvoRankingCells[ 5] = &m_lvoCell06;
	m_plvoRankingCells[ 6] = &m_lvoCell07;
	m_plvoRankingCells[ 7] = &m_lvoCell08;
	m_plvoRankingCells[ 8] = &m_lvoCell09;
	m_plvoRankingCells[ 9] = &m_lvoCell10;
	m_plvoRankingCells[10] = &m_lvoCell11;
	m_plvoRankingCells[11] = &m_lvoCell12;
	m_plvoRankingCells[12] = &m_lvoCell13;
	m_plvoRankingCells[13] = &m_lvoCell14;
	m_plvoRankingCells[14] = &m_lvoCell15;
	m_plvoRankingCells[15] = &m_lvoCell16;
	m_plvoRankingCells[16] = &m_lvoCell17;
	m_plvoRankingCells[17] = &m_lvoCell18;
	m_plvoRankingCells[18] = &m_lvoCell19;
	m_plvoRankingCells[19] = &m_lvoCell20;
	m_plvoRankingCells[20] = &m_lvoCell21;
	m_plvoRankingCells[21] = &m_lvoCell22;
	m_plvoRankingCells[22] = &m_lvoCell23;
	m_plvoRankingCells[23] = &m_lvoCell24;
	m_plvoRankingCells[24] = &m_lvoCell25;
	m_plvoRankingCells[25] = &m_lvoCell26;
	m_plvoRankingCells[26] = &m_lvoCell27;
	m_plvoRankingCells[27] = &m_lvoCell28;
	m_plvoRankingCells[28] = &m_lvoCell29;
	m_plvoRankingCells[29] = &m_lvoCell30;
	m_plvoRankingCells[30] = &m_lvoCell31;
	m_plvoRankingCells[31] = &m_lvoCell32;
	m_plvoRankingCells[32] = &m_lvoCell33;
	m_plvoRankingCells[33] = &m_lvoCell34;
	m_plvoRankingCells[34] = &m_lvoCell35;
	m_plvoRankingCells[35] = &m_lvoCell36;
	m_plvoRankingCells[36] = &m_lvoCell37;
	m_plvoRankingCells[37] = &m_lvoCell38;
	m_plvoRankingCells[38] = &m_lvoCell39;
	m_plvoRankingCells[39] = &m_lvoCell40;
	m_plvoRankingCells[40] = &m_lvoCell41;
	m_plvoRankingCells[41] = &m_lvoCell42;
	m_plvoRankingCells[42] = &m_lvoCell43;
	m_plvoRankingCells[43] = &m_lvoCell44;
	m_plvoRankingCells[44] = &m_lvoCell45;
	m_plvoRankingCells[45] = &m_lvoCell46;
	m_plvoRankingCells[46] = &m_lvoCell47;
	m_plvoRankingCells[47] = &m_lvoCell48;
	m_plvoRankingCells[48] = &m_lvoCell49;

	for( l = 0; l < 49; l++ )
	{
		m_plvoRankingCells[l]->SetDefaultValue( 1, TRUE );
	}// end for

	m_lvoRankingWidth.SetDefaultValue( 3L, TRUE );
	m_lvoRankingHeight.SetDefaultValue( 3L, TRUE );
	m_lvoRankingRank.SetDefaultValue( SVValueMedian, TRUE );

	for( l = 0; l < 49; l++ )
	{
		m_plvoRankingCells[l]->ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;
	}// end for

	m_lvoRankingWidth.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;
	m_lvoRankingHeight.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;
	m_lvoRankingRank.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;

	// Set default inputs and outputs
	addDefaultInputObjects();

	RebuildRanking();
}
////////////////////////////////////////////////////////////////////////////////
// .Title       : CreateObject
// -----------------------------------------------------------------------------
// .Description : ...
////////////////////////////////////////////////////////////////////////////////
BOOL SVRankingFilterClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = TRUE;

	bOk &= SVFilterClass::CreateObject( PCreateStructure );

	bOk &= RebuildRanking();

	return bOk;
}

HRESULT SVRankingFilterClass::ResetObject()
{
	HRESULT l_hrOk = SVFilterClass::ResetObject();

	if ( ! RebuildRanking() )
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

BOOL SVRankingFilterClass::OnValidate()
{
	BOOL bRetVal = TRUE;
	long lWidth;
	long lHeight;
	
	m_lvoRankingWidth.GetValue( lWidth );
	m_lvoRankingHeight.GetValue( lHeight );
	
	if( lWidth != 1 && lWidth != 3 && lWidth != 5 && lWidth != 7 )
		bRetVal = FALSE;

	if( lHeight != 1 && lHeight != 3 && lHeight != 5 && lHeight != 7 )
		bRetVal = FALSE;

	bRetVal = SVOperatorClass::OnValidate() || bRetVal;

	if( !bRetVal )
	{
		SetInvalid();
	}
	return bRetVal;	
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Initialization of class SVRankingFilterClass
// -----------------------------------------------------------------------------
// .Description : Initialization of newly Instantiated Object
////////////////////////////////////////////////////////////////////////////////
BOOL SVRankingFilterClass::RebuildRanking()
{
	
	SVMatroxBufferInterface::SVStatusCode l_Code;
	// First free old ranking

	m_milRanking.clear();

	// Average ranking information data definition
	long *pRankingData;
	long lValue;
	long lWidth;
	long lHeight;
	long l;
	long m;

	// Validate width and make sure it is valid
	m_lvoRankingWidth.GetValue( lWidth );
	if( lWidth >= 7 )
	{
		m_lvoRankingWidth.SetValue( 1L, 7L );
	}// end if
	else if( lWidth >= 5 )
	{
		m_lvoRankingWidth.SetValue( 1L, 5L );
	}// end else if
	else if( lWidth >= 3 )
	{
		m_lvoRankingWidth.SetValue( 1L, 3L );
	}// end else if
	else
	{
		m_lvoRankingWidth.SetValue( 1L, 1L );
	}// end else
	m_lvoRankingWidth.GetValue( lWidth );

	// Validate height and make sure it is valid
	m_lvoRankingHeight.GetValue( lHeight );
	if( lHeight >= 7 )
	{
		m_lvoRankingHeight.SetValue( 1L, 7L );
	}// end if
	else if( lHeight >= 5 )
	{
		m_lvoRankingHeight.SetValue( 1L, 5L );
	}// end else if
	else if( lHeight >= 3 )
	{
		m_lvoRankingHeight.SetValue( 1L, 3L );
	}// end else if
	else
	{
		m_lvoRankingHeight.SetValue( 1L, 1L );
	}// end else
	m_lvoRankingHeight.GetValue( lHeight );

	// Allocate a MIL ranking

	SVMatroxBufferCreateStruct l_Create;
	l_Create.m_lSizeX = lWidth;
	l_Create.m_lSizeY = lHeight;
	l_Create.m_eType = RANKING_BIT_DEPTH;
	l_Create.m_eAttribute = SVBufAttStructElement;
	l_Code = SVMatroxBufferInterface::Create( m_milRanking, l_Create );

	pRankingData = new long[lWidth*lHeight];
	for( l = 0; l < lWidth; l++ )
	{
		for( m = 0; m < lHeight; m++ )
		{
			m_plvoRankingCells[m * lWidth + l]->GetValue( lValue );
			pRankingData[m * lWidth + l] = lValue == 1 ? 1 : SVValueDontCare;
		}// end for

	}// end for

	// Put the custom data in it.
	l_Code = SVMatroxBufferInterface::PutBuffer( m_milRanking, pRankingData );

	delete [] pRankingData;	
	for( l = 0; l < lWidth * lHeight; l++ )
	{
		m_plvoRankingCells[l]->ObjectAttributesAllowedRef() = SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES | SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;
	}// end for

	for( l = lWidth * lHeight; l < 49; l++ )
	{
		m_plvoRankingCells[l]->ObjectAttributesAllowedRef() = SV_EMBEDABLE;
		m_plvoRankingCells[l]->ObjectAttributesSetRef() = SV_EMBEDABLE;
	}// end for

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : onRun
// -----------------------------------------------------------------------------
// .Description : Runs this operator.
//              : Returns FALSE, if operator cannot run ( may be deactivated ! )
////////////////////////////////////////////////////////////////////////////////
BOOL SVRankingFilterClass::onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus )
{ 
	long lRank;

	// Force a copy forward to keep the display correct
	for( long l = 0; l < 49; l++ )
	{
		m_plvoRankingCells[l]->CopyLastSetValue( RRunStatus.m_lResultDataIndex );
	}// end for

	m_lvoRankingWidth.CopyLastSetValue( RRunStatus.m_lResultDataIndex );
	m_lvoRankingHeight.CopyLastSetValue( RRunStatus.m_lResultDataIndex );
	m_lvoRankingRank.CopyLastSetValue( RRunStatus.m_lResultDataIndex );

	if( m_pCurrentUIOPL && !( RInputImageHandle.empty() ) && !( ROutputImageHandle.empty() ) )
	{
		SVImageBufferHandleImage l_InMilHandle;
		SVImageBufferHandleImage l_OutMilHandle;

		RInputImageHandle->GetData( l_InMilHandle );
		ROutputImageHandle->GetData( l_OutMilHandle );
		
		SVMatroxImageInterface::SVStatusCode l_Code;

		m_lvoRankingRank.GetValue( lRank );
		l_Code = SVMatroxImageInterface::Rank( l_OutMilHandle.GetBuffer(),
				( First == TRUE ) ? l_InMilHandle.GetBuffer() : l_OutMilHandle.GetBuffer(),
				m_milRanking, lRank );
	    if( SVMEE_STATUS_OK != l_Code )
		{
			// Signal that something was wrong...
			SetInvalid();
			RRunStatus.SetInvalid();
			return FALSE;
		}

		// Success...
		return TRUE;
	}

	// Signal that something was wrong...
	SetInvalid();
	RRunStatus.SetInvalid();
	return FALSE;
}

