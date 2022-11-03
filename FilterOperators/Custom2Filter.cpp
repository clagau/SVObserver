//******************************************************************************
//* COPYRIGHT (c) 2014 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "Definitions/StringTypeDef.h"
#include "ObjectInterfaces/SVImageBufferHandleInterface.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVMatroxLibrary/SVMatroxBufferCreateStruct.h"
#include "SVMatroxLibrary/SVMatroxImageInterface.h"
#include "InspectionEngine/RunStatus.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

#pragma region Declarations

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(Custom2Filter, SvPb::Custom2FilterClassId)

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
bool Custom2Filter::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVFilterClass::CreateObject(rCreateStructure);

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
	SVMatroxBufferInterface::Create( m_milKernel, CreateStruct );

	std::vector<long> KernelArray;
	m_KernelArray.GetArrayValues( KernelArray );
	pKernelData = new unsigned char[Width*Height];
	for( long i = 0; i < Height; i++ )
	{
		for( long j = 0; j < Width; j++ )
		{
			long unsigned Index = i * Width + j;
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

	BOOL Flag(false);
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

bool Custom2Filter::onRun( bool First, SvOi::SVImageBufferHandlePtr rInputImageHandle, SvOi::SVImageBufferHandlePtr rOutputImageHandle, SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	bool Result( false );

	if (m_pCurrentUIOPL && nullptr != rInputImageHandle && nullptr != rOutputImageHandle)
	{
		HRESULT StatusCode = SVMatroxImageInterface::Convolve(rOutputImageHandle->GetBuffer(),
			First ? rInputImageHandle->GetBuffer() : rOutputImageHandle->GetBuffer(),
			m_milKernel );

		if( StatusCode == S_OK )
		{
			Result = true;
		}
		else
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RunFilterFailed, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
				pErrorMessages->push_back(Msg);
			}
		}
	}
	else
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
			pErrorMessages->push_back(Msg);
		}
	}

	if( !Result )
	{
		// Signal that something was wrong...
		rRunStatus.SetInvalid();
	}
	return Result;
}
#pragma endregion Protected Methods

#pragma region Private Methods
void Custom2Filter::init()
{
	m_ObjectTypeInfo.m_SubType = SvPb::SVCustom2FilterObjectType;

	RegisterEmbeddedObject( &m_KernelArray, SvPb::FilterKernelEId, IDS_OBJECTNAME_CUSTOMFILTER_KERNELCELL, false, SvOi::SVResetItemOwner, true);

	RegisterEmbeddedObject( &m_KernelWidth, SvPb::FilterKernelWidthEId, IDS_OBJECTNAME_CUSTOMFILTER_KERNELWIDTH, false, SvOi::SVResetItemOwner, true);
	RegisterEmbeddedObject( &m_KernelHeight, SvPb::FilterKernelHeightEId, IDS_OBJECTNAME_CUSTOMFILTER_KERNELHEIGHT, false, SvOi::SVResetItemOwner, true);
	RegisterEmbeddedObject( &m_NormalizationFactor, SvPb::CustomFilterTransformEId, IDS_OBJECTNAME_CUSTOMFILTER_TRANSFORM, false, SvOi::SVResetItemOwner, true);
	RegisterEmbeddedObject( &m_AbsoluteValue, SvPb::CustomFilterAbsoluteEId, IDS_OBJECTNAME_CUSTOMFILTER_ABSOLUTE, false, SvOi::SVResetItemOwner, true);
	RegisterEmbeddedObject( &m_ClippingEnabled, SvPb::CustomFilterClippingEId, IDS_OBJECTNAME_CUSTOMFILTER_CLIPPING, false, SvOi::SVResetItemOwner, true);

	m_KernelArray.SetArraySize(SvDef::cStandardKernelSize*SvDef::cStandardKernelSize );
	m_KernelArray.SetDefaultValue( 1 );

	m_KernelWidth.SetDefaultValue(SvDef::cStandardKernelSize );
	m_KernelHeight.SetDefaultValue(SvDef::cStandardKernelSize );
	m_ClippingEnabled.SetDefaultValue(BOOL(true));
	m_AbsoluteValue.SetDefaultValue(BOOL(true));
	m_NormalizationFactor.SetDefaultValue(SvDef::cStandardKernelSize*SvDef::cStandardKernelSize );

	const UINT cAttributes = SvPb::audittrail | SvPb::setableOnline | SvPb::remotelySetable;
	m_KernelArray.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );

	m_KernelWidth.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	m_KernelHeight.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	m_ClippingEnabled.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	m_AbsoluteValue.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	m_NormalizationFactor.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );

	RebuildKernel();
}

long Custom2Filter::validateKernelSize(SvVol::SVLongValueObjectClass& rKernelSize )
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
	else if(SvDef::cMaxKernelSize < Size )
	{
		Size = SvDef::cMaxKernelSize;
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
	if( 1 != Width % 2 || 1 > Width || SvDef::cMaxKernelSize < Width )
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvDef::StringVector msgList;
			msgList.push_back(SvUl::Format(_T("%d"), Width));
			msgList.push_back(SvUl::Format(_T("%d"), SvDef::cMaxKernelSize));
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_DataInvalidKernelWidth, msgList, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
			pErrorMessages->push_back(Msg);
		}
	}

	long Height( 0 );
	m_KernelHeight.GetValue( Height );
	if( 1 != Height % 2 || 1 > Height || SvDef::cMaxKernelSize < Height )
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvDef::StringVector msgList;
			msgList.push_back(SvUl::Format(_T("%d"), Height));
			msgList.push_back(SvUl::Format(_T("%d"), SvDef::cMaxKernelSize));
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_DataInvalidKernelHeight, msgList, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
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
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_DataInvalidNormalizationFactor, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
			pErrorMessages->push_back(Msg);
		}
	}

	return Result;
}
#pragma endregion Private Methods

