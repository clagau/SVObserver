//*****************************************************************************
/// \copyright (c) 2019,2019 by Seidenader Maschinenbau GmbH
/// \file FillHelper.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Helper Function for filling the Overlay-protoBuf Message with live data.
//******************************************************************************
#pragma once


#pragma region Includes
#include "stdafx.h"
#include "SVUtilityLibrary/SVSafeArray.h"
#include "TriggerRecordController/ITriggerRecordR.h"
#include "FillHelper.h"
#pragma endregion Includes

#pragma region Declarations
#pragma endregion Declarations

namespace SvOv
{
bool fillValue(SvTrc::ITriggerRecordRPtr pTr, long trPos, SvPb::ValueObject* pValue)
{
	bool isSet = false;
	if (0 < trPos)
	{
		//trPos in PB is one based and in tr it is zero based.
		_variant_t var = pTr->getDataValue(trPos - 1);
		if (VT_EMPTY != var.vt)
		{
			pValue->set_value(var);
			isSet = true;
		}
	}
	return isSet;
}

void fillRect(SvTrc::ITriggerRecordRPtr pTr, SvPb::OverlayShapeRect& rRect)
{
	fillValue(pTr, rRect.x().trpos(), rRect.mutable_x());
	fillValue(pTr, rRect.y().trpos(), rRect.mutable_y());
	fillValue(pTr, rRect.w().trpos(), rRect.mutable_w());
	fillValue(pTr, rRect.h().trpos(), rRect.mutable_h());
}

bool fillRectArray(SvTrc::ITriggerRecordRPtr pTr, const SvPb::SVORectArray12Data& rRectArrayData, SvPb::OverlayShapeRectArray& rRectArray)
{
	_variant_t varX1 = pTr->getDataValue(rRectArrayData.x1trpos() - 1);
	auto x1Array = SvUl::getVectorFromOneDim<float>(varX1);
	_variant_t varY1 = pTr->getDataValue(rRectArrayData.y1trpos() - 1);
	auto y1Array = SvUl::getVectorFromOneDim<float>(varY1);
	_variant_t varX2 = pTr->getDataValue(rRectArrayData.x2trpos() - 1);
	auto x2Array = SvUl::getVectorFromOneDim<float>(varX2);
	_variant_t varY2 = pTr->getDataValue(rRectArrayData.y2trpos() - 1);
	auto y2Array = SvUl::getVectorFromOneDim<float>(varY2);
	if (0 == x1Array.size() || x1Array.size() != y1Array.size() || x1Array.size() != x2Array.size() || x1Array.size() != y2Array.size())
	{
		return false;
	}
	bool retValue = false;
	for (int i = 0; i < x1Array.size(); i++)
	{
		auto* pRect = rRectArray.add_rectarray();
		pRect->set_x(x1Array[i]);
		pRect->set_y(y1Array[i]);
		pRect->set_w(x2Array[i] - x1Array[i]);
		pRect->set_h(y2Array[i] - y1Array[i]);
		retValue = true;
	}
	return retValue;
}

bool fillRectArray(SvTrc::ITriggerRecordRPtr pTr, const SvPb::SVORectArrayPatternData& rRectArrayData, SvPb::OverlayShapeRectArray& rRectArray)
{
	_variant_t varX = pTr->getDataValue(rRectArrayData.xtrpos() - 1);
	auto xArray = SvUl::getVectorFromOneDim<float>(varX);
	_variant_t varY = pTr->getDataValue(rRectArrayData.ytrpos() - 1);
	auto yArray = SvUl::getVectorFromOneDim<float>(varY);
	_variant_t varAngle = pTr->getDataValue(rRectArrayData.angletrpos() - 1);
	auto angleArray = SvUl::getVectorFromOneDim<float>(varAngle);
	if (0 == xArray.size() || xArray.size() != yArray.size() || xArray.size() != angleArray.size())
	{
		return false;
	}
	bool retValue = false;
	for (int i = 0; i < xArray.size(); i++)
	{
		//@TODO[MZA][8.20][07.08.2019] by now rotated pattern removed and needed code uncomment. For use rotated add this code and fit it.
		//SVPoint<double> moveVector = SVRotatePoint(SVPoint<double>(0, 0), SVPoint<double>(rRectArrayData.centerx(), rRectArrayData.centery()), -angleArray[i]);
		//double xPos = xArray[i] - moveVector.m_x;
		//double yPos = yArray[i] - moveVector.m_y;
		//if (xPos < 0.0 || yPos < 0.0 || angleArray[i] < 0.0)
		double xPos = xArray[i] - rRectArrayData.centerx();
		double yPos = yArray[i] - rRectArrayData.centery();
		if (xPos < 0.0 || yPos < 0.0 || angleArray[i] != 0.0)
		{
			continue; //error, not set rect.
		}

		auto* pRect = rRectArray.add_rectarray();
		pRect->set_x(xPos);
		pRect->set_y(yPos);
		//pRect->set_angle(angleArray[i]);
		pRect->set_w(rRectArrayData.width());
		pRect->set_h(rRectArrayData.height());
		retValue = true;
	}
	return retValue;
}

void fillOverlay(SvPb::OverlayDesc& overlayDesc, SvTrc::ITriggerRecordRPtr pTr)
{
	for (int i = 0; i < overlayDesc.overlays_size(); i++)
	{
		auto* pOverlay = overlayDesc.mutable_overlays(i);
		if (nullptr == pOverlay) { continue; }

		//set overlay color
		fillValue(pTr, pOverlay->color().trpos(), pOverlay->mutable_color());

		auto* pBoundingBox = pOverlay->mutable_boundingshape();
		if (nullptr == pBoundingBox) { continue; }
		switch (pBoundingBox->shape_case())
		{
			case SvPb::OverlayBoundingShape::kRect:
			{
				auto* pRect = pBoundingBox->mutable_rect();
				if (nullptr == pRect) { continue; }
				fillRect(pTr, *pRect);
			}
			break;
		}

		for (int j = 0; j < pOverlay->shapegroups_size(); j++)
		{
			auto* pGroups = pOverlay->mutable_shapegroups(j);
			if (nullptr == pGroups) { continue; }
			for (int k = 0; k < pGroups->shapes_size(); k++)
			{
				auto* pShape = pGroups->mutable_shapes(k);
				if (nullptr == pShape) { continue; }
				fillValue(pTr, pShape->color().trpos(), pShape->mutable_color());
				switch (pShape->shape_case())
				{
					case SvPb::OverlayShape::kRect:
					{
						auto* pRect = pShape->mutable_rect();
						if (nullptr == pRect) { continue; }
						fillRect(pTr, *pRect);
					}
					break;
					case SvPb::OverlayShape::kRectArray:
					{
						bool isValid = false;
						auto* pRectArray = pShape->mutable_rectarray();
						if (nullptr != pRectArray)
						{
							switch (pRectArray->SVOData_case())
							{
								case SvPb::OverlayShapeRectArray::kX12Data:
									isValid = fillRectArray(pTr, pRectArray->x12data(), *pRectArray);
									pRectArray->clear_x12data();
									break;
								case SvPb::OverlayShapeRectArray::kPatternData:
									isValid = fillRectArray(pTr, pRectArray->patterndata(), *pRectArray);
									pRectArray->clear_patterndata();
									break;
								default:
									isValid = false;
									break;
							}
						}
						if (!isValid)
						{	//delete shape and if empty group
							pGroups->mutable_shapes()->erase(pGroups->shapes().begin() + k);
							if (0 == pGroups->shapes_size())
							{
								pOverlay->mutable_shapegroups()->erase(pOverlay->shapegroups().begin() + j);
							}
						}
					}
					break;
				}
			}
		}
	}
}
}