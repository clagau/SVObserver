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
#include "SVStatusLibrary/MessageManagerResource.h"
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
		CString depthString;
		depthString.Format("%d", m_Values.Get<long>(RingDepthTag));
		m_EditRingDepth.SetWindowText(depthString);
		m_EditImageIndex[0].SetWindowText(indexString1.c_str());
		m_EditImageIndex[1].SetWindowText(indexString2.c_str());

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

		CString Value;
		m_EditImageIndex[ButtonIndex].GetWindowText( Value );
		CString Title;
		Title.LoadString( IDS_OBJECTNAME_RINGBUFFER_INDEX1 + ButtonIndex	 );
		if (ShowObjectSelector( Value, Title))
		{
			m_EditImageIndex[ButtonIndex].SetWindowText( Value );
		}
	}

	bool TADialogRingBufferParameterPage::ShowObjectSelector(CString& name, const CString& Title)
	{
		bool result = false;
		SVString InspectionName = GetInspectionName();
		SVString PPQName = GetPPQName();

		SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterInput, InspectionName, SVString( _T("") ) );
		SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterOutput, InspectionName, SVString( _T("") ) );
		SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterOutput, PPQName, SVString( _T("")  ));
		SvOsl::ObjectTreeGenerator::Instance().setSelectorType( SvOsl::ObjectTreeGenerator::TypeSingleObject );

		SvOsl::SelectorOptions BuildOptions( m_InspectionID, SV_SELECTABLE_FOR_EQUATION, GetToolSetGUID() );
		SvOsl::ObjectTreeGenerator::Instance().BuildSelectableItems<GlobalSelector, NoSelector, ToolSetItemSelector<>>( BuildOptions );

		if(0 < name.GetLength())
		{
			SVStringSet nameSet;
			nameSet.insert(name);
			SvOsl::ObjectTreeGenerator::Instance().setCheckItems(nameSet);
		}

		CString mainTabTitle;
		mainTabTitle.LoadString( IDS_SELECT_TOOLSET_OUTPUT );
		CString FilterTab;
		FilterTab.LoadString( IDS_FILTER );

		INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog( Title, mainTabTitle, FilterTab, this );

		if( IDOK == Result )
		{
			name = SvOsl::ObjectTreeGenerator::Instance().getSingleObjectResult().getLocation().c_str(); 
			result = true;
		}

		return result;
	}
#pragma endregion Protected Methods

#pragma region Private Methods
	GUID TADialogRingBufferParameterPage::GetToolSetGUID() const
	{
		GUID toolsetGUID = GUID_NULL;

		typedef GuiCmd::GetTaskObjectInstanceID Command;
		typedef SVSharedPtr<Command> CommandPtr;

		SVObjectTypeInfoStruct info(SVToolSetObjectType);
		CommandPtr commandPtr = CommandPtr(new Command(m_InspectionID, info));
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			toolsetGUID = commandPtr->GetInstanceID();
		}

		return toolsetGUID;
	}

	SVString TADialogRingBufferParameterPage::GetInspectionName() const
	{
		SVString inspectionName;
		typedef GuiCmd::GetObjectName Command;
		typedef SVSharedPtr<Command> CommandPtr;

		CommandPtr commandPtr(new Command(m_InspectionID));
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			inspectionName = commandPtr->GetName();
		}
		return inspectionName;
	}

	SVString TADialogRingBufferParameterPage::GetPPQName() const
	{
		SVString PPQName;
		typedef GuiCmd::GetPPQObjectName Command;
		typedef SVSharedPtr<Command> CommandPtr;

		CommandPtr commandPtr(new Command(m_InspectionID));
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			PPQName = commandPtr->GetName();
		}
		return PPQName;
	}

	HRESULT TADialogRingBufferParameterPage::SetPageData()
	{
		HRESULT hResult = SetRingDepth();

		if (S_OK == hResult)
		{
			hResult = SetImageIndex(0);
		}
		if (S_OK == hResult)
		{
			hResult = SetImageIndex(1);
		}

		return hResult;
	}

	HRESULT TADialogRingBufferParameterPage::SetRingDepth()
	{
		CString csText;
		m_EditRingDepth.GetWindowText(csText);
		SVString value = csText;
		long depth = 0;
		bool isNumber = SvUl_SF::Convert2Number(value, depth, true);
		HRESULT Result = S_OK;
		if (isNumber && SvOi::cRingBufferDepthMin <= depth && SvOi::cRingBufferDepthMax >= depth)
		{
			m_Values.Set<long>(RingDepthTag, depth);
			Result = m_Values.Commit();
		}
		else
		{
			Result = E_FAIL;
		}

		if( S_OK != Result )
		{
			SvStl::MessageMgrDisplayAndNotify Exception( SvStl::LogAndDisplay );
			SVStringArray msgList;
			msgList.push_back(SvUl_SF::Format(_T("%d"), SvOi::cRingBufferDepthMin));
			msgList.push_back(SvUl_SF::Format(_T("%d"), SvOi::cRingBufferDepthMax));
			msgList.push_back(SVString(csText));
			Exception.setMessage( SVMSG_SVO_62_RINGBUFFER_INVALID_VALUE, SvOi::Tid_RingBuffer_Depth_Invalid_ValueString, msgList, SvStl::SourceFileParams(StdMessageParams), Result );
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
			Result = m_Values.Commit();
			if( S_OK != Result )
			{
				SvStl::MessageMgrDisplayAndNotify Exception( SvStl::LogAndDisplay );
				SVStringArray msgList;
				msgList.push_back( SvUl_SF::Format( _T("%d"), Index+1 ));
				Exception.setMessage( SVMSG_SVO_62_RINGBUFFER_INVALID_VALUE, SvOi::Tid_RingBuffer_ImageIndex_Invalid_ValueString, msgList, SvStl::SourceFileParams(StdMessageParams), Result );
			}
		}
		return Result;
	}
#pragma endregion Private Methods
} /* namespace SVOGui */ } /* namespace Seidenader */
