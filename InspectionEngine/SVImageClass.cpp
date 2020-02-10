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
#include "SVObjectLibrary/SVClsIds.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectLibrary/SVOutputInfoListClass.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVTimerLibrary/SVClock.h"
#include "TriggerRecordController/ITriggerRecordControllerRW.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

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
	bool l_bOk = SVObjectAppClass::CreateObject(rCreateStructure);

	SVObjectManagerClass::Instance().RegisterSubObject(GetUniqueObjectID());
	SVObjectManagerClass::Instance().RegisterSubObject(m_width.GetUniqueObjectID());
	SVObjectManagerClass::Instance().RegisterSubObject(m_height.GetUniqueObjectID());

	l_bOk &= (S_OK == UpdateFromToolInformation());

	SetObjectAttributesAllowed(SvPb::publishResultImage | SvPb::dataDefinitionImage, SvOi::SetAttributeType::AddAttribute);	// add this to older configs

	m_isCreated = l_bOk;

	return l_bOk;
}

bool SVImageClass::CloseObject()
{
	bool rc = (S_OK == ClearParentConnection());

	SvOi::ITool* pTool = dynamic_cast<SvOi::ITool*> (GetTool());
	if (nullptr != pTool)
	{
		pTool->SetToolImage(GUID_NULL);
	}

	if (m_isCreated)
	{
		rc = DestroyImage();
	}

	SVObjectManagerClass::Instance().UnregisterSubObject(GetUniqueObjectID());
	SVObjectManagerClass::Instance().UnregisterSubObject(m_width.GetUniqueObjectID());
	SVObjectManagerClass::Instance().UnregisterSubObject(m_height.GetUniqueObjectID());

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

	if (!m_ParentImageInfo.first.empty())
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

const SVGUID& SVImageClass::GetParentImageID() const
{
	return m_ParentImageInfo.first;
}

void SVImageClass::init()
{
	try
	{
		::InitializeCriticalSection(&m_hCriticalSection);

		m_bCriticalSectionCreated = true;
	}
	catch (...)
	{
		m_bCriticalSectionCreated = false;
	}

	m_ImageType = SvDef::SVImageTypeEnum::SVImageTypeUnknown;

	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVImageObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvPb::SVImageMonoType;

	// derived classes that are not result images (i.e. SVMainImageClass)
	// need to remove the PUBLISH attribute.
	SetObjectAttributesAllowed(SvPb::archivableImage | SvPb::publishResultImage | SvPb::dataDefinitionImage, SvOi::SetAttributeType::OverwriteAttribute);

	m_ParentImageInfo.second = nullptr;

	m_ImageInfo.SetOwnerImage(GetUniqueObjectID());

	m_ImageInfo.SetExtentProperty(SvPb::SVExtentPropertyPositionPointX, SvDef::cDefaultWindowToolLeft);
	m_ImageInfo.SetExtentProperty(SvPb::SVExtentPropertyPositionPointY, SvDef::cDefaultWindowToolTop);
	m_ImageInfo.SetExtentProperty(SvPb::SVExtentPropertyWidth, SvDef::cDefaultWindowToolWidth);
	m_ImageInfo.SetExtentProperty(SvPb::SVExtentPropertyHeight, SvDef::cDefaultWindowToolHeight);
	m_ImageInfo.SetExtentProperty(SvPb::SVExtentPropertyWidthScaleFactor, SvDef::cDefaultWindowToolWidthScaleFactor);
	m_ImageInfo.SetExtentProperty(SvPb::SVExtentPropertyHeightScaleFactor, SvDef::cDefaultWindowToolHeightScaleFactor);

	m_width.SetTypeName(_T("Image Width"));
	m_height.SetTypeName(_T("Image Height"));

	m_width.setResetOptions(false, SvOi::SVResetItemTool);
	m_width.SetObjectEmbedded(SVExtentWidthObjectGuid, this, SvUl::LoadStdString(IDS_OBJECTNAME_EXTENT_WIDTH).c_str());
	m_height.setResetOptions(false, SvOi::SVResetItemTool);
	m_height.SetObjectEmbedded(SVExtentHeightObjectGuid, this, SvUl::LoadStdString(IDS_OBJECTNAME_EXTENT_HEIGHT).c_str());

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

			m_ImageType = SvDef::SVImageTypeEnum::SVImageTypeUnknown;

			m_isCreated = false;

			bOk = Unlock() && bOk;

			assert(bOk);
		}

		if (!bOk)
		{
			SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ImageClass_DestroyError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10051);
		}
	}

	return bOk;
}

HRESULT SVImageClass::InitializeImage(SvDef::SVImageTypeEnum ImageType)
{
	if (m_ImageType != ImageType)
	{
		m_ImageType = ImageType;

		m_LastUpdate = SvTl::GetTimeStamp();
	}

	return S_OK;
}

HRESULT SVImageClass::InitializeImage(SVImageClass* pParentImage)
{
	HRESULT Result {S_OK};

	if (nullptr != pParentImage)
	{
		SVGUID ImageID = pParentImage->GetUniqueObjectID();
		if (m_ParentImageInfo.first != ImageID)
		{
			ClearParentConnection();

			m_ParentImageInfo.first = ImageID;
			m_ParentImageInfo.second = nullptr;

			m_LastUpdate = SvTl::GetTimeStamp();

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

		m_LastUpdate = SvTl::GetTimeStamp();

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

HRESULT SVImageClass::UpdateImage(const SVGUID& rParentID, const SVImageInfoClass& rImageInfo)
{
	HRESULT Result {S_OK};

	if (m_ParentImageInfo.first != rParentID || m_ImageInfo != rImageInfo)
	{
		if (m_ParentImageInfo.first != rParentID)
		{
			ClearParentConnection();

			m_ParentImageInfo.first = rParentID;
			m_ParentImageInfo.second = nullptr;
		}

		if (m_ImageInfo != rImageInfo)
		{
			m_ImageInfo = rImageInfo;
			m_ImageInfo.SetOwnerImage(GetUniqueObjectID());
			setImageSubType();
		}

		m_LastUpdate = SvTl::GetTimeStamp();

		Result = (ResetObject() ? S_OK : E_FAIL);
	}

	return Result;
}

HRESULT SVImageClass::UpdateImage(SvDef::SVImageTypeEnum ImageType)
{
	HRESULT l_Status = S_OK;

	if (m_ImageType != ImageType)
	{
		m_ImageType = ImageType;

		m_LastUpdate = SvTl::GetTimeStamp();

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
		UpdateChildren();
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
		m_LastReset = SvTl::GetTimeStamp();
	}

	//Update Position returns S_FALSE when ROI for LiniarTool without rotation is moved 
	// outside the parentwindow. When moved with the mouse this is corrected 
	//later should be ignored here S_OK != S_FALSE  &&
	if ( S_OK != hr  && S_NoParent != hr && nullptr != pErrorMessages && pErrorMessages->empty())
	{
		SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RebuildFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
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

	if (m_ImageType != SvDef::SVImageTypeEnum::SVImageTypeMain)
	{
		SVImageClass* l_pParentImage = GetParentImage();

		if (nullptr != l_pParentImage && (m_LastReset < l_pParentImage->GetLastResetTimeStamp() || m_LastReset < m_LastUpdate))
		{
			SVImageExtentClass imageExtent = GetImageExtents();
			SVImagePropertiesClass l_ImageProperties = m_ImageInfo.GetImageProperties();
			SVImageInfoClass l_ImageInfo = l_pParentImage->GetImageInfo();

			l_ImageInfo.SetOwnerImage(GetUniqueObjectID());

			l_ImageInfo.SetImageProperties(l_ImageProperties);

			if (m_ImageType == SvDef::SVImageTypeEnum::SVImageTypeDependent)
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

				m_LastUpdate = SvTl::GetTimeStamp();

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
		SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UpdateFromParentInformationFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
		pErrorMessages->push_back(Msg);
	}

	return Result;
}

HRESULT SVImageClass::UpdateFromToolInformation()
{
	HRESULT l_Status = S_OK;

	SVGUID ToolID;
	SVImageExtentClass toolExtent = m_ImageInfo.GetExtents();

	// When initialized from CreateObject(), tool is nullptr.
	SVTaskObjectClass*	pParentTask = dynamic_cast <SVTaskObjectClass*> (GetTool());
	if (nullptr != pParentTask)
	{
		if ((SvDef::SVImageTypeEnum::SVImageTypeMain != m_ImageType) &&
			(SvDef::SVImageTypeEnum::SVImageTypeIndependent != m_ImageType) &&
			(SvDef::SVImageTypeEnum::SVImageTypeDependent != m_ImageType) &&
			pParentTask->DoesObjectHaveExtents())
		{
			RECT l_Rect;
			SVImageExtentClass tempExtent = pParentTask->GetImageExtent();

			if (SvDef::SVImageTypeEnum::SVImageTypeLogical == m_ImageType)
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

		if (SvDef::SVImageTypeEnum::SVImageTypeIndependent != m_ImageType &&
			SvDef::SVImageTypeEnum::SVImageTypeDependent != m_ImageType)
		{
			SvOi::ITool* pTool = GetToolInterface();
			if (nullptr != pTool)
			{
				pTool->SetToolImage(GetUniqueObjectID());
			}
		}

		ToolID = nullptr != GetTool() ? GetTool()->GetUniqueObjectID() : GUID_NULL;
	}

	if (m_ImageInfo.GetOwnerID() != ToolID)
	{
		// Set the owning Tool before setting the extents
		m_ImageInfo.SetOwner(ToolID);

		m_LastUpdate = SvTl::GetTimeStamp();
	}

	if ((SvDef::SVImageTypeEnum::SVImageTypeMain != m_ImageType) && (SvDef::SVImageTypeEnum::SVImageTypeIndependent != m_ImageType))
	{
		l_Status = m_ImageInfo.SetExtents(toolExtent);

		if (S_OK == l_Status)
		{
			m_LastUpdate = SvTl::GetTimeStamp();
		}
	}
	return l_Status;
}

HRESULT SVImageClass::ClearParentConnection()
{
	HRESULT l_hrOk = S_OK;

	if (!m_ParentImageInfo.first.empty())
	{
		l_hrOk = SVObjectManagerClass::Instance().DisconnectObjects(m_ParentImageInfo.first, GetUniqueObjectID());
	}

	m_LastUpdate = SvTl::GetTimeStamp();

	return l_hrOk;
}

/*
Updated method to use GetParentImage() method which validates the Parent Image pointer attribute.
The Parent Image attribute should not be used unless it is validated first.
*/
HRESULT SVImageClass::UpdateChild(const SVGUID& rChildID, const SVImageInfoClass& rImageInfo)
{
	HRESULT l_hrOk = S_OK;

	if (Lock())
	{
		if (SvDef::SVImageTypeEnum::SVImageTypeDependent == m_ImageType ||
			SvDef::SVImageTypeEnum::SVImageTypeLogical == m_ImageType)
		{
			SVImageClass* l_pParentImage = GetParentImage();

			assert(nullptr != l_pParentImage);

			if (nullptr != l_pParentImage && l_pParentImage != this)
			{
				SVImageInfoClass l_svImageInfo = rImageInfo;
				SVImagePropertiesClass l_svImageProperties = m_ImageInfo.GetImageProperties();

				l_svImageInfo.SetImageProperties(l_svImageProperties);

				l_hrOk = l_pParentImage->UpdateChild(rChildID, l_svImageInfo);
			}
			else
			{
				l_hrOk = E_FAIL;
			}
		}
		else
		{
			SVImageInfoClass& rChildInfo = m_ChildArrays[rChildID];

			rChildInfo = rImageInfo;

			l_hrOk = m_ImageInfo.ValidateAgainstOutputSpace(rChildInfo.GetExtents());
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
HRESULT SVImageClass::RemoveChild(const SVGUID& rChildID)
{
	HRESULT l_hrOk = S_OK;

	if (Lock())
	{
		SVGuidImageChildMap::iterator l_Iter = m_ChildArrays.find(rChildID);

		if (l_Iter != m_ChildArrays.end())
		{
			SVImageClass* l_pImage = nullptr;

			l_Iter->second.GetOwnerImage(l_pImage);

			if (nullptr != l_pImage)
			{
				l_hrOk = l_pImage->RemoveObjectConnection(GetUniqueObjectID());
			}

			l_Iter->second.Initialize();

			m_ChildArrays.erase(l_Iter);
		}

		m_ParentImageInfo.second = nullptr;

		SVImageClass* l_pParentImage = GetParentImage();

		if (nullptr != l_pParentImage && l_pParentImage != this)
		{
			l_hrOk = l_pParentImage->RemoveChild(rChildID);
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

const SVImageInfoClass& SVImageClass::GetImageInfo() const
{
	return m_ImageInfo;
}

bool SVImageClass::SafeImageCopyToHandle(SvOi::SVImageBufferHandlePtr& p_rHandle, const SvTrc::ITriggerRecordRPtr pTriggerRecord)
{
	bool l_bOk = (nullptr != p_rHandle);

	if (l_bOk)
	{
		if (Lock())
		{
			HRESULT l_Code;

			SvTrc::IImagePtr pImageBuffer = getImageReadOnly(pTriggerRecord.get());
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

HRESULT SVImageClass::LoadImage(LPCTSTR fileName, const SvTrc::ITriggerRecordRWPtr& pTriggerRecord)
{
	HRESULT l_hrOk = E_FAIL;

	SvTrc::IImagePtr pBuffer = getImageToWrite(pTriggerRecord);
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

#ifdef _DEBUG
//Moved to precompiled header: #include <map>
namespace
{
std::map<DWORD, long> f_mapCritSec;
};
#endif

bool SVImageClass::Lock() const
{
	bool l_bOk = false;

	try
	{
		::EnterCriticalSection(&m_hCriticalSection);

		l_bOk = true;
#ifdef _DEBUG
		DWORD dwThreadId = ::GetCurrentThreadId();
		f_mapCritSec[dwThreadId]++;
		//! Double casting required to avoid warnings from 64 to 32 bit conversion
		if (dwThreadId == static_cast<DWORD> (reinterpret_cast<LONGLONG> (m_hCriticalSection.OwningThread)))
		{
			long lRecursionCount = f_mapCritSec[dwThreadId];
		}
#endif
	}
	catch (...)
	{
		l_bOk = false;
	}

	return l_bOk;
}

bool SVImageClass::Unlock() const
{
	bool l_bOk = false;

#ifdef _DEBUG
	DWORD dwThreadId = ::GetCurrentThreadId();
	//! Double casting required to avoid warnings from 64 to 32 bit conversion
	assert(dwThreadId == static_cast<DWORD> (reinterpret_cast<LONGLONG> (m_hCriticalSection.OwningThread)));
#endif

	try
	{
#ifdef _DEBUG
		//! Double casting required to avoid warnings from 64 to 32 bit conversion
		if (dwThreadId == static_cast<DWORD> (reinterpret_cast<LONGLONG> (m_hCriticalSection.OwningThread)))
		{
			long lRecursionCount = f_mapCritSec[dwThreadId];
			//assert( lRecursionCount == m_hCriticalSection.RecursionCount );
		}
		f_mapCritSec[dwThreadId]--;

		// BRW - In Windows 7, LockCount doesn't mean what it meant in Windows XP.
		// http://msdn.microsoft.com/en-us/library/windows/hardware/ff541979(v=vs.85).aspx
#endif

		::LeaveCriticalSection(&m_hCriticalSection);

		l_bOk = true;
	}
	catch (...)
	{
		l_bOk = false;
		assert(l_bOk == true);
	}

	assert(l_bOk == true);

	return l_bOk;
}

HRESULT SVImageClass::RemoveObjectConnection(const GUID& rObjectID)
{
	HRESULT l_Status = SVObjectAppClass::RemoveObjectConnection(rObjectID);

	SVGUID ObjectID = rObjectID;

	if (m_ParentImageInfo.first == ObjectID)
	{
		m_ParentImageInfo.first.clear();
		m_ParentImageInfo.second = nullptr;

		m_LastUpdate = SvTl::GetTimeStamp();
	}
	else
	{
		l_Status = RemoveChild(rObjectID);
	}
	return l_Status;
}

void SVImageClass::Persist(SvOi::IObjectWriter& rWriter)
{
	rWriter.StartElement(GetObjectName()); // use internal name for node name

	SVObjectAppClass::Persist(rWriter);

	PersistImageAttributes(rWriter);

	rWriter.EndElement();
}

void SVImageClass::PersistImageAttributes(SvOi::IObjectWriter& rWriter)
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
}

HRESULT SVImageClass::GetObjectValue(const std::string& rValueName, _variant_t& rValue) const
{
	HRESULT hr = S_OK;

	if (_T("PixelDepth") == rValueName)
	{
		long Value = 0;

		m_ImageInfo.GetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyPixelDepth, Value);

		rValue = Value;
	}
	else if (_T("BandNumber") == rValueName)
	{
		long Value = 0;

		m_ImageInfo.GetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, Value);

		rValue = Value;
	}
	else if (_T("BandLink") == rValueName)
	{
		long Value = 0;

		m_ImageInfo.GetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyBandLink, Value);

		rValue = Value;
	}
	else
	{
		hr = SVObjectAppClass::GetObjectValue(rValueName, rValue);
	}

	return hr;
}

HRESULT SVImageClass::SetObjectValue(SVObjectAttributeClass* PDataObject)
{
	HRESULT hr = E_FAIL;
	bool bOk = false;

	SvCl::SVObjectLongArrayClass svLongArray;

	if ((bOk = PDataObject->GetAttributeData(_T("PixelDepth"), svLongArray)))
	{
		for (int i = 0; i < static_cast<int> (svLongArray.size()); i++)
		{
			m_ImageInfo.SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyPixelDepth, svLongArray[i]);
		}
	}
	else if ((bOk = PDataObject->GetAttributeData(_T("BandNumber"), svLongArray)))
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
		if(SvDef::SVImageFormatEnum::SVImageFormatRGB888 ==  imageFormat || SvDef::SVImageFormatEnum::SVImageFormatRGB8888 == imageFormat)
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
	else if ((bOk = PDataObject->GetAttributeData(_T("BandLink"), svLongArray)))
	{
		for (int i = 0; i < static_cast<int> (svLongArray.size()); i++)
		{
			m_ImageInfo.SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyBandLink, svLongArray[i]);
		}
		setImageSubType();
	}
	else
	{
		hr = SVObjectAppClass::SetObjectValue(PDataObject);

		m_LastUpdate = SvTl::GetTimeStamp();

		return hr;
	}

	m_LastUpdate = SvTl::GetTimeStamp();

	// At some point this can go away,because it's not longer being scripted
	hr = bOk ? S_OK : E_FAIL;
	return hr;
}

//* JMS - New Image Object methods

HRESULT SVImageClass::UpdatePosition()
{
	HRESULT l_Status = S_OK;

	m_ParentImageInfo.second = dynamic_cast<SVImageClass*>(SVObjectManagerClass::Instance().GetObject(m_ParentImageInfo.first));

	if (SvDef::SVImageTypeEnum::SVImageTypeDependent == m_ImageType ||
		SvDef::SVImageTypeEnum::SVImageTypeLogical == m_ImageType)
	{
		if (nullptr != m_ParentImageInfo.second)
		{
			l_Status = m_ParentImageInfo.second->UpdateChild(GetUniqueObjectID(), m_ImageInfo);
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
		SVGuidImageChildMap::iterator l_Iter = m_ChildArrays.begin();

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
		SVGuidImageChildMap::iterator l_Iter = m_ChildArrays.begin();

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
HRESULT SVImageClass::ValidateAgainstParentExtents(SVImageExtentClass& p_rsvExtent)
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

void SVImageClass::setImage(SvTrc::IImagePtr pImage, const SvTrc::ITriggerRecordRWPtr& pTriggerRecord)
{
	assert(nullptr != pTriggerRecord);
	if (nullptr != pTriggerRecord)
	{
		assert(0 <= m_imagePosInTRC && !m_isChildImageInTRC);
		pTriggerRecord->setImage(m_imagePosInTRC, pImage);
	}
}

void SVImageClass::resetImage(const SvTrc::ITriggerRecordRWPtr& pTriggerRecord)
{
	assert(nullptr != pTriggerRecord);
	if (nullptr != pTriggerRecord)
	{
		assert(0 <= m_imagePosInTRC && !m_isChildImageInTRC);
		pTriggerRecord->setImage(m_imagePosInTRC, -1);
	}
}

SvTrc::IImagePtr SVImageClass::getLastImage(bool newIfNotAvailable) const
{
	SvTrc::ITriggerRecordRPtr lastTriggerRecord = nullptr;
	auto& rRecordController = SvTrc::getTriggerRecordControllerRWInstance();
	if (0 <= m_inspectionPosInTRC && !rRecordController.isResetStarted())
	{
		SvTrc::IImagePtr pImage = nullptr;
		int lastTRid = rRecordController.getLastTrId(m_inspectionPosInTRC);
		lastTriggerRecord = rRecordController.createTriggerRecordObject(m_inspectionPosInTRC, lastTRid);
	}
	if (nullptr == lastTriggerRecord && newIfNotAvailable)
	{
		return getTempImageBuffer();

	}
	assert(nullptr != lastTriggerRecord);
	return getImageReadOnly(lastTriggerRecord.get());
}

SvTrc::IImagePtr SVImageClass::getTempImageBuffer(bool createBufferExternIfNecessary) const
{
	SvTrc::IImagePtr pImage = nullptr;
	SVMatroxBufferCreateStruct bufferStruct;
	HRESULT Result = SVImageProcessingClass::FillBufferStructFromInfo(m_ImageInfo, bufferStruct);
	if (S_OK == Result)
	{
		pImage = SvTrc::getTriggerRecordControllerRWInstance().getImageBuffer(bufferStruct, createBufferExternIfNecessary);
	}
	return pImage;
}

SvTrc::IImagePtr SVImageClass::getImageReadOnly(const SvTrc::ITriggerRecordR* pTriggerRecord, bool lockImage) const
{
	SvTrc::IImagePtr pImage = nullptr;
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
		SvStl::MessageMgrStd e(SvStl::MsgType::Log);
		e.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_GetImageFailed_NoTR, SvStl::SourceFileParams(StdMessageParams));
	}
	return pImage;
}

SvTrc::IImagePtr SVImageClass::getImageToWrite(const SvTrc::ITriggerRecordRWPtr& pTriggerRecord)
{
	SvTrc::IImagePtr pImage = nullptr;
	assert(nullptr != pTriggerRecord);
	if (nullptr != pTriggerRecord)
	{
		assert(0 <= m_imagePosInTRC && !m_isChildImageInTRC);
		pImage = pTriggerRecord->createNewImageHandle(m_imagePosInTRC);
	}
	else
	{
		SvStl::MessageMgrStd e(SvStl::MsgType::Log);
		e.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_GetImageFailed_NoTR, SvStl::SourceFileParams(StdMessageParams));
	}
	return pImage;
}

void SVImageClass::GetOutputList(SVOutputInfoListClass& p_rOutputInfoList)
{
	if (0 != ObjectAttributesAllowed())
	{
		for (auto* pObject : m_embeddedList)
		{
			if (nullptr != pObject)
			{
				p_rOutputInfoList.Add(&(pObject->GetObjectOutputInfo()));
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
SvDef::SVImageTypeEnum SVImageClass::GetImageType() const
{
	return m_ImageType;
}

SvOi::ISVImage* SVImageClass::GetParentImageInterface() const
{
	return GetParentImage();
}

SvOi::SVImageBufferHandlePtr SVImageClass::getImageData() const
{
	SvOi::SVImageBufferHandlePtr dataSmartPointer;
	SvTrc::IImagePtr pImage = getLastImage();
	if (nullptr != pImage && !pImage->isEmpty())
	{
		dataSmartPointer = pImage->getHandle();
	}
	return dataSmartPointer;
}

std::string SVImageClass::getDisplayedName() const
{
	std::string strName;
	if (SvDef::SVImageTypeMain == m_ImageType)
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
	SvTrc::IImagePtr pImage = getLastImage();
	if (nullptr != pImage && !pImage->isEmpty())
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
			hr = SVMatroxBufferInterface::Export(pImage->getHandle()->GetBuffer(), strPath, efileformat);
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
	SvStl::MessageContainer msg(SVMSG_SVO_NULL_POINTER, SvStl::Tid_Default, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
	throw msg;
}
#pragma endregion virtual method (ISVImage)

void SVImageClass::setImageSubType()
{
	long bandNumber = 0;
	m_ImageInfo.GetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, bandNumber);
	if (1 == bandNumber)
	{
		m_outObjectInfo.m_ObjectTypeInfo.SubType = SvPb::SVImageMonoType;
	}
	else if (3 == bandNumber)
	{
		m_outObjectInfo.m_ObjectTypeInfo.SubType = SvPb::SVImageColorType;
	}
}

void SVImageClass::setInspectionPosForTrc()
{
	auto* pInsp = GetInspection();
	if (nullptr != pInsp)
	{
		m_inspectionPosInTRC = SvTrc::getInspectionPos(pInsp->GetUniqueObjectID());
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
		if (SvDef::SVImageTypeEnum::SVImageTypeDependent == m_ImageType ||
			SvDef::SVImageTypeEnum::SVImageTypeLogical == m_ImageType)
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
						m_imagePosInTRC = SvTrc::getTriggerRecordControllerRWInstance().addOrChangeChildImage(GetUniqueObjectID(), m_ParentImageInfo.first, bufferStruct, m_inspectionPosInTRC);
					}
					catch (const SvStl::MessageContainer& rExp)
					{
						//This is the topmost catch for MessageContainer exceptions
						SvStl::MessageMgrStd Exception(SvStl::MsgType::Log);
						SvStl::MessageData Msg(rExp.getMessage());
						Exception.setMessage(rExp.getMessage(), GetUniqueObjectID());
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
					SvStl::MessageMgrStd Msg(SvStl::MsgType::Log);
					Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UpdateTRCBuffersFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
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
					SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InitImageFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
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
					m_imagePosInTRC = SvTrc::getTriggerRecordControllerRWInstance().addOrChangeImage(GetUniqueObjectID(), bufferStruct, m_inspectionPosInTRC);
				}
				catch (const SvStl::MessageContainer& rExp)
				{
					//This is the topmost catch for MessageContainer exceptions
					SvStl::MessageMgrStd Exception(SvStl::MsgType::Log);
					SvStl::MessageData Msg(rExp.getMessage());
					Exception.setMessage(rExp.getMessage(), GetUniqueObjectID());
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
				SvStl::MessageMgrStd Msg(SvStl::MsgType::Log);
				Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UpdateTRCBuffersFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
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

} //namespace SvIe
