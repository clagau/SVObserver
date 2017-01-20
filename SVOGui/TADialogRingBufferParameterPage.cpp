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
//Moved to precompiled header: #include <boost/assign/list_of.hpp>
#include "TADialogRingBufferParameterPage.h"

#include "ObjectInterfaces/ObjectDefines.h"
#include "ObjectSelectorLibrary/ObjectTreeGenerator.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVMessage/SVMessage.h"
#include "TextDefinesSvOg.h"
#include "GuiCommands/GetObjectName.h"
#include "GuiCommands/GetPPQObjectName.h"
#include "GuiCommands/GetTaskObjectInstanceID.h"
#include "SVObjectLibrary/SVObjectSynchronousCommandTemplate.h"
#include "SVObjectLibrary/SVClsids.h"

#include "GlobalSelector.h"
#include "NoSelector.h"
#include "ToolSetItemSelector.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace Seidenader { namespace SVOGui
{
#pragma region Declarations
	static const std::string RingDepthTag("RingDepth");
	static const std::string RingBufferIndexTag[] = { std::string("RingBufferIndex1"), std::string("RingBufferIndex2") };
	static const std::string RingBufferLinkIndexTag[] = { std::string("RingBufferLinkIndex1"), std::string("RingBufferLinkIndex2") };

	BEGIN_MESSAGE_MAP(TADialogRingBufferParameterPage, CPropertyPage)
		ON_CONTROL_RANGE(BN_CLICKED, IDC_BUTTON_IMAGE_INDEX1, IDC_BUTTON_IMAGE_INDEX2, OnButtonImageIndex)
	END_MESSAGE_MAP()
#pragma endregion Declarations

#pragma region Constructor
	TADialogRingBufferParameterPage::TADialogRingBufferParameterPage( const GUID& rInspectionID, const GUID& rTaskObjectID ) 
		: CPropertyPage(TADialogRingBufferParameterPage::IDD)
		, m_InspectionID(rInspectionID)
		, m_TaskObjectID(rTaskObjectID)
		, m_objectSelector(rInspectionID)
		, m_Values(SvOg::BoundValues(rInspectionID, rTaskObjectID, boost::assign::map_list_of
		(RingDepthTag, RingBuffer_DepthGuid)
		(RingBufferIndexTag[0], RingBuffer_IndexGuid[0])
		(RingBufferLinkIndexTag[0], RingBufferLink_IndexGuid[0])
		(RingBufferIndexTag[1], RingBuffer_IndexGuid[1])
		(RingBufferLinkIndexTag[1], RingBufferLink_IndexGuid[1]) ) )
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
		DDX_Control(pDX, IDC_EDIT_IMAGE_INDEX1, m_EditImageIndex[0]);
		DDX_Control(pDX, IDC_EDIT_IMAGE_INDEX2, m_EditImageIndex[1]);
		DDX_Control(pDX, IDC_BUTTON_IMAGE_INDEX1, m_ButtonImageIndex1);
		DDX_Control(pDX, IDC_BUTTON_IMAGE_INDEX2, m_ButtonImageIndex2);
	}

	BOOL TADialogRingBufferParameterPage::OnInitDialog() 
	{
		CPropertyPage::OnInitDialog();

		m_Values.Init();

		// Put the Down Arrow on the Button
		m_downArrowBitmap.LoadOEMBitmap( OBM_DNARROW );

		m_ButtonImageIndex1.SetBitmap( static_cast<HBITMAP> (m_downArrowBitmap) );
		m_ButtonImageIndex2.SetBitmap( static_cast<HBITMAP> (m_downArrowBitmap) );

		SVString indexString1( m_Values.Get<CString>(RingBufferLinkIndexTag[0]) );
		if( indexString1.empty() )
		{
			indexString1 = m_Values.Get<CString>(RingBufferIndexTag[0]);
		}
		SVString indexString2( m_Values.Get<CString>(RingBufferLinkIndexTag[1]) );
		if( indexString2.empty() )
		{
			indexString2 = m_Values.Get<CString>(RingBufferIndexTag[1]);
		}
		//set edit controls
		SVString depthString = SvUl_SF::Format( _T("%d"), m_Values.Get<long>(RingDepthTag) );
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

	void TADialogRingBufferParameterPage::OnButtonImageIndex( UINT nID )
	{
		int ButtonIndex( nID-IDC_BUTTON_IMAGE_INDEX1 );

		CString Temp;
		m_EditImageIndex[ButtonIndex].GetWindowText( Temp );
		SVString Value = Temp;
		SVString Title = SvUl_SF::LoadSVString( IDS_OBJECTNAME_RINGBUFFER_INDEX1 + ButtonIndex	 );
		if (m_objectSelector.Show<ToolSetItemSelector<>>( Value, Title, this ))
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
			Result = m_Values.Commit();

			if( S_OK != Result )
			{
				SvStl::MessageContainerVector errorMessageList;
				errorMessageList = m_Values.getCommitErrorList();
				if (0 < errorMessageList.size())
				{
					SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
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
		SVString Value = Text;
		long depth = 0;
		bool isNumber = SvUl_SF::Convert2Number( Value, depth, true );
		if( isNumber && SvOi::cRingBufferDepthMin <= depth && SvOi::cRingBufferDepthMax >= depth )
		{
			m_Values.Set<long>(RingDepthTag, depth);
		}
		else
		{
			SvStl::MessageMgrStd Exception( SvStl::LogAndDisplay );
			SVStringVector msgList;
			msgList.push_back( SvUl_SF::Format(_T("%d"), SvOi::cRingBufferDepthMin) );
			msgList.push_back( SvUl_SF::Format(_T("%d"), SvOi::cRingBufferDepthMax) );
			msgList.push_back( Value );
			Exception.setMessage( SVMSG_SVO_62_RINGBUFFER_INVALID_VALUE, SvOi::Tid_RingBuffer_Depth_Invalid_ValueString, msgList, SvStl::SourceFileParams(StdMessageParams), Result );
			Result = E_FAIL;
		}

		return Result;
	}

	HRESULT TADialogRingBufferParameterPage::SetImageIndex( int Index )
	{
		HRESULT Result( S_OK );

		if( 0 <= Index && SvOi::cRingBufferNumberOutputImages > Index )
		{
			CString Value;
			m_EditImageIndex[Index].GetWindowText( Value );

			m_Values.Set<CString>(RingBufferIndexTag[Index], Value);
		}
		return Result;
	}
#pragma endregion Private Methods
} /* namespace SVOGui */ } /* namespace Seidenader */
