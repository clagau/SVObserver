#pragma once
#include "SVStatusLibrary/MessageManager.h"
#include "LinkedValueWidgetHelper.h"
#include "SVUtilityLibrary/SVClock.h"


namespace SvOg
{

enum class DrawNodeType
{
	NoDefined,
	BaseImage,
	Rectangle,
	Oval,
	Segment,
	Triangle,
	Lines,
	Points,
	Polygon,
	Text,
	BucketFill
};

enum class DrawNodeSubType
{
	NoDefined,
	MainNode,
	GeneralData,
	SizeData,
	Position,
	Points,
	Color,
	Angle,
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
		static double s_lastDisplay {0};
		static std::string s_lastFieldName;
		static int s_lastValue {-1};

		if (m_useMinMax)
		{
			if (m_value < m_min || m_value > m_max)
			{
				//To avoid to display the message more time (up to 4), a timer is added.
				auto currentTime = SvUl::GetTimeStamp();
				if (currentTime > s_lastDisplay + 100 || s_lastFieldName != m_fieldName || s_lastValue != m_value)
				{
					//timer must be set before calling message (calling of message can lead to call KillFocus, which call the check again.)
					s_lastDisplay = SvUl::GetTimeStamp();
					s_lastFieldName = m_fieldName;
					s_lastValue = m_value;
					SvDef::StringVector additionalTextList {m_fieldName, std::to_string(m_min), std::to_string(m_max)};
					SvStl::MessageManager Msg(SvStl::MsgType::Display);
					Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidParameterValue, additionalTextList, SvStl::SourceFileParams(StdMessageParams));
					//timer must be set after calling message, because the user can need different long to close the Message and after this an focus change can happened.
					s_lastDisplay = SvUl::GetTimeStamp();
				}
				return false;
			}
		}
		return true;
	}

	std::unique_ptr<LinkedValueWidgetHelper> m_Widget = nullptr;
	std::string m_fieldName;
	UINT m_nIDC;
	bool m_useNumberValue = false;
	int m_value = 0;
	bool m_useMinMax = false;
	int m_min = 0;
	int m_max = 255;
};
}