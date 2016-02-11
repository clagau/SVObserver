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
#include <boost/assign/list_of.hpp>
#include "TADialogRingBufferParameterPage.h"

#include "ObjectInterfaces/ObjectDefines.h"
#include "ObjectSelectorLibrary/ObjectTreeGenerator.h"
#include "SVStatusLibrary/MessageManagerResource.h"
#include "SVMessage/SVMessage.h"
#include "TextDefinesSvOg.h"
#include "GuiCommands/GetObjectName.h"
#include "GuiCommands/GetPPQObjectName.h"
#include "GuiCommands/GetTaskObjectInstanceID.h"
#include "GuiCommands/SetRingbufferIndexValue.h"
#include "GuiCommands/GetRingbufferIndexValue.h"
#include "SVObjectLibrary/SVObjectSynchronousCommandTemplate.h"
#include "SVObjectLibrary/SVClsids.h"
#include "GlobalSelector.h"
#include "NoSelector.h"
#include "ToolSetItemSelector.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace SvOi;

namespace Seidenader
{
	namespace SVOGui
	{
		static const std::string RingDepthTag("RingDepth");

		BEGIN_MESSAGE_MAP(TADialogRingBufferParameterPage, CPropertyPage)
			//{{AFX_MSG_MAP(TADialogRingBufferParameterPage)
			//ON_BN_CLICKED(IDC_BUTTON_RING_DEPTH, OnButtonBufferDepth)
			ON_BN_CLICKED(IDC_BUTTON_IMAGE_INDEX1, OnButtonImageIndex1)
			ON_BN_CLICKED(IDC_BUTTON_IMAGE_INDEX2, OnButtonImageIndex2)
			//}}AFX_MSG_MAP
		END_MESSAGE_MAP()

#pragma region Constructor
		TADialogRingBufferParameterPage::TADialogRingBufferParameterPage( const GUID& rInspectionID, const GUID& rTaskObjectID ) 
			: CPropertyPage(TADialogRingBufferParameterPage::IDD)
			, m_InspectionID(rInspectionID)
			, m_TaskObjectID(rTaskObjectID)
			, m_Values(SvOg::BoundValues(rInspectionID, rTaskObjectID, boost::assign::map_list_of(RingDepthTag, RingBuffer_DepthGuid)))
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
			//{{AFX_DATA_MAP(SVToolAdjustmentDialogFilterPageClass)
			DDX_Control(pDX, IDC_EDIT_RING_DEPTH, m_EditRingDepth);
			DDX_Control(pDX, IDC_EDIT_IMAGE_INDEX1, m_EditImageIndex[0]);
			DDX_Control(pDX, IDC_EDIT_IMAGE_INDEX2, m_EditImageIndex[1]);
			DDX_Control(pDX, IDC_BUTTON_RING_DEPTH, m_ButtonRingDepth);
			DDX_Control(pDX, IDC_BUTTON_IMAGE_INDEX1, m_ButtonImageIndex1);
			DDX_Control(pDX, IDC_BUTTON_IMAGE_INDEX2, m_ButtonImageIndex2);
			//}}AFX_DATA_MAP
		}

		BOOL TADialogRingBufferParameterPage::OnInitDialog() 
		{
			CPropertyPage::OnInitDialog();

			m_Values.Init();

			// Put the Down Arrow on the Button
			m_downArrowBitmap.LoadOEMBitmap( OBM_DNARROW );

			m_ButtonRingDepth.SetBitmap( static_cast<HBITMAP>(m_downArrowBitmap) );
			m_ButtonImageIndex1.SetBitmap( static_cast<HBITMAP>(m_downArrowBitmap) );
			m_ButtonImageIndex2.SetBitmap( static_cast<HBITMAP>(m_downArrowBitmap) );

			typedef GuiCmd::GetRingbufferIndexValue Command;
			typedef SVSharedPtr<Command> CommandPtr;

			CommandPtr commandPtr(new Command(m_TaskObjectID, 0));
			SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
			HRESULT hResult = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
			SVString indexString1("");
			SVString indexString2("");
			if(S_OK == hResult)
			{
				indexString1 = commandPtr->getValue();
			}
			CommandPtr command2Ptr(new Command(m_TaskObjectID, 1));
			SVObjectSynchronousCommandTemplate<CommandPtr> cmd2(m_InspectionID, command2Ptr);
			hResult = cmd2.Execute(TWO_MINUTE_CMD_TIMEOUT);
			if(S_OK == hResult)
			{
				indexString2 = command2Ptr->getValue();
			}
			//set edit controls
			CString depthString;
			depthString.Format("%d", m_Values.Get<long>(RingDepthTag));
			m_EditRingDepth.SetWindowText(depthString);
			m_EditImageIndex[0].SetWindowText(indexString1.c_str());
			m_EditImageIndex[1].SetWindowText(indexString2.c_str());

			//[MZA]: this button is not used yet, but it will needed for the next version which can set the depth per global variables.
			m_ButtonRingDepth.ShowWindow( HIDE_WINDOW );

			return TRUE;  // return TRUE unless you set the focus to a control
			// EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
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

		//void TADialogRingBufferParameterPage::OnButtonBufferDepth()
		//{
		//	CString csText;
		//	m_EditRingDepth.GetWindowText(csText); 
		//	CString Title;
		//	Title.Format(IDS_OBJECTNAME_RINGBUFFER_DEPTH);
		//	if (ShowObjectSelector(csText, Title))
		//	{
		//		m_EditRingDepth.SetWindowText(csText);
		//		SetRingDepth();
		//	}
		//}

		void TADialogRingBufferParameterPage::OnButtonImageIndex1()
		{
			ObjectSelectorForIndex(0, IDS_OBJECTNAME_RINGBUFFER_INDEX1);
		}

		void TADialogRingBufferParameterPage::OnButtonImageIndex2()
		{
			ObjectSelectorForIndex(1, IDS_OBJECTNAME_RINGBUFFER_INDEX2);
		}

		void TADialogRingBufferParameterPage::ObjectSelectorForIndex(int index, int titleStringID)
		{
			CString csText;
			m_EditImageIndex[index].GetWindowText(csText); 
			CString Title;
			Title.Format(titleStringID);
			if (ShowObjectSelector(csText, Title))
			{
				m_EditImageIndex[index].SetWindowText(csText);
				SetImageIndex(index);
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
			bool isNumber = value.Convert2Number(depth, true);
			HRESULT hResult = S_OK;
			if (isNumber && SvOi::IRingBufferTool::m_minRingBufferDepth <= depth && SvOi::IRingBufferTool::m_maxRingBufferDepth >= depth)
			{
				m_Values.Set<long>(RingDepthTag, depth);
				hResult = m_Values.Commit();
			}
			else
			{
				hResult = S_FALSE;
				m_EditRingDepth.SetFocus();
				SvStl::MessageMgrDisplayAndNotify Exception( SvStl::LogAndDisplay );
				CString strText;
				strText.Format(RingBuffer_Depth_Invalid_ValueString, SvOi::IRingBufferTool::m_minRingBufferDepth, SvOi::IRingBufferTool::m_maxRingBufferDepth, csText);
				Exception.setMessage( SVMSG_SVO_62_RINGBUFFER_INVALID_VALUE, strText, StdMessageParams, hResult );
			}
			return hResult;
		}

		HRESULT TADialogRingBufferParameterPage::SetImageIndex(int indexNumber)
		{
			CString csText;
			m_EditImageIndex[indexNumber].GetWindowText(csText);
			typedef GuiCmd::SetRingbufferIndexValue Command;
			typedef SVSharedPtr<Command> CommandPtr;

			CommandPtr commandPtr(new Command(m_TaskObjectID, indexNumber, csText));
			SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
			HRESULT hResult = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
			if(S_OK != hResult)
			{
				m_EditImageIndex[indexNumber].SetFocus();
				SvStl::MessageMgrDisplayAndNotify Exception( SvStl::LogAndDisplay );
				CString strText;
				strText.Format(RingBuffer_ImageIndex_Invalid_ValueString, indexNumber+1);
				Exception.setMessage( SVMSG_SVO_62_RINGBUFFER_INVALID_VALUE, strText, StdMessageParams, hResult );
			}
			return hResult;
		}
#pragma endregion Private Methods
	}  //end namespace SVOGUI
}  //end namespace Seidenader
