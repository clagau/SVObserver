//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : Custom2Filter
//* .File Name       : $Workfile:   Custom2Filter.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Oct 2014 11:14:58  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "Custom2Filter.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVImageProcessingClass.h"
#pragma endregion Includes

#pragma region Declarations

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( Custom2Filter, Custom2FilterClassGuid )

#pragma region Constructor
Custom2Filter::Custom2Filter( SVObjectClass* pOwner, int StringResourceID )
	: SVFilterClass( pOwner, StringResourceID )
{
	init();
}

Custom2Filter::~Custom2Filter()
{
	m_milKernel.clear();
}
#pragma endregion Constructor

#pragma region Public Methods
BOOL Custom2Filter::CreateObject( SVObjectLevelCreateStruct* pCreateStructure )
{
	BOOL bOk = TRUE;

	bOk &= SVFilterClass::CreateObject( pCreateStructure );

	bOk &= RebuildKernel();

	return bOk;
}

HRESULT Custom2Filter::ResetObject()
{
	HRESULT l_hrOk = SVFilterClass::ResetObject();

	if ( ! RebuildKernel() )
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}
#pragma endregion Public Methods

#pragma region Protected Methods
BOOL Custom2Filter::RebuildKernel()
{
	SVMatroxBufferInterface::SVStatusCode StatusCode;

	// First free old kernel
	m_milKernel.clear();

	unsigned char *pKernelData;
	long Value( 0 );
	long Width( 0 );
	long Height( 0 );

	Width = validateKernelSize( m_KernelWidth );
	Height = validateKernelSize( m_KernelHeight );

	SVMatroxBufferCreateStruct CreateStruct;
	CreateStruct.m_eAttribute = SVBufAttKernel;
	CreateStruct.m_eType = SV8BitSigned;
	CreateStruct.m_lSizeX = Width;
	CreateStruct.m_lSizeY = Height;
	StatusCode = SVMatroxBufferInterface::Create( m_milKernel, CreateStruct );

	LongArray KernelArray;
	m_KernelArray.GetValues( KernelArray );
	long unsigned Index(0);

	pKernelData = new unsigned char[Width*Height];
	for( long i = 0; i < Height; i++ )
	{
		for( long j = 0; j < Width; j++ )
		{
			Index = i * Width + j;
			Value = 0;
			if( Index < KernelArray.size() )
			{
				Value = KernelArray[ Index ];
			}
			pKernelData[Index] = static_cast<unsigned char> (Value);
		}
	}

	//Put the custom data in it.
	SVMatroxBufferInterface::PutBuffer( m_milKernel, pKernelData );

	Value = 0;
	SVMatroxBufferInterface::ControlNeighborhood( m_milKernel, SVKernelDefault, Value );
	SVMatroxBufferInterface::ControlNeighborhood( m_milKernel, SVBufOverScan, SVValueMirror );

	BOOL Flag(FALSE);
	m_ClippingEnabled.GetValue( Flag );
	if( Flag )
	{
		SVMatroxBufferInterface::ControlNeighborhood( m_milKernel, SVBufSaturation, SVValueEnable );
	}

	m_AbsoluteValue.GetValue( Flag );
	if( Flag )
	{
		SVMatroxBufferInterface::ControlNeighborhood( m_milKernel, SVAbsoluteValue, SVValueEnable );
	}

	m_NormalizationFactor.GetValue( Value );
	if( 1 != Value )
	{
		SVMatroxBufferInterface::ControlNeighborhood( m_milKernel, SVNormalization, Value ); 
	}

	delete [] pKernelData;

	return TRUE;
}

BOOL Custom2Filter::onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus )
{
	BOOL Result( FALSE );
	SVMatroxImageInterface::SVStatusCode StatusCode;

	m_KernelWidth.CopyLastSetValue( RRunStatus.m_lResultDataIndex );
	m_KernelHeight.CopyLastSetValue( RRunStatus.m_lResultDataIndex );
	m_ClippingEnabled.CopyLastSetValue( RRunStatus.m_lResultDataIndex );
	m_AbsoluteValue.CopyLastSetValue( RRunStatus.m_lResultDataIndex );
	m_NormalizationFactor.CopyLastSetValue( RRunStatus.m_lResultDataIndex );
	m_KernelArray.CopyLastSetValue( RRunStatus.m_lResultDataIndex );

	if( m_pCurrentUIOPL && !( ROutputImageHandle.empty() ) && !( RInputImageHandle.empty() ) )
	{
		SVImageBufferHandleImage MilHandle;
		ROutputImageHandle->GetData( MilHandle );

		SVImageBufferHandleImage InMilHandle;
		RInputImageHandle->GetData( InMilHandle );

		StatusCode = SVMatroxImageInterface::Convolve(MilHandle.GetBuffer(), 
			( First == TRUE ) ? InMilHandle.GetBuffer() : MilHandle.GetBuffer(), 
			m_milKernel );

		if( StatusCode == SVMEE_STATUS_OK )
		{
			Result = TRUE;
		}
	}

	if( !Result )
	{
		// Signal that something was wrong...
		SetInvalid();
		RRunStatus.SetInvalid();
	}
	return Result;
}

BOOL Custom2Filter::OnValidate()
{
	BOOL Result( TRUE );
	long Width( 0 );
	long Height( 0 );
	long NormalizationFactor( 0 );
	
	m_KernelWidth.GetValue( Width );
	m_KernelHeight.GetValue( Height );
	m_NormalizationFactor.GetValue( NormalizationFactor );
	
	//Check that Width and Height are odd and between 1 and MaxKernelSize
	if( 1 != Width % 2 || 1 > Width || MaxKernelSize < Width )
	{
		Result = FALSE;
	}

	if( 1 != Height % 2 || 1 > Height || MaxKernelSize < Height )
	{
		Result = FALSE;
	}

	if( 0 >= NormalizationFactor )
	{
		Result = FALSE;
	}

	Result = SVOperatorClass::OnValidate() || Result;

	if( !Result )
	{
		SetInvalid();
	}
	return Result;
}
#pragma endregion Protected Methods

#pragma region Private Methods
void Custom2Filter::init()
{
	RegisterEmbeddedObject( &m_KernelArray, Custom2FilterKernelGuid, IDS_OBJECTNAME_CUSTOMFILTER_KERNELCELL, false, SVResetItemOwner );

	RegisterEmbeddedObject( &m_KernelWidth, SVCustomFilterKernelWidthGuid, IDS_OBJECTNAME_CUSTOMFILTER_KERNELWIDTH, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_KernelHeight, SVCustomFilterKernelHeightGuid, IDS_OBJECTNAME_CUSTOMFILTER_KERNELHEIGHT, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_NormalizationFactor, SVCustomFilterTransformGuid, IDS_OBJECTNAME_CUSTOMFILTER_TRANSFORM, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_AbsoluteValue, SVCustomFilterAbsoluteGuid, IDS_OBJECTNAME_CUSTOMFILTER_ABSOLUTE, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_ClippingEnabled, SVCustomFilterClippingGuid, IDS_OBJECTNAME_CUSTOMFILTER_CLIPPING, false, SVResetItemOwner );

	m_KernelArray.SetArraySize( StandardKernelSize*StandardKernelSize );
	m_KernelArray.SetDefaultValue( 1, TRUE );

	m_KernelWidth.SetDefaultValue( StandardKernelSize, TRUE );
	m_KernelHeight.SetDefaultValue( StandardKernelSize, TRUE );
	m_ClippingEnabled.SetDefaultValue( TRUE, TRUE );
	m_AbsoluteValue.SetDefaultValue( TRUE, TRUE );
	m_NormalizationFactor.SetDefaultValue( StandardKernelSize*StandardKernelSize, TRUE );

	m_KernelArray.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;

	m_KernelWidth.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;
	m_KernelHeight.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;
	m_ClippingEnabled.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;
	m_AbsoluteValue.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;
	m_NormalizationFactor.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;

	// Set default inputs and outputs
	addDefaultInputObjects();

	RebuildKernel();
}

long Custom2Filter::validateKernelSize( SVLongValueObjectClass& rKernelSize )
{
	long Size(0);

	rKernelSize.GetValue( Size );
	//Check if odd otherwise make it odd
	if( 1 != Size % 2)
	{
		Size--;
	}
	if( 1 > Size )
	{
		Size = 1;
	}
	else if( MaxKernelSize < Size )
	{
		Size = MaxKernelSize; 
	}
	rKernelSize.SetValue( 1, Size );

	return Size;
}
#pragma endregion Private Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\Custom2Filter.cpp_v  $
 * 
 *    Rev 1.0   24 Oct 2014 11:14:58   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  942
 * SCR Title:  Create new Custom2 Filter SVO-324 SVO-67 SVO-74
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 */