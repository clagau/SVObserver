//*****************************************************************************
/// \copyright (c) 2021 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
/// \Author	Marc Ziegler
//*****************************************************************************
/// Helper function for DrawTool - GUI
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "DrawToolHelper.h"
#include "DisplayHelper.h"
#include "CDSVPictureDisplay.h"
#include "Definitions/Color.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
bool isChangable(const SvOg::ValueController& rValueController, SvPb::EmbeddedIdEnum embeddedId)
{
	auto data = rValueController.Get<SvOg::LinkedValueData>(embeddedId);
	return SvPb::LinkedSelectedOption::DirectValue == data.m_selectedOption;
}

std::vector<double> createPointPairVec(SvPb::EmbeddedIdEnum xEid, SvPb::EmbeddedIdEnum yEid, const SvOg::ValueController& rControl)
{
	std::vector<double> retVal;
	auto xVar = rControl.Get<SvOg::LinkedValueData>(xEid).m_Value;
	auto yVar = rControl.Get<SvOg::LinkedValueData>(yEid).m_Value;
	assert(VT_ARRAY == (VT_ARRAY & xVar.vt) && nullptr != xVar.parray && VT_ARRAY == (VT_ARRAY & yVar.vt) && nullptr != yVar.parray);
	if (VT_ARRAY == (VT_ARRAY & xVar.vt) && nullptr != xVar.parray && VT_ARRAY == (VT_ARRAY & yVar.vt) && nullptr != yVar.parray)
	{
		long sizeX = xVar.parray->rgsabound[0].cElements;
		long sizeY = yVar.parray->rgsabound[0].cElements;
		long sizeXY = std::min(sizeX, sizeY);
		for (long i = 0; i < sizeXY; ++i)
		{
			double value;
			if (S_OK == ::SafeArrayGetElement(xVar.parray, &i, &value))
			{
				retVal.push_back(value);
			}
			else
			{
				retVal.push_back(0);
				assert(false);
			}
			if (S_OK == ::SafeArrayGetElement(yVar.parray, &i, &value))
			{
				retVal.push_back(value);
			}
			else
			{
				retVal.push_back(0);
				assert(false);
			}
		}
	}
	return retVal;
}

double safeArrayGetValue(const variant_t& rValue, long pos)
{
	double value = .0;
	assert(VT_ARRAY == (VT_ARRAY & rValue.vt) && nullptr != rValue.parray);
	switch (rValue.vt)
	{
		case VT_ARRAY | VT_R8:
			::SafeArrayGetElement(rValue.parray, &pos, &value);
			break;
		case VT_ARRAY | VT_I4:
		{
			int val;
			::SafeArrayGetElement(rValue.parray, &pos, &val);
			value = val;
			break;
		}
		default:
			assert(false);
			break;
	}

	return value;
}

void setPointPairsToValueController(const variant_t& rValue, SvPb::EmbeddedIdEnum xEid, SvPb::EmbeddedIdEnum yEid, SvOg::ValueController& rControl)
{
	if (VT_ARRAY == (VT_ARRAY & rValue.vt) && nullptr != rValue.parray)
	{
		std::vector<double> xValues, yValues;
		long size = rValue.parray->rgsabound[0].cElements;
		for (long i = 0; i + 1 < size; ++i)
		{
			xValues.push_back(safeArrayGetValue(rValue, i));
			++i;
			yValues.push_back(safeArrayGetValue(rValue, i));
		}

		rControl.Set(xEid, SvUl::vectorToSafeArray(xValues));
		rControl.Set(yEid, SvUl::vectorToSafeArray(yValues));
	}
}

void setArrayToValueController(const variant_t& rValue, SvPb::EmbeddedIdEnum embeddedId, SvOg::ValueController& rControl)
{
	if (VT_ARRAY == (VT_ARRAY & rValue.vt) && nullptr != rValue.parray)
	{
		if ((VT_ARRAY | VT_R8) == rValue.vt)
		{
			rControl.Set(embeddedId, rValue);
		}
		else
		{
			std::vector<double> values;
			long size = rValue.parray->rgsabound[0].cElements;
			for (long i = 0; i < size; ++i)
			{
				values.push_back(safeArrayGetValue(rValue, i));
			}
			rControl.Set(embeddedId, SvUl::vectorToSafeArray(values));
		}
	}
}
}

namespace SvOg
{
std::map<long, variant_t> createOverlayData(DrawNodeType type, const ValueController& rValueController)
{
	std::map<long, variant_t> parMap;
	bool bMove = false;
	bool bAll = false;
	switch (type)
	{
		case DrawNodeType::Rectangle:
		{
			long x = rValueController.Get<LinkedValueData>(SvPb::LeftEId).m_Value;
			long y = rValueController.Get<LinkedValueData>(SvPb::TopEId).m_Value;
			long width = rValueController.Get<LinkedValueData>(SvPb::WidthEId).m_Value;
			long height = rValueController.Get<LinkedValueData>(SvPb::HeightEId).m_Value;
			parMap[CDSVPictureDisplay::P_X1] = x;
			parMap[CDSVPictureDisplay::P_Y1] = y;
			parMap[CDSVPictureDisplay::P_X2] = x + width;
			parMap[CDSVPictureDisplay::P_Y2] = y + height;
			parMap[CDSVPictureDisplay::P_Type] = CDSVPictureDisplay::RectangleROI;
			bMove = isChangable(rValueController, SvPb::LeftEId) && isChangable(rValueController, SvPb::TopEId);
			bAll = bMove && isChangable(rValueController, SvPb::WidthEId) && isChangable(rValueController, SvPb::HeightEId);
			break;
		}
		case DrawNodeType::Oval:
		{
			long x = rValueController.Get<LinkedValueData>(SvPb::CenterXEId).m_Value;
			long y = rValueController.Get<LinkedValueData>(SvPb::CenterYEId).m_Value;
			long width = rValueController.Get<LinkedValueData>(SvPb::WidthEId).m_Value;
			long height = rValueController.Get<LinkedValueData>(SvPb::HeightEId).m_Value;
			parMap[CDSVPictureDisplay::P_X1] = x - width / 2;
			parMap[CDSVPictureDisplay::P_Y1] = y - height / 2;
			parMap[CDSVPictureDisplay::P_X2] = x + static_cast<long>(ceil((width + 1) / 2.0));
			parMap[CDSVPictureDisplay::P_Y2] = y + static_cast<long>(ceil((height + 1) / 2.0));
			parMap[CDSVPictureDisplay::P_Type] = CDSVPictureDisplay::EllipseROI;
			bMove = isChangable(rValueController, SvPb::CenterXEId) && isChangable(rValueController, SvPb::CenterYEId);
			bAll = bMove && isChangable(rValueController, SvPb::WidthEId) && isChangable(rValueController, SvPb::HeightEId);
			break;
		}
		case DrawNodeType::Segment:
		{
			bool isXAxisAngleUsed = (0 != static_cast<int>(rValueController.Get<LinkedValueData>(SvPb::XAxisAngleEId).m_Value));
			if (isXAxisAngleUsed)
			{
				parMap[CDSVPictureDisplay::P_Type] = CDSVPictureDisplay::PolyPointsROI;
				parMap[CDSVPictureDisplay::P_SubType] = CDSVPictureDisplay::SubType_Points;
				std::vector<double> xyVec;
				xyVec.push_back(rValueController.Get<SvOg::LinkedValueData>(SvPb::CenterXEId).m_Value);
				xyVec.push_back(rValueController.Get<SvOg::LinkedValueData>(SvPb::CenterYEId).m_Value);

				COleSafeArray arraySafe;
				arraySafe.CreateOneDim(VT_R8, static_cast<DWORD>(xyVec.size()), xyVec.data());
				parMap[CDSVPictureDisplay::P_ARRAY_XY] = arraySafe;
				bMove = isChangable(rValueController, SvPb::CenterXEId) && isChangable(rValueController, SvPb::CenterYEId);
				bAll = false;
				break;
			}
			else
			{
				long x = rValueController.Get<LinkedValueData>(SvPb::CenterXEId).m_Value;
				long y = rValueController.Get<LinkedValueData>(SvPb::CenterYEId).m_Value;
				long width = rValueController.Get<LinkedValueData>(SvPb::WidthEId).m_Value;
				long height = rValueController.Get<LinkedValueData>(SvPb::HeightEId).m_Value;
				parMap[CDSVPictureDisplay::P_X1] = x - width / 2;
				parMap[CDSVPictureDisplay::P_Y1] = y - height / 2;
				parMap[CDSVPictureDisplay::P_X2] = x + static_cast<long>(ceil((width + 1) / 2.0));
				parMap[CDSVPictureDisplay::P_Y2] = y + static_cast<long>(ceil((height + 1) / 2.0));
				parMap[CDSVPictureDisplay::P_StartAngle] = rValueController.Get<LinkedValueData>(SvPb::StartAngleEId).m_Value;
				parMap[CDSVPictureDisplay::P_StopAngle] = rValueController.Get<LinkedValueData>(SvPb::EndAngleEId).m_Value;
				parMap[CDSVPictureDisplay::P_Type] = CDSVPictureDisplay::SegmentROI;
				bMove = isChangable(rValueController, SvPb::CenterXEId) && isChangable(rValueController, SvPb::CenterYEId);
				bAll = bMove && isChangable(rValueController, SvPb::WidthEId) && isChangable(rValueController, SvPb::HeightEId);
				break;
			}
		}
		case DrawNodeType::Triangle:
			parMap[CDSVPictureDisplay::P_X1] = rValueController.Get<LinkedValueData>(SvPb::X1EId).m_Value;
			parMap[CDSVPictureDisplay::P_Y1] = rValueController.Get<LinkedValueData>(SvPb::Y1EId).m_Value;
			parMap[CDSVPictureDisplay::P_X2] = rValueController.Get<LinkedValueData>(SvPb::X2EId).m_Value;
			parMap[CDSVPictureDisplay::P_Y2] = rValueController.Get<LinkedValueData>(SvPb::Y2EId).m_Value;
			parMap[CDSVPictureDisplay::P_X3] = rValueController.Get<LinkedValueData>(SvPb::X3EId).m_Value;
			parMap[CDSVPictureDisplay::P_Y3] = rValueController.Get<LinkedValueData>(SvPb::Y3EId).m_Value;
			parMap[CDSVPictureDisplay::P_Type] = CDSVPictureDisplay::TriangleROI;
			bMove = isChangable(rValueController, SvPb::X1EId) && isChangable(rValueController, SvPb::X2EId) && isChangable(rValueController, SvPb::X3EId) &&
				isChangable(rValueController, SvPb::Y1EId) && isChangable(rValueController, SvPb::Y2EId) && isChangable(rValueController, SvPb::Y3EId);
			bAll = bMove;
			break;
		case DrawNodeType::Lines: 
			parMap[CDSVPictureDisplay::P_X1] = rValueController.Get<LinkedValueData>(SvPb::X1EId).m_Value;
			parMap[CDSVPictureDisplay::P_Y1] = rValueController.Get<LinkedValueData>(SvPb::Y1EId).m_Value;
			parMap[CDSVPictureDisplay::P_X2] = rValueController.Get<LinkedValueData>(SvPb::X2EId).m_Value;
			parMap[CDSVPictureDisplay::P_Y2] = rValueController.Get<LinkedValueData>(SvPb::Y2EId).m_Value;
			parMap[CDSVPictureDisplay::P_Type] = CDSVPictureDisplay::LineROI;
			bMove = isChangable(rValueController, SvPb::X1EId) && isChangable(rValueController, SvPb::X2EId) &&
				isChangable(rValueController, SvPb::Y1EId) && isChangable(rValueController, SvPb::Y2EId);
			bAll = bMove;
			break;
		case DrawNodeType::Points:
		{
			parMap[CDSVPictureDisplay::P_Type] = CDSVPictureDisplay::PolyPointsROI;
			parMap[CDSVPictureDisplay::P_SubType] = CDSVPictureDisplay::SubType_Points;
			auto xyVec = createPointPairVec(SvPb::X1EId, SvPb::Y1EId, rValueController);

			COleSafeArray arraySafe;
			arraySafe.CreateOneDim(VT_R8, static_cast<DWORD>(xyVec.size()), xyVec.data());
			parMap[CDSVPictureDisplay::P_ARRAY_XY] = arraySafe;
			bMove = isChangable(rValueController, SvPb::X1EId) && isChangable(rValueController, SvPb::Y1EId);
			bAll = false;
			break;
		}
		case DrawNodeType::Polygon:
		{
			parMap[CDSVPictureDisplay::P_Type] = CDSVPictureDisplay::PolyPointsROI;
			parMap[CDSVPictureDisplay::P_SubType] = (5 == rValueController.Get<long>(SvPb::DrawAreaEId)) ? CDSVPictureDisplay::SubType_Polyline : CDSVPictureDisplay::SubType_Polygon;
			auto xyVec = createPointPairVec(SvPb::X1EId, SvPb::Y1EId, rValueController);

			COleSafeArray arraySafe;
			arraySafe.CreateOneDim(VT_R8, static_cast<DWORD>(xyVec.size()), xyVec.data());
			parMap[CDSVPictureDisplay::P_ARRAY_XY] = arraySafe;
			bMove = isChangable(rValueController, SvPb::X1EId) && isChangable(rValueController, SvPb::Y1EId);
			bAll = bMove;
			break;
		}
		case DrawNodeType::Text:
		{
			long charSize = (rValueController.Get<long>(SvPb::FontSizeEId) + 1) * 4;
			long textSize = static_cast<long>(SvUl::createStdString(rValueController.Get<LinkedValueData>(SvPb::TextEId).m_Value).size());
			parMap[CDSVPictureDisplay::P_X1] = rValueController.Get<LinkedValueData>(SvPb::X1EId).m_Value;
			parMap[CDSVPictureDisplay::P_Y1] = rValueController.Get<LinkedValueData>(SvPb::Y1EId).m_Value;
			parMap[CDSVPictureDisplay::P_X2] = static_cast<long>(parMap[CDSVPictureDisplay::P_X1]) + charSize * textSize;
			parMap[CDSVPictureDisplay::P_Y2] = static_cast<long>(parMap[CDSVPictureDisplay::P_Y1]) + 2 * charSize;
			parMap[CDSVPictureDisplay::P_Type] = CDSVPictureDisplay::RectangleROI;
			bMove = isChangable(rValueController, SvPb::X1EId) && isChangable(rValueController, SvPb::Y1EId);
			bAll = false;
			break;
		}
		case DrawNodeType::BucketFill:
		{
			parMap[CDSVPictureDisplay::P_Type] = CDSVPictureDisplay::PolyPointsROI;
			parMap[CDSVPictureDisplay::P_SubType] = CDSVPictureDisplay::SubType_Points;
			std::vector<double> xyVec;
			xyVec.push_back(rValueController.Get<SvOg::LinkedValueData>(SvPb::X1EId).m_Value);
			xyVec.push_back(rValueController.Get<SvOg::LinkedValueData>(SvPb::Y1EId).m_Value);

			COleSafeArray arraySafe;
			arraySafe.CreateOneDim(VT_R8, static_cast<DWORD>(xyVec.size()), xyVec.data());
			parMap[CDSVPictureDisplay::P_ARRAY_XY] = arraySafe;
			bMove = isChangable(rValueController, SvPb::X1EId) && isChangable(rValueController, SvPb::Y1EId);
			bAll = false;
			break;
		}
		default:
			return {};
	}
	parMap[CDSVPictureDisplay::P_Color] = SvDef::Green;
	parMap[CDSVPictureDisplay::P_SelectedColor] = SvDef::Green;
	parMap[CDSVPictureDisplay::P_AllowEdit] = bAll ? CDSVPictureDisplay::AllowResizeAndMove : bMove ? CDSVPictureDisplay::AllowMove : CDSVPictureDisplay::AllowNone;
	return parMap;
}

void setOverlayProperties(DrawNodeType type, ValueController& rValueController, VARIANT* ParameterList, VARIANT* ParameterValue)
{
	VariantParamMap ParaMap;
	SvOg::DisplayHelper::FillParameterMap(ParaMap, ParameterList, ParameterValue);
	bool isResizeable = 0 != (static_cast<int>(ParaMap[CDSVPictureDisplay::P_AllowEdit]) & static_cast<int>(CDSVPictureDisplay::AllowResize));
	bool isMoveable = 0 != (static_cast<int>(ParaMap[CDSVPictureDisplay::P_AllowEdit]) & static_cast<int>(CDSVPictureDisplay::AllowMove));

	long width = ParaMap[CDSVPictureDisplay::P_X2].lVal - ParaMap[CDSVPictureDisplay::P_X1].lVal;
	long Height = ParaMap[CDSVPictureDisplay::P_Y2].lVal - ParaMap[CDSVPictureDisplay::P_Y1].lVal;

	switch (type)
	{
		case DrawNodeType::Rectangle:
		{
			if (isResizeable)
			{
				rValueController.Set(SvPb::WidthEId, width);
				rValueController.Set(SvPb::HeightEId, Height);
			}
			if (isMoveable)
			{
				rValueController.Set(SvPb::LeftEId, ParaMap[CDSVPictureDisplay::P_X1].lVal);
				rValueController.Set(SvPb::TopEId, ParaMap[CDSVPictureDisplay::P_Y1].lVal);
			}
			break;
		}
		case DrawNodeType::Oval:
		{
			if (isResizeable)
			{
				rValueController.Set(SvPb::WidthEId, width);
				rValueController.Set(SvPb::HeightEId, Height);
			}
			if (isMoveable)
			{
				long CenterX = ParaMap[CDSVPictureDisplay::P_X1].lVal + width / 2;
				long CenterY = ParaMap[CDSVPictureDisplay::P_Y1].lVal + Height / 2;
				rValueController.Set(SvPb::CenterXEId, CenterX);
				rValueController.Set(SvPb::CenterYEId, CenterY);
			}
			break;
		}
		case DrawNodeType::Segment:
			if (isResizeable || isMoveable)
			{
				bool isXAxisAngleUsed = (0 != static_cast<int>(rValueController.Get<LinkedValueData>(SvPb::XAxisAngleEId).m_Value));
				if (isXAxisAngleUsed)
				{
					setPointPairsToValueController(ParaMap[CDSVPictureDisplay::P_ARRAY_XY], SvPb::CenterXEId, SvPb::CenterYEId, rValueController);
				}
				else
				{
					if (isResizeable)
					{
						rValueController.Set(SvPb::WidthEId, width);
						rValueController.Set(SvPb::HeightEId, Height);
						rValueController.Set(SvPb::StartAngleEId, ParaMap[CDSVPictureDisplay::P_StartAngle]);
						rValueController.Set(SvPb::EndAngleEId, ParaMap[CDSVPictureDisplay::P_StopAngle]);
					}
					if (isMoveable)
					{
						long CenterX = ParaMap[CDSVPictureDisplay::P_X1].lVal + width / 2;
						long CenterY = ParaMap[CDSVPictureDisplay::P_Y1].lVal + Height / 2;
						rValueController.Set(SvPb::CenterXEId, CenterX);
						rValueController.Set(SvPb::CenterYEId, CenterY);
					}
				}
			}
			break;
		case DrawNodeType::Triangle:
			if (isResizeable || isMoveable)
			{
				rValueController.Set(SvPb::X1EId, ParaMap[CDSVPictureDisplay::P_X1].lVal);
				rValueController.Set(SvPb::Y1EId, ParaMap[CDSVPictureDisplay::P_Y1].lVal);
				rValueController.Set(SvPb::X2EId, ParaMap[CDSVPictureDisplay::P_X2].lVal);
				rValueController.Set(SvPb::Y2EId, ParaMap[CDSVPictureDisplay::P_Y2].lVal);
				rValueController.Set(SvPb::X3EId, ParaMap[CDSVPictureDisplay::P_X3].lVal);
				rValueController.Set(SvPb::Y3EId, ParaMap[CDSVPictureDisplay::P_Y3].lVal);
			}
			break;
		case DrawNodeType::Lines:
			if (isResizeable || isMoveable)
			{
				setArrayToValueController(ParaMap[CDSVPictureDisplay::P_X1], SvPb::X1EId, rValueController);
				setArrayToValueController(ParaMap[CDSVPictureDisplay::P_Y1], SvPb::Y1EId, rValueController);
				setArrayToValueController(ParaMap[CDSVPictureDisplay::P_X2], SvPb::X2EId, rValueController);
				setArrayToValueController(ParaMap[CDSVPictureDisplay::P_Y2], SvPb::Y2EId, rValueController);
			}
			break;
		case DrawNodeType::Points:
			if (isResizeable || isMoveable)
			{
				setPointPairsToValueController(ParaMap[CDSVPictureDisplay::P_ARRAY_XY], SvPb::X1EId, SvPb::Y1EId, rValueController);
			}
			break;
		case DrawNodeType::Polygon:
			if (isResizeable || isMoveable)
			{
				setPointPairsToValueController(ParaMap[CDSVPictureDisplay::P_ARRAY_XY], SvPb::X1EId, SvPb::Y1EId, rValueController);
			}
			break;
		case DrawNodeType::Text:
			if (isMoveable)
			{
				rValueController.Set(SvPb::X1EId, ParaMap[CDSVPictureDisplay::P_X1].lVal);
				rValueController.Set(SvPb::Y1EId, ParaMap[CDSVPictureDisplay::P_Y1].lVal);
			}
			break;
		case DrawNodeType::BucketFill:
			if (isResizeable || isMoveable)
			{
				setPointPairsToValueController(ParaMap[CDSVPictureDisplay::P_ARRAY_XY], SvPb::X1EId, SvPb::Y1EId, rValueController);
			}
			break;
		default:
			break;
	}

	rValueController.Commit();
}
}