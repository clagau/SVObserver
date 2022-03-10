//*****************************************************************************
// \copyright COPYRIGHT (c) 2016 by Körber Pharma Inspection GmbH. All Rights Reserved
// All Rights Reserved
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "ImageAnalyzer.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "InspectionEngine/SVImageClass.h"
#pragma endregion Includes

namespace SvAo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

///For this class it is not necessary to call SV_IMPLEMENT_CLASS as it is a base class and only derived classes are instantiated.
//SV_IMPLEMENT_CLASS( ImageAnalyzer, ImageAnalyzerGuid );

ImageAnalyzer::ImageAnalyzer( LPCSTR ObjectName )
: Analyzer( ObjectName ) 
{
	init();
}

ImageAnalyzer::ImageAnalyzer( SVObjectClass* pOwner, int StringResourceID )
: Analyzer( pOwner, StringResourceID ) 
{
	init();
}

void ImageAnalyzer::init()
{
	// Set object type info...
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVAnalyzerObjectType;
	// Set sub type only in derived classes!

	// Set Input requirement
	m_inputImage.SetInputObjectType(SvPb::SVImageObjectType, SvPb::SVImageMonoType);
	registerInputObject( &m_inputImage, SvDef::ImageAnalyzerImageName, SvPb::ImageInputEId);
	m_inputImage.setAllowedMode(SvOi::InputAllowedMode::IsBeforeTool);
}

ImageAnalyzer::~ImageAnalyzer()
{
}

bool ImageAnalyzer::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	return __super::ResetObject(pErrorMessages) && ValidateLocal(pErrorMessages);
}
bool ImageAnalyzer::onRun( SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	return __super::onRun(rRunStatus, pErrorMessages) && ValidateLocal(pErrorMessages);
}

SvIe::SVImageClass* ImageAnalyzer::getInputImage(bool bRunMode /*= false*/)
{
	return m_inputImage.getInput<SvIe::SVImageClass>(bRunMode);
}

unsigned long ImageAnalyzer::GetInputPixelDepth()
{
	SVImageInfoClass ImageInfo; 
	unsigned long ulPixelDepth(0);
	SvIe::SVImageClass* pImage = getInputImage();
	if (!pImage)
	{
		SvStl::MessageManager MesMan( SvStl::MsgType::Data);
		MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16110, getObjectId());
		MesMan.Throw();
	}
	else
	{
		ulPixelDepth = pImage->getPixelDepth();
	}
	return ulPixelDepth;
}

bool ImageAnalyzer::ValidateLocal(SvStl::MessageContainerVector *pErrorMessages) const
{
	SVObjectClass* pObject = m_inputImage.GetInputObjectInfo().getObject();
	if( !m_inputImage.IsConnected() || nullptr == pObject || !pObject->IsCreated() )
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
			pErrorMessages->push_back(Msg);
		}
		return false;
	}

	return true;
}

} //namespace SvAo
