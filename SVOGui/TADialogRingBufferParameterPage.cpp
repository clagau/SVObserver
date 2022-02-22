//******************************************************************************
//* COPYRIGHT (c) 2005 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogFilterPageClass
//* .File Name       : $Workfile:   SVToolAdjustmentDialogFilterPageClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   24 Oct 2014 11:45:54  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "TADialogRingBufferParameterPage.h"
#include "Definitions/StringTypeDef.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool checkDepthValue(const variant_t& rValue, SvStl::MessageContainer& rMessageContainer)
{
	bool isOk{ true };
	try
	{
		int value = (int)rValue;
		isOk = (SvDef::cRingBufferDepthMin <= value && SvDef::cRingBufferDepthMax >= value);
	}
	catch (...)
	{
		isOk = false;
	}
	if (false == isOk)
	{
		SvDef::StringVector msgList;
		msgList.push_back(SvUl::Format(_T("%d"), SvDef::cRingBufferDepthMin));
		msgList.push_back(SvUl::Format(_T("%d"), SvDef::cRingBufferDepthMax));
		variant_t val;
		HRESULT hr = ::VariantChangeTypeEx(&val, &rValue, SvDef::LCID_USA, VARIANT_ALPHABOOL, VT_BSTR);	// use United States locale
		if (S_OK == hr)
		{
			msgList.push_back(SvUl::createStdString(val));
		}
		else
		{
			msgList.push_back("<undef>");
		}
		rMessageContainer.setMessage(SVMSG_SVO_62_RINGBUFFER_INVALID_VALUE, SvStl::Tid_RingBuffer_Depth_Invalid_ValueString, msgList, SvStl::SourceFileParams(StdMessageParams));
	}
	return isOk;
}
#pragma endregion Declarations

namespace SvOg
{
#pragma region Declarations
	BEGIN_MESSAGE_MAP(TADialogRingBufferParameterPage, CPropertyPage)
		ON_BN_CLICKED(IDC_BUTTON_RING_DEPTH, OnButtonDepth)
		ON_EN_KILLFOCUS(IDC_EDIT_RING_DEPTH, OnKillFocusDepth)
		ON_CONTROL_RANGE(BN_CLICKED, IDC_BUTTON_IMAGE_INDEX1, IDC_BUTTON_IMAGE_INDEX2, OnButtonImageIndex)
		ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_EDIT_IMAGE_INDEX1, IDC_EDIT_IMAGE_INDEX2, OnKillFocusImageIndex)
	END_MESSAGE_MAP()
#pragma endregion Declarations

#pragma region Constructor
	TADialogRingBufferParameterPage::TADialogRingBufferParameterPage( uint32_t inspectionID, uint32_t taskID ) 
		: CPropertyPage(TADialogRingBufferParameterPage::IDD)
		, m_InspectionID(inspectionID)
		, m_TaskObjectID(taskID)
		, m_values{ SvOg::BoundValues{ inspectionID, taskID } }
	{
	}

	TADialogRingBufferParameterPage::~TADialogRingBufferParameterPage()
	{
	}
#pragma endregion Constructor

#pragma region Public Methods
bool TADialogRingBufferParameterPage::QueryAllowExit()
{
	HRESULT hResult = SetPageData();
	return (S_OK == hResult);
}
#pragma endregion Public Methods

#pragma region Protected Methods
	void TADialogRingBufferParameterPage::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);
		DDX_Control(pDX, IDC_EDIT_RING_DEPTH, m_EditRingDepth);
		DDX_Control(pDX, IDC_BUTTON_RING_DEPTH, m_ButtonRingDepth);
		DDX_Control(pDX, IDC_EDIT_IMAGE_INDEX1, m_EditImageIndex[0]);
		DDX_Control(pDX, IDC_EDIT_IMAGE_INDEX2, m_EditImageIndex[1]);
		DDX_Control(pDX, IDC_BUTTON_IMAGE_INDEX1, m_ButtonImageIndex[0]);
		DDX_Control(pDX, IDC_BUTTON_IMAGE_INDEX2, m_ButtonImageIndex[1]);
	}

	BOOL TADialogRingBufferParameterPage::OnInitDialog() 
	{
		CPropertyPage::OnInitDialog();

		m_values.Init();

		m_DepthWidget = std::make_unique<LinkedValueWidgetHelper>(m_EditRingDepth, m_ButtonRingDepth, m_InspectionID, m_TaskObjectID, SvPb::RingBuffer_DepthEId, &m_values, ObjectSelectorData{m_TaskObjectID}, checkDepthValue);
		for (int i=0; i < m_ImageIndexWidget.size(); ++i)
		{
			m_ImageIndexWidget[i] = std::make_unique<LinkedValueWidgetHelper>(m_EditImageIndex[i], m_ButtonImageIndex[i], m_InspectionID, m_TaskObjectID, SvPb::RingBufferIndexEId + i, &m_values, ObjectSelectorData {m_TaskObjectID});
		}

		return TRUE;
	}

	BOOL TADialogRingBufferParameterPage::OnKillActive()
	{
		HRESULT hResult = SetPageData();
		if (S_OK == hResult)
		{
			return CPropertyPage::OnKillActive();
		}
		else
		{
			return FALSE;
		}
	}

	void TADialogRingBufferParameterPage::OnButtonDepth()
	{
		assert(m_DepthWidget);
		if (m_DepthWidget)
		{
			m_DepthWidget->OnButton();
		}
	}

	void TADialogRingBufferParameterPage::OnKillFocusDepth()
	{
		m_DepthWidget->EditboxToValue();
	}

	void TADialogRingBufferParameterPage::OnButtonImageIndex( UINT nID )
	{
		int ButtonIndex( nID-IDC_BUTTON_IMAGE_INDEX1 );

		assert(m_ImageIndexWidget[ButtonIndex]);
		if (m_ImageIndexWidget[ButtonIndex])
		{
			m_ImageIndexWidget[ButtonIndex]->OnButton();
		}
	}

	void TADialogRingBufferParameterPage::OnKillFocusImageIndex(UINT nID)
	{
		int ButtonIndex(nID - IDC_EDIT_IMAGE_INDEX1);

		assert(m_ImageIndexWidget[ButtonIndex]);
		if (m_ImageIndexWidget[ButtonIndex])
		{
			m_ImageIndexWidget[ButtonIndex]->EditboxToValue();
		}
	}
#pragma endregion Protected Methods

#pragma region Private Methods
	HRESULT TADialogRingBufferParameterPage::SetPageData()
	{
		HRESULT Result = SetRingDepth();

		if( S_OK == Result )
		{
			Result = m_values.Commit();

			if( S_OK != Result )
			{
				SvStl::MessageContainerVector errorMessageList;
				errorMessageList = m_values.getFailedMessageList();
				if (0 < errorMessageList.size())
				{
					SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
					Msg.setMessage( errorMessageList[0].getMessage() );
				}
			}
		}

		return Result;
	}

	HRESULT TADialogRingBufferParameterPage::SetRingDepth()
	{
		HRESULT Result( S_OK );
		auto data = m_values.Get<LinkedValueData>(SvPb::RingBuffer_DepthEId);
		if (SvPb::DirectValue == data.m_selectedOption)
		{
			SvStl::MessageContainer messageContainer;
			bool isOk = checkDepthValue(data.m_Value, messageContainer);
			if (false == isOk)
			{
				SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
				Exception.setMessage(messageContainer.getMessage());
				Result = E_FAIL;
			}
		}
		
		return Result;
	}
#pragma endregion Private Methods
} //namespace SvOg
