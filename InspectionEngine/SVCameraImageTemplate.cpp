//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCameraImageTemplate
//* .File Name       : $Workfile:   SVCameraImageTemplate.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   01 Oct 2013 12:16:26  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVCameraImageTemplate.h"
#include "SVVirtualCamera.h"
#include "ObjectInterfaces/IInspectionProcess.h"
#include "ObjectInterfaces/IObjectManager.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "SVUtilityLibrary/SVClock.h"
#pragma endregion Includes

namespace SvIe
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SVCameraImageTemplate::SVCameraImageTemplate(LPCSTR ObjectName)
	: SVImageClass(ObjectName)
	, m_pCamera(nullptr)
	, m_digitizerObjectID()
{
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVImageObjectType;
}

SVCameraImageTemplate::SVCameraImageTemplate(SVObjectClass* POwner, int StringResourceID)
	: SVImageClass(POwner, StringResourceID)
	, m_pCamera(nullptr)
	, m_digitizerObjectID()
{
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVImageObjectType;
}

SVCameraImageTemplate::~SVCameraImageTemplate()
{
}

HRESULT SVCameraImageTemplate::UpdateCameraImage(LPCTSTR p_szCameraName)
{
	m_pCamera = nullptr;
	m_digitizerObjectID = SvDef::InvalidObjectId;

	SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObjectCompleteName(p_szCameraName);

	if (nullptr != l_pObject)
	{
		m_digitizerObjectID = l_pObject->getObjectId();
	}

	return RebuildCameraImage();
}

HRESULT SVCameraImageTemplate::UpdateCameraImage(uint32_t cameraID)
{
	m_pCamera = nullptr;
	m_digitizerObjectID = cameraID;

	return RebuildCameraImage();
}

bool SVCameraImageTemplate::DestroyImage()
{
	m_pCamera = nullptr;
	m_digitizerObjectID = SvDef::InvalidObjectId;

	bool bOk = SVImageClass::DestroyImage();

	return bOk;
}

bool SVCameraImageTemplate::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	bool l_bOk = SVImageClass::CreateObject(rCreateStructure);

	l_bOk = l_bOk && (S_OK == RebuildCameraImage());

	m_isCreated = l_bOk;

	return l_bOk;
}

bool SVCameraImageTemplate::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	RebuildCameraImage();

	return SVImageClass::ResetObject(pErrorMessages);
}

SVVirtualCamera* SVCameraImageTemplate::GetCamera() const
{
	return m_pCamera;
}

std::string SVCameraImageTemplate::GetCameraName() const
{
	std::string l_String;

	if (nullptr != m_pCamera)
	{
		l_String = m_pCamera->GetName();
	}

	return l_String;
}

HRESULT SVCameraImageTemplate::SetObjectValue(SVObjectAttributeClass* pDataObject)
{
	HRESULT hr = S_FALSE;

	SvCl::SVObjectStdStringArrayClass ClassIDList;
	bool bOk{pDataObject->GetAttributeData(scDigitizerIDTag, ClassIDList)};
	if (bOk)
	{
		for (int i = 0; i < static_cast<int> (ClassIDList.size()); i++)
		{
			m_digitizerObjectID = calcObjectId(ClassIDList[i]);
		}
	}
	else
	{
		hr = SVImageClass::SetObjectValue(pDataObject);

		m_ObjectAttributesAllowed |= SvPb::remotelySetable;

		return hr;
	}

	m_LastUpdate = SvUl::GetTimeStamp();

	hr = bOk ? S_OK : S_FALSE;
	return hr;
}

void SVCameraImageTemplate::Persist(SvOi::IObjectWriter& rWriter, bool closeObject/* = true*/) const
{
	rWriter.StartElement(GetObjectName()); // use internal name for node name

	SVObjectAppClass::Persist(rWriter);
	SVImageClass::PersistImageAttributes(rWriter);

	_variant_t value = convertObjectIdToVariant(m_digitizerObjectID);
	rWriter.WriteAttribute(scDigitizerIDTag, value);
	value.Clear();

	if (closeObject)
	{
		rWriter.EndElement();
	}
}

HRESULT SVCameraImageTemplate::DisconnectBuffers()
{
	return S_OK;
}

HRESULT SVCameraImageTemplate::ReconnectBuffers()
{
	 HRESULT hr = RebuildCameraImage();

	if (S_OK == hr)
	{
		m_pCamera->GetImageInfo(&m_ImageInfo);

		m_ImageInfo.SetOwnerImage(getObjectId());

		m_LastUpdate = SvUl::GetTimeStamp();

		if (!ResetObject())
		{
			hr = S_FALSE;
		}
	}

	return hr;
}

HRESULT SVCameraImageTemplate::RebuildCameraImage()
{
	HRESULT l_Status = S_OK;

	SVVirtualCamera* pCamera(nullptr);
	SVObjectClass* pOwner = GetParent();

	if (SvDef::InvalidObjectId != m_digitizerObjectID)
	{
		pCamera = dynamic_cast<SVVirtualCamera*> (SvOi::getObject(m_digitizerObjectID));
	}

	SvOi::IInspectionProcess* pInspection = GetInspectionInterface();

	if ((nullptr == pCamera) || (nullptr != pOwner && pOwner->GetObjectType() == SvPb::SVToolSetObjectType))
	{
		if (nullptr != pInspection)
		{
			uint32_t cameraId = pInspection->getFirstCamera();
			SVVirtualCamera* pTempCamera = dynamic_cast<SVVirtualCamera*> (SvOi::getObject(cameraId));

			if (nullptr != pTempCamera)
			{
				pCamera = pTempCamera;
			}
		}
	}

	if (nullptr != pCamera)
	{
		if (m_pCamera != pCamera)
		{
			SVImageInfoClass CameraImageInfo;
			pCamera->GetImageInfo(&CameraImageInfo);
			if (CreateBuffers(CameraImageInfo))
			{
				m_pCamera = pCamera;
				m_digitizerObjectID = m_pCamera->getObjectId();
			}

			if (nullptr != pInspection)
			{
				pInspection->addSharedCamera(m_digitizerObjectID);
			}
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

} //namespace SvIe
