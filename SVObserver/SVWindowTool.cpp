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

#include "SVOCore/SVAnalyzer.h"
#include "SVInspectionProcess.h"
#include "SVLUTEquation.h"
#include "SVLUTOperator.h"
#include "SVObserver.h"
#include "SVOCore/SVStdImageOperatorListClass.h"
#include "SVOLicenseManager/SVOLicenseManager.h"
#include "SVOCore/SVThresholdClass.h"
#include "SVUserMaskOperatorClass.h"
#include "ToolSizeAdjustTask.h"
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

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
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVToolObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SVWindowToolObjectType;

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
	SVClassInfoStruct analyzerClassInfo;

	// Add the Blob analyzer
	analyzerClassInfo.m_ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.m_ObjectTypeInfo.SubType = SVBlobAnalyzerObjectType;
	analyzerClassInfo.m_ClassId = SVBlobAnalyzerClassGuid;
	analyzerClassInfo.m_ClassName = SvUl::LoadStdString( IDS_CLASSNAME_SVBLOBANALYZER );
	m_availableChildren.Add( analyzerClassInfo );

	// Add the White Pixel Analyzer
	analyzerClassInfo.m_ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.m_ObjectTypeInfo.SubType = SVPixelAnalyzerObjectType;
	analyzerClassInfo.m_ClassId = SVPixelAnalyzerClassGuid;
	analyzerClassInfo.m_ClassName = SvUl::LoadStdString( IDS_CLASSNAME_SVPIXELANALYZER );
	m_availableChildren.Add(analyzerClassInfo);

	// Add the Luminance Analyzer
	analyzerClassInfo.m_ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.m_ObjectTypeInfo.SubType = SVLuminanceAnalyzerObjectType;
	analyzerClassInfo.m_ClassId = SVLuminanceAnalyzerClassGuid;
	analyzerClassInfo.m_ClassName = SvUl::LoadStdString( IDS_CLASSNAME_SVLUMINANCEANALYZER );
	m_availableChildren.Add(analyzerClassInfo);

	// Add the Histogram Analyzer
	analyzerClassInfo.m_ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.m_ObjectTypeInfo.SubType = SVHistogramAnalyzerObjectType;
	analyzerClassInfo.m_ClassId = SVHistogramAnalyzerClassGuid;
	analyzerClassInfo.m_ClassName = SvUl::LoadStdString( IDS_CLASSNAME_SVHISTOGRAMANALYZER );
	m_availableChildren.Add(analyzerClassInfo);

	// Sri 04-12-00
	// Add the Pattern Analyzer	
	analyzerClassInfo.m_ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.m_ObjectTypeInfo.SubType = SVPatternAnalyzerObjectType;
	analyzerClassInfo.m_ClassId = SVPatternAnalyzerClassGuid;
	analyzerClassInfo.m_ClassName = SvUl::LoadStdString( IDS_CLASSNAME_SVPATTERNANALYZER );
	m_availableChildren.Add(analyzerClassInfo);

	if ( TheSVOLicenseManager().HasMatroxIdentificationLicense() )
	{
		// Add the BarCode Analyzer
		analyzerClassInfo.m_ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
		analyzerClassInfo.m_ObjectTypeInfo.SubType = SVBarCodeAnalyzerObjectType;
		analyzerClassInfo.m_ClassId = SVBarCodeAnalyzerClassGuid;
		analyzerClassInfo.m_ClassName = SvUl::LoadStdString( IDS_CLASSNAME_SVBARCODEANALYZER );
		m_availableChildren.Add(analyzerClassInfo);
	}

	if ( TheSVOLicenseManager().HasMatroxIdentificationLicense() )
	{
		// Add the Matrox OCV Analyzer	
		analyzerClassInfo.m_ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
		analyzerClassInfo.m_ObjectTypeInfo.SubType = SVOCVAnalyzerObjectType;
		analyzerClassInfo.m_ClassId = SVOCVAnalyzerClassGuid;
		analyzerClassInfo.m_ClassName = SvUl::LoadStdString( IDS_CLASSNAME_SVOCVANALYZER );
		m_availableChildren.Add(analyzerClassInfo);
	}
}

void SVWindowToolClass::AddUnaryImageOperatorListAsChild ()
{
	// Build an operator list...
	// ...use Standard image operator list, because we need an output image! RO_20Mar2000
	SVUnaryImageOperatorListClass* pOperatorList = new SVStdImageOperatorListClass;

	// Operator list defaults:
	if( pOperatorList )
	{
		// Requires a SVThresholdClass Object
		pOperatorList->Add( new SVThresholdClass( pOperatorList ) );

		// and Requires a SVUsermaskOperatorClass Object
		pOperatorList->Add( new SVUserMaskOperatorClass( pOperatorList ) );

		// and Requires a SVLUTOperatorClass Object
		pOperatorList->Add( new SVLUTOperatorClass( pOperatorList ) );

		// Add the UnaryImageOperatorList to the Tool's List
		Add( pOperatorList );
	}
	
	ToolSizeAdjustTask::AddToFriendlist(this,true,true,true);
	
}

SVWindowToolClass::~SVWindowToolClass()
{ 
}


bool SVWindowToolClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVToolClass::CreateObject(rCreateStructure);
	
	m_SourceImageNames.setStatic( true );
	m_SourceImageNames.setSaveValueFlag(false);
	m_SourceImageNames.SetObjectAttributesAllowed( SvDef::SV_REMOTELY_SETABLE | SvDef::SV_SETABLE_ONLINE, SvOi::SetAttributeType::RemoveAttribute );
	
	if(bOk)
	{
		bOk = (S_OK == ToolSizeAdjustTask::EnsureInFriendList(this, true, true, true)); 
	}
	
	m_isCreated = bOk;
	return bOk;
}

bool SVWindowToolClass::SetDefaultFormulas(SvStl::MessageContainerVector *pErrorMessages)
{
	bool bRetVal = true;

	// Set Default Formula of LUTEquation, if any...
	SVObjectTypeInfoStruct lutEquationInfo;
	lutEquationInfo.ObjectType	= SVEquationObjectType;
	lutEquationInfo.SubType		= SVLUTEquationObjectType;
	SVLUTEquationClass* pLUTEquation = dynamic_cast<SVLUTEquationClass*>(getFirstObject(lutEquationInfo));
	if( pLUTEquation )
	{
		bRetVal = pLUTEquation->SetDefaultFormula(pErrorMessages) && bRetVal;
	}

	return bRetVal;
}


HRESULT SVWindowToolClass::SetImageExtent(const SVImageExtentClass& rImageExtent)
{
	HRESULT l_hrOk = m_svToolExtent.ValidExtentAgainstParentImage( rImageExtent );

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

	l_hrOk = m_svToolExtent.UpdateExtentToParentExtents( NewExtent );

	if( S_OK == l_hrOk )
	{
		l_hrOk = SVToolClass::SetImageExtent( NewExtent );
	}
	return l_hrOk;
}


HRESULT SVWindowToolClass::SetImageExtentToFit( const SVImageExtentClass& rImageExtent )
{
	HRESULT l_hrOk = S_OK;

	l_hrOk = m_svToolExtent.UpdateExtentAgainstParentImage( rImageExtent );

	return l_hrOk;
}

SVTaskObjectClass *SVWindowToolClass::GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint )
{
	SVImageExtentClass l_svExtents;

	SVTaskObjectClass *l_psvObject = nullptr;

	if( S_OK == m_svToolExtent.GetImageExtent( l_svExtents ) &&
		SVExtentLocationPropertyUnknown  != l_svExtents.GetLocationPropertyAt( p_rsvPoint ) )
	{
		l_psvObject = this;
	}

	return l_psvObject;
}

bool SVWindowToolClass::DoesObjectHaveExtents() const
{
	return true;
}

SVStringValueObjectClass* SVWindowToolClass::GetInputImageNames()
{
	return &m_SourceImageNames;
}

bool SVWindowToolClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = SVToolClass::ResetObject(pErrorMessages);

	UpdateImageWithExtent();

	return Result;
}
