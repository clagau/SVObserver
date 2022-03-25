//******************************************************************************
//* COPYRIGHT (c) 2004 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
#include "ObjectInterfaces/IValueObject.h"
#include "Definitions/SVResetStruct.h"
#include "SVUtilityLibrary/SVUtilityGlobals.h"
#include "SVValueObjectLibrary/LinkedValue.h"
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
m_LinearToolTranslations {SvPb::SVExtentTranslationProfile, SvPb::SVExtentTranslationProfileShift}
;

SVToolExtentClass::SVToolExtentClass() 
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

	m_eTranslation = SvPb::SVExtentTranslationUnknown;
	m_eShape = SvDef::SVExtentShapeUnknown;

	KeepPrevError(l_svOk, m_Properties.Initialize());

	m_bAlwaysUpdate = true;

	m_pSelectedImage = nullptr;

	KeepPrevError(l_svOk, m_svRootOffsetData.Initialize());
	KeepPrevError(l_svOk, m_svSelectedOffsetData.Initialize());
	m_ImageExtent.setIsUpdated(false);
	
	return l_svOk;
}

void SVToolExtentClass::SetToolImage(SvIe::SVImageClass *pToolImage)
{
	if (m_pToolImage != pToolImage)
	{
		m_pToolImage = pToolImage;

		m_svRootOffsetData.Initialize();
		m_svSelectedOffsetData.Initialize();
		m_ImageExtent.setIsUpdated(false);
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
			l_Status = rimageExtent.GetExtentProperty(SvPb::SVExtentPropertyOutputHeight, dHeight);
			KeepPrevError(l_Status, rimageExtent.GetExtentProperty(SvPb::SVExtentPropertyOutputWidth, dWidth));
			KeepPrevError(l_Status, rNewExtent.SetExtentProperty(SvPb::SVExtentPropertyPositionPointX, dPosX));
			KeepPrevError(l_Status, rNewExtent.SetExtentProperty(SvPb::SVExtentPropertyPositionPointY, dPosY));
			KeepPrevError(l_Status, rNewExtent.SetExtentProperty(SvPb::SVExtentPropertyHeight, dHeight));
			KeepPrevError(l_Status, rNewExtent.SetExtentProperty(SvPb::SVExtentPropertyWidth, dWidth));
			KeepPrevError(l_Status, rNewExtent.SetExtentProperty(SvPb::SVExtentPropertyOutputHeight, dHeight));
			KeepPrevError(l_Status, rNewExtent.SetExtentProperty(SvPb::SVExtentPropertyOutputWidth, dWidth));
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
	HRESULT l_Status = E_FAIL;
	if (nullptr != m_pToolImage)
	{
		SvIe::SVImageClass* pParent = m_pToolImage->GetParentImage();
		if (nullptr != pParent)
		{
			rParentExtent = pParent->GetImageExtents();
			l_Status = S_OK;
		}
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

HRESULT SVToolExtentClass::UpdateImageWithExtent(SVToolExtentTypeEnum ToolExtentType)
{
	HRESULT l_Status = S_OK;

	if (nullptr != m_pToolImage)
	{
		SVImageExtentClass imageExtents;

		SvPb::SVImageTypeEnum l_Type = m_pToolImage->GetImageType();

		SvIe::SVImageClass* pParent = m_pToolImage->GetParentImage();

		if (nullptr != pParent)
		{
			if (SvPb::SVImageTypeEnum::SVImageTypeDependent == l_Type)
			{
				imageExtents = pParent->GetImageExtents();

				imageExtents.SetTranslation(GetTranslation());

				imageExtents.SetExtentProperty(SvPb::SVExtentPropertyPositionPoint, SVPoint<double>(0.0, 0.0));

				l_Status = SetImageExtent(imageExtents);
			}
			else
			{
				imageExtents = m_ImageExtent;

				SvOi::IInspectionProcess* pInspection = m_pToolImage->GetInspectionInterface();

				if (nullptr != pInspection && pInspection->IsResetStateSet(SvDef::SVResetAutoMoveAndResize))
				{
					if (imageExtents != m_pToolImage->GetImageExtents())
					{
						if (S_OK != m_pToolImage->ValidateAgainstParentExtents(imageExtents))
						{
							if (m_pTool && m_pTool->GetObjectSubType() == SvPb::SVLinearToolObjectType)
							{
								m_pTool->removeTaskMessages(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RebuildFailed);
							}
							pParent->GetImageExtentsToFit(imageExtents, imageExtents);

							SetImageExtent(imageExtents);
						}
					}
				}

				if (SVTransformationToolExtent == ToolExtentType)
				{
					double l_Value = 0.0;
					const SVImageExtentClass& rInputExtent = pParent->GetImageExtents();

					if (S_OK == rInputExtent.GetExtentProperty(SvPb::SVExtentPropertyOutputWidth, l_Value))
					{
						imageExtents.SetExtentProperty(SvPb::SVExtentPropertyWidth, l_Value);
					}

					if (S_OK == rInputExtent.GetExtentProperty(SvPb::SVExtentPropertyOutputHeight, l_Value))
					{
						imageExtents.SetExtentProperty(SvPb::SVExtentPropertyHeight, l_Value);
					}

					l_Status = SetImageExtent(imageExtents);
				}
			}
		}
		else
		{
			imageExtents = m_ImageExtent;
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

SvPb::SVExtentTranslationEnum SVToolExtentClass::GetTranslation() const
{
	return m_eTranslation;
}

void SVToolExtentClass::SetTranslation(SvPb::SVExtentTranslationEnum eTranslation)
{
	m_eTranslation = eTranslation;
}

HRESULT SVToolExtentClass::SetLinearTranslation(SvPb::SVExtentTranslationEnum eTranslation)
{
	HRESULT l_hrOk = S_FALSE;

	SVTranslationFilterSet::iterator l_OldIter = m_LinearToolTranslations.find(m_eTranslation);
	SVTranslationFilterSet::iterator l_NewIter = m_LinearToolTranslations.find(eTranslation);

	if ((m_eTranslation != eTranslation) && (l_OldIter != m_LinearToolTranslations.end()) && (l_NewIter != m_LinearToolTranslations.end()))
	{
		_variant_t PointY;
		_variant_t Height;

		if ((S_OK == GetExtentValue(SvPb::SVExtentPropertyPositionPointY, PointY)) &&
			(S_OK == GetExtentValue(SvPb::SVExtentPropertyHeight, Height)))
		{
			double NewY = static_cast<double> (PointY);
			double Offset = static_cast<double>(Height) / 2.0;

			NewY += (SvPb::SVExtentTranslationProfileShift == m_eTranslation) ? Offset : -Offset;
			l_hrOk = SetExtentValue(SvPb::SVExtentPropertyPositionPointY, NewY);
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

HRESULT SVToolExtentClass::GetExtentObject(SvPb::SVExtentPropertyEnum extentProperty, SvOi::IValueObject*& rpValueObject) const
{
	return m_Properties.GetExtentObject(extentProperty, rpValueObject);
}

void SVToolExtentClass::SetExtentObject(SvPb::SVExtentPropertyEnum extentProperty, SvOi::IValueObject* pValueObject)
{
	m_Properties.SetExtentObject(extentProperty, pValueObject);
}

HRESULT SVToolExtentClass::GetExtentValue(SvPb::SVExtentPropertyEnum extentProperty, _variant_t& rValue) const
{
	rValue.Clear();
	SvOi::IValueObject* pValueObject(nullptr);
	HRESULT l_hrOk = GetExtentObject(extentProperty, pValueObject);

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

bool SVToolExtentClass::isDirectValue(SvPb::SVExtentPropertyEnum extentProperty) const
{
	SvOi::IValueObject* pValueObject(nullptr);
	HRESULT l_hrOk = GetExtentObject(extentProperty, pValueObject);

	if (l_hrOk == S_OK && nullptr != pValueObject)
	{
		SvVol::LinkedValue* pLinkedObject = dynamic_cast<SvVol::LinkedValue*>(pValueObject);
		if (nullptr == pLinkedObject)
		{
			return true;
		}

		return (pLinkedObject->getSelectedOption() == SvPb::DirectValue);
	}
	return true;
}


HRESULT SVToolExtentClass::SetExtentValue(SvPb::SVExtentPropertyEnum extentProperty, const _variant_t& rValue)
{
	SvOi::IValueObject* pValueObject(nullptr);
	HRESULT l_hrOk = GetExtentObject(extentProperty, pValueObject);

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

HRESULT SVToolExtentClass::updateImageExtent(bool init )
{
	if (init || m_ImageExtent.GetTranslation() != m_eTranslation)
	{
		m_ImageExtent.Initialize();
		m_ImageExtent.SetTranslation(m_eTranslation);
	}
	
	
	HRESULT result = m_Properties.GetProperties(m_ImageExtent);
	if (S_OK == result)
	{
		if (m_ImageExtent.getIsUpdated()== false)
		{
			
			result = m_ImageExtent.UpdateData();
		}
	}
	else
	{
		m_ImageExtent.Initialize();
	}

	return result;
}


HRESULT SVToolExtentClass::ensureValidScaleFactorIfDirectValue(const SVImageExtentClass& rImageExtent, SvPb::SVExtentPropertyEnum extentProperty)
{
	double dValue = 0.0;

	HRESULT hrOk = rImageExtent.GetExtentProperty(extentProperty, dValue);

	if (S_OK != hrOk)
	{
		return hrOk;
	}
	if (false == isDirectValue(extentProperty))
	{
		return hrOk; // An indirect value must not be overwritten with a numeric value - regardless of whether it currently is invalid!
	}
	if (SvTo::isValidScaleFactor(dValue))
	{
		return SetExtentValue(extentProperty, dValue);
	}
	else
	{
		_variant_t value{0.0};
		GetExtentValue(extentProperty, value);
			
		if (false == SvTo::isValidScaleFactor(value))
		{
			return SetExtentValue(extentProperty, SvDef::cDefaultScaleFactor);
		}
	}

	return hrOk;
}

HRESULT SVToolExtentClass::SetImageExtent(const SVImageExtentClass& rImageExtent) 
{
	HRESULT l_hrOk = S_OK;

	double dValue = 0.0;

	m_ImageExtent = rImageExtent;

	SvPb::SVExtentTranslationEnum translation = rImageExtent.GetTranslation();

	if (SvPb::SVExtentTranslationUnknown != translation)
	{
		m_eTranslation = translation;
	}

	for (auto extentProperty :
			{
				SvPb::SVExtentPropertyPositionPointX,
				SvPb::SVExtentPropertyPositionPointY,
				SvPb::SVExtentPropertyPositionPointEndOfLineX,
				SvPb::SVExtentPropertyPositionPointEndOfLineY,
				SvPb::SVExtentPropertyRotationAngle,
				SvPb::SVExtentPropertyTranslationOffsetX,
				SvPb::SVExtentPropertyTranslationOffsetY,
				SvPb::SVExtentPropertyHeight,
				SvPb::SVExtentPropertyWidth,
				SvPb::SVExtentPropertyStartAngle,
				SvPb::SVExtentPropertyEndAngle,
				SvPb::SVExtentPropertyInnerRadius,
				SvPb::SVExtentPropertyOuterRadius
			}
		)
	{
		if (S_OK == rImageExtent.GetExtentProperty(extentProperty, dValue))
			l_hrOk &= SetExtentValue(extentProperty, dValue);
	}

	for (auto extentProperty :
			{
				SvPb::SVExtentPropertyHeightFactorContent,
				SvPb::SVExtentPropertyWidthFactorContent,
				SvPb::SVExtentPropertyHeightFactorFormat,
				SvPb::SVExtentPropertyWidthFactorFormat
			}
		)
	{
		// cppcheck-suppress useStlAlgorithm
		l_hrOk &= ensureValidScaleFactorIfDirectValue(rImageExtent, extentProperty);
	}


	return l_hrOk;
}

HRESULT SVToolExtentClass::GetRootOffsetData(SVExtentOffsetStruct& rsvOffsetData)
{
	HRESULT l_svOk = S_FALSE;

	if (nullptr == m_svRootOffsetData.m_psvImage)
	{
		l_svOk = UpdateOffsetDataToImage(m_svRootOffsetData, nullptr);
	}

	rsvOffsetData = m_svRootOffsetData;

	if (nullptr != rsvOffsetData.m_psvImage)
	{
		l_svOk = S_OK;
	}
	return l_svOk;
}

HRESULT SVToolExtentClass::GetSelectedOffsetData(SVExtentOffsetStruct& rsvOffsetData)
{
	HRESULT l_svOk = S_FALSE;

	
	if (nullptr == m_pSelectedImage )
	{
		rsvOffsetData = m_svRootOffsetData;
	}
	else
	{
		rsvOffsetData = m_svSelectedOffsetData;
	}

	if (nullptr != rsvOffsetData.m_psvImage)
	{
		l_svOk = S_OK;
	}
	return l_svOk;
}

HRESULT SVToolExtentClass::UpdateOffsetDataToImage(SVExtentOffsetStruct& rOffsetData, SvIe::SVImageClass* pAuxRefImage)
{
	if (nullptr == m_pToolImage)
	{
		m_CircularReference = false;
		return E_FAIL;
	}



	SVExtentOffsetStruct offsetData;
	SvIe::SVImageClass* pImageParent = m_pToolImage->GetParentImage();
	if (nullptr == pImageParent)
	{
		rOffsetData = offsetData;
		m_CircularReference = false;
		return S_OK;
	}
	HRESULT l_svOk{S_OK};
	if (m_CircularReference)
	{
		m_CircularReference = false;
		return E_FAIL;
	}
	
	m_CircularReference = true;
	offsetData.m_dRotationAngle = rOffsetData.m_dRotationAngle;
	offsetData.m_svRotationPoint = rOffsetData.m_svRotationPoint;;
	offsetData.m_svOffset = rOffsetData.m_svOffset;

	
	SvTo::SVToolClass* pToolParent = dynamic_cast<SvTo::SVToolClass*>(pImageParent->GetTool());

	if (pImageParent != pAuxRefImage && pImageParent != m_pToolImage &&
		nullptr != pToolParent && pToolParent != m_pTool)
	{
		/*l_svOk =*/ m_ImageExtent.UpdateSourceOffset(offsetData);
		l_svOk = pToolParent->UpdateOffsetDataToImage(offsetData, pAuxRefImage);
	}
	else if (pImageParent == pAuxRefImage || nullptr == pAuxRefImage)
	{
		l_svOk = m_ImageExtent.UpdateSourceOffset(offsetData);
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
			
			offsetData.m_bAlwaysUpdate |= m_bAlwaysUpdate;
			if (pAuxRefImage==0 || pAuxRefImage == pImageParent)
			{
				offsetData.m_psvImage = pImageParent;
				if (nullptr != pImageParent)
				{
					offsetData.m_csImageName = pImageParent->GetCompleteName();
				}
			}
		}
	}

	rOffsetData = offsetData;
	m_CircularReference = false;

	return l_svOk;
}

HRESULT SVToolExtentClass::UpdateOffsetData(bool bForceUpdate)
{
	HRESULT l_svOk = S_OK;

	try
	{
		if (bForceUpdate ||
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
					l_svOk = m_ImageExtent.UpdateSourceOffset(offsetData);
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
			if (bForceUpdate ||
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
	HRESULT result {S_OK};

	if (m_svRootOffsetData.m_bIsLinear)
	{
		inPoint = SVRotatePoint(m_svRootOffsetData.m_svRotationPoint, inPoint, m_svRootOffsetData.m_dRotationAngle);

		rOutPoint = m_svRootOffsetData.m_svOffset + inPoint;

		rOutPoint -= m_svRootOffsetData.m_svRotationPoint;
	}
	else
	{
		result = m_ImageExtent.TranslateFromOutputSpace(inPoint, rOutPoint);

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

	// SvPb::SVExtentTranslationEnum
	switch (m_eTranslation)
	{
		case SvPb::SVExtentTranslationUnknown:
		{
			Result = _T("Translation Unknown");
			break;
		}
		case SvPb::SVExtentTranslationNone:
		{
			Result = _T("Translation None");
			break;
		}
		case SvPb::SVExtentTranslationShift:
		{
			Result = _T("Translation Shift");
			break;
		}
		case SvPb::SVExtentTranslationRotate:
		{
			Result = _T("Translation Rotate");
			break;
		}
		case SvPb::SVExtentTranslationFlippedRotate:
		{
			Result = _T("Translation Flipped Rotate");
			break;
		}
		case SvPb::SVExtentTranslationProfile:
		{
			Result = _T("Translation Profile");
			break;
		}
		case SvPb::SVExtentTranslationProfileShift:
		{
			Result = _T("Translation Profile Shift");
			break;
		}
		case SvPb::SVExtentTranslationTransformShift:
		{
			Result = _T("Translation Transform Shift");
			break;
		}
		case SvPb::SVExtentTranslationTransformRotate:
		{
			Result = _T("Translation Transform Rotate");
			break;
		}
		case SvPb::SVExtentTranslationPolarUnwrap:
		{
			Result = _T("Translation Polar Unwrap");
			break;
		}
		case SvPb::SVExtentTranslationLine:
		{
			Result = _T("Translation Line");
			break;
		}
		case SvPb::SVExtentTranslationDoubleHeight:
		{
			Result = _T("Translation Double Height");
			break;
		}
		case SvPb::SVExtentTranslationResize:
		{
			Result = _T("Translation Resize");
			break;
		}
		case SvPb::SVExtentTranslationFlipVertical:
		{
			Result = _T("Translation Flip Vertical");
			break;
		}
		case SvPb::SVExtentTranslationFlipHorizontal:
		{
			Result = _T("Translation Flip Horizontal");
			break;
		}
		case SvPb::SVExtentTranslationFigureShift:
		{
			Result = _T("Translation Figure Shift");
			break;
		}
		case SvPb::SVExtentTranslationLinear:
		{
			Result = _T("Translation Linear");
			break;
		}
		case SvPb::SVExtentTranslationCylindricalWarpH:
		{
			Result = _T("Translation Cylindrical Warp Horizontal");
			break;
		}
		case SvPb::SVExtentTranslationCylindricalWarpV:
		{
			Result = _T("Translation Cylindrical Warp Vertical");
			break;
		}
		case SvPb::SVExtentTranslationHorizontalPerspective:
		{
			Result = _T("Translation Horizontal Perspective");
			break;
		}
		case SvPb::SVExtentTranslationVerticalPerspective:
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

HRESULT SVToolExtentClass::GetExtentPropertyInfo(SvPb::SVExtentPropertyEnum extentProperty, SvIe::SVExtentPropertyInfoStruct& rInfo) const
{
	return m_Properties.GetPropertyInfo(extentProperty, rInfo);
}

HRESULT SVToolExtentClass::SetExtentPropertyInfo(SvPb::SVExtentPropertyEnum extentProperty, const SvIe::SVExtentPropertyInfoStruct& rInfo)
{
	return m_Properties.SetPropertyInfo(extentProperty, rInfo);
}

void SVToolExtentClass::getExtentProperties(::google::protobuf::RepeatedPtrField< ::SvPb::ExtentParameter >& rExtentProperties) const
{
	m_ImageExtent.getExtentProperties(rExtentProperties);
	for (auto& rExtent : rExtentProperties)
	{
		SvIe::SVExtentPropertyInfoStruct info;
		HRESULT hr = GetExtentPropertyInfo(rExtent.type(), info);
		if (S_OK == hr)
		{
			rExtent.set_issetbyreset(info.bSetByReset);
			if (!rExtent.filteredoutflag()) //if flag already set, do not check to set it again
			{
				if (SvPb::SVExtentPropertyRotationAngle != rExtent.type())
				{
					rExtent.set_filteredoutflag(info.bFormula || info.bHidden || rExtent.name().empty());
				}
				else
				{
					SvIe::SVExtentPropertyInfoStruct infoStart;
					SvIe::SVExtentPropertyInfoStruct infoEnd;
					GetExtentPropertyInfo(SvPb::SVExtentPropertyEndAngle, infoEnd);
					GetExtentPropertyInfo(SvPb::SVExtentPropertyStartAngle, infoStart);
					if (infoStart.bFormula || infoEnd.bFormula)
					{
						rExtent.set_filteredoutflag(true);
					}
					else
					{
						rExtent.set_filteredoutflag(info.bFormula || info.bHidden);
					}
				}
			}
		}
		else
		{
			rExtent.set_filteredoutflag(true);
		}
	}
}

} //namespace SvTo
