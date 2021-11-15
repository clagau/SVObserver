//*****************************************************************************
/// \copyright (c) 2021 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \Author	Marc Ziegler
//*****************************************************************************
/// Class to Control  the DrawTool part of the GUI.
//*****************************************************************************
#pragma once
#include "DrawToolHelperStructs.h"
//#include "DataController.h"

namespace SvOg
{
class ValueController;

class DrawToolController
{
public:
	DrawToolController(ValueController& rValues, std::array<EditCtrlData, 5>& rEditCtrlDataList);
	void init();

	std::string getNodeText(TreeNodeData& rData);
	bool setBOSAData(TreeNodeData& rData);
	bool setBaseImageGeneralData();
	bool setBaseImageSizeData();

	bool useBackgroundImage() const { return m_useBackgroundImage; };
	void setUseBackgroundImage(bool isUsed) { m_useBackgroundImage = isUsed; };
	bool isColor() const { return m_isColor; };
	void setIsColor(bool isColor) { m_isColor = isColor; };
	bool isAutoFit() const { return m_useAutoFit; };
	void setAutoFit(bool useAutofit) { m_useAutoFit = useAutofit; };

private:
	bool setSizeData(TreeNodeData& rData);
	bool setPositionData(TreeNodeData& rData);
	bool setColorData(TreeNodeData& rData);

private:
	std::array<EditCtrlData, 5>& m_rEditCtrlDataList;
	ValueController& m_rValues;

	bool m_useBackgroundImage = false;
	bool m_isColor = false;
	bool m_useAutoFit = false;
};
}