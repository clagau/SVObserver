//*****************************************************************************
/// \copyright (c) 2021 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \Author	Marc Ziegler
//*****************************************************************************
/// Class to Control  the DrawTool part of the GUI.
//*****************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "DrawToolController.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvOg
{
DrawToolController::DrawToolController(ValueController& rValues, std::array<EditCtrlData, 5>& rEditCtrlDataList)
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
			textStr = SvUl::Format("%s Image: ", m_isColor ? "Color" : "Gray");
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
					break;
				}
				case DrawNodeType::Rectangle:
				case DrawNodeType::Oval:
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
				{
					textStr = "Center";
					if (rData.m_pValues)
					{
						long x = rData.m_pValues->Get<LinkedValueData>(SvPb::CenterXEId).m_Value;
						long y = rData.m_pValues->Get<LinkedValueData>(SvPb::CenterYEId).m_Value;
						textStr += SvUl::Format(": %d / %d", x, y);
					}
				}
				break;
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
				case DrawNodeSubType::Position:
					return setPositionData(rData);
				case DrawNodeSubType::SizeData:
					return setSizeData(rData);
				case DrawNodeSubType::Color:
					return setColorData(rData);
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

bool DrawToolController::setSizeData(TreeNodeData& rData)
{
	if (rData.m_pValues)
	{
		return S_OK == rData.m_pValues->Commit(PostAction::doReset | PostAction::doRunOnce, true);
	}
	assert(false);
	return true;
}

bool DrawToolController::setPositionData(TreeNodeData& rData)
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
		return S_OK == rData.m_pValues->Commit(PostAction::doReset | PostAction::doRunOnce, true);
	}
	assert(false);
	return true;
}
}