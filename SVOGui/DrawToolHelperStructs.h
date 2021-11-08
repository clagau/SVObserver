#pragma once
#include "SVStatusLibrary/MessageManager.h"
#include "LinkedValueWidgetHelper.h"


namespace SvOg
{

enum class DrawNodeType
{
	NoDefined,
	BaseImage,
	Rectangle,
	Oval
};

enum class DrawNodeSubType
{
	NoDefined,
	MainNode,
	GeneralData,
	SizeData,
	Position,
	Color
};

struct TreeNodeData
{
	uint32_t m_objectId = SvDef::InvalidObjectId;
	DrawNodeType m_type = DrawNodeType::NoDefined;
	DrawNodeSubType m_subType = DrawNodeSubType::NoDefined;
	std::shared_ptr<ValueController> m_pValues = nullptr;
	HTREEITEM m_lastInternItem = TVI_LAST;
};

class EditCtrlData
{
public:
	explicit EditCtrlData(int nIDC) : m_nIDC(nIDC) {};
	bool checkLimitsAndDisplayError() const
	{
		if (m_useMinMax)
		{
			if (m_value < m_min || m_value > m_max)
			{
				SvDef::StringVector additionalTextList {m_fieldName, std::to_string(m_min), std::to_string(m_max)};
				SvStl::MessageManager Msg(SvStl::MsgType::Display);
				Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidParameterValue, additionalTextList, SvStl::SourceFileParams(StdMessageParams));
				return false;
			}
		}
		return true;
	}

	std::unique_ptr<LinkedValueWidgetHelper> m_Widget;
	std::string m_fieldName;
	UINT m_nIDC;
	int m_value = 0;
	bool m_useMinMax = false;
	int m_min = 0;
	int m_max = 255;
};
}