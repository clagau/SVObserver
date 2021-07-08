//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImage
//* .File Name       : $Workfile:   SVImageClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.8  $
//* .Check In Date   : $Date:   26 Jun 2014 17:41:18  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "Definitions/GlobalConst.h"
#include "ObjectInterfaces/IInspectionProcess.h"
#include "ObjectInterfaces/ITool.h"
#include "SVImageClass.h"
#include "SVImageProcessingClass.h"
#include "SVTaskObject.h"
#include "SVFileSystemLibrary/SVFileNameClass.h"
#include "SVMatroxLibrary/SVMatroxBufferCreateStruct.h"
#include "SVMatroxLibrary/SVMatroxBufferCreateChildStruct.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVMatroxLibrary/SVMatroxImageInterface.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVUtilityLibrary/SVClock.h"
#include "ObjectInterfaces/ITriggerRecordControllerRW.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "ObjectInterfaces/ILinkedObject.h"
#pragma endregion Includes

//#define TRACE_IMAGE true 
namespace SvIe
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

constexpr long cColorBandNumber = 3L;
constexpr long cMonoBandNumber = 1L;
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(SVImageClass, SvPb::ImageClassId);

SVImageClass::SVImageClass(LPCSTR ObjectName)
	: SVObjectAppClass(ObjectName)
{
	init();
}

SVImageClass::SVImageClass(SVObjectClass* POwner, int StringResourceID)
	: SVObjectAppClass(POwner, StringResourceID)
{
	init();
}

bool SVImageClass::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	bool bOk = SVObjectAppClass::CreateObject(rCreateStructure);

	SVObjectManagerClass::Instance().RegisterSubObject(getObjectId());
	if (bOk)
	{
		for (auto* pObject : m_embeddedList)
		{
			if (IsCreated() && nullptr != pObject)
			{
				SVObjectLevelCreateStruct createStruct = rCreateStructure;
				createStruct.OwnerObjectInfo.SetObject(this);
				bOk &= pObject->createAllObjects(createStruct);
				assert(bOk);
			}
		}
	}

	bOk &= (S_OK == UpdateFromToolInformation());

	SetObjectAttributesAllowed(SvPb::publishResultImage | SvPb::dataDefinitionImage, SvOi::SetAttributeType::AddAttribute);	// add this to older configs

	m_isCreated = bOk;

	return bOk;
}

bool SVImageClass::CloseObject()
{
	bool rc = (S_OK == ClearParentConnection());

	SvOi::ITool* pTool = dynamic_cast<SvOi::ITool*> (GetTool());
	if (nullptr != pTool && pTool->GetToolImage() == getObjectId())
	{
		pTool->SetToolImage(SvDef::InvalidObjectId);
	}

	rc = DestroyImage() && rc;

	SVObjectManagerClass::Instance().UnregisterSubObject(getObjectId());
	SVObjectManagerClass::Instance().UnregisterSubObject(m_width.getObjectId());
	SVObjectManagerClass::Instance().UnregisterSubObject(m_height.getObjectId());

	rc = SVObjectAppClass::CloseObject() && rc;

	return rc;
}

/*
This method now verifies the Parent Image pointer attribute.
If the attribute is invalid, the Parent Image pointer and the Parent Index are both cleared.
The Parent Image attribute should not be used unless it is validated first.
*/
SVImageClass *SVImageClass::GetParentImage() const
{
	SVImageClass* l_pParent = nullptr;

	if (SvDef::InvalidObjectId != m_ParentImageInfo.first)
	{
		if (SVObjectManagerClass::Instance().GetState() == SVObjectManagerClass::ReadOnly)
		{
			if (nullptr == m_ParentImageInfo.second)
			{
				SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject(m_ParentImageInfo.first);

				if (nullptr != l_pObject)
				{
					m_ParentImageInfo.second = dynamic_cast<SVImageClass*>(l_pObject);
				}
			}

			if (nullptr != m_ParentImageInfo.second)
			{
				l_pParent = m_ParentImageInfo.second;
			}
		}
		else
		{
			SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject(m_ParentImageInfo.first);

			if (nullptr != l_pObject)
			{
				l_pParent = dynamic_cast<SVImageClass*>(l_pObject);
			}

			m_ParentImageInfo.second = nullptr;
		}
	}

	return l_pParent;
}

void SVImageClass::init()
{
	::InitializeCriticalSection(&m_hCriticalSection);
	m_bCriticalSectionCreated = true;

	m_ImageType = SvPb::SVImageTypeEnum::SVImageTypeUnknown;

	m_ObjectTypeInfo.m_ObjectType = SvPb::SVImageObjectType;
	m_ObjectTypeInfo.m_SubType = SvPb::SVImageMonoType;

	// derived classes that are not result images (i.e. SVMainImageClass)
	// need to remove the PUBLISH attribute.
	SetObjectAttributesAllowed(SvPb::archivableImage | SvPb::publishResultImage | SvPb::dataDefinitionImage, SvOi::SetAttributeType::OverwriteAttribute);

	m_ParentImageInfo.second = nullptr;

	m_ImageInfo.SetOwnerImage(getObjectId());

	m_ImageInfo.SetExtentProperty(SvPb::SVExtentPropertyPositionPointX, SvDef::cDefaultWindowToolLeft);
	m_ImageInfo.SetExtentProperty(SvPb::SVExtentPropertyPositionPointY, SvDef::cDefaultWindowToolTop);
	m_ImageInfo.SetExtentProperty(SvPb::SVExtentPropertyWidth, SvDef::cDefaultWindowToolWidth);
	m_ImageInfo.SetExtentProperty(SvPb::SVExtentPropertyHeight, SvDef::cDefaultWindowToolHeight);
	m_ImageInfo.SetExtentProperty(SvPb::SVExtentPropertyWidthFactorContent, SvDef::cDefaultScaleFactor);
	m_ImageInfo.SetExtentProperty(SvPb::SVExtentPropertyHeightFactorContent, SvDef::cDefaultScaleFactor);
	m_ImageInfo.SetExtentProperty(SvPb::SVExtentPropertyWidthFactorFormat, SvDef::cDefaultScaleFactor);
	m_ImageInfo.SetExtentProperty(SvPb::SVExtentPropertyHeightFactorFormat, SvDef::cDefaultScaleFactor);

	m_width.SetTypeName(_T("Image Width"));
	m_height.SetTypeName(_T("Image Height"));

	m_width.setResetOptions(false, SvOi::SVResetItemTool);
	m_width.SetObjectEmbedded(SvPb::ExtentWidthEId, this, SvUl::LoadStdString(IDS_OBJECTNAME_EXTENT_WIDTH).c_str());
	m_height.setResetOptions(false, SvOi::SVResetItemTool);
	m_height.SetObjectEmbedded(SvPb::ExtentHeightEId, this, SvUl::LoadStdString(IDS_OBJECTNAME_EXTENT_HEIGHT).c_str());

	m_width.SetDefaultValue(100, true);
	m_height.SetDefaultValue(100, true);

	m_embeddedList.push_back(&m_width);
	m_embeddedList.push_back(&m_height);
}

SVImageClass::~SVImageClass()
{
	SVImageClass::CloseObject();

	ClearParentConnection();

	RemoveChildren();

	if (m_bCriticalSectionCreated)
	{
		::DeleteCriticalSection(&m_hCriticalSection);

		m_bCriticalSectionCreated = false;
	}
}

bool SVImageClass::DestroyImage()
{
	bool bOk = true;

	if (m_isCreated)
	{
		bOk = Lock();

		assert(bOk);

		if (bOk)
		{
			bOk = (S_OK == ClearParentConnection());

			assert(bOk);

			bOk = S_OK == RemoveChildren() && bOk;

			assert(bOk);

			m_ImageType = SvPb::SVImageTypeEnum::SVImageTypeUnknown;

			m_isCreated = false;

			bOk = Unlock() && bOk;

			assert(bOk);
		}

		if (!bOk)
		{
			SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ImageClass_DestroyError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10051);
		}
	}

	return bOk;
}

HRESULT SVImageClass::InitializeImage(SvPb::SVImageTypeEnum ImageType)
{
	if (m_ImageType != ImageType)
	{
		m_ImageType = ImageType;

		m_LastUpdate = SvUl::GetTimeStamp();
	}

	return S_OK;
}

HRESULT SVImageClass::InitializeImage(SVImageClass* pParentImage)
{
	HRESULT Result {S_OK};

	if (nullptr != pParentImage)
	{
		uint32_t ImageID = pParentImage->getObjectId();
		if (m_ParentImageInfo.first != ImageID)
		{
			ClearParentConnection();

			m_ParentImageInfo.first = ImageID;
			m_ParentImageInfo.second = nullptr;

			m_LastUpdate = SvUl::GetTimeStamp();

			Result = UpdateFromParentInformation();
		}
	}

	return Result;
}

HRESULT SVImageClass::UpdateImage(const SVImageExtentClass& rExtent, bool doNotRebuildStorage)
{
	HRESULT Result {S_OK};

	if (m_ImageInfo.GetExtents() != rExtent)
	{
		HRESULT Temp = m_ImageInfo.SetExtents(rExtent);

		if (S_OK == Result)
		{
			Result = Temp;
		}

		m_LastUpdate = SvUl::GetTimeStamp();

		Temp = UpdateFromParentInformation();
		if (!doNotRebuildStorage && S_OK == Temp)
		{
			Temp = RebuildStorage();
		}

		if (S_OK == Result)
		{
			Result = Temp;
		}
	}

	return Result;
}

HRESULT SVImageClass::UpdateImage(uint32_t parentID, const SVImageInfoClass& rImageInfo)
{
	HRESULT Result {S_OK};

	if (m_ParentImageInfo.first != parentID || m_ImageInfo != rImageInfo)
	{
		if (m_ParentImageInfo.first != parentID)
		{
			ClearParentConnection();

			m_ParentImageInfo.first = parentID;
			m_ParentImageInfo.second = nullptr;
		}

		if (m_ImageInfo != rImageInfo)
		{
			m_ImageInfo = rImageInfo;
			m_ImageInfo.SetOwnerImage(getObjectId());
			setImageSubType();
		}

		m_LastUpdate = SvUl::GetTimeStamp();

		Result = (ResetObject() ? S_OK : E_FAIL);
	}

	return Result;
}

HRESULT SVImageClass::UpdateImage(SvPb::SVImageTypeEnum ImageType)
{
	HRESULT l_Status = S_OK;

	if (m_ImageType != ImageType)
	{
		m_ImageType = ImageType;

		m_LastUpdate = SvUl::GetTimeStamp();

		l_Status = (ResetObject() ? S_OK : E_FAIL);
	}

	return l_Status;
}

const double& SVImageClass::GetLastResetTimeStamp() const
{
	return m_LastReset;
}

bool SVImageClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	setInspectionPosForTrc();

	bool Result = (S_OK == UpdateFromParentInformation(pErrorMessages)) && (S_OK == RebuildStorage(pErrorMessages)) && __super::ResetObject(pErrorMessages);

	setImageSubType();

	double value = 0.0;
	m_ImageInfo.GetExtentProperty(SvPb::SVExtentPropertyEnum::SVExtentPropertyOutputWidth, value);
	m_width.setValue(value);
	m_ImageInfo.GetExtentProperty(SvPb::SVExtentPropertyEnum::SVExtentPropertyOutputHeight, value);
	m_height.setValue(value);

	return Result;
}


HRESULT SVImageClass::RebuildStorage(SvStl::MessageContainerVector *pErrorMessages)
{
	HRESULT hr = S_OK;

	
	if (m_LastReset <= m_LastUpdate)
	{
		// One of the use cases for RebuildStorage() is, when a Tool is added 
		// (for all embedded images).  If the embedded image is a Logical 
		// Image, then the Parent wont be assigned yet.  The assignment 
		// happens in OnCreate of the Tool.  Therefore the following 
		// UpdatePosition() will return an E_FAIL.
		hr = UpdatePosition();
		

		//Update children but do not check if they cause errors as these are handled somewhere else
		// cppcheck-suppress  unreadVariable symbolName=huc
#pragma warning(suppress : 4189)
		auto huc = UpdateChildren();
#if defined (TRACE_THEM_ALL) || defined (TRACE_IMAGE) 		
		if (huc != S_OK)
		{
			OutputDebugString("Updatechildren returns FAIL\n");
		}
#endif 
	}

	if (S_OK == hr)
	{
		if (!UpdateTRCBuffers(pErrorMessages))
		{
			hr = E_FAIL;
		}
	}

	if (S_OK == hr)
	{
		m_LastReset = SvUl::GetTimeStamp();
	}

	//Update Position returns S_FALSE when ROI for LiniarTool without rotation is moved 
	// outside the parentwindow. When moved with the mouse this is corrected 
	//later should be ignored here S_OK != S_FALSE  &&
	if ( S_OK != hr  && S_NoParent != hr && nullptr != pErrorMessages && pErrorMessages->empty())
	{
		SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RebuildFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
		pErrorMessages->push_back(Msg);
	}
	
	return hr==S_OK? S_OK : E_FAIL;
}

const SVImageExtentClass& SVImageClass::GetImageExtents() const
{
	return m_ImageInfo.GetExtents();
}

HRESULT SVImageClass::GetImageExtentsToFit(SVImageExtentClass inExtent, SVImageExtentClass &rOutExtent)
{
	HRESULT l_hrOk = m_ImageInfo.GetImageExtentsToFit(inExtent, rOutExtent);

	return l_hrOk;
}

HRESULT SVImageClass::UpdateFromParentInformation(SvStl::MessageContainerVector *pErrorMessages)
{
	HRESULT Result {S_OK};

	if (m_ImageType != SvPb::SVImageTypeEnum::SVImageTypeMain)
	{
		SVImageClass* l_pParentImage = GetParentImage();

		if (nullptr != l_pParentImage && (m_LastReset < l_pParentImage->GetLastResetTimeStamp() || m_LastReset < m_LastUpdate))
		{
			SVImageExtentClass imageExtent = GetImageExtents();

			SVImagePropertiesClass l_ImageProperties = m_ImageInfo.GetImageProperties();
			SVImageInfoClass l_ImageInfo = l_pParentImage->GetImageInfo();

			l_ImageInfo.SetOwnerImage(getObjectId());

			l_ImageInfo.SetImageProperties(l_ImageProperties);

			if (m_ImageType == SvPb::SVImageTypeEnum::SVImageTypeDependent)
			{
				imageExtent = l_ImageInfo.GetExtents();

				Result = imageExtent.SetExtentProperty(SvPb::SVExtentPropertyPositionPoint, SVPoint<double>(0.0, 0.0));
			}

			if (S_OK == Result)
			{
				Result = l_ImageInfo.SetExtents(imageExtent);
			}

			if (S_OK == Result)
			{
				m_ImageInfo = l_ImageInfo;

				m_LastUpdate = SvUl::GetTimeStamp();

				Result = UpdateFromToolInformation();
			}

		}
		else
		{
			Result = UpdateFromToolInformation();
		}
	}

	if (S_FALSE == Result)
	{
		Result = E_FAIL;
	}

	if (S_OK != Result && nullptr != pErrorMessages)
	{
		SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UpdateFromParentInformationFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
		pErrorMessages->push_back(Msg);
	}

	return Result;
}

HRESULT SVImageClass::UpdateFromToolInformation()
{
	HRESULT l_Status = S_OK;

	uint32_t ToolID = SvDef::InvalidObjectId;
	SVImageExtentClass toolExtent = m_ImageInfo.GetExtents();

	// When initialized from CreateObject(), tool is nullptr.
	SVTaskObjectClass*	pParentTask = dynamic_cast <SVTaskObjectClass*> (GetTool());
	if (nullptr != pParentTask)
	{
		if ((SvPb::SVImageTypeEnum::SVImageTypeMain != m_ImageType) &&
			(SvPb::SVImageTypeEnum::SVImageTypeIndependent != m_ImageType) &&
			(SvPb::SVImageTypeEnum::SVImageTypeDependent != m_ImageType) &&
			pParentTask->DoesObjectHaveExtents())
		{
			RECT l_Rect;
			SVImageExtentClass tempExtent = pParentTask->GetImageExtent();

			if (SvPb::SVImageTypeEnum::SVImageTypeLogical == m_ImageType)
			{
				// @Hack
				// It does not make sense that a logical buffer is not a 1:1 
				// pixel correlation to its parent physical buffer.  For this 
				// reason the translation type will be ignored when retrieving
				// the logical rectangle.  
				// The usage that this is specifically excluded for is for 
				// creating a logical ROI buffer, which should not reflect the 
				// output buffer translation.
				tempExtent.SetTranslation(SvPb::SVExtentTranslationShift);
				l_Status = tempExtent.GetLogicalRectangle(l_Rect);
			}
			else
			{
				l_Status = tempExtent.GetOutputRectangle(l_Rect);
			}

			if (S_OK == l_Status)
			{
				if (0 < (l_Rect.bottom - l_Rect.top + 1) && 0 < (l_Rect.right - l_Rect.left + 1))
				{
					toolExtent = tempExtent;
				}
			}
		}

		
		if (SvPb::SVImageTypeEnum::SVImageTypePhysical == m_ImageType)
		{
			SvOi::ITool* pTool = GetToolInterface();
			if (nullptr != pTool)
			{
				pTool->SetToolImage(getObjectId());

#if defined (TRACE_THEM_ALL) || defined (TRACE_IMAGE) 		
				std::string toolname;
				if (GetTool())
				{
					toolname = GetTool()->GetCompleteName();
				}
				auto msg = std::format("Image was set:tool:{}, type:{}, name:{}\n ",
					toolname, int(GetImageType()), GetCompleteName());
				OutputDebugString(msg.c_str());
#endif 
			}
		}
#if defined (TRACE_THEM_ALL) || defined (TRACE_IMAGE) 			
		else
		{
			SvOi::ITool* pTool = GetToolInterface();
			if (nullptr != pTool)
			{

				std::string toolname;
				if (GetTool())
				{
					toolname = GetTool()->GetCompleteName();
				}
				auto msg = std::format("Image was set:tool:{}, type:{}, name:{}\n ",
					toolname, int(GetImageType()), GetCompleteName());
				OutputDebugString(msg.c_str());
			}

		}
#endif 

		ToolID = nullptr != GetTool() ? GetTool()->getObjectId() : SvDef::InvalidObjectId;
	}

	if (m_ImageInfo.GetOwnerID() != ToolID)
	{
		// Set the owning Tool before setting the extents
		m_ImageInfo.SetOwner(ToolID);

		m_LastUpdate = SvUl::GetTimeStamp();
	}

	if ((SvPb::SVImageTypeEnum::SVImageTypeMain != m_ImageType) && (SvPb::SVImageTypeEnum::SVImageTypeIndependent != m_ImageType))
	{
		l_Status = m_ImageInfo.SetExtents(toolExtent);

		if (S_OK == l_Status)
		{
			m_LastUpdate = SvUl::GetTimeStamp();
		}
	}
	return l_Status;
}

HRESULT SVImageClass::ClearParentConnection()
{
	HRESULT l_hrOk = S_OK;

	if (SvDef::InvalidObjectId != m_ParentImageInfo.first)
	{
		l_hrOk = SVObjectManagerClass::Instance().DisconnectObjects(m_ParentImageInfo.first, getObjectId());
	}

	m_LastUpdate = SvUl::GetTimeStamp();

	return l_hrOk;
}

/*
Updated method to use GetParentImage() method which validates the Parent Image pointer attribute.
The Parent Image attribute should not be used unless it is validated first.
*/
HRESULT SVImageClass::UpdateChild(uint32_t childID, const SVImageInfoClass& rImageInfo)
{
	HRESULT l_hrOk = S_OK;

	if (Lock())
	{
		if (SvPb::SVImageTypeEnum::SVImageTypeDependent == m_ImageType ||
			SvPb::SVImageTypeEnum::SVImageTypeLogical == m_ImageType)
		{
			SVImageClass* l_pParentImage = GetParentImage();

			assert(nullptr != l_pParentImage);

			if (nullptr != l_pParentImage && l_pParentImage != this)
			{
				SVImageInfoClass l_svImageInfo = rImageInfo;
				SVImagePropertiesClass l_svImageProperties = m_ImageInfo.GetImageProperties();

				l_svImageInfo.SetImageProperties(l_svImageProperties);

				l_hrOk = l_pParentImage->UpdateChild(childID, l_svImageInfo);
			}
			else
			{
				l_hrOk = E_FAIL;
			}
		}
		else
		{
			//add entry to childMap if not exist.
			SVImageInfoClass& rChildInfo = m_ChildArrays[childID];
			rChildInfo = rImageInfo;

			auto* pChildObject = SvOi::getObject(childID);
			auto isROI{ false };
			if (pChildObject)
			{
				isROI = (pChildObject->GetEmbeddedID() == SvPb::LogicalROIImageEId);
			}


			if (isROI)
			{
				l_hrOk = m_ImageInfo.ValidateAgainstOutputSpace(rChildInfo.GetExtents());
			}

			auto* pChildObjectImage = dynamic_cast<SVImageClass*>(pChildObject);

			if (nullptr != pChildObjectImage)
			{
				auto pTool = dynamic_cast<SVTaskObjectClass*>(pChildObjectImage->GetTool());

				if (nullptr != pTool && isROI)
				{
					SvStl::MessageManager e(SvStl::MsgType::Data);
					e.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SizeOfChildROIInvalid, SvStl::SourceFileParams(StdMessageParams), 0, childID);

					if (S_OK != l_hrOk)
					{
						pTool->addOrRemoveResetErrorMessage(e.getMessageContainer(), true);
					}
					else
					{
						pTool->addOrRemoveResetErrorMessage(e.getMessageContainer(), false);
					}
				}
			}
#if defined (TRACE_THEM_ALL) || defined (TRACE_IMAGE) 		
			std::string msg = { "ValidateAgainstOuput: " };
			msg += GetCompleteName();

			if (l_hrOk == S_OK)
				msg += ": OK ";
			else
				msg += ": NOT OK ";

			if (GetTool())
			{
				GetTool()->GetName();
				GetTool()->GetObjectName();
				GetTool()->GetCompleteName();
				msg += std::format("(tool:{}) ", GetTool()->GetCompleteName());
			}
			else
				msg += "(no tool )";

			if (pChildObject)
			{

				auto name = pChildObject->GetCompleteName();
				msg += std::format("  CHILDOBJECT(ROI{}): {} ", isROI, name);
				auto* pChildImage = dynamic_cast<SVImageClass*>(pChildObject);
				if (pChildImage && pChildImage->GetTool())
				{
					msg += std::format("(Tool: {} )", pChildImage->GetTool()->GetCompleteName());
					pChildImage->GetTool()->GetName();
					pChildImage->GetTool()->GetObjectName();
					pChildImage->GetTool()->GetCompleteName();
					pChildImage->GetImageType();
				}
				else
					msg += "(no tool )";

			}
			//
			msg += "\n";
			::OutputDebugString(msg.c_str());
#endif
		}

		if (!Unlock())
		{
			l_hrOk = E_FAIL;
		}
	}
	else
	{
		l_hrOk = E_FAIL;
	}

	return l_hrOk;
}

/*
Updated method to use GetParentImage() method which validates the Parent Image pointer attribute.
The Parent Image attribute should not be used unless it is validated first.
*/
HRESULT SVImageClass::RemoveChild(uint32_t childID)
{
	HRESULT l_hrOk = S_OK;
	if (m_removeChildCircularReference)
	{
		return l_hrOk;
	}
	m_removeChildCircularReference = true;

	if (Lock())
	{
		auto l_Iter = m_ChildArrays.find(childID);

		if (l_Iter != m_ChildArrays.end())
		{
			SVImageClass* l_pImage = nullptr;

			l_Iter->second.GetOwnerImage(l_pImage);

			if (nullptr != l_pImage)
			{
				l_hrOk = l_pImage->RemoveObjectConnection(getObjectId());
			}

			l_Iter->second.Initialize();

			m_ChildArrays.erase(l_Iter);
		}

		m_ParentImageInfo.second = nullptr;

		SVImageClass* l_pParentImage = GetParentImage();

		if (nullptr != l_pParentImage && l_pParentImage != this)
		{
			l_hrOk = l_pParentImage->RemoveChild(childID);
		}

		if (!Unlock())
		{
			l_hrOk = E_FAIL;
		}
	}
	else
	{
		l_hrOk = E_FAIL;
	}

	m_removeChildCircularReference = false;
	return l_hrOk;
}

const SVImageInfoClass& SVImageClass::GetImageInfo() const
{
	return m_ImageInfo;
}

bool SVImageClass::SafeImageCopyToHandle(SvOi::SVImageBufferHandlePtr& p_rHandle, const SvOi::ITriggerRecordRPtr pTriggerRecord)
{
	bool l_bOk = (nullptr != p_rHandle);

	if (l_bOk)
	{
		if (Lock())
		{
			HRESULT l_Code;

			SvOi::ITRCImagePtr pImageBuffer = getImageReadOnly(pTriggerRecord.get());
			l_bOk = nullptr != pImageBuffer && !pImageBuffer->isEmpty();

			if (l_bOk)
			{
				l_Code = SVMatroxBufferInterface::CopyBuffer(p_rHandle->GetBuffer(), pImageBuffer->getHandle()->GetBuffer());
			}
			else
			{
				l_Code = SVMatroxBufferInterface::ClearBuffer(p_rHandle->GetBuffer(), 0.0);
			}

			l_bOk = Unlock() && l_bOk && S_OK == l_Code;
		}
	}
	return l_bOk;
}

HRESULT SVImageClass::LoadImage(LPCTSTR fileName, const SvOi::ITriggerRecordRWPtr& pTriggerRecord)
{
	HRESULT l_hrOk = E_FAIL;

	SvOi::ITRCImagePtr pBuffer = getImageToWrite(pTriggerRecord);
	if (nullptr != pBuffer && !pBuffer->isEmpty())
	{
		HRESULT l_Code = SVMatroxBufferInterface::ClearBuffer(pBuffer->getHandle()->GetBuffer(), 0);

		if (l_Code == S_OK)
		{
			if (0 < strlen(fileName))
			{
				SVFileNameClass	svfncImageFile(fileName);
				SVMatroxFileTypeEnum fileformatID = SVMatroxImageInterface::getFileType(svfncImageFile.GetExtension().c_str());

				if (fileformatID != -1 && 0 == _access(fileName, 0))
				{
					std::string l_strFile(fileName);

					l_Code = SVMatroxBufferInterface::Import(pBuffer->getHandle()->GetBuffer(), l_strFile, fileformatID, false);

					if (l_Code == S_OK)
					{
						l_hrOk = S_OK;
					}
				}
			}
			else
			{
				l_hrOk = S_OK;
			}
		}
	}// end if (nullptr != pBuffer && !pBuffer->isEmpty())

	return l_hrOk;
}

/*
Updated method to use GetParentImage() method which validates the Parent Image pointer attribute.
The Parent Image attribute should not be used unless it is validated first.
*/
const SVImageClass*const SVImageClass::GetRootImage() const
{
	SVImageClass* pParentImage = GetParentImage();

	if (nullptr != pParentImage && pParentImage != this)
	{
		return pParentImage->GetRootImage();
	}

	return this;
}

bool SVImageClass::Lock() const
{
	::EnterCriticalSection(&m_hCriticalSection);

	return true;
}

bool SVImageClass::Unlock() const
{
	::LeaveCriticalSection(&m_hCriticalSection);

	return true;
}

HRESULT SVImageClass::RemoveObjectConnection(uint32_t objectID)
{
	HRESULT l_Status = SVObjectAppClass::RemoveObjectConnection(objectID);

	if (m_ParentImageInfo.first == objectID)
	{
		m_ParentImageInfo.first = SvDef::InvalidObjectId;
		m_ParentImageInfo.second = nullptr;

		m_LastUpdate = SvUl::GetTimeStamp();
	}
	else
	{
		l_Status = RemoveChild(objectID);
	}
	return l_Status;
}

void SVImageClass::Persist(SvOi::IObjectWriter& rWriter) const
{
	rWriter.StartElement(GetObjectName()); // use internal name for node name

	SVObjectAppClass::Persist(rWriter);

	PersistImageAttributes(rWriter);

	rWriter.EndElement();
}

void SVImageClass::PersistImageAttributes(SvOi::IObjectWriter& rWriter) const
{
	long TempValue = 0;

	// Add image pixel depth as trivial members
	m_ImageInfo.GetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyPixelDepth, TempValue);
	_variant_t Value(TempValue);
	rWriter.WriteAttribute(scPixelDepthTag, Value);

	// Add Band Number as trivial members
	m_ImageInfo.GetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, TempValue);
	Value = TempValue;
	rWriter.WriteAttribute(scBandNumberTag, Value);

	// Add Band Link as trivial members
	m_ImageInfo.GetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyBandLink, TempValue);
	Value = TempValue;
	rWriter.WriteAttribute(scBandLinkTag, Value);

	// Set up embedded object definitions...
	if (0 < m_embeddedList.size())
	{
		rWriter.StartElement(scEmbeddedsTag);
		// Get embedded object script...
		for (const auto* pObject : m_embeddedList)
		{
			if (nullptr != pObject)
			{
				pObject->Persist(rWriter);
			}
		}
		rWriter.EndElement();
	}
}

HRESULT SVImageClass::SetObjectValue(SVObjectAttributeClass* pDataObject)
{
	HRESULT hr = E_FAIL;

	SvCl::SVObjectLongArrayClass svLongArray;
	bool bOk{pDataObject->GetAttributeData(_T("PixelDepth"), svLongArray)};
	if(bOk)
	{
		for (int i = 0; i < static_cast<int> (svLongArray.size()); i++)
		{
			m_ImageInfo.SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyPixelDepth, svLongArray[i]);
		}
	}
	else if(true == (bOk = pDataObject->GetAttributeData(_T("BandNumber"), svLongArray)))
	{
		long BandNumber{0L};
		for (int i = 0; i < static_cast<int> (svLongArray.size()); i++)
		{
			BandNumber = svLongArray[i];
			m_ImageInfo.SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, BandNumber);
		}
		//In previous versions the Band Number was saved incorrectly this is a check to see if it fits to the format 
		long formatValue {0L};
		m_ImageInfo.GetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyFormat, formatValue);
		SvDef::SVImageFormatEnum imageFormat{static_cast<SvDef::SVImageFormatEnum> (formatValue)};
		if(SvDef::SVImageFormatEnum::SVImageFormatBGR888 ==  imageFormat || SvDef::SVImageFormatEnum::SVImageFormatBGR888X == imageFormat)
		{
			if(cMonoBandNumber == BandNumber)
			{
				m_ImageInfo.SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, cColorBandNumber);
			}
		}
		else
		{
			if (cColorBandNumber == BandNumber)
			{
				m_ImageInfo.SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, cMonoBandNumber);
			}
		}
	}
	else if (true == (bOk = pDataObject->GetAttributeData(_T("BandLink"), svLongArray)))
	{
		for (int i = 0; i < static_cast<int> (svLongArray.size()); i++)
		{
			m_ImageInfo.SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyBandLink, svLongArray[i]);
		}
		setImageSubType();
	}
	else
	{
		hr = SVObjectAppClass::SetObjectValue(pDataObject);

		m_LastUpdate = SvUl::GetTimeStamp();

		return hr;
	}

	m_LastUpdate = SvUl::GetTimeStamp();

	// At some point this can go away,because it's not longer being scripted
	hr = bOk ? S_OK : E_FAIL;
	return hr;
}

//* JMS - New Image Object methods

HRESULT SVImageClass::UpdatePosition()
{
	HRESULT l_Status = S_OK;

	m_ParentImageInfo.second = dynamic_cast<SVImageClass*>(SVObjectManagerClass::Instance().GetObject(m_ParentImageInfo.first));

	if (SvPb::SVImageTypeEnum::SVImageTypeDependent == m_ImageType ||
		SvPb::SVImageTypeEnum::SVImageTypeLogical == m_ImageType)
	{
		if (nullptr != m_ParentImageInfo.second)
		{
			l_Status = m_ParentImageInfo.second->UpdateChild(getObjectId(), m_ImageInfo);
		}
		else
		{
			
			l_Status = S_NoParent;
		}
	}

	return l_Status;
}

HRESULT SVImageClass::UpdateChildren()
{
	HRESULT l_hrOk = S_OK;

	if (Lock())
	{
		auto l_Iter = m_ChildArrays.begin();

		while (l_Iter != m_ChildArrays.end())
		{
			if (S_OK != UpdateChild(l_Iter->first, l_Iter->second))
			{
				l_hrOk = E_FAIL;
			}

			++l_Iter;
		}

		if (!Unlock())
		{
			l_hrOk = E_FAIL;
		}
	}
	else
	{
		l_hrOk = E_FAIL;
	}
	return l_hrOk;
}

HRESULT SVImageClass::RemoveChildren()
{
	HRESULT l_hrOk = S_OK;

	if (Lock())
	{
		auto l_Iter = m_ChildArrays.begin();

		while (l_Iter != m_ChildArrays.end())
		{
			if (S_OK != RemoveChild(l_Iter->first))
			{
				l_hrOk = E_FAIL;
			}

			l_Iter = m_ChildArrays.begin();
		}

		if (!Unlock())
		{
			l_hrOk = E_FAIL;
		}
	}
	else
	{
		l_hrOk = E_FAIL;
	}
	return l_hrOk;
}

/*
Updated method to use GetParentImage() method which validates the Parent Image pointer attribute.
The Parent Image attribute should not be used unless it is validated first.
*/
HRESULT SVImageClass::ValidateAgainstParentExtents(const SVImageExtentClass& p_rsvExtent)
{
	HRESULT l_hrOk = S_OK;

	SVImageClass* l_pParentImage = GetParentImage();

	if (nullptr != l_pParentImage && l_pParentImage != this)
	{
		l_hrOk = l_pParentImage->ValidateAgainstOutputExtents(p_rsvExtent);
	}
	return l_hrOk;
}

HRESULT SVImageClass::ValidateAgainstOutputExtents(const SVImageExtentClass& p_rsvExtent)
{
	return m_ImageInfo.ValidateAgainstOutputSpace(p_rsvExtent);
}

// Method to keep the Translation Offset in sync between the ToolExtents and ImageExtents
// Called by SVShitftTool::oRun
void SVImageClass::SetTranslationOffset(double offsetX, double offsetY)
{
	m_ImageInfo.SetExtentProperty(SvPb::SVExtentPropertyTranslationOffsetX, offsetX);
	m_ImageInfo.SetExtentProperty(SvPb::SVExtentPropertyTranslationOffsetY, offsetY);
}

void SVImageClass::setImage(SvOi::ITRCImagePtr pImage, const SvOi::ITriggerRecordRWPtr& pTriggerRecord)
{
	assert(nullptr != pTriggerRecord);
	if (nullptr != pTriggerRecord)
	{
		assert(0 <= m_imagePosInTRC && !m_isChildImageInTRC);
		pTriggerRecord->setImage(m_imagePosInTRC, pImage);
	}
}

void SVImageClass::resetImage(const SvOi::ITriggerRecordRWPtr& pTriggerRecord)
{
	assert(nullptr != pTriggerRecord);
	if (nullptr != pTriggerRecord)
	{
		assert(0 <= m_imagePosInTRC && !m_isChildImageInTRC);
		pTriggerRecord->setImage(m_imagePosInTRC, -1);
	}
}

SvOi::SVImageBufferHandlePtr SVImageClass::getLastImage(bool newIfNotAvailable) const
{
	SvOi::ITriggerRecordRPtr lastTriggerRecord = nullptr;
	auto* pTRC = SvOi::getTriggerRecordControllerRWInstance();
	if (nullptr != pTRC && 0 <= m_inspectionPosInTRC && false == pTRC->isResetStarted())
	{
		int lastTRid = pTRC->getLastTrId(m_inspectionPosInTRC);
		lastTriggerRecord = pTRC->createTriggerRecordObject(m_inspectionPosInTRC, lastTRid);
	}
	SvOi::ITRCImagePtr pImage;
	if (nullptr == lastTriggerRecord && newIfNotAvailable)
	{
		pImage = getTempImageBuffer();
	}

	if (nullptr != lastTriggerRecord)
	{
		pImage = getImageReadOnly(lastTriggerRecord.get());
	}
	return nullptr != pImage ? pImage->getHandle() : nullptr;
}

SvOi::ITRCImagePtr SVImageClass::getTempImageBuffer(bool createBufferExternIfNecessary) const
{
	SvOi::ITRCImagePtr pImage = nullptr;
	SVMatroxBufferCreateStruct bufferStruct;
	HRESULT Result = SVImageProcessingClass::FillBufferStructFromInfo(m_ImageInfo, bufferStruct);
	auto* pTRC = SvOi::getTriggerRecordControllerRWInstance();
	if (nullptr != pTRC && S_OK == Result)
	{
		pImage = pTRC->getImageBuffer(bufferStruct, createBufferExternIfNecessary);
	}
	return pImage;
}

SvOi::ITRCImagePtr SVImageClass::getImageReadOnly(const SvOi::ITriggerRecordR* pTriggerRecord, bool lockImage) const
{
	SvOi::ITRCImagePtr pImage = nullptr;
	assert(nullptr != pTriggerRecord);
	if (nullptr != pTriggerRecord)
	{
		assert(0 <= m_imagePosInTRC);
		if (!m_isChildImageInTRC)
		{
			pImage = pTriggerRecord->getImage(m_imagePosInTRC, lockImage);
		}
		else
		{
			pImage = pTriggerRecord->getChildImage(m_imagePosInTRC, lockImage);
		}
	}
	else
	{
		SvStl::MessageManager e(SvStl::MsgType::Log);
		e.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_GetImageFailed_NoTR, SvStl::SourceFileParams(StdMessageParams));
	}
	return pImage;
}

SvOi::ITRCImagePtr SVImageClass::getImageToWrite(const SvOi::ITriggerRecordRWPtr& pTriggerRecord)
{
	SvOi::ITRCImagePtr pImage = nullptr;
	assert(nullptr != pTriggerRecord);
	if (nullptr != pTriggerRecord)
	{
		assert(0 <= m_imagePosInTRC && !m_isChildImageInTRC);
		pImage = pTriggerRecord->createNewImageHandle(m_imagePosInTRC);
	}
	else
	{
		SvStl::MessageManager e(SvStl::MsgType::Log);
		e.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_GetImageFailed_NoTR, SvStl::SourceFileParams(StdMessageParams));
	}
	return pImage;
}

void SVImageClass::getOutputList(std::back_insert_iterator<std::vector<SvOi::IObjectClass*>> inserter) const
{
	if (0 != ObjectAttributesAllowed())
	{
		for (auto* pObject : m_embeddedList)
		{
			if (nullptr != pObject)
			{
				// cppcheck-suppress unreadVariable symbolName=inserter ; cppCheck doesn't know back_insert_iterator
				// cppcheck-suppress useStlAlgorithm ; std::accumulate will not used because in the current way it is more clear
				inserter = pObject;
			}
		}
	}
}

HRESULT SVImageClass::GetChildObject(SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index) const
{
	HRESULT l_Status = SVObjectAppClass::GetChildObject(rpObject, rNameInfo, Index);

	if (S_OK != l_Status && 0 != ObjectAttributesAllowed())
	{
		if (static_cast<const size_t> (Index) < rNameInfo.m_NameArray.size() && rNameInfo.m_NameArray[Index] == GetName() && Index + 1 == rNameInfo.m_NameArray.size() - 1)
		{
			for (const auto* pObject : m_embeddedList)
			{
				if (nullptr != pObject)
				{
					l_Status = pObject->GetChildObject(rpObject, rNameInfo, Index + 1);
					if (S_OK == l_Status && nullptr != rpObject)
					{
						return S_OK;
					}
				}
			}
		}
		else
		{
			l_Status = S_FALSE;
		}
	}

	return l_Status;
}
void SVImageClass::setEditModeFreezeFlag(bool flag)
{
	__super::setEditModeFreezeFlag(flag);
	if (m_editModeFreezeFlag && !m_isChildImageInTRC)
	{
		if (nullptr == m_savedBuffer)
		{
			copyCurrent2SaveImage();
		}
	}
	else
	{
		m_savedBuffer = nullptr;
		m_ImageInfoOfSavedBuffer = SVImageInfoClass();
	}
}

void SVImageClass::goingOffline()
{
	if (m_editModeFreezeFlag && !m_isChildImageInTRC)
	{
		copyCurrent2SaveImage();
	}
}

void SVImageClass::copiedSavedImage(SvOi::ITriggerRecordRWPtr pTr)
{
	if (m_editModeFreezeFlag && !m_isChildImageInTRC)
	{
		auto image = getImageToWrite(pTr);
		assert(nullptr != m_savedBuffer && nullptr != image && !image->isEmpty());
		if (nullptr != m_savedBuffer && nullptr != image && !image->isEmpty())
		{
			HRESULT result = SVMatroxBufferInterface::CopyBuffer(image->getHandle()->GetBuffer(), m_savedBuffer->GetBuffer());
			assert(S_OK == result); UNREFERENCED_PARAMETER(result);
		}
	}
}

void SVImageClass::fillSelectorList(std::back_insert_iterator<std::vector<SvPb::TreeItem>> treeInserter, SvOi::IsObjectAllowedFunc pFunctor, UINT attribute, bool wholeArray, SvPb::SVObjectTypeEnum nameToType, SvPb::ObjectSelectorType requiredType, bool stopIfClosed /*= false*/, bool /*firstObject = false*/) const
{
	if (0 != ObjectAttributesAllowed())
	{
		nameToType = (SvPb::SVNotSetObjectType == nameToType) ? GetObjectType() : nameToType;
		__super::fillSelectorList(treeInserter, pFunctor, attribute, wholeArray, nameToType, requiredType, stopIfClosed);

		for (auto* pObject : m_embeddedList)
		{
			if (nullptr != pObject)
			{
				pObject->fillSelectorList(treeInserter, pFunctor, attribute, wholeArray, nameToType, requiredType, stopIfClosed);
			}
		}
	}
}

void SVImageClass::fillObjectList(std::back_insert_iterator<std::vector<SvOi::IObjectClass*>> inserter, const SvDef::SVObjectTypeInfoStruct& rObjectInfo, bool addHidden /*= false*/, bool stopIfClosed /*= false*/, bool /*firstObject = false*/)
{
	if (0 != ObjectAttributesAllowed() || addHidden)
	{
		__super::fillObjectList(inserter, rObjectInfo, addHidden, stopIfClosed);

		for (auto* pObject : m_embeddedList)
		{
			if (nullptr != pObject)
			{
				pObject->fillObjectList(inserter, rObjectInfo, addHidden, stopIfClosed);
			}
		}
	}
}

SVObjectClass* SVImageClass::OverwriteEmbeddedObject(uint32_t uniqueID, SvPb::EmbeddedIdEnum embeddedID)
{
	// Check here all embedded members ( embedded objects could be only identified by embeddedID!!!! )... 
	for (SVObjectPtrVector::iterator Iter = m_embeddedList.begin(); m_embeddedList.end() != Iter; ++Iter)
	{
		SVObjectClass* pObject = *Iter;
		if (nullptr != pObject)
		{
			if (pObject->GetEmbeddedID() == embeddedID)
			{
				return pObject->OverwriteEmbeddedObject(uniqueID, embeddedID);
			}
		}
	}
	return __super::OverwriteEmbeddedObject(uniqueID, embeddedID);
}

HRESULT SVImageClass::SetValuesForAnObject(uint32_t aimObjectID, SVObjectAttributeClass* pDataObject)
{
	for (SVObjectPtrVector::iterator Iter = m_embeddedList.begin(); m_embeddedList.end() != Iter; ++Iter)
	{
		SVObjectClass* pObject = *Iter;
		if (nullptr != pObject)
		{
			// check if it's this object
			if (aimObjectID == pObject->getObjectId())
			{
				// Set the Object's Data Member Value
				if (S_OK == pObject->SetObjectValue(pDataObject))
				{
					return S_OK;
				}
			}
		}
	}

	return __super::SetValuesForAnObject(aimObjectID, pDataObject);
}

HRESULT SVImageClass::TranslateFromOutputSpaceToImage(SVImageClass* pImage, SVPoint<double> inPoint, SVPoint<double>& rOutPoint) const
{
	HRESULT l_hr = E_FAIL;

	rOutPoint.clear();

	if (nullptr != pImage)
	{
		const SVImageClass* pCurrentImage {this};
		do
		{
			//This translation needs to be done for each image until the required parent image is reached
			const SVImageExtentClass rExtents = pCurrentImage->GetImageExtents();
			rExtents.TranslateFromOutputSpace(inPoint, inPoint);
			pCurrentImage = pCurrentImage->GetParentImage();
		} while (pImage != pCurrentImage && nullptr != pCurrentImage);

		if (pImage == pCurrentImage)
		{
			rOutPoint = inPoint;
			l_hr = S_OK;
		}
	}
	return l_hr;
}

#pragma region virtual method (ISVImage)
SvPb::SVImageTypeEnum SVImageClass::GetImageType() const
{
	return m_ImageType;
}

SvOi::ISVImage* SVImageClass::GetParentImageInterface() const
{
	return GetParentImage();
}

std::string SVImageClass::getDisplayedName() const
{
	std::string strName;
	if (SvPb::SVImageTypeEnum::SVImageTypeMain == m_ImageType)
	{
		auto pParent = GetParent();
		if (nullptr != pParent)
		{
			if (pParent->GetObjectType() == SvPb::SVToolObjectType)
			{
				strName = pParent->GetObjectNameBeforeObjectType(SvPb::SVToolSetObjectType);
				strName += _T(".Image1");
			}
			else
			{
				strName = pParent->GetName();
				strName += _T(".Image1");
			}
		}
	}
	else
	{
		SVImageInfoClass imageInfo = GetImageInfo();
		if (imageInfo.GetOwner())
		{
			strName = GetObjectNameBeforeObjectType(SvPb::SVToolSetObjectType);
		}
	}
	return strName;
}

long SVImageClass::getPixelDepth() const
{
	long pixelDepth = 0;
	m_ImageInfo.GetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyPixelDepth, pixelDepth);
	return pixelDepth;
}

HRESULT SVImageClass::Save(const std::string& rFilename)
{
	HRESULT hr = S_OK;

	// Get output buffer handle...
	SvOi::SVImageBufferHandlePtr pImage = getLastImage();
	if (nullptr != pImage && !pImage->empty())
	{
		std::string ext;
		size_t pos = rFilename.find_last_of(".");
		if (std::string::npos != pos)
		{
			ext = rFilename.substr(pos, rFilename.size() - pos);
		}
		SVMatroxFileTypeEnum efileformat(SVMatroxImageInterface::getFileType(ext.c_str()));

		if (efileformat != SVFileUnknown)
		{
			std::string strPath = rFilename.c_str();
			hr = SVMatroxBufferInterface::Export(pImage->GetBuffer(), strPath, efileformat);
		}
		else
		{
			hr = E_INVALIDARG;
		}
	}
	else
	{
		hr = E_HANDLE;
	}
	return hr;
}

RECT SVImageClass::GetOutputRectangle() const
{
	RECT rectTemp;
	m_ImageInfo.GetOutputRectangle(rectTemp);
	return rectTemp;
}

SvPb::OverlayDesc SVImageClass::getOverlayStruct() const
{
	auto* pInsp = dynamic_cast<SvOi::IInspectionProcess*>(GetInspection());
	assert(nullptr != pInsp);
	if (nullptr != pInsp)
	{
		return pInsp->getOverlayStruct(*this);
	}
	SvStl::MessageContainer msg(SVMSG_SVO_NULL_POINTER, SvStl::Tid_Default, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
	throw msg;
}
#pragma endregion virtual method (ISVImage)

void SVImageClass::setImageSubType()
{
	long bandNumber = 0;
	m_ImageInfo.GetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, bandNumber);
	if (1 == bandNumber)
	{
		m_ObjectTypeInfo.m_SubType = SvPb::SVImageMonoType;
	}
	else if (3 == bandNumber)
	{
		m_ObjectTypeInfo.m_SubType = SvPb::SVImageColorType;
	}
}

void SVImageClass::setInspectionPosForTrc()
{
	auto* pInsp = GetInspection();
	if (nullptr != pInsp)
	{
		m_inspectionPosInTRC = SvOi::getInspectionPos(pInsp->getObjectId());
	}
	else
	{
		m_inspectionPosInTRC = -1;
	}
}

bool SVImageClass::UpdateTRCBuffers(SvStl::MessageContainerVector *pErrorMessages)
{
	if (SVSVIMStateClass::CheckState(SV_STATE_RUNNING))
	{
		//[MZA][8.10][26.06.2018] In Run-mode it is not valid to updated TRC-Buffer. Return true to avoid abort of the run.
		return true;
	}

	bool retValue = true;
	if (0 <= m_inspectionPosInTRC)
	{
		if (SvPb::SVImageTypeEnum::SVImageTypeDependent == m_ImageType ||
			SvPb::SVImageTypeEnum::SVImageTypeLogical == m_ImageType)
		{
			SVImageClass* pParent = GetParentImage();
			if (nullptr != pParent)
			{
				MatroxBufferChildDataStruct bufferStruct;
				HRESULT Result = SVImageProcessingClass::FillChildBufferStructFromInfo(pParent->GetImageInfo(), m_ImageInfo, bufferStruct);
				if (S_OK == Result)
				{
					try
					{
						m_isChildImageInTRC = true;
						auto& rTRC = SvOi::getTriggerRecordControllerRWInstanceThrow();
						m_imagePosInTRC = rTRC.addOrChangeChildImage(getObjectId(), m_ParentImageInfo.first, bufferStruct, m_inspectionPosInTRC);
					}
					catch (const SvStl::MessageContainer& rExp)
					{
						//This is the topmost catch for MessageContainer exceptions
						SvStl::MessageManager Exception(SvStl::MsgType::Log);
						SvStl::MessageData Msg(rExp.getMessage());
						Exception.setMessage(rExp.getMessage(), getObjectId());
						if (nullptr != pErrorMessages)
						{
							pErrorMessages->push_back(Exception.getMessageContainer());
						}
						retValue = false;
						m_imagePosInTRC = -1;
					}
				}
				else
				{
					SvStl::MessageManager Msg(SvStl::MsgType::Log);
					Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UpdateTRCBuffersFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
					assert(false);
					if (nullptr != pErrorMessages)
					{
						pErrorMessages->push_back(Msg.getMessageContainer());
					}
					retValue = false;
					m_imagePosInTRC = -1;
				}
			}
			else
			{
				retValue = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InitImageFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
					pErrorMessages->push_back(Msg);
				}
			}
		}
		else
		{
			SVMatroxBufferCreateStruct bufferStruct;
			HRESULT Result = SVImageProcessingClass::FillBufferStructFromInfo(m_ImageInfo, bufferStruct);
			if (S_OK == Result)
			{
				try
				{
					m_isChildImageInTRC = false;
					auto& rTRC = SvOi::getTriggerRecordControllerRWInstanceThrow();
					m_imagePosInTRC = rTRC.addOrChangeImage(getObjectId(), bufferStruct, m_inspectionPosInTRC);
				}
				catch (const SvStl::MessageContainer& rExp)
				{
					//This is the topmost catch for MessageContainer exceptions
					SvStl::MessageManager Exception(SvStl::MsgType::Log);
					SvStl::MessageData Msg(rExp.getMessage());
					Exception.setMessage(rExp.getMessage(), getObjectId());
					if (nullptr != pErrorMessages)
					{
						pErrorMessages->push_back(Exception.getMessageContainer());
					}
					retValue = false;
					m_imagePosInTRC = -1;
				}
			}
			else
			{
				SvStl::MessageManager Msg(SvStl::MsgType::Log);
				Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UpdateTRCBuffersFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
				assert(false);
				if (nullptr != pErrorMessages)
				{
					pErrorMessages->push_back(Msg.getMessageContainer());
				}
				retValue = false;
				m_imagePosInTRC = -1;
			}
		}
	}
	return retValue;
}

void SVImageClass::copyCurrent2SaveImage()
{
	if (m_ImageInfoOfSavedBuffer != m_ImageInfo)
	{
		m_ImageInfoOfSavedBuffer = m_ImageInfo;
		SVImageProcessingClass::CreateImageBuffer(m_ImageInfoOfSavedBuffer, m_savedBuffer);
	}

	auto image = getLastImage();
	assert(nullptr != m_savedBuffer);
	if (nullptr != m_savedBuffer && nullptr != image)
	{
		HRESULT result = SVMatroxBufferInterface::CopyBuffer(m_savedBuffer->GetBuffer(), image->GetBuffer());
		assert(S_OK == result);		UNREFERENCED_PARAMETER(result);
	}
}

SVImageClass* castObjectToImage(SvOi::IObjectClass* pObject)
{
	SvIe::SVImageClass* pImage;
	auto* pLinked = dynamic_cast<SvOi::ILinkedObject*>(pObject);
	if (nullptr != pLinked)
	{
		try
		{
			pImage = dynamic_cast<SvIe::SVImageClass*> (const_cast<SvOi::IObjectClass*>(pLinked->getLinkedObject()));
		}
		catch (...)
		{
			pImage = nullptr;
		}
	}
	else
	{
		pImage = dynamic_cast<SvIe::SVImageClass*> (pObject);
	}
	return pImage;
}
} //namespace SvIe
