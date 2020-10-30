//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
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
#pragma endregion Declarations

namespace SvOg
{
#pragma region Declarations
	BEGIN_MESSAGE_MAP(TADialogRingBufferParameterPage, CPropertyPage)
		ON_BN_CLICKED(IDC_BUTTON_RING_DEPTH, OnButtonDepth)
		ON_CONTROL_RANGE(BN_CLICKED, IDC_BUTTON_IMAGE_INDEX1, IDC_BUTTON_IMAGE_INDEX2, OnButtonImageIndex)
	END_MESSAGE_MAP()
#pragma endregion Declarations

#pragma region Constructor
	TADialogRingBufferParameterPage::TADialogRingBufferParameterPage( uint32_t inspectionID, uint32_t taskID ) 
		: CPropertyPage(TADialogRingBufferParameterPage::IDD)
		, m_InspectionID(inspectionID)
		, m_TaskObjectID(taskID)
		, m_objectSelector(inspectionID)
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
		DDX_Control(pDX, IDC_BUTTON_IMAGE_INDEX1, m_ButtonImageIndex1);
		DDX_Control(pDX, IDC_BUTTON_IMAGE_INDEX2, m_ButtonImageIndex2);
	}

	BOOL TADialogRingBufferParameterPage::OnInitDialog() 
	{
		CPropertyPage::OnInitDialog();

		m_values.Init();

		// Put the Down Arrow on the Button
		m_downArrowBitmap.LoadOEMBitmap( OBM_DNARROW );

		m_ButtonRingDepth.SetBitmap(static_cast<HBITMAP> (m_downArrowBitmap));
		m_ButtonImageIndex1.SetBitmap( static_cast<HBITMAP> (m_downArrowBitmap) );
		m_ButtonImageIndex2.SetBitmap( static_cast<HBITMAP> (m_downArrowBitmap) );

		std::string depthString(m_values.Get<CString>(SvPb::RingBufferLink_DepthEId));
		if (depthString.empty())
		{
			depthString = m_values.Get<CString>(SvPb::RingBuffer_DepthEId);
		}
		std::string indexString1( m_values.Get<CString>(SvPb::RingBufferIndexLinkEId) );
		if( indexString1.empty() )
		{
			indexString1 = m_values.Get<CString>(SvPb::RingBufferIndexEId);
		}
		std::string indexString2( m_values.Get<CString>(SvPb::RingBufferIndexLinkEId + 1) );
		if( indexString2.empty() )
		{
			indexString2 = m_values.Get<CString>(SvPb::RingBufferIndexEId+1);
		}
		//set edit controls
		m_EditRingDepth.SetWindowText( depthString.c_str() );
		m_EditImageIndex[0].SetWindowText( indexString1.c_str() );
		m_EditImageIndex[1].SetWindowText( indexString2.c_str() );

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
		CString Temp;
		m_EditRingDepth.GetWindowText(Temp);
		std::string Value = Temp;
		std::string Title = SvUl::LoadStdString(IDS_OBJECTNAME_RINGBUFFER_DEPTH);
		if (m_objectSelector.Show(Value, Title, this, SvPb::allNumberValueObjects))
		{
			m_EditRingDepth.SetWindowText(Value.c_str());
		}
	}

	void TADialogRingBufferParameterPage::OnButtonImageIndex( UINT nID )
	{
		int ButtonIndex( nID-IDC_BUTTON_IMAGE_INDEX1 );

		CString Temp;
		m_EditImageIndex[ButtonIndex].GetWindowText( Temp );
		std::string Value = Temp;
		std::string Title = SvUl::LoadStdString( IDS_OBJECTNAME_RINGBUFFER_INDEX1 + ButtonIndex	 );
		if (m_objectSelector.Show( Value, Title, this, SvPb::allNumberValueObjects))
		{
			m_EditImageIndex[ButtonIndex].SetWindowText( Value.c_str() );
		}
	}
#pragma endregion Protected Methods

#pragma region Private Methods
	HRESULT TADialogRingBufferParameterPage::SetPageData()
	{
		HRESULT Result = SetRingDepth();

		if( S_OK == Result )
		{
			Result = SetImageIndex(0);
		}
		if( S_OK == Result )
		{
			Result = SetImageIndex(1);
		}

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

		CString Text;
		m_EditRingDepth.GetWindowText(Text);
		std::string Value = Text;
		long depth = 0;
		bool isNumber = SvUl::Convert2Number( Value, depth, true );
		if( !isNumber || (SvDef::cRingBufferDepthMin <= depth && SvDef::cRingBufferDepthMax >= depth) )
		{
			m_values.Set<CString>(SvPb::RingBuffer_DepthEId, Text);
		}
		else
		{
			SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display );
			SvDef::StringVector msgList;
			msgList.push_back( SvUl::Format(_T("%d"), SvDef::cRingBufferDepthMin) );
			msgList.push_back( SvUl::Format(_T("%d"), SvDef::cRingBufferDepthMax) );
			msgList.push_back( Value );
			Exception.setMessage( SVMSG_SVO_62_RINGBUFFER_INVALID_VALUE, SvStl::Tid_RingBuffer_Depth_Invalid_ValueString, msgList, SvStl::SourceFileParams(StdMessageParams), Result );
			Result = E_FAIL;
		}

		return Result;
	}

	HRESULT TADialogRingBufferParameterPage::SetImageIndex( int Index )
	{
		HRESULT Result( S_OK );

		if( 0 <= Index && SvDef::cRingBufferNumberOutputImages > Index )
		{
			CString Value;
			m_EditImageIndex[Index].GetWindowText( Value );

			m_values.Set<CString>(SvPb::RingBufferIndexEId+Index, Value);
		}
		return Result;
	}
#pragma endregion Private Methods
} //namespace SvOg
