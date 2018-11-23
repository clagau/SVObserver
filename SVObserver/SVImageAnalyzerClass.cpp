//*****************************************************************************
// \copyright COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVImageAnalyzerClass.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "InspectionEngine/SVImageClass.h"
#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS( SVImageAnalyzerClass, SVImageAnalyzerClassGuid );

SVImageAnalyzerClass::SVImageAnalyzerClass( LPCSTR ObjectName )
: SVAnalyzerClass( ObjectName ) 
{
	init();
}

SVImageAnalyzerClass::SVImageAnalyzerClass( SVObjectClass* pOwner, int StringResourceID )
: SVAnalyzerClass( pOwner, StringResourceID ) 
{
	init();
}

void SVImageAnalyzerClass::init()
{
	// Set object type info...
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVAnalyzerObjectType;
	// Set sub type only in derived classes!

	// Set Input requirement
	m_inputImageObjectInfo.SetInputObjectType(SvPb::SVImageObjectType, SvPb::SVImageMonoType);
	m_inputImageObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputImageObjectInfo, SvDef::ImageAnalyzerImageName );

	addDefaultInputObjects();
}

SVImageAnalyzerClass::~SVImageAnalyzerClass()
{
}

bool SVImageAnalyzerClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	return __super::ResetObject(pErrorMessages) && ValidateLocal(pErrorMessages);
}
bool SVImageAnalyzerClass::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	return __super::onRun(rRunStatus, pErrorMessages) && ValidateLocal(pErrorMessages);
}

SVImageClass* SVImageAnalyzerClass::getInputImage(bool bRunMode /*= false*/)
{
	return SvOl::getInput<SVImageClass>(m_inputImageObjectInfo, bRunMode);
}

unsigned long SVImageAnalyzerClass::GetInputPixelDepth()
{
	SVImageInfoClass ImageInfo; 
	unsigned long ulPixelDepth(0);
	SVImageClass* pImage = getInputImage();
	if (!pImage)
	{
		SvStl::MessageMgrStd MesMan( SvStl::MsgType::Data);
		MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16110, GetUniqueObjectID());
		MesMan.Throw();
	}
	else
	{
		ulPixelDepth = pImage->getPixelDepth();
	}
	return ulPixelDepth;
}

bool SVImageAnalyzerClass::ValidateLocal(SvStl::MessageContainerVector *pErrorMessages) const
{
	SVObjectClass* pObject = m_inputImageObjectInfo.GetInputObjectInfo().getObject();
	if( !m_inputImageObjectInfo.IsConnected() || nullptr == pObject || !pObject->IsValid() ) 
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
		return false;
	}

	return true;
}
