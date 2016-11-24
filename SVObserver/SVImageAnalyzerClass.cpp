//*****************************************************************************
// \copyright COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVImageAnalyzerClass.h"
#include "SVStatusLibrary/MessageManager.h"
#include "ObjectInterfaces/ErrorNumbers.h"
#include "SVOCore/SVImageClass.h"
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

SVImageAnalyzerClass::SVImageAnalyzerClass( BOOL bCreateDefaultTaskList, SVObjectClass* pOwner, int StringResourceID )
: SVAnalyzerClass( bCreateDefaultTaskList, pOwner, StringResourceID ) 
{
	init();
}

void SVImageAnalyzerClass::init()
{
	// Set object type info...
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	// Set sub type only in derived classes!

	// Set Input requirement
	inputImageObjectInfo.SetInputObjectType( SVImageObjectType );
	inputImageObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputImageObjectInfo, _T( "ImageAnalyzerImage" ) );

	addDefaultInputObjects();
}

SVImageAnalyzerClass::~SVImageAnalyzerClass()
{
}

BOOL SVImageAnalyzerClass::CloseObject()
{
	return SVAnalyzerClass::CloseObject();
}

BOOL SVImageAnalyzerClass::CreateObject( SVObjectLevelCreateStruct* pCreateStructure )
{
	m_isCreated = SVAnalyzerClass::CreateObject( pCreateStructure );

	return m_isCreated;
}

SVImageClass* SVImageAnalyzerClass::getInputImage()
{
	if( inputImageObjectInfo.IsConnected() && inputImageObjectInfo.GetInputObjectInfo().PObject )
	{
		return ( SVImageClass* ) inputImageObjectInfo.GetInputObjectInfo().PObject;
	}
	return nullptr;
}

unsigned long SVImageAnalyzerClass::GetInputPixelDepth()
{
	SVImageInfoClass ImageInfo; 
	unsigned long ulPixelDepth(0);
	SVImageClass* pImage = getInputImage();
	if (!pImage)
	{
		SvStl::MessageMgrStd MesMan( SvStl::DataOnly );
		MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16110);
		MesMan.Throw();
	}
	else
	{
		ulPixelDepth = pImage->getPixelDepth();
	}
	return ulPixelDepth;
}

BOOL SVImageAnalyzerClass::OnValidate()
{
	BOOL l_bOk = true;

	l_bOk = l_bOk && inputImageObjectInfo.IsConnected();
	l_bOk = l_bOk && nullptr != inputImageObjectInfo.GetInputObjectInfo().PObject;
	l_bOk = l_bOk && inputImageObjectInfo.GetInputObjectInfo().PObject->IsValid();

	return l_bOk;
}
