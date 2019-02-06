//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolExtentClass
//* .File Name       : $Workfile:   SVToolExtentClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   08 May 2013 16:44:04  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <boost/config.hpp>
#include "SVToolExtentClass.h"
#include "SVTool.h"
#include "InspectionEngine/SVImageClass.h"
#include "ObjectInterfaces/IInspectionProcess.h"
#include "Definitions/SVResetStruct.h"
#include "SVValueObjectLibrary/SVValueObject.h"
#pragma endregion Includes

namespace SvTo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SVToolExtentClass::SVTranslationFilterSet SVToolExtentClass::
m_LinearToolTranslations {SvDef::SVExtentTranslationProfile, SvDef::SVExtentTranslationProfileShift}
;

SVToolExtentClass::SVToolExtentClass(SVImageExtentClass& rImageExtent) :
m_rImageExtent{rImageExtent}
{
	Initialize();
}

SVToolExtentClass::~SVToolExtentClass()
{
	Initialize();
}

HRESULT SVToolExtentClass::Initialize()
{
	HRESULT l_svOk = S_OK;

	m_pTool = nullptr;
	m_pToolImage = nullptr;

	m_eTranslation = SvDef::SVExtentTranslationUnknown;
	m_eShape = SvDef::SVExtentShapeUnknown;

	KeepPrevError(l_svOk, m_Properties.Initialize());

	m_bAlwaysUpdate = true;

	m_pSelectedImage = nullptr;

	KeepPrevError(l_svOk, m_svRootOffsetData.Initialize());
	KeepPrevError(l_svOk, m_svSelectedOffsetData.Initialize());

	return l_svOk;
}

void SVToolExtentClass::SetToolImage(SvIe::SVImageClass *pToolImage)
{
	if (m_pToolImage != pToolImage)
	{
		m_pToolImage = pToolImage;

		m_svRootOffsetData.Initialize();
		m_svSelectedOffsetData.Initialize();
	}
}

void SVToolExtentClass::SetSelectedImage(SvIe::SVImageClass* pSelectedImage)
{
	if (m_pSelectedImage != pSelectedImage)
	{
		m_pSelectedImage = pSelectedImage;

		m_svSelectedOffsetData.Initialize();
	}
}

HRESULT SVToolExtentClass::ValidExtentAgainstParentImage(const SVImageExtentClass& rImageExtent) const
{
	HRESULT l_Status = S_OK;

	if (nullptr != m_pToolImage && nullptr != m_pToolImage->GetParentImage())
	{
		l_Status = m_pToolImage->GetParentImage()->ValidateAgainstOutputExtents(rImageExtent);
	}
	else
	{
		l_Status = S_FALSE;
	}
	return l_Status;
}

HRESULT SVToolExtentClass::UpdateExtentToParentExtents(SVImageExtentClass& rNewExtent)
{
	HRESULT l_Status = S_OK;

	if (nullptr != m_pToolImage)
	{
		SvIe::SVImageClass* pParent = m_pToolImage->GetParentImage();

		if (nullptr != pParent)
		{
			double dPosX(0.0), dPosY(0.0), dWidth(0.0), dHeight(0.0);
			const SVImageExtentClass rimageExtent = pParent->GetImageExtents();
			// Get the Width and Height form the parent extent. Set the Position point to zero, zero.
			l_Status = rimageExtent.GetExtentProperty(SvDef::SVExtentPropertyOutputHeight, dHeight);
			KeepPrevError(l_Status, rimageExtent.GetExtentProperty(SvDef::SVExtentPropertyOutputWidth, dWidth));
			KeepPrevError(l_Status, rNewExtent.SetExtentProperty(SvDef::SVExtentPropertyPositionPointX, dPosX));
			KeepPrevError(l_Status, rNewExtent.SetExtentProperty(SvDef::SVExtentPropertyPositionPointY, dPosY));
			KeepPrevError(l_Status, rNewExtent.SetExtentProperty(SvDef::SVExtentPropertyHeight, dHeight));
			KeepPrevError(l_Status, rNewExtent.SetExtentProperty(SvDef::SVExtentPropertyWidth, dWidth));
			KeepPrevError(l_Status, rNewExtent.SetExtentProperty(SvDef::SVExtentPropertyOutputHeight, dHeight));
			KeepPrevError(l_Status, rNewExtent.SetExtentProperty(SvDef::SVExtentPropertyOutputWidth, dWidth));
		}
		else
		{
			l_Status = E_FAIL;
		}

		if (S_OK == l_Status)
		{
			if (rNewExtent != m_pToolImage->GetImageExtents())
			{
				l_Status = m_pToolImage->UpdateImage(rNewExtent, true);
			}
		}
	}
	else
	{
		l_Status = E_FAIL;
	}
	return l_Status;
}

HRESULT SVToolExtentClass::GetParentExtent(SVImageExtentClass& rParentExtent) const
{
	HRESULT l_Status = S_OK;
	SvIe::SVImageClass* pParent = m_pToolImage->GetParentImage();

	if (nullptr != pParent)
	{
		rParentExtent = pParent->GetImageExtents();
	}
	else
	{
		l_Status = E_FAIL;
	}
	return l_Status;
}

HRESULT SVToolExtentClass::UpdateExtentAgainstParentImage(const SVImageExtentClass& rImageExtent)
{
	HRESULT l_Status = S_OK;

	if (nullptr != m_pToolImage)
	{
		if (S_OK == l_Status)
		{
			if (rImageExtent != m_pToolImage->GetImageExtents())
			{
				l_Status = m_pToolImage->UpdateImage(rImageExtent, true);
			}
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVToolExtentClass::UpdateImageWithExtent(SVToolExtentTypeEnum p_ToolExtentType)
{
	HRESULT l_Status = S_OK;

	if (nullptr != m_pToolImage)
	{
		SVImageExtentClass imageExtents;

		SvDef::SVImageTypeEnum l_Type = m_pToolImage->GetImageType();

		SvIe::SVImageClass* pParent = m_pToolImage->GetParentImage();

		if (nullptr != pParent)
		{
			if (SvDef::SVImageTypeEnum::SVImageTypeDependent == l_Type)
			{
				imageExtents = pParent->GetImageExtents();

				imageExtents.SetTranslation(GetTranslation());

				imageExtents.SetExtentProperty(SvDef::SVExtentPropertyPositionPoint, SVPoint<double>(0.0, 0.0));

				l_Status = SetImageExtent(imageExtents);
			}
			else
			{
				imageExtents = m_rImageExtent;

				SvOi::IInspectionProcess* pInspection = m_pToolImage->GetInspectionInterface();

				if (nullptr != pInspection && pInspection->IsResetStateSet(SvDef::SVResetAutoMoveAndResize))
				{
					if (imageExtents != m_pToolImage->GetImageExtents())
					{
						if (S_OK != m_pToolImage->ValidateAgainstParentExtents(imageExtents))
						{
							pParent->GetImageExtentsToFit(imageExtents, imageExtents);

							SetImageExtent(imageExtents);
						}
					}
				}

				if (SVTransformationToolExtent == p_ToolExtentType)
				{
					double l_Value = 0.0;
					const SVImageExtentClass& rInputExtent = pParent->GetImageExtents();

					if (S_OK == rInputExtent.GetExtentProperty(SvDef::SVExtentPropertyOutputWidth, l_Value))
					{
						imageExtents.SetExtentProperty(SvDef::SVExtentPropertyWidth, l_Value);
					}

					if (S_OK == rInputExtent.GetExtentProperty(SvDef::SVExtentPropertyOutputHeight, l_Value))
					{
						imageExtents.SetExtentProperty(SvDef::SVExtentPropertyHeight, l_Value);
					}

					l_Status = SetImageExtent(imageExtents);
				}
			}
		}
		else
		{
			imageExtents = m_rImageExtent;
		}

		if (S_OK == l_Status)
		{
			if (imageExtents != m_pToolImage->GetImageExtents())
			{
				l_Status = m_pToolImage->UpdateImage(imageExtents, true);
			}
		}

		if (S_OK == l_Status)
		{
			if (nullptr != m_pTool)
			{
				SvOi::IInspectionProcess* pInspection = m_pTool->GetInspectionInterface();
				if (pInspection)
				{
					pInspection->ForceOffsetUpdate();
				}
			}

			if (S_OK == l_Status)
			{
				l_Status = UpdateOffsetData(true);
			}
		}
	}
	else
	{
		l_Status = E_FAIL;
	}
	return l_Status;
}

SvDef::SVExtentTranslationEnum SVToolExtentClass::GetTranslation()
{
	return m_eTranslation;
}

void SVToolExtentClass::SetTranslation(SvDef::SVExtentTranslationEnum eTranslation)
{
	m_eTranslation = eTranslation;
}

HRESULT SVToolExtentClass::SetLinearTranslation(SvDef::SVExtentTranslationEnum eTranslation)
{
	HRESULT l_hrOk = S_FALSE;

	SVTranslationFilterSet::iterator l_OldIter = m_LinearToolTranslations.find(m_eTranslation);
	SVTranslationFilterSet::iterator l_NewIter = m_LinearToolTranslations.find(eTranslation);

	if ((m_eTranslation != eTranslation) && (l_OldIter != m_LinearToolTranslations.end()) && (l_NewIter != m_LinearToolTranslations.end()))
	{
		_variant_t PointY;
		_variant_t Height;

		if ((S_OK == GetExtentValue(SvDef::SVExtentPropertyPositionPointY, PointY)) &&
			(S_OK == GetExtentValue(SvDef::SVExtentPropertyHeight, Height)))
		{
			double NewY = static_cast<double> (PointY);
			double Offset = static_cast<double>(Height) / 2.0;

			NewY += (SvDef::SVExtentTranslationProfileShift == m_eTranslation) ? Offset : -Offset;
			l_hrOk = SetExtentValue(SvDef::SVExtentPropertyPositionPointY, NewY);
		}
		else
		{
			l_hrOk = S_FALSE;
		}

		m_eTranslation = eTranslation;
	}
	else
	{
		SetTranslation(eTranslation);
	}
	return l_hrOk;
}

HRESULT SVToolExtentClass::GetExtentObject(SvDef::SVExtentPropertyEnum p_eProperty, SvOi::IValueObject*& rpValueObject) const
{
	return m_Properties.GetExtentObject(p_eProperty, rpValueObject);
}

void SVToolExtentClass::SetExtentObject(SvDef::SVExtentPropertyEnum p_eProperty, SvOi::IValueObject* pValueObject)
{
	m_Properties.SetExtentObject(p_eProperty, pValueObject);
}

HRESULT SVToolExtentClass::GetExtentValue(SvDef::SVExtentPropertyEnum p_eProperty, _variant_t& rValue) const
{
	HRESULT l_hrOk = S_OK;

	SvOi::IValueObject* pValueObject(nullptr);

	rValue.Clear();

	l_hrOk = GetExtentObject(p_eProperty, pValueObject);

	if (nullptr != pValueObject)
	{
		l_hrOk = pValueObject->getValue(rValue);
	}
	else if (S_OK == l_hrOk)
	{
		l_hrOk = E_FAIL;
	}

	return l_hrOk;
}

HRESULT SVToolExtentClass::SetExtentValue(SvDef::SVExtentPropertyEnum p_eProperty, const _variant_t& rValue)
{
	HRESULT l_hrOk = S_OK;

	SvOi::IValueObject* pValueObject(nullptr);

	l_hrOk = GetExtentObject(p_eProperty, pValueObject);

	if (nullptr != pValueObject)
	{
		l_hrOk = pValueObject->setValue(rValue);
	}
	else if (S_OK == l_hrOk)
	{
		l_hrOk = E_FAIL;
	}
	return l_hrOk;
}

HRESULT SVToolExtentClass::updateImageExtent()
{
	m_rImageExtent.Initialize();
	m_rImageExtent.SetTranslation(m_eTranslation);

	HRESULT result = m_Properties.GetProperties(m_rImageExtent);
	if (S_OK == result)
	{
		result = m_rImageExtent.UpdateData();
	}
	else
	{
		m_rImageExtent.Initialize();
	}

	return result;
}

HRESULT SVToolExtentClass::SetImageExtent(const SVImageExtentClass& rImageExtent)
{
	HRESULT l_hrOk = S_OK;

	double dValue = 0.0;

	m_rImageExtent = rImageExtent;

	SvDef::SVExtentTranslationEnum translation = rImageExtent.GetTranslation();

	if (SvDef::SVExtentTranslationUnknown != translation)
	{
		m_eTranslation = translation;
	}

	if (S_OK == rImageExtent.GetExtentProperty(SvDef::SVExtentPropertyPositionPointX, dValue))
	{
		l_hrOk &= SetExtentValue(SvDef::SVExtentPropertyPositionPointX, dValue);
	}

	if (S_OK == rImageExtent.GetExtentProperty(SvDef::SVExtentPropertyPositionPointY, dValue))
	{
		l_hrOk &= SetExtentValue(SvDef::SVExtentPropertyPositionPointY, dValue);
	}

	if (S_OK == rImageExtent.GetExtentProperty(SvDef::SVExtentPropertyPositionPointEndOfLineX, dValue))
	{
		l_hrOk &= SetExtentValue(SvDef::SVExtentPropertyPositionPointEndOfLineX, dValue);
	}

	if (S_OK == rImageExtent.GetExtentProperty(SvDef::SVExtentPropertyPositionPointEndOfLineY, dValue))
	{
		l_hrOk &= SetExtentValue(SvDef::SVExtentPropertyPositionPointEndOfLineY, dValue);
	}

	if (S_OK == rImageExtent.GetExtentProperty(SvDef::SVExtentPropertyRotationAngle, dValue))
	{
		l_hrOk &= SetExtentValue(SvDef::SVExtentPropertyRotationAngle, dValue);
	}

	if (S_OK == rImageExtent.GetExtentProperty(SvDef::SVExtentPropertyTranslationOffsetX, dValue))
	{
		l_hrOk &= SetExtentValue(SvDef::SVExtentPropertyTranslationOffsetX, dValue);
	}

	if (S_OK == rImageExtent.GetExtentProperty(SvDef::SVExtentPropertyTranslationOffsetY, dValue))
	{
		l_hrOk &= SetExtentValue(SvDef::SVExtentPropertyTranslationOffsetY, dValue);
	}

	if (S_OK == rImageExtent.GetExtentProperty(SvDef::SVExtentPropertyHeight, dValue))
	{
		l_hrOk &= SetExtentValue(SvDef::SVExtentPropertyHeight, dValue);
	}

	if (S_OK == rImageExtent.GetExtentProperty(SvDef::SVExtentPropertyWidth, dValue))
	{
		l_hrOk &= SetExtentValue(SvDef::SVExtentPropertyWidth, dValue);
	}

	if (S_OK == rImageExtent.GetExtentProperty(SvDef::SVExtentPropertyStartAngle, dValue))
	{
		l_hrOk &= SetExtentValue(SvDef::SVExtentPropertyStartAngle, dValue);
	}

	if (S_OK == rImageExtent.GetExtentProperty(SvDef::SVExtentPropertyEndAngle, dValue))
	{
		l_hrOk &= SetExtentValue(SvDef::SVExtentPropertyEndAngle, dValue);
	}

	if (S_OK == rImageExtent.GetExtentProperty(SvDef::SVExtentPropertyInnerRadius, dValue))
	{
		l_hrOk &= SetExtentValue(SvDef::SVExtentPropertyInnerRadius, dValue);
	}

	if (S_OK == rImageExtent.GetExtentProperty(SvDef::SVExtentPropertyOuterRadius, dValue))
	{
		l_hrOk &= SetExtentValue(SvDef::SVExtentPropertyOuterRadius, dValue);
	}

	if (S_OK == rImageExtent.GetExtentProperty(SvDef::SVExtentPropertyHeightScaleFactor, dValue))
	{
		l_hrOk &= SetExtentValue(SvDef::SVExtentPropertyHeightScaleFactor, dValue);
	}

	if (S_OK == rImageExtent.GetExtentProperty(SvDef::SVExtentPropertyWidthScaleFactor, dValue))
	{
		l_hrOk &= SetExtentValue(SvDef::SVExtentPropertyWidthScaleFactor, dValue);
	}
	return l_hrOk;
}

namespace local	// struct local
{
struct IsFormula
{
	explicit IsFormula(SVToolExtentClass* pToolExtents) : m_pToolExtents(pToolExtents) {}
	bool operator () (SvDef::SVExtentPropertyEnum eProperty)
	{
		SvIe::SVExtentPropertyInfoStruct info;
		HRESULT hr = m_pToolExtents->GetExtentPropertyInfo(eProperty, info);
		if (S_OK == hr)
		{
			return info.bFormula || info.bHidden;
		}
		return false;
	}
	SVToolExtentClass* m_pToolExtents;
};
}

HRESULT SVToolExtentClass::GetFilteredImageExtentPropertyList(SVExtentPropertyVector& rPropertyVector)
{
	HRESULT hr = S_OK;

	rPropertyVector.clear();

	rPropertyVector = m_rImageExtent.GetExtentPropertyVector(SvDef::SVExtentPropertyAll);

	SVExtentPropertyVector::iterator iter;
	// remove those that are formula
	iter = std::remove_if(rPropertyVector.begin(), rPropertyVector.end(), local::IsFormula(this));
	if (iter != rPropertyVector.end())
	{
		rPropertyVector.erase(iter, rPropertyVector.end());
	}
	if (SvDef::SVExtentTranslationPolarUnwrap == m_rImageExtent.GetTranslation())
	{
		// special case for Rotation Angle
		SvIe::SVExtentPropertyInfoStruct infoStart;
		SvIe::SVExtentPropertyInfoStruct infoEnd;
		GetExtentPropertyInfo(SvDef::SVExtentPropertyEndAngle, infoEnd);
		GetExtentPropertyInfo(SvDef::SVExtentPropertyStartAngle, infoStart);
		if (infoStart.bFormula || infoEnd.bFormula)
		{
			iter = std::find(rPropertyVector.begin(), rPropertyVector.end(), SvDef::SVExtentPropertyRotationAngle);
			if (iter != rPropertyVector.end())
			{
				rPropertyVector.erase(iter);
			}
		}
	}
	// objects - on ResetObject - set toolextent flag
	return hr;
}

HRESULT SVToolExtentClass::GetRootOffsetData(SVExtentOffsetStruct& p_rsvOffsetData)
{
	HRESULT l_svOk = S_FALSE;

	if (nullptr == m_svRootOffsetData.m_psvImage)
	{
		l_svOk = UpdateOffsetDataToImage(m_svRootOffsetData, nullptr);
	}

	p_rsvOffsetData = m_svRootOffsetData;

	if (nullptr != p_rsvOffsetData.m_psvImage)
	{
		l_svOk = S_OK;
	}
	return l_svOk;
}

HRESULT SVToolExtentClass::GetSelectedOffsetData(SVExtentOffsetStruct& p_rsvOffsetData)
{
	HRESULT l_svOk = S_FALSE;

	if (nullptr == m_pSelectedImage)
	{
		p_rsvOffsetData = m_svRootOffsetData;
	}
	else
	{
		p_rsvOffsetData = m_svSelectedOffsetData;
	}

	if (nullptr != p_rsvOffsetData.m_psvImage)
	{
		l_svOk = S_OK;
	}
	return l_svOk;
}

HRESULT SVToolExtentClass::UpdateOffsetDataToImage(SVExtentOffsetStruct& rOffsetData, SvIe::SVImageClass* pToolImage)
{
	HRESULT l_svOk = S_OK;

	if (nullptr != m_pToolImage)
	{
		SVExtentOffsetStruct offsetData;

		SvIe::SVImageClass* pImageParent = m_pToolImage->GetParentImage();

		if (nullptr != pImageParent)
		{
			SvTo::SVToolClass* pToolParent = dynamic_cast<SvTo::SVToolClass*>(pImageParent->GetTool());

			if (pImageParent != pToolImage && pImageParent != m_pToolImage &&
				nullptr != pToolParent && pToolParent != m_pTool)
			{
				l_svOk = pToolParent->UpdateOffsetDataToImage(offsetData, pToolImage);
			}
			else if (pImageParent == pToolImage || nullptr == pToolImage)
			{
				l_svOk = m_rImageExtent.UpdateSourceOffset(offsetData);
			}
			else
			{
				l_svOk = E_FAIL;
			}

			if (S_OK == l_svOk)
			{
				if (nullptr == rOffsetData.m_psvImage)
				{
					if (nullptr != rOffsetData.m_psvRootImage)
					{
						offsetData.m_psvRootImage = rOffsetData.m_psvRootImage;
					}
					else
					{
						if (nullptr != m_svRootOffsetData.m_psvRootImage)
						{
							offsetData.m_psvRootImage = m_svRootOffsetData.m_psvRootImage;
						}
						else
						{
							offsetData.m_psvRootImage = pImageParent;
						}
					}

					offsetData.m_psvImage = pImageParent;
					offsetData.m_bAlwaysUpdate |= m_bAlwaysUpdate;

					if (nullptr != pImageParent)
					{
						offsetData.m_csImageName = pImageParent->GetCompleteName();
					}
				}
			}
		}
		rOffsetData = offsetData;
	}
	else
	{
		l_svOk = E_FAIL;
	}
	return l_svOk;
}

HRESULT SVToolExtentClass::UpdateOffsetData(bool p_bForceUpdate)
{
	HRESULT l_svOk = S_OK;

	try
	{
		if (p_bForceUpdate ||
			m_bAlwaysUpdate ||
			m_svRootOffsetData.m_bAlwaysUpdate ||
			nullptr == m_svRootOffsetData.m_psvImage)
		{
			if (nullptr != m_pToolImage)
			{
				SVExtentOffsetStruct offsetData;

				SvIe::SVImageClass* pImageParent = m_pToolImage->GetParentImage();

				if (nullptr != pImageParent)
				{
					SvTo::SVToolClass* pToolParent = dynamic_cast<SvTo::SVToolClass*>(pImageParent->GetTool());

					if (nullptr != pToolParent && pToolParent != m_pTool)
					{
						l_svOk = pToolParent->GetRootOffsetData(offsetData);
					}
					else
					{
						offsetData.m_psvRootImage = pImageParent;
						offsetData.m_psvImage = pImageParent;
						offsetData.m_csImageName = pImageParent->GetCompleteName();
					}
				}
				else
				{
					offsetData.m_psvRootImage = m_pToolImage;
					offsetData.m_psvImage = m_pToolImage;
					offsetData.m_csImageName = m_pToolImage->GetCompleteName();
				}

				if (S_OK == l_svOk)
				{
					l_svOk = m_rImageExtent.UpdateSourceOffset(offsetData);
				}

				if (S_OK == l_svOk)
				{
					offsetData.m_bAlwaysUpdate |= m_bAlwaysUpdate;

					m_svRootOffsetData = offsetData;
				}
			}
		}

		if (S_OK == l_svOk)
		{
			if (p_bForceUpdate ||
				m_bAlwaysUpdate ||
				m_svSelectedOffsetData.m_bAlwaysUpdate ||
				nullptr == m_svSelectedOffsetData.m_psvImage ||
				m_svSelectedOffsetData.m_psvImage != m_pSelectedImage)
			{
				if (nullptr == m_pSelectedImage)
				{
					m_svSelectedOffsetData.Initialize();
				}
				else
				{
					SVExtentOffsetStruct offsetData;

					l_svOk = UpdateOffsetDataToImage(offsetData, m_pSelectedImage);

					if (S_OK == l_svOk)
					{
						m_svSelectedOffsetData = offsetData;
					}
				}
			}
		}
	}
	catch (...)
	{
		l_svOk = S_FALSE;
	}

	return l_svOk;
}

HRESULT SVToolExtentClass::UpdateOffsetData(bool bForceUpdate, SvIe::SVImageClass* pToolImage)
{
	SetToolImage(pToolImage);

	return UpdateOffsetData(bForceUpdate);
}

HRESULT SVToolExtentClass::TranslatePointToSource(SVPoint<double> inPoint, SVPoint<double>& rOutPoint)
{
	HRESULT result{S_OK};

	if (m_svRootOffsetData.m_bIsLinear)
	{
		inPoint = SVRotatePoint(m_svRootOffsetData.m_svRotationPoint, inPoint, m_svRootOffsetData.m_dRotationAngle);

		rOutPoint = m_svRootOffsetData.m_svOffset + inPoint;

		rOutPoint -= m_svRootOffsetData.m_svRotationPoint;
	}
	else
	{
		result = m_rImageExtent.TranslateFromOutputSpace(inPoint, rOutPoint);

		if (S_OK == result)
		{
			try
			{
				if (nullptr != m_pToolImage)
				{
					SvIe::SVImageClass* pImageParent = m_pToolImage->GetParentImage();

					if (nullptr != pImageParent)
					{
						SvTo::SVToolClass* pToolParent = dynamic_cast<SvTo::SVToolClass*>(pImageParent->GetTool());

						if (nullptr != pToolParent)
						{
							result = pToolParent->TranslatePointToSource(inPoint, inPoint);
						}
					}
				}

				if (S_OK == result)
				{
					rOutPoint = inPoint;
				}
			}
			catch (...)
			{
				result = S_FALSE;
			}
		}
	}
	return result;
}

std::string SVToolExtentClass::GetAuxiliaryDrawTypeString() const
{
	std::string Result;

	// SvDef::SVExtentTranslationEnum
	switch (m_eTranslation)
	{
		case SvDef::SVExtentTranslationUnknown:
		{
			Result = _T("Translation Unknown");
			break;
		}
		case SvDef::SVExtentTranslationNone:
		{
			Result = _T("Translation None");
			break;
		}
		case SvDef::SVExtentTranslationShift:
		{
			Result = _T("Translation Shift");
			break;
		}
		case SvDef::SVExtentTranslationRotate:
		{
			Result = _T("Translation Rotate");
			break;
		}
		case SvDef::SVExtentTranslationFlippedRotate:
		{
			Result = _T("Translation Flipped Rotate");
			break;
		}
		case SvDef::SVExtentTranslationProfile:
		{
			Result = _T("Translation Profile");
			break;
		}
		case SvDef::SVExtentTranslationProfileShift:
		{
			Result = _T("Translation Profile Shift");
			break;
		}
		case SvDef::SVExtentTranslationTransformShift:
		{
			Result = _T("Translation Transform Shift");
			break;
		}
		case SvDef::SVExtentTranslationTransformRotate:
		{
			Result = _T("Translation Transform Rotate");
			break;
		}
		case SvDef::SVExtentTranslationPolarUnwrap:
		{
			Result = _T("Translation Polar Unwrap");
			break;
		}
		case SvDef::SVExtentTranslationLine:
		{
			Result = _T("Translation Line");
			break;
		}
		case SvDef::SVExtentTranslationDoubleHeight:
		{
			Result = _T("Translation Double Height");
			break;
		}
		case SvDef::SVExtentTranslationResize:
		{
			Result = _T("Translation Resize");
			break;
		}
		case SvDef::SVExtentTranslationFlipVertical:
		{
			Result = _T("Translation Flip Vertical");
			break;
		}
		case SvDef::SVExtentTranslationFlipHorizontal:
		{
			Result = _T("Translation Flip Horizontal");
			break;
		}
		case SvDef::SVExtentTranslationFigureShift:
		{
			Result = _T("Translation Figure Shift");
			break;
		}
		case SvDef::SVExtentTranslationLinear:
		{
			Result = _T("Translation Linear");
			break;
		}
		case SvDef::SVExtentTranslationCylindricalWarpH:
		{
			Result = _T("Translation Cylindrical Warp Horizontal");
			break;
		}
		case SvDef::SVExtentTranslationCylindricalWarpV:
		{
			Result = _T("Translation Cylindrical Warp Vertical");
			break;
		}
		case SvDef::SVExtentTranslationHorizontalPerspective:
		{
			Result = _T("Translation Horizontal Perspective");
			break;
		}
		case SvDef::SVExtentTranslationVerticalPerspective:
		{
			Result = _T("Translation Vertical Perspective");
			break;
		}
		default:
		{
			Result = _T("Translation Unknown");
			break;
		}
	}
	return Result;
}

HRESULT SVToolExtentClass::GetExtentPropertyInfo(SvDef::SVExtentPropertyEnum p_eProperty, SvIe::SVExtentPropertyInfoStruct& rInfo) const
{
	return m_Properties.GetPropertyInfo(p_eProperty, rInfo);
}

HRESULT SVToolExtentClass::SetExtentPropertyInfo(SvDef::SVExtentPropertyEnum p_eProperty, const SvIe::SVExtentPropertyInfoStruct& rInfo)
{
	return m_Properties.SetPropertyInfo(p_eProperty, rInfo);
}

} //namespace SvIe
