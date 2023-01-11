//******************************************************************************
//* COPYRIGHT (c) 2004 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include <corecrt_math_defines.h>
#include "SVUtilityLibrary/GeoHelper.h"
#pragma endregion Includes


namespace SvTo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations



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

	SvUl::KeepPrevError(l_svOk, m_Properties.Initialize());

	m_bAlwaysUpdate = true;

	m_pSelectedImage = nullptr;

	SvUl::KeepPrevError(l_svOk, m_svRootOffsetData.Initialize());
	SvUl::KeepPrevError(l_svOk, m_svSelectedOffsetData.Initialize());
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
			SvUl::KeepPrevError(l_Status, rimageExtent.GetExtentProperty(SvPb::SVExtentPropertyOutputWidth, dWidth));
			SvUl::KeepPrevError(l_Status, rNewExtent.SetExtentProperty(SvPb::SVExtentPropertyPositionPointX, dPosX));
			SvUl::KeepPrevError(l_Status, rNewExtent.SetExtentProperty(SvPb::SVExtentPropertyPositionPointY, dPosY));
			SvUl::KeepPrevError(l_Status, rNewExtent.SetExtentProperty(SvPb::SVExtentPropertyHeight, dHeight));
			SvUl::KeepPrevError(l_Status, rNewExtent.SetExtentProperty(SvPb::SVExtentPropertyWidth, dWidth));
			SvUl::KeepPrevError(l_Status, rNewExtent.SetExtentProperty(SvPb::SVExtentPropertyOutputHeight, dHeight));
			SvUl::KeepPrevError(l_Status, rNewExtent.SetExtentProperty(SvPb::SVExtentPropertyOutputWidth, dWidth));
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

				l_Status = SetImageExtentAndProperty(imageExtents);
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

							SetImageExtentAndProperty(imageExtents);
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

					l_Status = SetImageExtentAndProperty(imageExtents);
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
	Log_Assert(m_ImageExtent.GetTranslation() == m_eTranslation);
	return m_eTranslation;
}

void SVToolExtentClass::SetTranslation(SvPb::SVExtentTranslationEnum eTranslation)
{
	m_eTranslation = eTranslation;
}

HRESULT SVToolExtentClass::SetLinearTranslation(SvPb::SVExtentTranslationEnum eTranslation)
{
	HRESULT l_hrOk = S_FALSE;

	auto isLt = [](SvPb::SVExtentTranslationEnum et){ return (et == SvPb::SVExtentTranslationProfile) || (et == SvPb::SVExtentTranslationProfileShift); };
	
	if ((m_eTranslation != eTranslation) && isLt(m_eTranslation) && isLt(eTranslation))
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
			
			result = m_ImageExtent.UpdateDataRecalculateOutput();
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

HRESULT SVToolExtentClass::SetImageExtentAndProperty(const SVImageExtentClass& rImageExtent) 
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
		inPoint = SvUl::SVRotatePoint(m_svRootOffsetData.m_svRotationPoint, inPoint, m_svRootOffsetData.m_dRotationAngle);

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

SvPb::SVExtentLocationPropertyEnum SVToolExtentClass::GetLocationPropertyAt(const SVPoint<double>& rPoint) const
{
	const SVImageExtentClass* pEx = &(m_ImageExtent);

	Log_Assert(pEx->GetTranslation() == m_eTranslation);

	SvPb::SVExtentLocationPropertyEnum l_eLocation = SvPb::SVExtentLocationPropertyUnknown;

	SVPoint<double> outputPoint;

	RECT l_oRect {0L, 0L, 0L, 0L};
	bool l_bValid = false;

	if (m_eTranslation == SvPb::SVExtentTranslationPolarUnwrap)
	{
		double dValue;
		pEx->GetExtentProperty(SvPb::SVExtentPropertyPositionPointX, dValue);
		l_oRect.left = static_cast<long>(dValue - 2);
		l_oRect.right = static_cast<long>(dValue + 2);
		pEx->GetExtentProperty(SvPb::SVExtentPropertyPositionPointY, dValue);
		l_oRect.top = static_cast<long>(dValue - 2);
		l_oRect.bottom = static_cast<long>(dValue + 2);
		if (::PtInRect(&l_oRect, static_cast<POINT> (rPoint)))
		{
			l_eLocation = SvPb::SVExtentLocationPropertyCenter;
		}
	}

	switch (m_eTranslation)
	{
		case SvPb::SVExtentTranslationResize:
		case SvPb::SVExtentTranslationDoubleHeight:
		case SvPb::SVExtentTranslationFlipHorizontal:
		case SvPb::SVExtentTranslationFlipVertical:
		case SvPb::SVExtentTranslationFigureShift:
		{
			outputPoint = rPoint;
			l_bValid = S_OK == pEx->GetRectangle(l_oRect);
			break;
		}

		default:
		{
			l_bValid = S_OK == TranslateToOutputSpace(rPoint, outputPoint) &&
				S_OK == pEx->GetOutputRectangle(l_oRect);
		}
	}

	if (l_bValid)
	{
		RECT l_oTemp = l_oRect;

		// Check special case for Polar Unwrap tool when > 360
		if (l_eLocation == SvPb::SVExtentLocationPropertyUnknown && m_eTranslation == SvPb::SVExtentTranslationPolarUnwrap)
		{
			double dStartAngle {0.0};
			double dEndAngle {0.0};
			double dOuterRadius {0.0};
			double dInnerRadius {0.0};

			if (S_OK == pEx->GetExtentProperty(SvPb::SVExtentPropertyStartAngle, dStartAngle) &&
				S_OK == pEx->GetExtentProperty(SvPb::SVExtentPropertyEndAngle, dEndAngle) &&
				S_OK == pEx->GetExtentProperty(SvPb::SVExtentPropertyInnerRadius, dInnerRadius) &&
				dEndAngle - dStartAngle > 360)
			{
				pEx->GetExtentProperty(SvPb::SVExtentPropertyOuterRadius, dOuterRadius);
				double l_dCircum = 2.0 * M_PI * dOuterRadius;
				if (l_oRect.right > l_dCircum && l_oRect.right > 4)
				{
					l_oTemp.right = l_oRect.right - (long)l_dCircum;
					l_oTemp.left = l_oTemp.right;
					l_oTemp.top = (long)-dInnerRadius;
					if (::InflateRect(&l_oTemp, 2, -2) && ::PtInRect(&l_oTemp, static_cast<POINT> (outputPoint)))
					{
						l_eLocation = SvPb::SVExtentLocationPropertyRight;
					}
				}
			}
		}


		long l_lInflateX = -2;
		long l_lInflateY = -2;

		if (l_eLocation == SvPb::SVExtentLocationPropertyUnknown)
		{
			l_oTemp = l_oRect;

			if (l_oRect.bottom - 4 < 9)
			{
				l_lInflateY = 1;
			}
			// cppcheck-suppress knownConditionTrueFalse 
			if (l_oRect.right - 4 < 9)
			{
				l_lInflateX = 1;
			}

			if (::InflateRect(&l_oTemp, l_lInflateX, l_lInflateY) && ::PtInRect(&l_oTemp, static_cast<POINT> (outputPoint)))
			{
				if (m_eTranslation == SvPb::SVExtentTranslationRotate ||
					m_eTranslation == SvPb::SVExtentTranslationFlippedRotate ||
					m_eTranslation == SvPb::SVExtentTranslationProfile)
				{
					l_oTemp = l_oRect;

					if (l_lInflateY < 1)
					{
						l_lInflateY = 2;
					}

					if (l_oRect.right < 1)
					{
						l_lInflateX = 0;

						l_oTemp.left = l_oRect.right + 1;
						l_oTemp.right = l_oRect.right + 1;
					}
					else if (l_oRect.right < 9)
					{
						l_lInflateX = 1;

						l_oTemp.left = l_oRect.right;
						l_oTemp.right = l_oRect.right;
					}
					else if (l_oRect.right < 16)
					{
						l_lInflateX = 1;

						l_oTemp.left = l_oRect.right - 2;
						l_oTemp.right = l_oRect.right - 2;
					}
					else
					{
						l_lInflateX = 2;
						//Use a quarter of the width for the rotating pick area
						l_oTemp.left = l_oRect.right - (abs(l_oRect.right - l_oRect.left) / 4);
						l_oTemp.right = l_oRect.right - 5;
					}

					l_oTemp.top = l_oRect.bottom / 2;
					l_oTemp.bottom = l_oRect.bottom / 2;

					if (::InflateRect(&l_oTemp, l_lInflateX, l_lInflateY) && ::PtInRect(&l_oTemp, static_cast<POINT> (outputPoint)))
					{
						l_eLocation = SvPb::SVExtentLocationPropertyRotate;
					}
				}

				if (l_eLocation == SvPb::SVExtentLocationPropertyUnknown)
				{
					l_eLocation = SvPb::SVExtentLocationPropertyCenter;
				}
			}
		}

		if (m_eTranslation != SvPb::SVExtentTranslationLine &&
			l_eLocation == SvPb::SVExtentLocationPropertyUnknown)
		{
			l_oTemp = l_oRect;

			l_oTemp.bottom = l_oRect.top;

			l_lInflateX = -2;

			if (l_oRect.right < 9)
			{
				l_oTemp.right = l_oRect.left;

				l_lInflateX = 2;
			}

			if (::InflateRect(&l_oTemp, l_lInflateX, 2) && ::PtInRect(&l_oTemp, static_cast<POINT> (outputPoint)))
			{
				l_eLocation = SvPb::SVExtentLocationPropertyTop;
			}
		}

		if (l_eLocation == SvPb::SVExtentLocationPropertyUnknown)
		{
			l_oTemp = l_oRect;

			l_oTemp.left = l_oRect.right;

			l_lInflateY = -2;

			if (l_oRect.bottom < 9)
			{
				l_oTemp.bottom = l_oRect.top;

				l_lInflateY = 2;
			}

			if (::InflateRect(&l_oTemp, 2, l_lInflateY) && ::PtInRect(&l_oTemp, static_cast<POINT> (outputPoint)))
			{
				l_eLocation = SvPb::SVExtentLocationPropertyRight;
			}
		}

		if (m_eTranslation != SvPb::SVExtentTranslationLine &&
			l_eLocation == SvPb::SVExtentLocationPropertyUnknown)
		{
			l_oTemp = l_oRect;

			l_oTemp.top = l_oRect.bottom;

			l_lInflateX = -2;

			if (l_oRect.right < 9)
			{
				l_oTemp.bottom = l_oRect.top;

				l_lInflateX = 2;
			}

			if (::InflateRect(&l_oTemp, l_lInflateX, 2) && ::PtInRect(&l_oTemp, static_cast<POINT> (outputPoint)))
			{
				l_eLocation = SvPb::SVExtentLocationPropertyBottom;
			}
		}

		if (m_eTranslation != SvPb::SVExtentTranslationProfile &&
			l_eLocation == SvPb::SVExtentLocationPropertyUnknown)
		{
			l_oTemp = l_oRect;

			l_oTemp.right = l_oRect.left;

			l_lInflateY = -2;

			if (l_oRect.bottom < 9)
			{
				l_oTemp.bottom = l_oRect.top;

				l_lInflateY = 2;
			}

			if (::InflateRect(&l_oTemp, 2, l_lInflateY) && ::PtInRect(&l_oTemp, static_cast<POINT> (outputPoint)))
			{
				l_eLocation = SvPb::SVExtentLocationPropertyLeft;
			}
		}

		if (m_eTranslation != SvPb::SVExtentTranslationProfile &&
			m_eTranslation != SvPb::SVExtentTranslationLine &&
			l_eLocation == SvPb::SVExtentLocationPropertyUnknown)
		{
			l_oTemp = l_oRect;

			l_oTemp.right = l_oRect.left;
			l_oTemp.bottom = l_oRect.top;

			if (::InflateRect(&l_oTemp, 2, 2) && ::PtInRect(&l_oTemp, static_cast<POINT> (outputPoint)))
			{
				l_eLocation = SvPb::SVExtentLocationPropertyTopLeft;
			}
		}

		if (m_eTranslation != SvPb::SVExtentTranslationLine &&
			l_eLocation == SvPb::SVExtentLocationPropertyUnknown)
		{
			l_oTemp = l_oRect;

			l_oTemp.left = l_oRect.right;
			l_oTemp.bottom = l_oRect.top;

			if (::InflateRect(&l_oTemp, 2, 2) && ::PtInRect(&l_oTemp, static_cast<POINT> (outputPoint)))
			{
				l_eLocation = SvPb::SVExtentLocationPropertyTopRight;
			}
		}

		if (m_eTranslation != SvPb::SVExtentTranslationProfile &&
			m_eTranslation != SvPb::SVExtentTranslationLine &&
			l_eLocation == SvPb::SVExtentLocationPropertyUnknown)
		{
			l_oTemp = l_oRect;

			l_oTemp.right = l_oRect.left;
			l_oTemp.top = l_oRect.bottom;

			if (::InflateRect(&l_oTemp, 2, 2) && ::PtInRect(&l_oTemp, static_cast<POINT> (outputPoint)))
			{
				l_eLocation = SvPb::SVExtentLocationPropertyBottomLeft;
			}
		}

		if (m_eTranslation != SvPb::SVExtentTranslationLine &&
			l_eLocation == SvPb::SVExtentLocationPropertyUnknown)
		{
			l_oTemp = l_oRect;

			l_oTemp.left = l_oRect.right;
			l_oTemp.top = l_oRect.bottom;

			if (::InflateRect(&l_oTemp, 2, 2) && ::PtInRect(&l_oTemp, static_cast<POINT> (outputPoint)))
			{
				l_eLocation = SvPb::SVExtentLocationPropertyBottomRight;
			}
		}

		if (m_eTranslation == SvPb::SVExtentTranslationPolarUnwrap)
		{
			long l_lInnerRadius = 0;

			if (S_OK == pEx->GetExtentProperty(SvPb::SVExtentPropertyInnerRadius, l_lInnerRadius))
			{
				if (l_eLocation == SvPb::SVExtentLocationPropertyUnknown)
				{
					if (0 < l_oRect.right - 4 && 0 < l_lInnerRadius - 4)
					{
						l_oTemp = l_oRect;
						l_oTemp.bottom = 0;
						l_oTemp.top = -l_lInnerRadius;

						if (::InflateRect(&l_oTemp, -2, -2) && ::PtInRect(&l_oTemp, static_cast<POINT> (outputPoint)))
						{
							l_eLocation = SvPb::SVExtentLocationPropertyCenter;
						}
					}
				}

				if (l_eLocation == SvPb::SVExtentLocationPropertyUnknown)
				{
					l_oTemp = l_oRect;

					l_oTemp.bottom = -l_lInnerRadius;
					l_oTemp.top = -l_lInnerRadius;

					if (::InflateRect(&l_oTemp, 2, 2) && ::PtInRect(&l_oTemp, static_cast<POINT> (outputPoint)))
					{
						l_eLocation = SvPb::SVExtentLocationPropertyCenter;
					}
				}

				if (l_eLocation == SvPb::SVExtentLocationPropertyUnknown)
				{
					l_oTemp = l_oRect;

					l_oTemp.right = l_oRect.left;
					l_oTemp.bottom = 0;
					l_oTemp.top = -l_lInnerRadius;

					if (::InflateRect(&l_oTemp, 2, 0) && ::PtInRect(&l_oTemp, static_cast<POINT> (outputPoint)))
					{
						l_eLocation = SvPb::SVExtentLocationPropertyLeft;
					}
				}

				if (l_eLocation == SvPb::SVExtentLocationPropertyUnknown)
				{
					l_oTemp = l_oRect;

					l_oTemp.left = l_oRect.right;
					l_oTemp.bottom = 0;
					l_oTemp.top = -l_lInnerRadius;

					if (::InflateRect(&l_oTemp, 2, 0) && ::PtInRect(&l_oTemp, static_cast<POINT> (outputPoint)))
					{
						l_eLocation = SvPb::SVExtentLocationPropertyRight;
					}
				}
			}

			if (l_eLocation == SvPb::SVExtentLocationPropertyTop ||
				l_eLocation == SvPb::SVExtentLocationPropertyBottom)
			{
				double dOuterRadius {0.0};
				double dInnerRadius {0.0};
				if (S_OK == pEx->GetExtentProperty(SvPb::SVExtentPropertyInnerRadius, dInnerRadius) &&
					S_OK == pEx->GetExtentProperty(SvPb::SVExtentPropertyOuterRadius, dOuterRadius))
				{
					if (dOuterRadius < dInnerRadius)
					{
						if (l_eLocation == SvPb::SVExtentLocationPropertyTop)
						{
							l_eLocation = SvPb::SVExtentLocationPropertyBottom;
						}
						else
						{
							l_eLocation = SvPb::SVExtentLocationPropertyTop;
						}
					}
				}
			}
		}
		if (m_eTranslation == SvPb::SVExtentTranslationVerticalPerspective)
		{
			if (l_eLocation == SvPb::SVExtentLocationPropertyTopRight ||
				l_eLocation == SvPb::SVExtentLocationPropertyTopLeft)
			{
				l_eLocation = SvPb::SVExtentLocationPropertyTop;
			}
			if (l_eLocation == SvPb::SVExtentLocationPropertyBottomRight ||
				l_eLocation == SvPb::SVExtentLocationPropertyBottomLeft)
			{
				l_eLocation = SvPb::SVExtentLocationPropertyBottom;
			}
			if (l_eLocation == SvPb::SVExtentLocationPropertyCenter)
			{
				l_eLocation = SvPb::SVExtentLocationPropertyUnknown;
			}
		}
		if (m_eTranslation == SvPb::SVExtentTranslationHorizontalPerspective)
		{
			if (l_eLocation == SvPb::SVExtentLocationPropertyTopRight ||
				l_eLocation == SvPb::SVExtentLocationPropertyBottomRight)
			{
				l_eLocation = SvPb::SVExtentLocationPropertyRight;
			}
			if (l_eLocation == SvPb::SVExtentLocationPropertyTopLeft ||
				l_eLocation == SvPb::SVExtentLocationPropertyBottomLeft)
			{
				l_eLocation = SvPb::SVExtentLocationPropertyLeft;
			}
			if (l_eLocation == SvPb::SVExtentLocationPropertyCenter)
			{
				l_eLocation = SvPb::SVExtentLocationPropertyUnknown;
			}
		}
	}

	return l_eLocation;
}

HRESULT SVToolExtentClass::UpdateDraggingROI(SvPb::SVExtentLocationPropertyEnum eLocation, const SVPoint<double>& rStartPoint, const SVPoint<double>& rEndPoint)
{
	 SVImageExtentClass* pEx = &(m_ImageExtent);

	Log_Assert(pEx->GetTranslation() == m_eTranslation);

	HRESULT l_hrOk = S_FALSE;

	if (m_eTranslation == SvPb::SVExtentTranslationPolarUnwrap)
	{
		l_hrOk = UpdatePolar(eLocation, rStartPoint, rEndPoint);
	}
	else if (m_eTranslation == SvPb::SVExtentTranslationLine)
	{
		l_hrOk = UpdateLine(eLocation, rStartPoint, rEndPoint);
	}
	else if (m_eTranslation == SvPb::SVExtentTranslationVerticalPerspective)
	{
		l_hrOk = UpdateVerticalPerspective(eLocation, rStartPoint, rEndPoint);
	}
	else if (m_eTranslation == SvPb::SVExtentTranslationHorizontalPerspective)
	{
		l_hrOk = UpdateHorizontalPerspective(eLocation, rStartPoint, rEndPoint);
	}
	else
	{
		SVPoint<double> outputStart;
		SVPoint<double> outputEnd;

		RECT l_oRect {0L, 0L, 0L, 0L};

		bool l_bValid = false;
		switch (m_eTranslation)
		{
			case SvPb::SVExtentTranslationResize:
			{
				//@WARNING [Jim][8 July 2015] No identification of error cases.
				// GetExtentProperty () only l_bValid is identified, which is 
				// not very helpful. Currently none of these cases give much 
				// help with error identification.

				// Get mouse start position relative to input image ROI.
				l_bValid = (S_OK == TranslateToLocalSpace(rStartPoint, outputStart)) &&
					// Get mouse end/current position relative to input image ROI.
					(S_OK == TranslateToLocalSpace(rEndPoint, outputEnd)) &&
					// Get ROI rectangle.								
					(S_OK == pEx->GetRectangle(l_oRect));


				if (l_bValid)
				{
					if (eLocation == SvPb::SVExtentLocationPropertyBottomLeft)
					{
						outputEnd.m_y = rEndPoint.m_y;
					}
					else if (eLocation == SvPb::SVExtentLocationPropertyTopRight)
					{
						outputEnd.m_x = rEndPoint.m_x;
					}
					else if (eLocation != SvPb::SVExtentLocationPropertyTop &&
						eLocation != SvPb::SVExtentLocationPropertyLeft &&
						eLocation != SvPb::SVExtentLocationPropertyTopLeft)
					{
						outputStart = rStartPoint;
						outputEnd = rEndPoint;
					}
				}
				break;
			}
			// Note: These cases do not work correctly (cannot drag the left or top using the mouse in the gui), 
			// a possible solution might be to use the same logic as the case above (SvDef::SVExtentTranslationResize)
			// however for these cases the position (not on the GUI) is always fixed to 0,0, so that might not 
			// work. 
			case SvPb::SVExtentTranslationDoubleHeight:
			case SvPb::SVExtentTranslationFlipHorizontal:
			case SvPb::SVExtentTranslationFlipVertical:
			case SvPb::SVExtentTranslationFigureShift:
			{
				outputStart = rStartPoint;
				outputEnd = rEndPoint;
				l_bValid = S_OK == pEx->GetRectangle(l_oRect);
				break;
			}

			default:
			{
				l_bValid = S_OK == TranslateToOutputSpace(rStartPoint, outputStart) &&
					S_OK == TranslateToOutputSpace(rEndPoint, outputEnd) &&
					S_OK == pEx->GetOutputRectangle(l_oRect);
			}
		}

		if (l_bValid)
		{
			switch (eLocation)
			{
				case SvPb::SVExtentLocationPropertyTopLeft:
				{
					l_hrOk = UpdateFromOutputSpace(eLocation, static_cast<long> (outputEnd.m_x), static_cast<long> (outputEnd.m_y));

					break;
				}
				case SvPb::SVExtentLocationPropertyTopRight:
				{
					l_hrOk = UpdateFromOutputSpace(eLocation, static_cast<long> (outputEnd.m_x - l_oRect.right), static_cast<long> (outputEnd.m_y));

					break;
				}
				case SvPb::SVExtentLocationPropertyBottomRight:
				{
					l_hrOk = UpdateFromOutputSpace(eLocation, static_cast<long> (outputEnd.m_x - l_oRect.right), static_cast<long> (outputEnd.m_y - l_oRect.bottom));

					break;
				}
				case SvPb::SVExtentLocationPropertyBottomLeft:
				{
					l_hrOk = UpdateFromOutputSpace(eLocation, static_cast<long> (outputEnd.m_x), static_cast<long> (outputEnd.m_y - l_oRect.bottom));

					break;
				}
				case SvPb::SVExtentLocationPropertyLeft:
				{
					l_hrOk = UpdateFromOutputSpace(eLocation, static_cast<long> (outputEnd.m_x), static_cast<long> (outputEnd.m_y));

					break;
				}
				case SvPb::SVExtentLocationPropertyRight:
				{
					l_hrOk = UpdateFromOutputSpace(eLocation, static_cast<long> (outputEnd.m_x - l_oRect.right), static_cast<long> (outputEnd.m_y));

					break;
				}
				case SvPb::SVExtentLocationPropertyTop:
				{
					l_hrOk = UpdateFromOutputSpace(eLocation, static_cast<long> (outputEnd.m_x), static_cast<long> (outputEnd.m_y));

					break;
				}
				case SvPb::SVExtentLocationPropertyBottom:
				{
					l_hrOk = UpdateFromOutputSpace(eLocation, static_cast<long> (outputEnd.m_x), static_cast<long> (outputEnd.m_y - l_oRect.bottom));

					break;
				}
				case SvPb::SVExtentLocationPropertyCenter:
				{
					long l_lX = static_cast<long> (rEndPoint.m_x - rStartPoint.m_x);
					long l_lY = static_cast<long> (rEndPoint.m_y - rStartPoint.m_y);

					l_hrOk = UpdateFromOutputSpace(eLocation, l_lX, l_lY);

					break;
				}
				case SvPb::SVExtentLocationPropertyRotate:
				{
					SVPoint<double> position;

					if (S_OK == GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, position))
					{
						double l_dAngle = SvUl::SVGetRotationAngle(position, rEndPoint);

						if (S_OK == SetExtentProperty(SvPb::SVExtentPropertyRotationAngle, l_dAngle))
						{
							l_hrOk = pEx->UpdateDataRecalculateOutput();
						}
						else
						{
							l_hrOk = S_FALSE;
						}
					}

					break;
				}
			}
		}
	}

	return l_hrOk;
}


HRESULT  SVToolExtentClass::UpdateVerticalPerspective(SvPb::SVExtentLocationPropertyEnum eLocation, const SVPoint<double>& rStart, const SVPoint<double>& rEnd)
{
	
	SVImageExtentClass* pEx = &(m_ImageExtent);

	Log_Assert(pEx->GetTranslation() == m_eTranslation);

	HRESULT l_hrOk = S_FALSE;

	SVPoint<double> position;

	double l_dY = rStart.m_y - rEnd.m_y;

	if (S_OK == GetExtentProperty(SvPb::SVExtentPropertyTranslationOffset, position))
	{
		switch (eLocation)
		{
			case SvPb::SVExtentLocationPropertyTop:
			case SvPb::SVExtentLocationPropertyTopRight:
			case SvPb::SVExtentLocationPropertyTopLeft:
			{
				position.m_y -= l_dY;
				l_hrOk = S_OK;

				break;
			}
			case SvPb::SVExtentLocationPropertyBottom:
			case SvPb::SVExtentLocationPropertyBottomRight:
			case SvPb::SVExtentLocationPropertyBottomLeft:
			{
				position.m_y += l_dY;
				l_hrOk = S_OK;
				break;
			}
		}

		if (S_OK == l_hrOk)
		{
			if (S_OK == SetExtentProperty(SvPb::SVExtentPropertyTranslationOffset, position))
			{
				l_hrOk = pEx->UpdateDataRecalculateOutput();
			}
			else
			{
				l_hrOk = S_FALSE;
			}
		}
	}
	return l_hrOk;
}

HRESULT  SVToolExtentClass::UpdateHorizontalPerspective(SvPb::SVExtentLocationPropertyEnum eLocation, const SVPoint<double>& rStart, const SVPoint<double>& rEnd)
{
	SVImageExtentClass* pEx = &(m_ImageExtent);

	Log_Assert(pEx->GetTranslation() == m_eTranslation);
	HRESULT l_hrOk = S_FALSE;

	SVPoint<double> position;

	double l_dX = rStart.m_x - rEnd.m_x;

	if (S_OK == GetExtentProperty(SvPb::SVExtentPropertyTranslationOffset, position))
	{
		switch (eLocation)
		{
			case SvPb::SVExtentLocationPropertyBottomLeft:
			case SvPb::SVExtentLocationPropertyTopLeft:
			case SvPb::SVExtentLocationPropertyLeft:
			{
				position.m_x -= l_dX;
				l_hrOk = S_OK;

				break;
			}
			case SvPb::SVExtentLocationPropertyRight:
			case SvPb::SVExtentLocationPropertyBottomRight:
			case SvPb::SVExtentLocationPropertyTopRight:
			{
				position.m_x += l_dX;
				l_hrOk = S_OK;
				break;
			}
		}

		if (S_OK == l_hrOk)
		{
			if (S_OK == SetExtentProperty(SvPb::SVExtentPropertyTranslationOffset, position))
			{
				l_hrOk = pEx->UpdateDataRecalculateOutput();
			}
			else
			{
				l_hrOk = S_FALSE;
			}
		}
	}
	return l_hrOk;
}

HRESULT SVToolExtentClass::UpdateLine(SvPb::SVExtentLocationPropertyEnum eLocation, const SVPoint<double>& rStart, const SVPoint<double>& rEnd)
{
	SVImageExtentClass* pEx = &(m_ImageExtent);

	Log_Assert(pEx->GetTranslation() == m_eTranslation);

	HRESULT l_hrOk = S_FALSE;

	SVPoint<double> startPoint;
	SVPoint<double> endPoint;

	if (S_OK == GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, startPoint) &&
		S_OK == GetExtentProperty(SvPb::SVExtentPropertyPositionPointEndOfLine, endPoint))
	{
		SVPoint<double> point {rEnd};
		point -= rStart;

		switch (eLocation)
		{
			case SvPb::SVExtentLocationPropertyLeft:
			{
				startPoint += point;

				l_hrOk = S_OK;

				break;
			}
			case SvPb::SVExtentLocationPropertyRight:
			{
				endPoint += point;

				l_hrOk = S_OK;

				break;
			}
			case SvPb::SVExtentLocationPropertyCenter:
			{
				startPoint += point;
				endPoint += point;

				l_hrOk = S_OK;

				break;
			}
		}

		if (S_OK == l_hrOk)
		{
			if (S_OK == SetExtentProperty(SvPb::SVExtentPropertyPositionPoint, startPoint) &&
				S_OK == SetExtentProperty(SvPb::SVExtentPropertyPositionPointEndOfLine, endPoint))
			{
				l_hrOk = pEx->UpdateDataRecalculateOutput();
			}
			else
			{
				l_hrOk = S_FALSE;
			}
		}
	}

	return l_hrOk;
}

HRESULT SVToolExtentClass::UpdatePolar(SvPb::SVExtentLocationPropertyEnum p_eLocation, const SVPoint<double>& rStart, const SVPoint<double>& rEnd)
{
	
	
	 SVImageExtentClass* pEx = &(m_ImageExtent);

	Log_Assert(pEx->GetTranslation() == m_eTranslation);
	HRESULT l_hrOk = S_FALSE;

	SVPoint<double> position;

	double l_dInnerRadius = 0.0;
	double l_dOuterRadius = 0.0;
	double l_dStartAngle = 0.0;
	double l_dEndAngle = 0.0;

	if (S_OK == GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, position) &&
		S_OK == GetExtentProperty(SvPb::SVExtentPropertyStartAngle, l_dStartAngle) &&
		S_OK == GetExtentProperty(SvPb::SVExtentPropertyEndAngle, l_dEndAngle) &&
		S_OK == GetExtentProperty(SvPb::SVExtentPropertyInnerRadius, l_dInnerRadius) &&
		S_OK == GetExtentProperty(SvPb::SVExtentPropertyOuterRadius, l_dOuterRadius))
	{
		double l_dNewAngle = SvUl::SVGetFlippedRotationAngle(position, rEnd);
		double l_dNewRadius = SvUl::SVGetRadius(position, rEnd);

		switch (p_eLocation)
		{
			case SvPb::SVExtentLocationPropertyTopLeft:
			{
				long l_lMult = (long)(l_dStartAngle) / 360;

				if (l_dStartAngle < 0)
				{
					l_lMult -= 1;
				}

				l_dNewAngle += (double)(l_lMult) * 360.0;

				if (180.0 < ::fabs(l_dNewAngle - l_dStartAngle))
				{
					if (l_dStartAngle < l_dNewAngle)
					{
						l_dStartAngle = l_dNewAngle - 360;
					}
					else
					{
						l_dStartAngle = l_dNewAngle + 360;
					}
				}
				else
				{
					l_dStartAngle = l_dNewAngle;
				}


				l_dInnerRadius = l_dNewRadius;

				l_hrOk = S_OK;

				break;
			}
			case SvPb::SVExtentLocationPropertyTopRight:
			{
				long l_lMult = (long)(l_dEndAngle) / 360;

				if (l_dEndAngle < 0)
				{
					l_lMult -= 1;
				}

				l_dNewAngle += (double)(l_lMult) * 360.0;

				if (180.0 < ::fabs(l_dNewAngle - l_dEndAngle))
				{
					if (l_dEndAngle < l_dNewAngle)
					{
						l_dEndAngle = l_dNewAngle - 360;
					}
					else
					{
						l_dEndAngle = l_dNewAngle + 360;
					}
				}
				else
				{
					l_dEndAngle = l_dNewAngle;
				}

				if (l_dEndAngle - l_dStartAngle > 720)
				{
					l_dStartAngle = l_dEndAngle - 720;
				}


				l_dInnerRadius = l_dNewRadius;

				l_hrOk = S_OK;

				break;
			}
			case SvPb::SVExtentLocationPropertyBottomRight:
			{
				long l_lMult = (long)(l_dEndAngle) / 360;

				if (l_dEndAngle < 0)
				{
					l_lMult -= 1;
				}

				l_dNewAngle += (double)(l_lMult) * 360.0;

				if (180.0 < ::fabs(l_dNewAngle - l_dEndAngle))
				{
					if (l_dEndAngle < l_dNewAngle)
					{
						l_dEndAngle = l_dNewAngle - 360;
					}
					else
					{
						l_dEndAngle = l_dNewAngle + 360;
					}
				}
				else
				{
					l_dEndAngle = l_dNewAngle;
				}

				if (l_dEndAngle - l_dStartAngle > 720)
				{
					l_dStartAngle = l_dEndAngle - 720;
				}


				l_dOuterRadius = l_dNewRadius;

				l_hrOk = S_OK;

				break;
			}
			case SvPb::SVExtentLocationPropertyBottomLeft:
			{
				long l_lMult = (long)(l_dStartAngle) / 360;

				if (l_dStartAngle < 0)
				{
					l_lMult -= 1;
				}

				l_dNewAngle += (double)(l_lMult) * 360.0;

				if (180.0 < ::fabs(l_dNewAngle - l_dStartAngle))
				{
					if (l_dStartAngle < l_dNewAngle)
					{
						l_dStartAngle = l_dNewAngle - 360;
					}
					else
					{
						l_dStartAngle = l_dNewAngle + 360;
					}
				}
				else
				{
					l_dStartAngle = l_dNewAngle;
				}


				l_dOuterRadius = l_dNewRadius;

				l_hrOk = S_OK;

				break;
			}
			case SvPb::SVExtentLocationPropertyLeft:
			{
				long l_lMult = (long)(l_dStartAngle) / 360;

				if (l_dStartAngle < 0)
				{
					l_lMult -= 1;
				}

				l_dNewAngle += (double)(l_lMult) * 360.0;

				if (180.0 < ::fabs(l_dNewAngle - l_dStartAngle))
				{
					if (l_dStartAngle < l_dNewAngle)
					{
						l_dStartAngle = l_dNewAngle - 360;
					}
					else
					{
						l_dStartAngle = l_dNewAngle + 360;
					}
				}
				else
				{
					l_dStartAngle = l_dNewAngle;
				}

				l_hrOk = S_OK;

				break;
			}
			case SvPb::SVExtentLocationPropertyRight:
			{
				long l_lMult = (long)(l_dEndAngle) / 360;

				if (l_dEndAngle < 0)
				{
					l_lMult -= 1;
				}

				l_dNewAngle += (double)(l_lMult) * 360.0;

				if (180.0 < ::fabs(l_dNewAngle - l_dEndAngle))
				{
					if (l_dEndAngle < l_dNewAngle)
					{
						l_dEndAngle = l_dNewAngle - 360;
					}
					else
					{
						l_dEndAngle = l_dNewAngle + 360;
					}
				}
				else
				{
					l_dEndAngle = l_dNewAngle;
				}

				if (l_dEndAngle - l_dStartAngle > 720)
				{
					l_dStartAngle = l_dEndAngle - 720;
				}

				l_hrOk = S_OK;

				break;
			}
			case SvPb::SVExtentLocationPropertyTop:
			{
				l_dInnerRadius = l_dNewRadius;

				l_hrOk = S_OK;

				break;
			}
			case SvPb::SVExtentLocationPropertyBottom:
			{
				l_dOuterRadius = l_dNewRadius;

				l_hrOk = S_OK;

				break;
			}
			case SvPb::SVExtentLocationPropertyCenter:
			{
				position += rEnd - rStart;

				l_hrOk = S_OK;

				break;
			}
		}

		if (S_OK == l_hrOk)
		{
			if (S_OK == SetExtentProperty(SvPb::SVExtentPropertyPositionPoint, position) &&
				S_OK == SetExtentProperty(SvPb::SVExtentPropertyStartAngle, l_dStartAngle) &&
				S_OK == SetExtentProperty(SvPb::SVExtentPropertyEndAngle, l_dEndAngle) &&
				S_OK == SetExtentProperty(SvPb::SVExtentPropertyInnerRadius, l_dInnerRadius) &&
				S_OK == SetExtentProperty(SvPb::SVExtentPropertyOuterRadius, l_dOuterRadius))
			{
				l_hrOk = pEx->UpdateDataRecalculateOutput();
			}
			else
			{
				l_hrOk = S_FALSE;
			}
		}
	}

	return l_hrOk;
}

HRESULT SVToolExtentClass::UpdatePolarFromOutputSpace(SvPb::SVExtentLocationPropertyEnum eLocation, long p_dX, long p_dY)
{
	
	 SVImageExtentClass* pEx = &(m_ImageExtent);

	Log_Assert(pEx->GetTranslation() == m_eTranslation);
	
	HRESULT l_hrOk = S_FALSE;
	SVPoint<double> position;

	double l_dInnerRadius = 0.0;
	double l_dOuterRadius = 0.0;
	double l_dStartAngle = 0.0;
	double l_dEndAngle = 0.0;

	if (S_OK == GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, position)
		&& S_OK == GetExtentProperty(SvPb::SVExtentPropertyStartAngle, l_dStartAngle)
		&& S_OK == GetExtentProperty(SvPb::SVExtentPropertyEndAngle, l_dEndAngle)
		&& S_OK == GetExtentProperty(SvPb::SVExtentPropertyInnerRadius, l_dInnerRadius)
		&& S_OK == GetExtentProperty(SvPb::SVExtentPropertyOuterRadius, l_dOuterRadius))
	{
		switch (eLocation)
		{
			case SvPb::SVExtentLocationPropertyLeft:
			{
				double l_dNewAngle = l_dStartAngle + SvUl::Radians2Degrees(p_dX) / l_dOuterRadius;

				if (l_dNewAngle > l_dEndAngle)
					l_dNewAngle = l_dEndAngle;

				l_dStartAngle = l_dNewAngle;
				l_hrOk = S_OK;

				break;
			}
			case SvPb::SVExtentLocationPropertyRight:
			{
				double l_dNewAngle = l_dEndAngle + SvUl::Radians2Degrees(p_dX) / l_dOuterRadius;

				if (l_dNewAngle < l_dStartAngle)
					l_dNewAngle = l_dStartAngle;

				l_dEndAngle = l_dNewAngle;
				l_hrOk = S_OK;

				break;
			}
			case SvPb::SVExtentLocationPropertyTop:
			{
				l_dInnerRadius += p_dY;

				l_hrOk = S_OK;

				break;
			}
			case SvPb::SVExtentLocationPropertyBottom:
			{
				l_dOuterRadius += p_dY;

				l_hrOk = S_OK;

				break;
			}
			case SvPb::SVExtentLocationPropertyCenter:
			{
				position.m_x += p_dX;
				position.m_y += p_dY;

				l_hrOk = S_OK;

				break;
			}
		}// end switch ( p_eLocation )

		if (S_OK == l_hrOk)
		{
			if (S_OK == SetExtentProperty(SvPb::SVExtentPropertyPositionPoint, position) &&
				S_OK == SetExtentProperty(SvPb::SVExtentPropertyStartAngle, l_dStartAngle) &&
				S_OK == SetExtentProperty(SvPb::SVExtentPropertyEndAngle, l_dEndAngle) &&
				S_OK == SetExtentProperty(SvPb::SVExtentPropertyInnerRadius, l_dInnerRadius) &&
				S_OK == SetExtentProperty(SvPb::SVExtentPropertyOuterRadius, l_dOuterRadius))
			{
				l_hrOk = pEx->UpdateDataRecalculateOutput();
			}
			else
			{
				l_hrOk = S_FALSE;
			}
		}
	}
	return l_hrOk;
}

HRESULT SVToolExtentClass::UpdateFromOutputSpace(SvPb::SVExtentLocationPropertyEnum p_eLocation, long lX, long lY)
{
	
	SVImageExtentClass* pEx = &(m_ImageExtent);

	Log_Assert(pEx->GetTranslation() == m_eTranslation);

	HRESULT l_hrOk = S_FALSE;

	switch (m_eTranslation)
	{
		case SvPb::SVExtentTranslationLine:
		{
			SVPoint<double> startPoint;
			SVPoint<double> endPoint;

			if (S_OK == GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, startPoint) &&
				S_OK == GetExtentProperty(SvPb::SVExtentPropertyPositionPointEndOfLine, endPoint))
			{
				switch (p_eLocation)
				{
					case SvPb::SVExtentLocationPropertyTop:
					{
						startPoint.m_x += lX;
						startPoint.m_y += lY;

						l_hrOk = S_OK;

						break;
					}
					case SvPb::SVExtentLocationPropertyLeft:
					{
						startPoint.m_x += lX;
						startPoint.m_y += lY;

						l_hrOk = S_OK;

						break;
					}
					case SvPb::SVExtentLocationPropertyBottom:
					{
						endPoint.m_x += lX;
						endPoint.m_y += lY;

						l_hrOk = S_OK;

						break;
					}
					case SvPb::SVExtentLocationPropertyRight:
					{
						endPoint.m_x += lX;
						endPoint.m_y += lY;

						l_hrOk = S_OK;

						break;
					}
					case SvPb::SVExtentLocationPropertyCenter:
					{
						startPoint.m_x += lX;
						startPoint.m_y += lY;

						endPoint.m_x += lX;
						endPoint.m_y += lY;

						l_hrOk = S_OK;

						break;
					}
				}

				if (S_OK == l_hrOk)
				{
					if (S_OK == SetExtentProperty(SvPb::SVExtentPropertyPositionPoint, startPoint) &&
						S_OK == SetExtentProperty(SvPb::SVExtentPropertyPositionPointEndOfLine, endPoint))
					{
						l_hrOk = pEx->UpdateDataRecalculateOutput();
					}
					else
					{
						l_hrOk = S_FALSE;
					}
				}
			}

			break;
		}
		case SvPb::SVExtentTranslationProfile:
		{
			SVPoint<double> position;

			double l_dWidth = 0.0;
			double l_dHeight = 0.0;

			if (S_OK == GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, position) &&
				S_OK == GetExtentProperty(SvPb::SVExtentPropertyHeight, l_dHeight) &&
				S_OK == GetExtentProperty(SvPb::SVExtentPropertyWidth, l_dWidth))
			{
				switch (p_eLocation)
				{
					case SvPb::SVExtentLocationPropertyTopRight:
					{
						l_dWidth += lX;
						l_dHeight -= lY * 2.0;

						l_hrOk = S_OK;

						break;
					}
					case SvPb::SVExtentLocationPropertyBottomRight:
					{
						l_dWidth += lX;
						l_dHeight += lY * 2.0;

						l_hrOk = S_OK;

						break;
					}
					case SvPb::SVExtentLocationPropertyRight:
					{
						l_dWidth += lX;

						l_hrOk = S_OK;

						break;
					}
					case SvPb::SVExtentLocationPropertyTop:
					{
						l_dHeight -= lY * 2.0;

						l_hrOk = S_OK;

						break;
					}
					case SvPb::SVExtentLocationPropertyBottom:
					{
						l_dHeight += lY * 2.0;

						l_hrOk = S_OK;

						break;
					}
					case SvPb::SVExtentLocationPropertyCenter:
					{
						position.m_x += lX;
						position.m_y += lY;

						l_hrOk = S_OK;

						break;
					}
				}

				if (S_OK == l_hrOk)
				{
					if (0.0 < l_dWidth && 0.0 < l_dHeight &&
						S_OK == SetExtentProperty(SvPb::SVExtentPropertyPositionPoint, position) &&
						S_OK == SetExtentProperty(SvPb::SVExtentPropertyHeight, l_dHeight) &&
						S_OK == SetExtentProperty(SvPb::SVExtentPropertyWidth, l_dWidth))
					{
						l_hrOk = pEx->UpdateDataRecalculateOutput();
					}
					else
					{
						l_hrOk = S_FALSE;
					}
				}
			}

			break;
		}
		case SvPb::SVExtentTranslationShift:
		case SvPb::SVExtentTranslationRotate:
		case SvPb::SVExtentTranslationProfileShift:
		case SvPb::SVExtentTranslationFlippedRotate:
		case SvPb::SVExtentTranslationDoubleHeight:
		case SvPb::SVExtentTranslationResize:
		case SvPb::SVExtentTranslationFlipHorizontal:
		case SvPb::SVExtentTranslationFlipVertical:
		case SvPb::SVExtentTranslationFigureShift:
		{
			SVPoint<double> position;

			double l_dWidth = 0.0;
			double l_dHeight = 0.0;

			if (S_OK == GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, position) &&
				S_OK == GetExtentProperty(SvPb::SVExtentPropertyHeight, l_dHeight) &&
				S_OK == GetExtentProperty(SvPb::SVExtentPropertyWidth, l_dWidth))
			{
				switch (p_eLocation)
				{
					case SvPb::SVExtentLocationPropertyTopLeft:
					{
						position.m_x += lX;
						position.m_y += lY;

						l_dWidth -= lX;
						l_dHeight -= lY;

						l_hrOk = S_OK;

						break;
					}
					case SvPb::SVExtentLocationPropertyTopRight:
					{
						position.m_y += lY;

						l_dWidth += lX;
						l_dHeight -= lY;

						l_hrOk = S_OK;

						break;
					}
					case SvPb::SVExtentLocationPropertyBottomRight:
					{
						l_dWidth += lX;
						l_dHeight += lY;

						l_hrOk = S_OK;

						break;
					}
					case SvPb::SVExtentLocationPropertyBottomLeft:
					{
						position.m_x += lX;

						l_dWidth -= lX;
						l_dHeight += lY;

						l_hrOk = S_OK;

						break;
					}
					case SvPb::SVExtentLocationPropertyLeft:
					{
						position.m_x += lX;

						l_dWidth -= lX;

						l_hrOk = S_OK;

						break;
					}
					case SvPb::SVExtentLocationPropertyRight:
					{
						l_dWidth += lX;

						l_hrOk = S_OK;

						break;
					}
					case SvPb::SVExtentLocationPropertyTop:
					{
						position.m_y += lY;

						l_dHeight -= lY;

						l_hrOk = S_OK;

						break;
					}
					case SvPb::SVExtentLocationPropertyBottom:
					{
						l_dHeight += lY;

						l_hrOk = S_OK;

						break;
					}
					case SvPb::SVExtentLocationPropertyCenter:
					{
						position.m_x += lX;
						position.m_y += lY;

						l_hrOk = S_OK;

						break;
					}
				}

				if (S_OK == l_hrOk)
				{
					if (0.0 < l_dWidth && 0.0 < l_dHeight &&
						S_OK == SetExtentProperty(SvPb::SVExtentPropertyPositionPoint, position) &&
						S_OK == SetExtentProperty(SvPb::SVExtentPropertyHeight, l_dHeight) &&
						S_OK == SetExtentProperty(SvPb::SVExtentPropertyWidth, l_dWidth))
					{
						l_hrOk = pEx->UpdateDataRecalculateOutput();
					}
					else
					{
						l_hrOk = S_FALSE;
					}
				}
			}

			break;
		}

		case SvPb::SVExtentTranslationPolarUnwrap:
		{
			l_hrOk = UpdatePolarFromOutputSpace(p_eLocation, lX, lY);
			break;
		}
		case SvPb::SVExtentTranslationVerticalPerspective:
		{
			switch (p_eLocation)
			{
				case SvPb::SVExtentLocationPropertyTop:
				case SvPb::SVExtentLocationPropertyBottom:
				case SvPb::SVExtentLocationPropertyCenter:
				{
					SVPoint<double> position;

					if (S_OK == GetExtentProperty(SvPb::SVExtentPropertyTranslationOffset, position))
					{
						position.m_y += lY;
						l_hrOk = S_OK;
					}

					if (S_OK == l_hrOk)
					{
						if (S_OK == SetExtentProperty(SvPb::SVExtentPropertyTranslationOffset, position))
						{
							l_hrOk = pEx->UpdateDataRecalculateOutput();
						}
						else
						{
							l_hrOk = S_FALSE;
						}
					}

					break;
				}
			}
			break;
		}
		case SvPb::SVExtentTranslationHorizontalPerspective:
		{
			switch (p_eLocation)
			{
				case SvPb::SVExtentLocationPropertyLeft:
				case SvPb::SVExtentLocationPropertyCenter:
				case SvPb::SVExtentLocationPropertyRight:
				{
					SVPoint<double> position;

					if (S_OK == GetExtentProperty(SvPb::SVExtentPropertyTranslationOffset, position))
					{
						position.m_x += lX;
						l_hrOk = S_OK;
					}

					if (S_OK == l_hrOk)
					{
						if (S_OK == SetExtentProperty(SvPb::SVExtentPropertyTranslationOffset, position))
						{
							l_hrOk = pEx->UpdateDataRecalculateOutput();
						}
						else
						{
							l_hrOk = S_FALSE;
						}
					}

					break;
				}
			}
			break;
		}

	}
	SetImageExtentAndProperty(m_ImageExtent);
	return l_hrOk;
}

// Translate the point to be relative (local space).  Usually this relates to the ROI of the parent image.
HRESULT  SVToolExtentClass::TranslateToLocalSpace(SVPoint<double> value, SVPoint<double>& rResult)
{
	SVPoint<double> position;
	HRESULT hr = GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, position);

	if (S_OK == hr)
	{
		rResult = value - position;

		// optional...
		SVPoint<double> displacement;
		if (S_OK == GetExtentProperty(SvPb::SVExtentPropertyTranslationOffset, displacement))
		{
			rResult -= displacement;
		}
	}
	return hr;
}
HRESULT SVToolExtentClass::TranslateToOutputSpace(SVPoint<double> value, SVPoint<double>& rResult) const
{
	HRESULT l_hrOk {S_OK};
	
	Log_Assert(m_ImageExtent.GetTranslation() == m_eTranslation);
	
	rResult.clear();

	switch (m_eTranslation)
	{
		case SvPb::SVExtentTranslationNone:
		case SvPb::SVExtentTranslationFigureShift:
		{
			rResult = value;

			break;
		}
		case SvPb::SVExtentTranslationShift:
		{
			SVPoint<double> position;

			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, position);

			if (S_OK == l_hrOk)
			{
				SVPoint<double> displacement;

				GetExtentProperty(SvPb::SVExtentPropertyTranslationOffset, displacement);

				rResult = value - position - displacement;
			}

			break;
		}// end multiple case:
		case SvPb::SVExtentTranslationProfileShift:
		{
			SVPoint<double> position;

			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, position);

			if (S_OK == l_hrOk)
			{
				rResult = value - position;
			}

			break;
		}// end multiple case:
		case SvPb::SVExtentTranslationTransformShift:
		{
			SVPoint<double> position;
			SVPoint<double> displacement;

			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, position);

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyTranslationOffset, displacement);
			}

			if (S_OK == l_hrOk)
			{
				rResult = value - position - displacement;
			}

			break;
		}
		case SvPb::SVExtentTranslationRotate:
		case SvPb::SVExtentTranslationProfile:
		case SvPb::SVExtentTranslationLinear:
		{
			SVPoint<double> rotation;
			SVPoint<double> outputRotation;

			double dAngle = 0.0;

			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, rotation);

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyRotationAngle, dAngle);
			}

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyOutputPositionPoint, outputRotation);
			}

			if (S_OK == l_hrOk)
			{
				rResult = SvUl::SVRotatePoint(rotation, value, -dAngle);

				rResult = rResult - rotation + outputRotation;
			}

			break;
		}// end mulitple case:
		case SvPb::SVExtentTranslationTransformRotate:
		{
			SVPoint<double> rotation;
			SVPoint<double> outputRotation;
			SVPoint<double> displacement;

			double dAngle = 0.0;

			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, rotation);

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyRotationAngle, dAngle);
			}

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyOutputPositionPoint, outputRotation);
			}

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyTranslationOffset, displacement);
			}

			if (S_OK == l_hrOk)
			{
				rResult = SvUl::SVRotatePoint(rotation, value, -dAngle);

				rResult = rResult - rotation + outputRotation + displacement;
			}

			break;
		}
		case SvPb::SVExtentTranslationLine:
		{
			SVPoint<double> startPoint;
			SVPoint<double> endPoint;
			SVPoint<double> outputRotation;

			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, startPoint);

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyPositionPointEndOfLine, endPoint);
			}

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyOutputPositionPoint, outputRotation);
			}

			if (S_OK == l_hrOk)
			{
				double dAngle = SvUl::SVGetRotationAngle(startPoint, endPoint);

				rResult = SvUl::SVRotatePoint(startPoint, value, -dAngle);

				rResult = rResult - startPoint + outputRotation;
			}

			break;
		}// end case SVExtentTranslationLine:
		case SvPb::SVExtentTranslationFlippedRotate:
		{
			SVPoint<double> rotation;
			SVPoint<double> outputRotation;

			double dAngle = 0.0;

			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, rotation);

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyRotationAngle, dAngle);
			}

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyOutputPositionPoint, outputRotation);
			}

			if (S_OK == l_hrOk)
			{
				rResult = SvUl::SVRotateAndFlipPoint(rotation, value, -dAngle);

				rResult = rResult - rotation + outputRotation;
			}

			break;
		}// end case SVExtentTranslationFlippedRotate:

		case SvPb::SVExtentTranslationPolarUnwrap:
		{
			SVPoint<double> centerPoint;

			double dStartAngle = 0.0;
			double dOuterRadius = 0.0;
			double dInnerRadius = 0.0;

			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, centerPoint);

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyOuterRadius, dOuterRadius);
			}

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyInnerRadius, dInnerRadius);
			}

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyStartAngle, dStartAngle);
			}

			if (S_OK == l_hrOk)
			{
				double l_dRadius = SvUl::SVGetRadius(centerPoint, value);
				double l_dAngle = SvUl::SVGetFlippedRotationAngle(centerPoint, value);

				l_dAngle -= dStartAngle;
				l_dAngle -= ((long)(l_dAngle) / 360) * 360.0;

				if (dOuterRadius > dInnerRadius)
				{
					rResult.m_x = SvUl::Degrees2Radians(l_dAngle) * dOuterRadius;
					rResult.m_y = l_dRadius - dInnerRadius;
				}
				else
				{
					rResult.m_x = SvUl::Degrees2Radians(l_dAngle) * dInnerRadius;
					rResult.m_y = l_dRadius - dOuterRadius;
				}
			}

			break;
		}// end case SVExtentTranslationPolarUnwrap:

		case SvPb::SVExtentTranslationCylindricalWarpH:
		{
			double dInputWidth = 0.0;
			double dOutputWidth = 0.0;
			double dAngle = 0.0;
			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyWidth, dInputWidth);
			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyOutputWidth, dOutputWidth);
			}
			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyStartAngle, dAngle);
			}
			if (S_OK == l_hrOk)
			{
				rResult.m_y = value.m_y;
				double l_dInputRadius = dInputWidth / 2.0;
				double l_dRadius = (dInputWidth / std::sin(dAngle / 360.0 * M_PI)) / 2.0;
				if (value.m_x < l_dInputRadius)
				{
					double l_dXFromCenter = l_dInputRadius - value.m_x;
					double l_dAngleAtPoint = 90.0 - std::acos(l_dXFromCenter / l_dRadius);
					double l_dCircum = l_dAngleAtPoint / 180.0 * M_PI * l_dRadius;
					rResult.m_x = dOutputWidth / 2.0 - l_dCircum;
				}
				else
				{
					double l_dXFromCenter = value.m_x - l_dInputRadius;
					double l_dAngleAtPoint = 90.0 - std::acos(l_dXFromCenter / l_dRadius);
					double l_dCircum = l_dAngleAtPoint / 180.0 * M_PI * l_dRadius;
					rResult.m_x = dOutputWidth / 2.0 + l_dCircum;
				}
			}
			break;
		}// end case SVExtentTranslationCylindricalWarpH:

		case SvPb::SVExtentTranslationCylindricalWarpV:
		{
			double dInputHeight = 0.0;
			double dOutputHeight = 0.0;
			double dAngle = 0.0;
			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyHeight, dInputHeight);
			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyOutputHeight, dOutputHeight);
			}
			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyStartAngle, dAngle);
			}
			if (S_OK == l_hrOk)
			{
				rResult.m_x = value.m_x;
				double l_dInputRadius = dInputHeight / 2.0; // small height
				double l_dRadius = (dInputHeight / std::sin(dAngle / 360.0 * M_PI)) / 2.0;
				if (value.m_y < l_dInputRadius) // Quadrant detection
				{
					double l_dYFromCenter = l_dInputRadius - value.m_y;
					double l_dAngleAtPoint = 90.0 - std::acos(l_dYFromCenter / l_dRadius);
					double l_dCircum = l_dAngleAtPoint / 180.0 * M_PI * l_dRadius;
					rResult.m_y = dOutputHeight / 2.0 - l_dCircum;
				}
				else
				{
					double l_dYFromCenter = value.m_y - l_dInputRadius;
					double l_dAngleAtPoint = 90.0 - std::acos(l_dYFromCenter / l_dRadius);
					double l_dCircum = l_dAngleAtPoint / 180.0 * M_PI * l_dRadius;
					rResult.m_y = dOutputHeight / 2.0 + l_dCircum;
				}

			}
			break;
		}// end case SVExtentTranslationCylindricalWarpV:
		case SvPb::SVExtentTranslationVerticalPerspective:
		{
			double dFullHeight = 0.0;
			double dFullWidth = 0.0;
			double dyOffset = 0.0;
			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyWidth, dFullWidth);
			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyHeight, dFullHeight);
			}
			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyTranslationOffsetY, dyOffset);
			}
			if (S_OK == l_hrOk)
			{
				rResult = value;			// Copy points 1 to 1. all X and Y when center points.
				double l_dHeight = dFullHeight / 2; // One Quadrants Width
				double l_dWidth = dFullWidth / 2;	// One Quadrant
				double l_dYhyp;						// Point on hypotnuse

				if (value.m_y < l_dHeight && value.m_x < l_dWidth)
				{
					//******** Upper Left Quadrant ******************
					l_dYhyp = dyOffset - (dyOffset / l_dWidth) * value.m_x;
					rResult.m_y = l_dHeight - (l_dHeight - value.m_y) * l_dHeight / (l_dHeight - l_dYhyp);
				}
				else
					if (value.m_y > l_dHeight && value.m_x < l_dWidth)
					{
						//******** Lower Left Quadrant ******************
						l_dYhyp = dFullHeight - dyOffset + (dyOffset / l_dWidth) * value.m_x;
						rResult.m_y = l_dHeight + (l_dHeight - value.m_y) * l_dHeight / (l_dHeight - l_dYhyp);
					}
					else
						if (value.m_y < l_dHeight && value.m_x > l_dWidth)
						{
							//******** Upper Right Quadrant ******************
							l_dYhyp = dyOffset - (dyOffset / l_dWidth) * (dFullWidth - value.m_x);
							rResult.m_y = l_dHeight - (l_dHeight - value.m_y) * l_dHeight / (l_dHeight - l_dYhyp);
						}
						else
							if (value.m_y > l_dHeight && value.m_x > l_dWidth)
							{
								//******** Lower Right Quadrant ******************
								l_dYhyp = dFullHeight - (dyOffset / l_dWidth) * (value.m_x - l_dWidth);
								rResult.m_y = l_dHeight + (l_dHeight - value.m_y) * l_dHeight / (l_dHeight - l_dYhyp);

							}
			}

			break;
		}
		case SvPb::SVExtentTranslationHorizontalPerspective:
		{
			double dFullHeight = 0.0;
			double dFullWidth = 0.0;
			double dxOffset = 0.0;
			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyWidth, dFullWidth);
			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyHeight, dFullHeight);
			}
			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyTranslationOffsetX, dxOffset);
			}
			if (S_OK == l_hrOk)
			{
				rResult = value;			// Copy points 1 to 1. all X and Y when center points.
				double dHeight = dFullHeight / 2; // One Quadrants Width
				double dWidth = dFullWidth / 2;	// One Quadrant
				double dXhyp;						// Point on hypotenuse

				if (value.m_y < dHeight && value.m_x < dWidth)
				{
					//******** Upper Left Quadrant ******************
					dXhyp = dxOffset - (dxOffset / dHeight) * value.m_y;
					rResult.m_x = dWidth - (dWidth - value.m_x) * dWidth / (dWidth - dXhyp);
				}
				else
					if (value.m_y > dHeight && value.m_x < dWidth)
					{
						//******** Lower Left Quadrant ******************
						dXhyp = dxOffset - (dxOffset / dHeight) * (dFullHeight - value.m_y);
						rResult.m_x = dWidth - (dWidth - value.m_x) * dWidth / (dWidth - dXhyp);
					}
					else
						if (value.m_y < dHeight && value.m_x > dWidth)
						{
							//******** Upper Right Quadrant ******************
							dXhyp = dFullWidth - dxOffset + (dxOffset / dHeight) * value.m_y;
							rResult.m_x = dWidth + (dWidth - value.m_x) * dWidth / (dWidth - dXhyp);
						}
						else
							if (value.m_y > dHeight && value.m_x > dWidth)
							{
								//******** Lower Right Quadrant ******************
								dXhyp = dFullWidth - (dxOffset / dHeight) * (value.m_y - dHeight);
								rResult.m_x = dWidth + (dWidth - value.m_x) * dWidth / (dWidth - dXhyp);

							}
			}

			break;
		}

		case SvPb::SVExtentTranslationDoubleHeight:
		case SvPb::SVExtentTranslationResize:
		{
			SVPoint<double> position;
			double heightScaleFactor = 1.0;
			double widthScaleFactor = 1.0;

			//@WARNING [Jim][8 July 2015]  No identification of error cases.  
			// GetExtentProperty () only returns S_FALSE, which is not 
			// very helpful. Currently none of these cases give much help
			// with error identification.
			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, position);
			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyHeightFactorContent, heightScaleFactor);
			}

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyWidthFactorContent, widthScaleFactor);
			}

			if (S_OK == l_hrOk)
			{
				rResult.m_x = (value.m_x - position.m_x) * widthScaleFactor;
				rResult.m_y = (value.m_y - position.m_y) * heightScaleFactor;
			}

			break;
		}

		case SvPb::SVExtentTranslationFlipVertical:
		{
			SVPoint<double> position;
			double dHeight = 0.0;

			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, position);

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyHeight, dHeight);
			}

			if (S_OK == l_hrOk)
			{
				rResult.m_x = value.m_x - position.m_x;
				rResult.m_y = (dHeight - 1) - (value.m_y - position.m_y);
			}

			break;
		}
		case SvPb::SVExtentTranslationFlipHorizontal:
		{
			SVPoint<double> position;
			double dWidth = 0.0;

			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, position);

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyWidth, dWidth);
			}

			if (S_OK == l_hrOk)
			{
				rResult.m_x = (dWidth - 1) - (value.m_x - position.m_x);
				rResult.m_y = value.m_y - position.m_y;
			}

			break;
		}
		default:
		{
			l_hrOk = S_FALSE;

			break;
		}
	}

	return l_hrOk;
}

} //namespace SvTo
