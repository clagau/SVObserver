//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVWindowTool
//* .File Name       : $Workfile:   SVWindowTool.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.6  $
//* .Check In Date   : $Date:   15 May 2014 15:03:02  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVWindowTool.h"
#include "SVObjectLibrary/SVClsids.h"
#include "AnalyzerOperators/SVAnalyzer.h"
#include "Operators/SVConditional.h"
#include "Operators/SVLUTEquation.h"
#include "Operators/SVLUTOperator.h"
#include "Operators/SVStdImageOperatorListClass.h"
#include "Operators/SVThresholdClass.h"
#include "Operators/SVUserMaskOperatorClass.h"
#include "SVMatroxLibrary/SVOLicenseManager.h"
#include "Operators/ToolSizeAdjustTask.h"
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvTo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( SVWindowToolClass, SVWindowToolClassGuid );

SVWindowToolClass::SVWindowToolClass( SVObjectClass* POwner, int StringResourceID )
	:SVToolClass( POwner, StringResourceID )
{
	init();
}

void SVWindowToolClass::init()
{
	m_canResizeToParent = true;
	// Set up your type... in this case this will reference that this tool is a 
	// Window Tool.
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVToolObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvPb::SVWindowToolObjectType;

	BuildEmbeddedObjectList ();

	BuildAvailableAnalyzerList (); 

	AddUnaryImageOperatorListAsChild ();
}

void SVWindowToolClass::BuildEmbeddedObjectList ()
{
	// Register Embedded Object
	RegisterEmbeddedObject( &m_SourceImageNames, 
							SVSourceImageNamesGuid, 
							IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, 
							false, 
							SvOi::SVResetItemTool );
}

void SVWindowToolClass::BuildAvailableAnalyzerList ()
{
	// Default taskObjectList items:

	// Populate the available analyzer list
	SvIe::SVClassInfoStruct analyzerClassInfo;

	// Add the Blob analyzer
	analyzerClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVAnalyzerObjectType;
	analyzerClassInfo.m_ObjectTypeInfo.SubType = SvPb::SVBlobAnalyzerObjectType;
	analyzerClassInfo.m_ClassId = SVBlobAnalyzerClassGuid;
	analyzerClassInfo.m_ClassName = SvUl::LoadStdString( IDS_CLASSNAME_SVBLOBANALYZER );
	m_availableChildren.push_back( analyzerClassInfo );

	// Add the White Pixel Analyzer
	analyzerClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVAnalyzerObjectType;
	analyzerClassInfo.m_ObjectTypeInfo.SubType = SvPb::SVPixelAnalyzerObjectType;
	analyzerClassInfo.m_ClassId = SVPixelAnalyzerClassGuid;
	analyzerClassInfo.m_ClassName = SvUl::LoadStdString( IDS_CLASSNAME_SVPIXELANALYZER );
	m_availableChildren.push_back(analyzerClassInfo);

	// Add the Luminance Analyzer
	analyzerClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVAnalyzerObjectType;
	analyzerClassInfo.m_ObjectTypeInfo.SubType = SvPb::SVLuminanceAnalyzerObjectType;
	analyzerClassInfo.m_ClassId = SVLuminanceAnalyzerClassGuid;
	analyzerClassInfo.m_ClassName = SvUl::LoadStdString( IDS_CLASSNAME_SVLUMINANCEANALYZER );
	m_availableChildren.push_back(analyzerClassInfo);

	// Add the Histogram Analyzer
	analyzerClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVAnalyzerObjectType;
	analyzerClassInfo.m_ObjectTypeInfo.SubType = SvPb::SVHistogramAnalyzerObjectType;
	analyzerClassInfo.m_ClassId = SVHistogramAnalyzerClassGuid;
	analyzerClassInfo.m_ClassName = SvUl::LoadStdString( IDS_CLASSNAME_SVHISTOGRAMANALYZER );
	m_availableChildren.push_back(analyzerClassInfo);

	// Sri 04-12-00
	// Add the Pattern Analyzer	
	analyzerClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVAnalyzerObjectType;
	analyzerClassInfo.m_ObjectTypeInfo.SubType = SvPb::SVPatternAnalyzerObjectType;
	analyzerClassInfo.m_ClassId = SVPatternAnalyzerClassGuid;
	analyzerClassInfo.m_ClassName = SvUl::LoadStdString( IDS_CLASSNAME_SVPATTERNANALYZER );
	m_availableChildren.push_back(analyzerClassInfo);

	if (SVOLicenseManager::Instance().HasMatroxIdentificationLicense())
	{
		// Add the BarCode Analyzer
		analyzerClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVAnalyzerObjectType;
		analyzerClassInfo.m_ObjectTypeInfo.SubType = SvPb::SVBarCodeAnalyzerObjectType;
		analyzerClassInfo.m_ClassId = SVBarCodeAnalyzerClassGuid;
		analyzerClassInfo.m_ClassName = SvUl::LoadStdString( IDS_CLASSNAME_SVBARCODEANALYZER );
		m_availableChildren.push_back(analyzerClassInfo);

		// Add the Matrox OCV Analyzer	
		analyzerClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVAnalyzerObjectType;
		analyzerClassInfo.m_ObjectTypeInfo.SubType = SvPb::SVOCVAnalyzerObjectType;
		analyzerClassInfo.m_ClassId = SVOCVAnalyzerClassGuid;
		analyzerClassInfo.m_ClassName = SvUl::LoadStdString( IDS_CLASSNAME_SVOCVANALYZER );
		m_availableChildren.push_back(analyzerClassInfo);
	}
}

void SVWindowToolClass::AddUnaryImageOperatorListAsChild ()
{
	// Build an operator list...
	// ...use Standard image operator list, because we need an output image! RO_20Mar2000
	SvOp::SVUnaryImageOperatorListClass* pOperatorList = new SvOp::SVStdImageOperatorListClass;

	// Operator list defaults:
	if(nullptr != pOperatorList)
	{
		// Requires a SVThresholdClass Object
		pOperatorList->Add( new SvOp::SVThresholdClass( pOperatorList ) );

		// and Requires a SVUsermaskOperatorClass Object
		pOperatorList->Add( new SvOp::SVUserMaskOperatorClass( pOperatorList ) );

		// and Requires a SVLUTOperatorClass Object
		pOperatorList->Add( new SvOp::SVLUTOperatorClass( pOperatorList ) );

		// Add the UnaryImageOperatorList to the Tool's List
		Add( pOperatorList );
	}
	
	SvOp::ToolSizeAdjustTask::AddToFriendlist(this,true,true,true);
	
}

SVWindowToolClass::~SVWindowToolClass()
{ 
}


bool SVWindowToolClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVToolClass::CreateObject(rCreateStructure);
	
	m_SourceImageNames.setSaveValueFlag(false);
	m_SourceImageNames.SetObjectAttributesAllowed( SvPb::remotelySetable | SvPb::setableOnline, SvOi::SetAttributeType::RemoveAttribute );
	
	if(bOk)
	{
		bOk = (S_OK == SvOp::ToolSizeAdjustTask::EnsureInFriendList(this, true, true, true));
	}
	
	m_isCreated = bOk;
	return bOk;
}

bool SVWindowToolClass::SetDefaultFormulas(SvStl::MessageContainerVector *pErrorMessages)
{
	bool bRetVal = true;

	// Set Default Formula of LUTEquation, if any...
	SvDef::SVObjectTypeInfoStruct lutEquationInfo;
	lutEquationInfo.ObjectType	= SvPb::SVEquationObjectType;
	lutEquationInfo.SubType		= SvPb::SVLUTEquationObjectType;
	SvOp::SVLUTEquationClass* pLUTEquation = dynamic_cast<SvOp::SVLUTEquationClass*>(getFirstObject(lutEquationInfo));
	if( pLUTEquation )
	{
		bRetVal = pLUTEquation->SetDefaultFormula(pErrorMessages) && bRetVal;
	}

	return bRetVal;
}


HRESULT SVWindowToolClass::SetImageExtent(const SVImageExtentClass& rImageExtent)
{
	HRESULT l_hrOk = m_toolExtent.ValidExtentAgainstParentImage( rImageExtent );

	if( S_OK == l_hrOk )
	{
		l_hrOk = SVToolClass::SetImageExtent( rImageExtent );
	}

	return l_hrOk;
}

HRESULT SVWindowToolClass::SetImageExtentToParent()
{
	HRESULT l_hrOk = S_OK;
	SVImageExtentClass NewExtent;

	l_hrOk = m_toolExtent.UpdateExtentToParentExtents( NewExtent );

	if( S_OK == l_hrOk )
	{
		l_hrOk = SVToolClass::SetImageExtent( NewExtent );
	}
	return l_hrOk;
}


HRESULT SVWindowToolClass::SetImageExtentToFit( const SVImageExtentClass& rImageExtent )
{
	HRESULT l_hrOk = S_OK;

	l_hrOk = m_toolExtent.UpdateExtentAgainstParentImage( rImageExtent );

	return l_hrOk;
}

SvIe::SVTaskObjectClass* SVWindowToolClass::GetObjectAtPoint(const SVPoint<double>& rPoint )
{
	SVTaskObjectClass *pObject{nullptr};

	if (SvDef::SVExtentLocationPropertyUnknown != GetImageExtent().GetLocationPropertyAt(rPoint))
	{
		pObject = this;
	}

	return pObject;
}

bool SVWindowToolClass::DoesObjectHaveExtents() const
{
	return true;
}

SvOi::ParametersForML SVWindowToolClass::getParameterForMonitorList(SvStl::MessageContainerVector& rMessages) const
{
	bool isNoError = true;
	bool isAuxNoError = true;
	SvOi::ParametersForML retList;
	retList.push_back(SvOi::ParameterPairForML(m_statusColor.GetCompleteName(), m_statusColor.GetUniqueObjectID()));
	if (nullptr != m_pToolConditional)
	{
		retList.push_back(m_pToolConditional->getResultData());
	}
	else
	{
		isNoError = false;
	}
	isNoError = addEntryToMonitorList(retList, SVExtentWidthObjectGuid) && isNoError;
	isNoError = addEntryToMonitorList(retList, SVExtentHeightObjectGuid) && isNoError;
	isAuxNoError = addEntryToMonitorList(retList, SVAuxiliarySourceXObjectGuid);
	isAuxNoError = addEntryToMonitorList(retList, SVAuxiliarySourceYObjectGuid) && isAuxNoError;

	if (!isAuxNoError)
	{
		SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SetAuxiliaryParameterToMonitorListFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
		rMessages.push_back(Msg);
	}
	else if (!isNoError)
	{
		SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SetParameterToMonitorListFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
		rMessages.push_back(Msg);
	}

	SvAo::SVAnalyzerClass* pCurrentAnalyzer = dynamic_cast<SvAo::SVAnalyzerClass *>(getFirstObject(SvDef::SVObjectTypeInfoStruct(SvPb::SVAnalyzerObjectType)));
	if (nullptr != pCurrentAnalyzer)
	{
		pCurrentAnalyzer->addParameterForMonitorList(rMessages, std::back_inserter(retList));
	}

	return retList;
}

SvVol::SVStringValueObjectClass* SVWindowToolClass::GetInputImageNames()
{
	return &m_SourceImageNames;
}

bool SVWindowToolClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = SVToolClass::ResetObject(pErrorMessages);

	UpdateImageWithExtent();

	return Result;
}

} //namespace SvTo
