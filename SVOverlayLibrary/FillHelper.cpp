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
#include "ObjectInterfaces/ITriggerRecordR.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "FillHelper.h"
#pragma endregion Includes

#pragma region Declarations
#pragma endregion Declarations

namespace
{
	const double g_pi = std::acos(-1);

	/// Fill live data to protobuf ValueObject
	/// \param rTr [in] triggerReocrd
	/// \param trPos [in] Position of the data in triggerRecord
	/// \param pValue [in,out] protoBuf Message
	/// \returns bool True if value is set.
	bool fillValue(const SvOi::ITriggerRecordR& rTr, long trPos, SvPb::ValueObject* pValue)
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

	/// Fill live data to protobuf message for rectangle.
	/// \param rTr [in] triggerReocrd
	/// \param rRect [in,out] protoBuf Message
	void fillRect(const SvOi::ITriggerRecordR& rTr, SvPb::OverlayShapeRect& rRect)
	{
		fillValue(rTr, rRect.x().trpos(), rRect.mutable_x());
		fillValue(rTr, rRect.y().trpos(), rRect.mutable_y());
		fillValue(rTr, rRect.w().trpos(), rRect.mutable_w());
		fillValue(rTr, rRect.h().trpos(), rRect.mutable_h());
	}

	std::vector<double> getDataArray(const SvOi::ITriggerRecordR& rTr, int pos)
	{
		if (0 < pos)
		{
			_variant_t varX1 = rTr.getDataValue(pos - 1);
			if (VT_ARRAY & varX1.vt)
			{
				return SvUl::getVectorFromOneDim<double>(varX1);
			}
			else if (varX1.vt != VT_EMPTY)
			{
				return{ varX1 };
			}
		}
		return {};
	}

	/// Fill live data to protobuf message for rectangle with x1/2 data (e.g. for blob-Overlays).
	/// \param rTr [in] triggerReocrd
	/// \param rRectArrayData [in] protoBuf SVObserver-input Data
	/// \param rRectArray [in] protoBuf output Data
	/// \returns bool True if value is set.
	bool fillRectArray(const SvOi::ITriggerRecordR& rTr, const SvPb::SVORectArray12Data& rRectArrayData, SvPb::OverlayShapeRectArray& rRectArray)
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

	/// Fill live data to protobuf message for rectangle with pattern data.
	/// \param rTr [in] triggerReocrd
	/// \param rRectArrayData [in] protoBuf SVObserver-input Data
	/// \param rRectArray [in] protoBuf output Data
	/// \returns bool True if value is set.
	bool fillRectArray(const SvOi::ITriggerRecordR& rTr, const SvPb::SVORectArrayPatternData& rRectArrayData, SvPb::OverlayShapeRectArray& rRectArray)
	{
		int numberOfPattern = 0;
		if (0 < rRectArrayData.numberpos())
		{
			//trPos in PB is one based and in tr it is zero based.
			_variant_t var = rTr.getDataValue(rRectArrayData.numberpos() - 1);
			if (VT_EMPTY != var.vt)
			{
				numberOfPattern = var;
			}
		}
		if (0 == numberOfPattern)
		{
			return false;
		}
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
				double angleTmp = rRectArrayData.centerangle() - angleArray[i];
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

	/// Fill live data to protobuf message for rectangle.
	/// \param rTr [in] triggerReocrd
	/// \param rShape [in] protoBuf SVObserver-input Data
	/// \returns bool True if value is set.
	bool fillRectArray(const SvOi::ITriggerRecordR& rTr, SvPb::OverlayShape& rShape)
	{
		bool isValid = false;
		auto* pRectArray = rShape.mutable_rectarray();
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
		return isValid;
	}

	bool fillGraph(const SvOi::ITriggerRecordR& rTr, SvPb::OverlayShape& rShape)
	{
		bool isValid = false;
		auto* pGraph = rShape.mutable_graph();
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
						//Graph with array from x values is used by now only in vertical linear analyzer,
						//and here we have to display from top and not as defined from button, so set the values from high to low.
						std::generate(values.begin(), values.end(), [n = static_cast<double>(values.size() - 1)]() mutable { return n--; });
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
		return isValid;
	}

	/// Fill live data for a selected Marker
	/// \param rSelectedData [in] Protobuf message for the selected data.
	/// \param rTr [in] triggerRecord
	/// \param startPos [in] start position of the marker area.
	/// \param stopPos [in] stop position of the marker area.
	/// \param pValue [in,out] Value to be filled.
	void fillSelectedMarker(const SvPb::SVOSelectedMarker& rSelectedData, const SvOi::ITriggerRecordR& rTr, double startPos, double stopPos, SvPb::ValueObject* pValue)
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

	bool fillImage(const SvOi::ITriggerRecordR& rTr, SvPb::OverlayShape& rShape)
	{
		auto* pImageData = rShape.mutable_image();
		if (nullptr != pImageData)
		{
			switch (pImageData->DataType_case())
			{
			case SvPb::OverlayShapeImage::kImageDymData:
			{
				auto* pData = pImageData->mutable_imagedymdata();
				if (nullptr == pData) { return false; }
				auto pImage = rTr.getImage(pData->trposimage());
				if (nullptr == pImage || false == pImage->isValid()) { return false; }
				auto bitmapInfo = pImage->getHandle()->GetBitmapInfo();
				long width{  static_cast<long>(std::min<double>(pImageData->w().value(), abs(bitmapInfo.GetWidth()))) };
				long height{ static_cast<long>(std::min<double>(pImageData->h().value(), abs(bitmapInfo.GetHeight()))) };
				bool isHeightNegativ = (0 > bitmapInfo.GetHeight());
				if (0 >= height || 0 >= width) { return false; }
				unsigned char* pValueDest = generateBmpString(*pImageData->mutable_image(), width, height);
				unsigned char* pSrcLine = nullptr;
				auto buffer = pImage->getHandle()->GetBuffer();
				SVMatroxBufferInterface::GetHostAddress(&pSrcLine, buffer);
				long pitchX;
				SVMatroxBufferInterface::Get(buffer, SVPitch, pitchX);
				bool isNonBlack = true;
				char valueArray[4];
				switch (pData->Values_case())
				{
				case SvPb::OverlayShapeImageDynamic::kNonBlack:
					isNonBlack = true;
					*(reinterpret_cast<uint32_t*>(&valueArray)) = pData->nonblack();
					break;
				case SvPb::OverlayShapeImageDynamic::kBlack:
					isNonBlack = false;
					*(reinterpret_cast<uint32_t*>(&valueArray)) = pData->black();
					break;
				default:
					return false;
				}
				
				
				for (int y = 0; y < height; ++y)
				{
					for (int x = 0; x < width; ++x)
					{
						int offsetSrc = (y * pitchX) + x;
						int offsetDest = isHeightNegativ ? (4 * (height-y-1) * width) + (4 * x): (4 * y * width) + (4 * x);
						bool isSet = (isNonBlack && pSrcLine[offsetSrc]) || (false == isNonBlack && 0 == pSrcLine[offsetSrc]);
						if (isSet)
						{
							std::memcpy(pValueDest+offsetDest, valueArray, 4);
						}
						else
						{
							std::memset(pValueDest + offsetDest, 0, 4);
						}
					}
				}
			}
				break;
			default:
				//nothing to do because all static
				break;
			}
		}
		return true;
	}

	bool fillBoundingBox(const SvOi::ITriggerRecordR& rTr, SvPb::Overlay& rOverlay)
	{
		auto* pBoundingBox = rOverlay.mutable_boundingshape();
		if (nullptr == pBoundingBox) { return false; }
		switch (pBoundingBox->shape_case())
		{
		case SvPb::OverlayBoundingShape::kRect:
		{
			auto* pRect = pBoundingBox->mutable_rect();
			if (nullptr == pRect) { return false; }
			fillRect(rTr, *pRect);
		}
		break;
		case SvPb::OverlayBoundingShape::kSlice:
		{
			auto* pSlice = pBoundingBox->mutable_slice();
			if (nullptr == pSlice) { return false; }
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
			if (nullptr == pPerspective) { return false; }
			fillValue(rTr, pPerspective->offset().trpos(), pPerspective->mutable_offset());
		}
		break;
		}
		return true;
	}

	bool fillShape(const SvOi::ITriggerRecordR& rTr, SvPb::OverlayShape& rShape)
	{
		bool isValid{ true };
		fillValue(rTr, rShape.color().trpos(), rShape.mutable_color());
		switch (rShape.shape_case())
		{
		case SvPb::OverlayShape::kRect:
		{
			auto* pRect = rShape.mutable_rect();
			if (nullptr == pRect) { return false; }
			fillRect(rTr, *pRect);
		}
		break;
		case SvPb::OverlayShape::kRectArray:
		{
			isValid = fillRectArray(rTr, rShape);
		}
		break;
		case SvPb::OverlayShape::kMarker:
		{
			auto* pMarker = rShape.mutable_marker();
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
			auto* pMarker = rShape.mutable_markers();
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
			auto* pMarker = rShape.mutable_blockmarker();
			if (nullptr != pMarker)
			{
				if (SvPb::OverlayShapeBlockMarker::kPixelCountingAnalyzerResultPos == pMarker->DataType_case())
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
			}
		}
		break;
		case SvPb::OverlayShape::kGraph:
		{
			isValid = fillGraph(rTr, rShape);
		}
		break;
		case SvPb::OverlayShape::kImage:
		{
			isValid = fillImage(rTr, rShape);
		}
		break;
		}
		return isValid;
	}
}


namespace SvOv
{
	void fillOverlay(SvPb::OverlayDesc& overlayDesc, const SvOi::ITriggerRecordR& rTr)
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

			if (false == fillBoundingBox(rTr, *pOverlay)) { continue; };

			for (int j = 0; j < pOverlay->shapegroups_size(); j++)
			{
				auto* pGroups = pOverlay->mutable_shapegroups(j);
				if (nullptr != pGroups)
				{
					for (int k = 0; k < pGroups->shapes_size(); k++)
					{
						auto* pShape = pGroups->mutable_shapes(k);
						if (nullptr == pShape || false == fillShape(rTr, *pShape))
						{	//delete shape and if empty group
							pGroups->mutable_shapes()->erase(pGroups->shapes().begin() + k);
							--k;
						}
					}
				}
				if (nullptr == pGroups || 0 == pGroups->shapes_size())
				{
					pOverlay->mutable_shapegroups()->erase(pOverlay->shapegroups().begin() + j);
					--j;
				}
			}
		}
	}	
}