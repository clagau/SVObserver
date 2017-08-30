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
#include "SVOCore/SVImageProcessingClass.h"
#include "ObjectInterfaces/ICustom2Filter.h"
#include "SVObjectLibrary/SVClsids.h"
#pragma endregion Includes

#pragma region Declarations

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(Custom2Filter, Custom2FilterClassGuid)

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
bool Custom2Filter::CreateObject( SVObjectLevelCreateStruct* pCreateStructure )
{
	bool bOk = SVFilterClass::CreateObject( pCreateStructure );

	RebuildKernel();

	return bOk;
}

bool Custom2Filter::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	RebuildKernel();

	return Result && ValidateLocal(pErrorMessages);
}
#pragma endregion Public Methods

#pragma region Protected Methods
void Custom2Filter::RebuildKernel()
{
	HRESULT StatusCode;

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

	SvOi::ICustom2Filter::LongArray KernelArray;
	m_KernelArray.GetArrayValues( KernelArray );
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
}

bool Custom2Filter::onRun( bool First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	bool Result( false );
	HRESULT StatusCode;

	if( m_pCurrentUIOPL && !( ROutputImageHandle.empty() ) && !( RInputImageHandle.empty() ) )
	{
		SVImageBufferHandleImage MilHandle;
		ROutputImageHandle->GetData( MilHandle );

		SVImageBufferHandleImage InMilHandle;
		RInputImageHandle->GetData( InMilHandle );

		StatusCode = SVMatroxImageInterface::Convolve(MilHandle.GetBuffer(), 
			First ? InMilHandle.GetBuffer() : MilHandle.GetBuffer(), 
			m_milKernel );

		if( StatusCode == S_OK )
		{
			Result = true;
		}
		else
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RunFilterFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
		}
	}
	else
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	if( !Result )
	{
		// Signal that something was wrong...
		SetInvalid();
		rRunStatus.SetInvalid();
	}
	return Result;
}
#pragma endregion Protected Methods

#pragma region Private Methods
void Custom2Filter::init()
{
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SVCustom2FilterObjectType;

	RegisterEmbeddedObject( &m_KernelArray, Custom2FilterKernelGuid, IDS_OBJECTNAME_CUSTOMFILTER_KERNELCELL, false, SvOi::SVResetItemOwner );

	RegisterEmbeddedObject( &m_KernelWidth, SVCustomFilterKernelWidthGuid, IDS_OBJECTNAME_CUSTOMFILTER_KERNELWIDTH, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject( &m_KernelHeight, SVCustomFilterKernelHeightGuid, IDS_OBJECTNAME_CUSTOMFILTER_KERNELHEIGHT, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject( &m_NormalizationFactor, SVCustomFilterTransformGuid, IDS_OBJECTNAME_CUSTOMFILTER_TRANSFORM, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject( &m_AbsoluteValue, SVCustomFilterAbsoluteGuid, IDS_OBJECTNAME_CUSTOMFILTER_ABSOLUTE, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject( &m_ClippingEnabled, SVCustomFilterClippingGuid, IDS_OBJECTNAME_CUSTOMFILTER_CLIPPING, false, SvOi::SVResetItemOwner );

	m_KernelArray.SetArraySize( SvOi::ICustom2Filter::StandardKernelSize*SvOi::ICustom2Filter::StandardKernelSize );
	m_KernelArray.SetDefaultValue( 1, TRUE );

	m_KernelWidth.SetDefaultValue( SvOi::ICustom2Filter::StandardKernelSize, TRUE );
	m_KernelHeight.SetDefaultValue( SvOi::ICustom2Filter::StandardKernelSize, TRUE );
	m_ClippingEnabled.SetDefaultValue( TRUE, TRUE );
	m_AbsoluteValue.SetDefaultValue( TRUE, TRUE );
	m_NormalizationFactor.SetDefaultValue( SvOi::ICustom2Filter::StandardKernelSize*SvOi::ICustom2Filter::StandardKernelSize, TRUE );

	const UINT cAttributes = SvOi::SV_PRINTABLE | SvOi::SV_SETABLE_ONLINE | SvOi::SV_REMOTELY_SETABLE;
	m_KernelArray.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );

	m_KernelWidth.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	m_KernelHeight.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	m_ClippingEnabled.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	m_AbsoluteValue.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	m_NormalizationFactor.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );

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
	else if( SvOi::ICustom2Filter::MaxKernelSize < Size )
	{
		Size = SvOi::ICustom2Filter::MaxKernelSize; 
	}
	rKernelSize.SetValue(Size);

	return Size;
}

bool Custom2Filter::ValidateLocal(SvStl::MessageContainerVector *pErrorMessages) const
{
	bool Result( true );

	//Check that Width and Height are odd and between 1 and MaxKernelSize
	long Width( 0 );
	m_KernelWidth.GetValue( Width );
	if( 1 != Width % 2 || 1 > Width || SvOi::ICustom2Filter::MaxKernelSize < Width )
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SVStringVector msgList;
			msgList.push_back(SvUl_SF::Format(_T("%d"), Width));
			msgList.push_back(SvUl_SF::Format(_T("%d"), SvOi::ICustom2Filter::MaxKernelSize));
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_DataInvalidKernelWidth, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	long Height( 0 );
	m_KernelHeight.GetValue( Height );
	if( 1 != Height % 2 || 1 > Height || SvOi::ICustom2Filter::MaxKernelSize < Height )
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SVStringVector msgList;
			msgList.push_back(SvUl_SF::Format(_T("%d"), Height));
			msgList.push_back(SvUl_SF::Format(_T("%d"), SvOi::ICustom2Filter::MaxKernelSize));
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_DataInvalidKernelHeight, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	long NormalizationFactor( 0 );
	m_NormalizationFactor.GetValue( NormalizationFactor );
	if( 0 >= NormalizationFactor )
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_DataInvalidNormalizationFactor, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	return Result;
}
#pragma endregion Private Methods

