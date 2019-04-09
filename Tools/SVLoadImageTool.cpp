//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolLoadImage
//* .File Name       : $Workfile:   SVToolLoadImage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   07 May 2013 08:29:16  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVLoadImageTool.h"
#include "SVObjectLibrary/SVClsIds.h"
#include "Definitions/SVImageFormatEnum.h"
#include "AnalyzerOperators/SVAnalyzer.h"
#include "SVUtilityLibrary/SVUtilityGlobals.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "Operators/ToolSizeAdjustTask.h"
#pragma endregion Includes

namespace SvTo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( SVLoadImageToolClass, SVLoadImageToolClassGuid );

SVLoadImageToolClass::SVLoadImageToolClass( SVObjectClass* POwner, int StringResourceID )
					 :SVToolClass( POwner, StringResourceID )
{
	init();
}

void SVLoadImageToolClass::init()
{
	m_canResizeToParent = true;
	// Set up your type...
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVToolObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType    = SvPb::SVToolLoadImageObjectType;

	// Identify our input type needs
	
	// Register Embedded Objects
	RegisterEmbeddedObject( &m_fileImage, SVOutputImageObjectGuid, IDS_OBJECTNAME_IMAGE1 );
	RegisterEmbeddedObject( &m_currentPathName, SVPathNameObjectGuid, IDS_OBJECTNAME_PATHNAME, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &m_continuousReload, SVContinuousReloadObjectGuid, IDS_OBJECTNAME_CONTINUOUS_RELOAD, false, SvOi::SVResetItemNone );

	// Set Embedded defaults
	m_currentPathName.SetDefaultValue( _T( "" ), true );
	m_continuousReload.SetDefaultValue( false, true );

	m_fileImage.InitializeImage( SvDef::SVImageTypeEnum::SVImageTypePhysical );

	// Default taskObjectList items:

	// Set Translation
	m_toolExtent.SetTranslation(SvPb::SVExtentTranslationFigureShift);
	SvOp::ToolSizeAdjustTask::AddToFriendlist(this, true, true, false);
	// Set default inputs and outputs
	addDefaultInputObjects();
}

SVLoadImageToolClass::~SVLoadImageToolClass()
{ 
}

bool SVLoadImageToolClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVToolClass::CreateObject(rCreateStructure);

	if( bOk )
	{
		// Create Image...
		SVImageInfoClass ImageInfo = m_fileImage.GetImageInfo();

		// Setup...
		ImageInfo.SetOwner( GetUniqueObjectID() );
		ImageInfo.SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyFormat, SvDef::SVImageFormatMono8 );
		ImageInfo.SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, 1 );
		ImageInfo.SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyBandLink, 0 );
		ImageInfo.SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyPixelDepth,8);

		bOk = ( S_OK == m_fileImage.UpdateImage(GUID_NULL, ImageInfo) );
	}

	// Set / Reset Printable Flags
	m_fileImage.SetObjectAttributesAllowed( SvPb::remotelySetable | SvPb::setableOnline, SvOi::SetAttributeType::AddAttribute );
	m_currentPathName.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );
	m_continuousReload.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );

	if (bOk)
	{
		bOk = (S_OK == SvOp::ToolSizeAdjustTask::EnsureInFriendList(this, true, true, false));
	}

	m_isCreated = bOk;

	return bOk;
}

bool SVLoadImageToolClass::CloseObject()
{
	if (SVToolClass::CloseObject())
	{
		return m_fileImage.CloseObject();
	}
	return false;
}

bool SVLoadImageToolClass::onRun(SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
{
	// All inputs and outputs must be validated first
	if (ValidateLocal(pErrorMessages) && __super::onRun(rRunStatus, pErrorMessages))
	{
		BOOL bReload = false;
		std::string ImagePathName;

		if (S_OK != m_continuousReload.GetValue(bReload) || S_OK != m_currentPathName.GetValue(ImagePathName))
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
				pErrorMessages->push_back(Msg);
			}
			rRunStatus.SetInvalid();
			return false;
		}

		if (bReload || m_ReloadFileImage || nullptr == m_fileImage.getImageReadOnly(rRunStatus.m_triggerRecord.get()))
		{
			if (S_OK != m_fileImage.LoadImage(ImagePathName.c_str(), rRunStatus.m_triggerRecord))
			{
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_FailedToLoadImage, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
					pErrorMessages->push_back(Msg);
				}
				rRunStatus.SetInvalid();
				return false;
			}

			m_ReloadFileImage = false;
		}
		
		if (!SVSVIMStateClass::CheckState(SV_STATE_RUNNING))
		{
			//If not in runMode reload always, because outside of runMode it is not sure if the last image still valid.
			m_ReloadFileImage = true;
		}

		return true;
	}

	rRunStatus.SetInvalid();
	return false;
}

bool SVLoadImageToolClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = SVToolClass::ResetObject(pErrorMessages);
	
	m_ReloadFileImage = true;

	UpdateImageWithExtent();

	return Result && ValidateLocal(pErrorMessages);
}

bool SVLoadImageToolClass::isInputImage(const SVGUID& rImageGuid) const
{
	bool Result(false);

	//! Use static_cast to avoid time penalty in run mode for dynamic_cast
	//! We are sure that when m_pObject is not nullptr then it is a SVImageClass
	SvIe::SVImageClass* pImage = static_cast<SvIe::SVImageClass*> (m_pCurrentToolSet->getCurrentImageInterface());
	if (nullptr != pImage && rImageGuid == pImage->GetUniqueObjectID())
	{
		Result = true;
	}
	else
	{
		Result = SVToolClass::isInputImage(rImageGuid);
	}

	return Result;
}

SVToolClass* SVLoadImageToolClass::GetObjectAtPoint(const SVPoint<double>& rPoint)
{
	if (SvPb::SVExtentLocationPropertyUnknown != GetImageExtent().GetLocationPropertyAt(rPoint))
	{
		return this;
	}

	return nullptr;
}

bool SVLoadImageToolClass::DoesObjectHaveExtents() const
{
	return true;
}

HRESULT SVLoadImageToolClass::SetImageExtent(const SVImageExtentClass& rImageExtent)
{
	HRESULT l_hrOk = S_FALSE;

	const SVExtentFigureStruct& rFigure = rImageExtent.GetFigure();

	if ( (rFigure.m_svTopLeft.m_x >= 0) && (rFigure.m_svTopLeft.m_y >= 0) )
	{
		l_hrOk = SVToolClass::SetImageExtent(rImageExtent);
	}
	else
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

HRESULT SVLoadImageToolClass::SetImageExtentToParent()
{
	HRESULT l_hrOk = S_OK;
	SVImageExtentClass NewExtent;
	std::string ImagePathName;

	if (S_OK == l_hrOk)
	{
		l_hrOk = m_currentPathName.GetValue(ImagePathName);
	}

	if (S_OK == l_hrOk)
	{
		l_hrOk = NewExtent.SetDataFromFile(ImagePathName.c_str());
	}

	if (S_OK == l_hrOk)
	{
		l_hrOk = SVToolClass::SetImageExtent(NewExtent);
	}
	return l_hrOk;
}

HRESULT SVLoadImageToolClass::GetParentExtent( SVImageExtentClass& rParentExtent ) const
{
	HRESULT hr = S_OK;
	std::string ImagePathName;
	if( S_OK == hr )
	{
		hr = m_currentPathName.GetValue( ImagePathName );
	}
	if( S_OK == hr )
	{
		hr = rParentExtent.SetDataFromFile(ImagePathName.c_str());
	}
	return hr;
}

bool SVLoadImageToolClass::ValidateLocal(SvStl::MessageContainerVector *pErrorMessages) const
{
	std::string PathName;
	m_currentPathName.GetValue( PathName );

	if( ::SVFileExists( PathName.c_str() ) )
	{
		return true;
	}

	if (nullptr != pErrorMessages)
	{
		SvStl::MessageContainer message( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_FailedToLoadImage, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
		pErrorMessages->push_back(message);
	}
	return false;
}

} //namespace SvTo
