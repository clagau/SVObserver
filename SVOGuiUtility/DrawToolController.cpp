//*****************************************************************************
/// \copyright (c) 2021 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
/// \Author	Marc Ziegler
//*****************************************************************************
/// Class to Control  the DrawTool part of the GUI.
//*****************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "DrawToolController.h"
#include "DataController.h"
#pragma endregion Includes

namespace SvOgu
{
DrawToolController::DrawToolController(ValueController& rValues, std::array<EditCtrlData, NumberOfCtrls>& rEditCtrlDataList)
	: m_rEditCtrlDataList{rEditCtrlDataList}
	, m_rValues{rValues}
{
}

void DrawToolController::init()
{
	m_useBackgroundImage = m_rValues.Get<bool>(SvPb::UseBackgroundImageEId);
	m_isColor = m_rValues.Get<bool>(SvPb::IsColorImageEId);
	m_useAutoFit = m_rValues.Get<bool>(SvPb::AutoFitSizeEId);
}

std::string DrawToolController::getNodeText(TreeNodeData& rData)
{
	std::string textStr;

	switch (rData.m_subType)
	{
		case DrawNodeSubType::GeneralData:
		{
			switch (rData.m_type)
			{
				case DrawNodeType::BaseImage:
					textStr = SvUl::Format("%s Image: ", m_isColor ? "Color" : "Gray");
					break;
				case DrawNodeType::Text:
					textStr = "General Data";
					if (nullptr != rData.m_pValues)
					{
						auto list = rData.m_pValues->GetEnumTypes(SvPb::FontSizeEId);
						auto tmpValue = rData.m_pValues->Get<long>(SvPb::FontSizeEId);
						auto iter = std::ranges::find_if(list, [tmpValue](const auto& rEntry) { return rEntry.second == tmpValue; });
						if (list.end() != iter)
						{
							textStr += ": " + iter->first;
						}
						double scaleX = rData.m_pValues->Get<LinkedValueData>(SvPb::FontScaleXEId).m_Value;
						double scaleY = rData.m_pValues->Get<LinkedValueData>(SvPb::FontScaleYEId).m_Value;

						if (1. != scaleX || 1. != scaleY)
						{
							textStr += SvUl::Format("(%.2f/%.2f)", scaleX, scaleY);
						}

						_variant_t textVar = rData.m_pValues->Get<LinkedValueData>(SvPb::TextEId).m_Value;
						if (VT_BSTR == textVar.vt)
						{
							std::string text = SvUl::createStdString(textVar.bstrVal);
							textStr += SvUl::Format(", \"%s\"", text.c_str());
						}
					}
					break;
				default:
					textStr = "General Data";
					if (nullptr != rData.m_pValues)
					{
						auto list = rData.m_pValues->GetEnumTypes(SvPb::DrawAreaEId);
						auto tmpValue = rData.m_pValues->Get<long>(SvPb::DrawAreaEId);
						auto iter = std::ranges::find_if(list, [tmpValue](const auto& rEntry) { return rEntry.second == tmpValue; });
						if (list.end() != iter)
						{
							textStr += ": " + iter->first;
						}
					}
					break;
			}
			break;
		}
		case DrawNodeSubType::SizeData:
		{
			textStr = "Size";
			switch (rData.m_type)
			{
				case DrawNodeType::BaseImage:
				{
					long width = m_rValues.Get<long>(SvPb::ExtentWidthEId);
					long height = m_rValues.Get<long>(SvPb::ExtentHeightEId);
					textStr += SvUl::Format(": %d / %d", width, height);
					if (isAutoFit() && useBackgroundImage())
					{
						textStr += "; Autofit";
					}
					break;
				}
				case DrawNodeType::Rectangle:
				case DrawNodeType::Oval:
				case DrawNodeType::Segment:
				{
					if (rData.m_pValues)
					{
						long width = rData.m_pValues->Get<LinkedValueData>(SvPb::WidthEId).m_Value;
						long height = rData.m_pValues->Get<LinkedValueData>(SvPb::HeightEId).m_Value;
						textStr += SvUl::Format(": %d / %d", width, height);
					}
					break;
				}
			}
			break;
		}

		case DrawNodeSubType::Position:
		{
			switch (rData.m_type)
			{
				case DrawNodeType::Rectangle:
				{
					textStr = "Position";
					if (rData.m_pValues)
					{
						long x = rData.m_pValues->Get<LinkedValueData>(SvPb::LeftEId).m_Value;
						long y = rData.m_pValues->Get<LinkedValueData>(SvPb::TopEId).m_Value;
						textStr += SvUl::Format(": %d / %d", x, y);
					}
					break;
				}
				case DrawNodeType::Oval:
				case DrawNodeType::Segment:
				{
					textStr = "Center";
					if (rData.m_pValues)
					{
						long x = rData.m_pValues->Get<LinkedValueData>(SvPb::CenterXEId).m_Value;
						long y = rData.m_pValues->Get<LinkedValueData>(SvPb::CenterYEId).m_Value;
						textStr += SvUl::Format(": %d / %d", x, y);
					}
					break;
				}
			}
			break;
		}
		case DrawNodeSubType::Points:
		{
			textStr = "Points";
			switch (rData.m_type)
			{
				case DrawNodeType::Triangle:
				{
					if (rData.m_pValues)
					{
						long x1 = rData.m_pValues->Get<LinkedValueData>(SvPb::X1EId).m_Value;
						long y1 = rData.m_pValues->Get<LinkedValueData>(SvPb::Y1EId).m_Value;
						long x2 = rData.m_pValues->Get<LinkedValueData>(SvPb::X2EId).m_Value;
						long y2 = rData.m_pValues->Get<LinkedValueData>(SvPb::Y2EId).m_Value;
						long x3 = rData.m_pValues->Get<LinkedValueData>(SvPb::X3EId).m_Value;
						long y3 = rData.m_pValues->Get<LinkedValueData>(SvPb::Y3EId).m_Value;
						textStr += SvUl::Format(": %d / %d; %d / %d; %d / %d ", x1, y1, x2, y2, x3, y3);
					}
					break;
				}
				case DrawNodeType::Lines:
				{
					if (rData.m_pValues)
					{
						auto rXValueData = rData.m_pValues->Get<LinkedValueData>(SvPb::X1EId);
						auto rYValueData = rData.m_pValues->Get<LinkedValueData>(SvPb::Y1EId);
						auto xValues = SvUl::SafeArrayToVector<double>(rXValueData.m_Value.parray);
						auto yValues = SvUl::SafeArrayToVector<double>(rYValueData.m_Value.parray);
						auto rX2ValueData = rData.m_pValues->Get<LinkedValueData>(SvPb::X2EId);
						auto rY2ValueData = rData.m_pValues->Get<LinkedValueData>(SvPb::Y2EId);
						auto x2Values = SvUl::SafeArrayToVector<double>(rX2ValueData.m_Value.parray);
						auto y2Values = SvUl::SafeArrayToVector<double>(rY2ValueData.m_Value.parray);
						auto numbers = std::min({xValues.size(), yValues.size(),x2Values.size(), y2Values.size()});
						textStr += SvUl::Format(": %d Lines", numbers);
					}
					break;
				}
				break;
				case DrawNodeType::Points:
				case DrawNodeType::Polygon:
				{
					if (rData.m_pValues)
					{
						auto rXValueData = rData.m_pValues->Get<LinkedValueData>(SvPb::X1EId);
						auto rYValueData = rData.m_pValues->Get<LinkedValueData>(SvPb::Y1EId);
						auto xValues = SvUl::SafeArrayToVector<double>(rXValueData.m_Value.parray);
						auto yValues = SvUl::SafeArrayToVector<double>(rYValueData.m_Value.parray);
						auto numbers = std::min(xValues.size(), yValues.size());
						textStr += SvUl::Format(": %d Points", numbers);
					}
					break;
				}
				break;
				case DrawNodeType::Text:
				{
					if (rData.m_pValues)
					{
						long x = rData.m_pValues->Get<LinkedValueData>(SvPb::X1EId).m_Value;
						long y = rData.m_pValues->Get<LinkedValueData>(SvPb::Y1EId).m_Value;
						textStr = SvUl::Format("Position: %d / %d", x, y);
					}
					break;
				}
				case DrawNodeType::BucketFill:
				{
					if (rData.m_pValues)
					{
						long x = rData.m_pValues->Get<LinkedValueData>(SvPb::X1EId).m_Value;
						long y = rData.m_pValues->Get<LinkedValueData>(SvPb::Y1EId).m_Value;
						textStr = SvUl::Format("Point: %d / %d", x, y);
					}
					break;
				}
			}
			break;
		}
		case DrawNodeSubType::Angle:
		{
			textStr = "Angle";
			switch (rData.m_type)
			{
				case DrawNodeType::Segment:
				{
					if (rData.m_pValues)
					{
						double start = rData.m_pValues->Get<LinkedValueData>(SvPb::StartAngleEId).m_Value;
						double stop = rData.m_pValues->Get<LinkedValueData>(SvPb::EndAngleEId).m_Value;
						double xAxis = rData.m_pValues->Get<LinkedValueData>(SvPb::XAxisAngleEId).m_Value;
						textStr += SvUl::Format(": %.1f / %.1f / %.1f", start, stop, xAxis);
					}
					break;
				}
			}
			break;
		}
		case DrawNodeSubType::Color:
		{
			textStr = "Color";
			if (rData.m_pValues)
			{
				byte color1 = rData.m_pValues->Get<byte>(SvPb::Color1EId);
				if (m_isColor)
				{
					byte color2 = rData.m_pValues->Get<byte>(SvPb::Color2EId);
					byte color3 = rData.m_pValues->Get<byte>(SvPb::Color3EId);
					textStr += SvUl::Format(": %d / %d / %d", color1, color2, color3);
				}
				else
				{
					textStr += SvUl::Format(": %d", color1);
				}
			}
			break;
		}
	}
	return textStr;
}

bool DrawToolController::setBOSAData(TreeNodeData& rData)
{
	switch (rData.m_type)
	{
		case DrawNodeType::BaseImage:
			switch (rData.m_subType)
			{
				case DrawNodeSubType::GeneralData:
					return setBaseImageGeneralData();
				case DrawNodeSubType::SizeData:
					return setBaseImageSizeData();
				case DrawNodeSubType::Color:
					return setColorData(rData);
				default:
					break;
			}
			break;
		default:
			switch (rData.m_subType)
			{
				case DrawNodeSubType::GeneralData:
					return setCommonData(rData);
				case DrawNodeSubType::Position:
					return setCommonData(rData);
				case DrawNodeSubType::Points:
					return setCommonData(rData);
				case DrawNodeSubType::SizeData:
					return setCommonData(rData);
				case DrawNodeSubType::Color:
					return setColorData(rData);
				case DrawNodeSubType::Angle:
					return setCommonData(rData);
				default:
					break;
			}
			break;
	}

	return true;
}

bool DrawToolController::setBaseImageGeneralData()
{
	m_rValues.Set(SvPb::UseBackgroundImageEId, m_useBackgroundImage);
	m_rValues.Set(SvPb::IsColorImageEId, m_isColor);
	m_rValues.Set(SvPb::AutoFitSizeEId, m_useAutoFit);

	return S_OK == m_rValues.Commit(PostAction::doReset | PostAction::doRunOnce, true);
}

bool DrawToolController::setBaseImageSizeData()
{
	m_rValues.Set(SvPb::AutoFitSizeEId, m_useAutoFit);
	if (m_rEditCtrlDataList[1].checkLimitsAndDisplayError())
	{
		m_rValues.Set<double>(SvPb::ExtentWidthEId, m_rEditCtrlDataList[1].m_value);
	}
	else
	{
		return false;
	}

	if (m_rEditCtrlDataList[2].checkLimitsAndDisplayError())
	{
		m_rValues.Set<double>(SvPb::ExtentHeightEId, m_rEditCtrlDataList[2].m_value);
	}
	else
	{
		return false;
	}

	bool retValue = S_OK == m_rValues.Commit(PostAction::doReset | PostAction::doRunOnce, true);
	m_rValues.Init();
	return retValue;
}

bool DrawToolController::setCommonData(TreeNodeData& rData)
{
	if (rData.m_pValues)
	{
		return S_OK == rData.m_pValues->Commit(PostAction::doReset | PostAction::doRunOnce, true);
	}
	assert(false);
	return true;
}

bool DrawToolController::setColorData(TreeNodeData& rData)
{
	if (rData.m_pValues)
	{
		if (m_rEditCtrlDataList[1].checkLimitsAndDisplayError())
		{
			rData.m_pValues->Set<byte>(SvPb::Color1EId, static_cast<byte>(m_rEditCtrlDataList[1].m_value));
		}
		else
		{
			return false;
		}

		if (m_isColor)
		{
			if (m_rEditCtrlDataList[2].checkLimitsAndDisplayError())
			{
				rData.m_pValues->Set<byte>(SvPb::Color2EId, static_cast<byte>(m_rEditCtrlDataList[2].m_value));
			}
			else
			{
				return false;
			}
			if (m_rEditCtrlDataList[3].checkLimitsAndDisplayError())
			{
				rData.m_pValues->Set<byte>(SvPb::Color3EId, static_cast<byte>(m_rEditCtrlDataList[3].m_value));
			}
			else
			{
				return false;
			}
		}

		if (DrawNodeType::Text == rData.m_type)
		{
			bool isTransparent = rData.m_pValues->Get<bool>(SvPb::IsBackgroundTransparentEId);
			if (false == isTransparent)
			{
				if (m_rEditCtrlDataList[6].checkLimitsAndDisplayError())
				{
					rData.m_pValues->Set<byte>(SvPb::BackgroundColor1EId, static_cast<byte>(m_rEditCtrlDataList[6].m_value));
				}
				else
				{
					return false;
				}

				if (m_isColor)
				{
					if (m_rEditCtrlDataList[7].checkLimitsAndDisplayError())
					{
						rData.m_pValues->Set<byte>(SvPb::BackgroundColor2EId, static_cast<byte>(m_rEditCtrlDataList[7].m_value));
					}
					else
					{
						return false;
					}

					if (m_rEditCtrlDataList[8].checkLimitsAndDisplayError())
					{
						rData.m_pValues->Set<byte>(SvPb::BackgroundColor3EId, static_cast<byte>(m_rEditCtrlDataList[8].m_value));
					}
					else
					{
						return false;
					}
				}
			}
		}

		return S_OK == rData.m_pValues->Commit(PostAction::doReset | PostAction::doRunOnce, true);
	}
	assert(false);
	return true;
}
}