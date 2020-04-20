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
#include <math.h>
#include "SVUtilityLibrary/SVSafeArray.h"
#include "TriggerRecordController/ITriggerRecordR.h"
#include "FillHelper.h"
#pragma endregion Includes

#pragma region Declarations
#pragma endregion Declarations

namespace SvOv
{
	const double g_pi = std::acos(-1);

	bool fillValue(const SvTrc::ITriggerRecordR& rTr, long trPos, SvPb::ValueObject* pValue)
	{
		bool isSet = false;
		if (0 < trPos)
		{
			//trPos in PB is one based and in tr it is zero based.
			_variant_t var = rTr.getDataValue(trPos - 1);
			if (VT_EMPTY != var.vt)
			{
				pValue->set_value(var);
				isSet = true;
			}
		}
		return isSet;
	}

	void fillRect(const SvTrc::ITriggerRecordR& rTr, SvPb::OverlayShapeRect& rRect)
	{
		fillValue(rTr, rRect.x().trpos(), rRect.mutable_x());
		fillValue(rTr, rRect.y().trpos(), rRect.mutable_y());
		fillValue(rTr, rRect.w().trpos(), rRect.mutable_w());
		fillValue(rTr, rRect.h().trpos(), rRect.mutable_h());
	}

	std::vector<double> getDataArray(const SvTrc::ITriggerRecordR& rTr, int pos)
	{
		if (0 < pos)
		{
			_variant_t varX1 = rTr.getDataValue(pos - 1);
			if (VT_ARRAY & varX1.vt)
			{
				return SvUl::getVectorFromOneDim<double>(varX1);
			}
			else
			{
				return{ varX1 };
			}
		}
		return {};
	}

	bool fillRectArray(const SvTrc::ITriggerRecordR& rTr, const SvPb::SVORectArray12Data& rRectArrayData, SvPb::OverlayShapeRectArray& rRectArray)
	{
		auto x1Array = getDataArray(rTr, rRectArrayData.x1trpos());
		auto y1Array = getDataArray(rTr, rRectArrayData.y1trpos());
		auto x2Array = getDataArray(rTr, rRectArrayData.x2trpos());
		auto y2Array = getDataArray(rTr, rRectArrayData.y2trpos());
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

	bool fillRectArray(const SvTrc::ITriggerRecordR& rTr, const SvPb::SVORectArrayPatternData& rRectArrayData, SvPb::OverlayShapeRectArray& rRectArray)
	{
		auto xArray = getDataArray(rTr, rRectArrayData.xtrpos());
		auto yArray = getDataArray(rTr, rRectArrayData.ytrpos());
		auto angleArray = getDataArray(rTr, rRectArrayData.angletrpos());
		if (0 == xArray.size() || xArray.size() != yArray.size() || xArray.size() != angleArray.size())
		{
			return false;
		}
		bool retValue = false;
		for (int i = 0; i < xArray.size(); i++)
		{
			double xPos;
			double yPos;
			if (angleArray[i] == 0.0)
			{
				xPos = xArray[i] - rRectArrayData.centerx();
				yPos = yArray[i] - rRectArrayData.centery();
			}
			else
			{				
				double angleTmp = rRectArrayData.centerangle()-angleArray[i];
				//The fmod check is to make sure that exactly 0.0 is returned
				bool callCos = (std::fmod(angleTmp, 90.0) != 0.0) || fmod(angleTmp, 180.0) == 0.0;
				double dXCos = callCos ? std::cos(g_pi * angleTmp / 180.0) : 0.0;
				double dYSin = (std::fmod(angleTmp, 180.0) != 0.0) ? std::sin(g_pi * angleTmp / 180.0) : 0.0;
				xPos = xArray[i] - rRectArrayData.centerradius() * dXCos;
				yPos = yArray[i] - rRectArrayData.centerradius() * dYSin;
			}
			
			if (xPos < 0.0 || yPos < 0.0)
			{
				continue; //error, not set rect.
			}

			auto* pRect = rRectArray.add_rectarray();
			pRect->set_x(xPos);
			pRect->set_y(yPos);
			pRect->set_angle(angleArray[i]);
			pRect->set_w(rRectArrayData.width());
			pRect->set_h(rRectArrayData.height());
			retValue = true;
		}
		return retValue;
	}

	void fillOverlay(SvPb::OverlayDesc& overlayDesc, const SvTrc::ITriggerRecordR& rTr)
	{
		for (int i = 0; i < overlayDesc.overlays_size(); i++)
		{
			auto* pOverlay = overlayDesc.mutable_overlays(i);
			if (nullptr == pOverlay) { continue; }

			//set overlay color
			fillValue(rTr, pOverlay->color().trpos(), pOverlay->mutable_color());

			//set overlay state values
			fillValue(rTr, pOverlay->passed().trpos(), pOverlay->mutable_passed());
			fillValue(rTr, pOverlay->failed().trpos(), pOverlay->mutable_failed());
			fillValue(rTr, pOverlay->warned().trpos(), pOverlay->mutable_warned());

			auto* pBoundingBox = pOverlay->mutable_boundingshape();
			if (nullptr == pBoundingBox) { continue; }
			switch (pBoundingBox->shape_case())
			{
			case SvPb::OverlayBoundingShape::kRect:
			{
				auto* pRect = pBoundingBox->mutable_rect();
				if (nullptr == pRect) { continue; }
				fillRect(rTr, *pRect);
			}
			break;
			case SvPb::OverlayBoundingShape::kSlice:
			{
				auto* pSlice = pBoundingBox->mutable_slice();
				if (nullptr == pSlice) { continue; }
				fillValue(rTr, pSlice->centerx().trpos(), pSlice->mutable_centerx());
				fillValue(rTr, pSlice->centery().trpos(), pSlice->mutable_centery());
				fillValue(rTr, pSlice->startangle().trpos(), pSlice->mutable_startangle());
				fillValue(rTr, pSlice->endangle().trpos(), pSlice->mutable_endangle());
				fillValue(rTr, pSlice->innerradius().trpos(), pSlice->mutable_innerradius());
				fillValue(rTr, pSlice->outerradius().trpos(), pSlice->mutable_outerradius());
			}
			break;
			case SvPb::OverlayBoundingShape::kPerspective:
			{
				auto* pPerspective = pBoundingBox->mutable_perspective();
				if (nullptr == pPerspective) { continue; }
				fillValue(rTr, pPerspective->offset().trpos(), pPerspective->mutable_offset());
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
					fillValue(rTr, pShape->color().trpos(), pShape->mutable_color());
					switch (pShape->shape_case())
					{
					case SvPb::OverlayShape::kRect:
					{
						auto* pRect = pShape->mutable_rect();
						if (nullptr == pRect) { continue; }
						fillRect(rTr, *pRect);
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
								isValid = fillRectArray(rTr, pRectArray->x12data(), *pRectArray);
								pRectArray->clear_x12data();
								break;
							case SvPb::OverlayShapeRectArray::kPatternData:
								isValid = fillRectArray(rTr, pRectArray->patterndata(), *pRectArray);
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
					case SvPb::OverlayShape::kMarker:
					{
						auto* pMarker = pShape->mutable_marker();
						if (nullptr != pMarker)
						{
							if (!pMarker->has_selectedmarkerdata())
							{
								fillValue(rTr, pMarker->value().trpos(), pMarker->mutable_value());
							}
							else
							{
								fillSelectedMarker(pMarker->selectedmarkerdata(), rTr, pMarker->minvalue(), pMarker->maxvalue(), pMarker->mutable_value());
							}
						}
					}
					break;
					case SvPb::OverlayShape::kMarkers:
					{
						auto* pMarker = pShape->mutable_markers();
						if (nullptr != pMarker)
						{
							std::vector<double> values = getDataArray(rTr, pMarker->trpos());
							if (0 < values.size())
							{
								*pMarker->mutable_value() = { values.begin(), values.end() };
							}
						}
					}
					break;
					case SvPb::OverlayShape::kBlockMarker:
					{
						auto* pMarker = pShape->mutable_blockmarker();
						if (nullptr != pMarker)
						{
							switch (pMarker->DataType_case())
							{
							case SvPb::OverlayShapeBlockMarker::kPixelCountingAnalyzerResultPos:
							{
								std::vector<double> values = getDataArray(rTr, pMarker->pixelcountinganalyzerresultpos());
								int lastValue = 0;
								for (int pos = 0; values.size() > pos; ++pos)
								{
									int value = static_cast<int>(values[pos]);
									if (value != lastValue)
									{
										lastValue = value;
										pMarker->add_changepos(pos);
									}
								}
							}
							break;
							default:
								break;
							}
						}
					}
					break;
					case SvPb::OverlayShape::kGraph:
					{
						bool isValid = false;
						auto* pGraph = pShape->mutable_graph();
						if (nullptr != pGraph)
						{
							if (0 < pGraph->trposx())
							{
								std::vector<double> values = getDataArray(rTr, pGraph->trposx());
								if (0 < values.size())
								{
									*pGraph->mutable_x() = { values.begin(), values.end() };
									if (0 == pGraph->trposy() && 0 == pGraph->y_size())
									{
										pGraph->set_minvaluey(0);
										pGraph->set_maxvaluey(static_cast<double>(values.size() - 1));
										std::iota(values.begin(), values.end(), 0.F);
										*pGraph->mutable_y() = { values.begin(), values.end() };
									}
								}
							}

							if (0 < pGraph->trposy())
							{
								std::vector<double> values = getDataArray(rTr, pGraph->trposy());
								if (0 < values.size())
								{
									*pGraph->mutable_y() = { values.begin(), values.end() };
									if (0 == pGraph->trposx() && 0 == pGraph->x_size())
									{
										pGraph->set_minvaluex(0);
										pGraph->set_maxvaluex(static_cast<double>(values.size() - 1));
										std::iota(values.begin(), values.end(), 0.F);
										*pGraph->mutable_x() = { values.begin(), values.end() };
									}
								}
							}

							isValid = (0 < pGraph->x_size() && pGraph->x_size() == pGraph->y_size());
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

	void fillSelectedMarker(const SvPb::SVOSelectedMarker& rSelectedData, const SvTrc::ITriggerRecordR& rTr, double startPos, double stopPos, SvPb::ValueObject* pValue)
	{
		auto x1Array = getDataArray(rTr, rSelectedData.trpos_edgedata());
		if (0 < x1Array.size())
		{
			switch (rSelectedData.type())
			{
			case SvPb::SVOSelectedMarker_PosType_First:
				pValue->set_value(x1Array[0]);
				break;
			case SvPb::SVOSelectedMarker_PosType_Last:
				pValue->set_value(x1Array[x1Array.size() - 1]);
				break;
			case SvPb::SVOSelectedMarker_PosType_This:
				auto pos = rSelectedData.pos();
				long posLong = static_cast<long>(pos);
				double dPercent = pos - posLong;
				if (pos - 1 < static_cast<long>(x1Array.size()))
				{
					bool fromLast = rSelectedData.fromlast();
					double l_dFirstEdge = 0 <= posLong - 1 ? x1Array[posLong - 1] : (fromLast ? stopPos : startPos);
					double l_dSecondEdge = (0 <= posLong && posLong < static_cast<long>(x1Array.size())) ? x1Array[posLong] : (fromLast ? startPos : stopPos);
					double value = l_dFirstEdge + (l_dSecondEdge - l_dFirstEdge) * dPercent;
					value = (0.0 < value) ? value : 0.0;
					pValue->set_value(value);
				}
				break;
			}
		}
	}

}