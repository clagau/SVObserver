//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVGlobal.h"
#include "SVInspectionProcess.h"
#include "SVVirtualCamera.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"
#include "SVUtilityLibrary/SetBits.h"
#pragma endregion Includes

SVCameraImageTemplate::SVCameraImageTemplate(LPCSTR ObjectName)
	: SVImageClass(ObjectName)
	, mpCamera(nullptr)
	, digitizerObjectID()
{
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVImageObjectType;

	// SVMainImageClass is not a result image.
	// We need to remove the PUBLISH attribute.
	SetObjectAttributesAllowed(SvPb::publishResultImage, SvOi::SetAttributeType::RemoveAttribute);
}

SVCameraImageTemplate::SVCameraImageTemplate(SVObjectClass* POwner, int StringResourceID)
	: SVImageClass(POwner, StringResourceID)
	, mpCamera(nullptr)
	, digitizerObjectID()
{
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVImageObjectType;

	// SVMainImageClass is not a result image.
	// We need to remove the PUBLISH attribute.
	SetObjectAttributesAllowed(SvPb::publishResultImage, SvOi::SetAttributeType::RemoveAttribute);
}

SVCameraImageTemplate::~SVCameraImageTemplate()
{
}

HRESULT SVCameraImageTemplate::UpdateCameraImage(LPCTSTR p_szCameraName)
{
	mpCamera = nullptr;
	digitizerObjectID.clear();

	SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObjectCompleteName(p_szCameraName);

	if (nullptr != l_pObject)
	{
		digitizerObjectID = l_pObject->GetUniqueObjectID();
	}

	return RebuildCameraImage();
}

HRESULT SVCameraImageTemplate::UpdateCameraImage(const SVGUID& p_CameraID)
{
	mpCamera = nullptr;
	digitizerObjectID = p_CameraID;

	return RebuildCameraImage();
}

bool SVCameraImageTemplate::DestroyImage()
{
	mpCamera = nullptr;
	digitizerObjectID.clear();

	bool bOk = SVImageClass::DestroyImage();

	return bOk;
}

bool SVCameraImageTemplate::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	bool l_bOk = SVImageClass::CreateObject(rCreateStructure);

	l_bOk = l_bOk && (S_OK == RebuildCameraImage());

	// SVMainImageClass is not a result image.
	// We need to remove the PUBLISH attribute.
	SetObjectAttributesAllowed(SvPb::publishResultImage, SvOi::SetAttributeType::RemoveAttribute);
	SetObjectAttributesAllowed(SvPb::dataDefinitionImage, SvOi::SetAttributeType::AddAttribute);

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
	return mpCamera;
}

std::string SVCameraImageTemplate::GetCameraName() const
{
	std::string l_String;

	if (nullptr != mpCamera)
	{
		l_String = mpCamera->GetName();
	}

	return l_String;
}

HRESULT SVCameraImageTemplate::GetObjectValue(const std::string& rValueName, _variant_t& rValue) const
{
	HRESULT hr = S_OK;

	if (scDigitizerIDTag == rValueName)
	{


		rValue = digitizerObjectID.ToVARIANT();

	}
	else
	{
		hr = SVImageClass::GetObjectValue(rValueName, rValue);
	}

	return hr;
}

HRESULT SVCameraImageTemplate::SetObjectValue(SVObjectAttributeClass* PDataObject)
{
	HRESULT hr = S_FALSE;
	bool bOk = false;

	SvCl::SVObjectStdStringArrayClass ClassIDList;

	if ((bOk = PDataObject->GetAttributeData(scDigitizerIDTag, ClassIDList)))
	{
		for (int i = 0; i < static_cast<int> (ClassIDList.size()); i++)
		{
			// convert the guidStr to a Guid
			AfxGetClassIDFromString(ClassIDList[i].c_str(), &(digitizerObjectID.ToGUID()));
		}
	}
	else
	{
		hr = SVImageClass::SetObjectValue(PDataObject);

		m_ObjectAttributesAllowed |= SvPb::remotelySetable;

		return hr;
	}

	m_LastUpdate = SvTl::GetTimeStamp();

	hr = bOk ? S_OK : S_FALSE;
	return hr;
}

void SVCameraImageTemplate::Persist(SvOi::IObjectWriter& rWriter)
{
	rWriter.StartElement(GetObjectName()); // use internal name for node name

	SVObjectAppClass::Persist(rWriter);
	SVImageClass::PersistImageAttributes(rWriter);

	_variant_t value;
	value.SetString(digitizerObjectID.ToString().c_str());
	rWriter.WriteAttribute(scDigitizerIDTag, value);
	value.Clear();

	rWriter.EndElement();
}

GUID SVCameraImageTemplate::GetDigitizerID()
{
	return digitizerObjectID.ToGUID();
}

HRESULT SVCameraImageTemplate::DisconnectBuffers()
{
	return S_OK;
}

HRESULT SVCameraImageTemplate::ReconnectBuffers()
{
	HRESULT hr = S_OK;

	hr = RebuildCameraImage();

	if (S_OK == hr)
	{
		mpCamera->GetImageInfo(&m_ImageInfo);

		m_ImageInfo.SetOwnerImage(GetUniqueObjectID());

		m_LastUpdate = SvTl::GetTimeStamp();

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

	if (!(digitizerObjectID.empty()))
	{
		pCamera = dynamic_cast<SVVirtualCamera*>(SVObjectManagerClass::Instance().GetObject(digitizerObjectID.ToGUID()));
	}

	if ((nullptr == pCamera) || (nullptr != pOwner && pOwner->GetObjectType() == SvPb::SVToolSetObjectType))
	{
		SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*>(GetInspection());

		if (nullptr != pInspection)
		{
			SVVirtualCamera* pTempCamera = pInspection->GetFirstCamera();

			if (nullptr != pTempCamera)
			{
				pCamera = pTempCamera;
			}
		}
	}

	if (nullptr != pCamera)
	{
		if (mpCamera != pCamera)
		{
			SVImageInfoClass CameraImageInfo;
			pCamera->GetImageInfo(&CameraImageInfo);
			if (CreateBuffers(CameraImageInfo))
			{
				mpCamera = pCamera;
				digitizerObjectID = mpCamera->GetUniqueObjectID();
			}

			SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*>(GetInspection());

			if (nullptr != pInspection)
			{
				pInspection->AddSharedCamera(mpCamera);
			}
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

