//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCustomFilters
//* .File Name       : $Workfile:   SVCustomFilters.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:04:46  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVCustomFilters.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVImageProcessingClass.h"
#pragma endregion Includes

////////////////////////////////////////////////////////////////////////////////
// Other Necessary Include File(s) - Module Link(s)
////////////////////////////////////////////////////////////////////////////////

//******************************************************************************
//* DEFINITIONS OF MODULE-LOCAL VARIABLES:
//******************************************************************************

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define KERNEL_BIT_DEPTH     8L

SV_IMPLEMENT_CLASS( SVCustomFilterClass, SVCustomFilterClassGuid )

//******************************************************************************
// Constructor(s):
//******************************************************************************
SVCustomFilterClass::SVCustomFilterClass( SVObjectClass* POwner, int StringResourceID )
					: SVFilterClass( POwner, StringResourceID )
{
	init();
}

SVCustomFilterClass::~SVCustomFilterClass()
{
	m_milKernel.clear();
}

void SVCustomFilterClass::init()
{
	long l( 0 );

	m_outObjectInfo.ObjectTypeInfo.SubType = SVCustomFilterObjectType;

	RegisterEmbeddedObject( &m_lvoCell01, SVCustomFilterCell01Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL01, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell02, SVCustomFilterCell02Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL02, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell03, SVCustomFilterCell03Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL03, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell04, SVCustomFilterCell04Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL04, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell05, SVCustomFilterCell05Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL05, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell06, SVCustomFilterCell06Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL06, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell07, SVCustomFilterCell07Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL07, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell08, SVCustomFilterCell08Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL08, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell09, SVCustomFilterCell09Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL09, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell10, SVCustomFilterCell10Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL10, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell11, SVCustomFilterCell11Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL11, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell12, SVCustomFilterCell12Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL12, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell13, SVCustomFilterCell13Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL13, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell14, SVCustomFilterCell14Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL14, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell15, SVCustomFilterCell15Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL15, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell16, SVCustomFilterCell16Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL16, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell17, SVCustomFilterCell17Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL17, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell18, SVCustomFilterCell18Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL18, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell19, SVCustomFilterCell19Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL19, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell20, SVCustomFilterCell20Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL20, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell21, SVCustomFilterCell21Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL21, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell22, SVCustomFilterCell22Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL22, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell23, SVCustomFilterCell23Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL23, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell24, SVCustomFilterCell24Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL24, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell25, SVCustomFilterCell25Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL25, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell26, SVCustomFilterCell26Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL26, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell27, SVCustomFilterCell27Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL27, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell28, SVCustomFilterCell28Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL28, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell29, SVCustomFilterCell29Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL29, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell30, SVCustomFilterCell30Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL30, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell31, SVCustomFilterCell31Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL31, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell32, SVCustomFilterCell32Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL32, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell33, SVCustomFilterCell33Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL33, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell34, SVCustomFilterCell34Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL34, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell35, SVCustomFilterCell35Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL35, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell36, SVCustomFilterCell36Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL36, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell37, SVCustomFilterCell37Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL37, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell38, SVCustomFilterCell38Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL38, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell39, SVCustomFilterCell39Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL39, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell40, SVCustomFilterCell40Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL40, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell41, SVCustomFilterCell41Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL41, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell42, SVCustomFilterCell42Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL42, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell43, SVCustomFilterCell43Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL43, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell44, SVCustomFilterCell44Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL44, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell45, SVCustomFilterCell45Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL45, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell46, SVCustomFilterCell46Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL46, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell47, SVCustomFilterCell47Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL47, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell48, SVCustomFilterCell48Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL48, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoCell49, SVCustomFilterCell49Guid, IDS_OBJECTNAME_CUSTOMFILTER_CELL49, false, SVResetItemOwner );

	RegisterEmbeddedObject( &m_lvoKernelWidth, SVCustomFilterKernelWidthGuid, IDS_OBJECTNAME_CUSTOMFILTER_KERNELWIDTH, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoKernelHeight, SVCustomFilterKernelHeightGuid, IDS_OBJECTNAME_CUSTOMFILTER_KERNELHEIGHT, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_bvoClippingOn, SVCustomFilterClippingGuid, IDS_OBJECTNAME_CUSTOMFILTER_CLIPPING, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_bvoAbsoluteValue, SVCustomFilterAbsoluteGuid, IDS_OBJECTNAME_CUSTOMFILTER_ABSOLUTE, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_lvoTransformationFactor, SVCustomFilterTransformGuid, IDS_OBJECTNAME_CUSTOMFILTER_TRANSFORM, false, SVResetItemOwner );

	m_plvoKernelCells[ 0] = &m_lvoCell01;
	m_plvoKernelCells[ 1] = &m_lvoCell02;
	m_plvoKernelCells[ 2] = &m_lvoCell03;
	m_plvoKernelCells[ 3] = &m_lvoCell04;
	m_plvoKernelCells[ 4] = &m_lvoCell05;
	m_plvoKernelCells[ 5] = &m_lvoCell06;
	m_plvoKernelCells[ 6] = &m_lvoCell07;
	m_plvoKernelCells[ 7] = &m_lvoCell08;
	m_plvoKernelCells[ 8] = &m_lvoCell09;
	m_plvoKernelCells[ 9] = &m_lvoCell10;
	m_plvoKernelCells[10] = &m_lvoCell11;
	m_plvoKernelCells[11] = &m_lvoCell12;
	m_plvoKernelCells[12] = &m_lvoCell13;
	m_plvoKernelCells[13] = &m_lvoCell14;
	m_plvoKernelCells[14] = &m_lvoCell15;
	m_plvoKernelCells[15] = &m_lvoCell16;
	m_plvoKernelCells[16] = &m_lvoCell17;
	m_plvoKernelCells[17] = &m_lvoCell18;
	m_plvoKernelCells[18] = &m_lvoCell19;
	m_plvoKernelCells[19] = &m_lvoCell20;
	m_plvoKernelCells[20] = &m_lvoCell21;
	m_plvoKernelCells[21] = &m_lvoCell22;
	m_plvoKernelCells[22] = &m_lvoCell23;
	m_plvoKernelCells[23] = &m_lvoCell24;
	m_plvoKernelCells[24] = &m_lvoCell25;
	m_plvoKernelCells[25] = &m_lvoCell26;
	m_plvoKernelCells[26] = &m_lvoCell27;
	m_plvoKernelCells[27] = &m_lvoCell28;
	m_plvoKernelCells[28] = &m_lvoCell29;
	m_plvoKernelCells[29] = &m_lvoCell30;
	m_plvoKernelCells[30] = &m_lvoCell31;
	m_plvoKernelCells[31] = &m_lvoCell32;
	m_plvoKernelCells[32] = &m_lvoCell33;
	m_plvoKernelCells[33] = &m_lvoCell34;
	m_plvoKernelCells[34] = &m_lvoCell35;
	m_plvoKernelCells[35] = &m_lvoCell36;
	m_plvoKernelCells[36] = &m_lvoCell37;
	m_plvoKernelCells[37] = &m_lvoCell38;
	m_plvoKernelCells[38] = &m_lvoCell39;
	m_plvoKernelCells[39] = &m_lvoCell40;
	m_plvoKernelCells[40] = &m_lvoCell41;
	m_plvoKernelCells[41] = &m_lvoCell42;
	m_plvoKernelCells[42] = &m_lvoCell43;
	m_plvoKernelCells[43] = &m_lvoCell44;
	m_plvoKernelCells[44] = &m_lvoCell45;
	m_plvoKernelCells[45] = &m_lvoCell46;
	m_plvoKernelCells[46] = &m_lvoCell47;
	m_plvoKernelCells[47] = &m_lvoCell48;
	m_plvoKernelCells[48] = &m_lvoCell49;

	for( l = 0; l < 49; l++ )
	{
		m_plvoKernelCells[l]->SetDefaultValue( 1, TRUE );
	}// end for

	m_lvoKernelWidth.SetDefaultValue( 3L, TRUE );
	m_lvoKernelHeight.SetDefaultValue( 3L, TRUE );
	m_bvoClippingOn.SetDefaultValue( TRUE, TRUE );
	m_bvoAbsoluteValue.SetDefaultValue( TRUE, TRUE );
	m_lvoTransformationFactor.SetDefaultValue( 1, TRUE );

	for( l = 0; l < 49; l++ )
	{
		m_plvoKernelCells[l]->ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;
	}// end for

	m_lvoKernelWidth.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;
	m_lvoKernelHeight.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;
	m_bvoClippingOn.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;
	m_bvoAbsoluteValue.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;
	m_lvoTransformationFactor.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;

	// Set default inputs and outputs
	addDefaultInputObjects();

	RebuildKernel();
}
////////////////////////////////////////////////////////////////////////////////
// .Title       : CreateObject
// -----------------------------------------------------------------------------
// .Description : ...
////////////////////////////////////////////////////////////////////////////////
BOOL SVCustomFilterClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = TRUE;

	bOk &= SVFilterClass::CreateObject( PCreateStructure );

	bOk &= RebuildKernel();

	return bOk;
}

HRESULT SVCustomFilterClass::ResetObject()
{
	HRESULT l_hrOk = SVFilterClass::ResetObject();

	if ( ! RebuildKernel() )
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

BOOL SVCustomFilterClass::OnValidate()
{
	BOOL bRetVal = TRUE;
	long lWidth;
	long lHeight;
	
	m_lvoKernelWidth.GetValue( lWidth );
	m_lvoKernelHeight.GetValue( lHeight );
	
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
// .Title       : Initialization of class SVCustomFilterClass
// -----------------------------------------------------------------------------
// .Description : Initialization of newly Instantiated Object
////////////////////////////////////////////////////////////////////////////////
BOOL SVCustomFilterClass::RebuildKernel()
{
	SVMatroxBufferInterface::SVStatusCode l_Code;

	// First free old kernel
	m_milKernel.clear();

	// Average kernel information data definition
	unsigned char *pKernelData;
	long lValue;
	long lWidth;
	long lHeight;
	long l;
	long m;
	BOOL bValue;

	// Validate width and make sure it is valid
	m_lvoKernelWidth.GetValue( lWidth );
	if( lWidth >= 7 )
	{
		m_lvoKernelWidth.SetValue( 1L, 7L );
	}// end if
	else if( lWidth >= 5 )
	{
		m_lvoKernelWidth.SetValue( 1L, 5L );
	}// end else if
	else if( lWidth >= 3 )
	{
		m_lvoKernelWidth.SetValue( 1L, 3L );
	}// end else if
	else
	{
		m_lvoKernelWidth.SetValue( 1L, 1L );
	}// end else
	m_lvoKernelWidth.GetValue( lWidth );

	// Validate height and make sure it is valid
	m_lvoKernelHeight.GetValue( lHeight );
	if( lHeight >= 7 )
	{
		m_lvoKernelHeight.SetValue( 1L, 7L );
	}// end if
	else if( lHeight >= 5 )
	{
		m_lvoKernelHeight.SetValue( 1L, 5L );
	}// end else if
	else if( lHeight >= 3 )
	{
		m_lvoKernelHeight.SetValue( 1L, 3L );
	}// end else if
	else
	{
		m_lvoKernelHeight.SetValue( 1L, 1L );
	}// end else
	m_lvoKernelHeight.GetValue( lHeight );

	// Allocate a MIL kernel.

	SVMatroxBufferCreateStruct l_Create;
	l_Create.m_eAttribute = SVBufAttKernel;
	l_Create.m_eType = SV8BitSigned;
	l_Create.m_lSizeX = lWidth;
	l_Create.m_lSizeY = lHeight;
	l_Code = SVMatroxBufferInterface::Create( m_milKernel, l_Create );

	pKernelData = new unsigned char[lWidth*lHeight];
	for( l = 0; l < lWidth; l++ )
	{
		for( m = 0; m < lHeight; m++ )
		{
			m_plvoKernelCells[m * lWidth + l]->GetValue( lValue );
			pKernelData[m * lWidth + l] = static_cast<unsigned char>(lValue);
		}// end for

	}// end for

	// Put the custom data in it.
	SVMatroxBufferInterface::PutBuffer( m_milKernel, pKernelData );

	long l_lValue=0;
	SVMatroxBufferInterface::ControlNeighborhood( m_milKernel, SVKernelDefault, l_lValue ); 
	SVMatroxBufferInterface::ControlNeighborhood( m_milKernel, SVBufOverScan, SVValueMirror ); 

	m_bvoClippingOn.GetValue( bValue );
	if( bValue )
	{
		SVMatroxBufferInterface::ControlNeighborhood( m_milKernel, SVBufSaturation, SVValueEnable ); 
	}// end if

	m_bvoAbsoluteValue.GetValue( bValue );
	if( bValue )
	{
		SVMatroxBufferInterface::ControlNeighborhood( m_milKernel, SVAbsoluteValue, SVValueEnable ); 
	}// end if

	m_lvoTransformationFactor.GetValue( lValue );
	if( lValue != 1 )
	{
		SVMatroxBufferInterface::ControlNeighborhood( m_milKernel, SVNormalization, lValue ); 
	}// end if

	delete [] pKernelData;	
	for( l = 0; l < lWidth * lHeight; l++ )
	{
		m_plvoKernelCells[l]->ObjectAttributesAllowedRef() = (SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES | SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE);
	}// end for

	for( l = lWidth * lHeight; l < 49; l++ )
	{
		m_plvoKernelCells[l]->ObjectAttributesAllowedRef() = SV_EMBEDABLE;
		m_plvoKernelCells[l]->ObjectAttributesSetRef() = SV_EMBEDABLE;
	}// end for

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : onRun
// -----------------------------------------------------------------------------
// .Description : Runs this operator.
//              : Returns FALSE, if operator cannot run ( may be deactivated ! )
////////////////////////////////////////////////////////////////////////////////
BOOL SVCustomFilterClass::onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus )
{ 
	// Force a copy forward to keep the display correct
	for( long l = 0; l < 49; l++ )
	{
		m_plvoKernelCells[l]->CopyLastSetValue( RRunStatus.m_lResultDataIndex );
	}// end for

	SVMatroxImageInterface::SVStatusCode l_Code;

	m_lvoKernelWidth.CopyLastSetValue( RRunStatus.m_lResultDataIndex );
	m_lvoKernelHeight.CopyLastSetValue( RRunStatus.m_lResultDataIndex );
	m_bvoClippingOn.CopyLastSetValue( RRunStatus.m_lResultDataIndex );
	m_bvoAbsoluteValue.CopyLastSetValue( RRunStatus.m_lResultDataIndex );
	m_lvoTransformationFactor.CopyLastSetValue( RRunStatus.m_lResultDataIndex );

	if( m_pCurrentUIOPL && !( ROutputImageHandle.empty() ) && !( RInputImageHandle.empty() ) )
	{
		SVImageBufferHandleImage l_MilHandle;
		ROutputImageHandle->GetData( l_MilHandle );

		SVImageBufferHandleImage l_InMilHandle;
		RInputImageHandle->GetData( l_InMilHandle );

		l_Code = SVMatroxImageInterface::Convolve(l_MilHandle.GetBuffer(), 
						( First == TRUE ) ? 
						l_InMilHandle.GetBuffer() : 
						l_MilHandle.GetBuffer(), 
						m_milKernel );


		long l_MILError( 0 );
		if( l_Code != SVMEE_STATUS_OK )
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
