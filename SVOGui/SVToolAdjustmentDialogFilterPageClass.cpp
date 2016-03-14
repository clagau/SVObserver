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
#include "SVToolAdjustmentDialogFilterPageClass.h"
#include "ObjectInterfaces/ObjectDefines.h"
#include "SVCustomFilterDlg.h"
#include "SVRankingFilterDlg.h"
#include "SVThinningFilterDlg.h"
#include "SVThickeningFilterDlg.h"
#include "Custom2FilterDlg.h"
#include "SVWatershedFilterDlg.h"
#include "SVObjectLibrary/SVObjectSynchronousCommandTemplate.h"
#include "TextDefinesSvOg.h"
#include "GuiCommands/GetCreatableObjects.h"
#include "GuiCommands/GetTaskObjectInstanceID.h"
#include "GuiCommands/GetAvailableObjects.h"
#include "GuiCommands/InspectionRunOnce.h"
#include "GuiCommands/GetObjectTypeInfo.h"
#include "GuiCommands/ConstructAndInsertTaskObject.h"
#include "GuiCommands/DestroyChildObject.h"
#include "GuiCommands/ShouldInspectionReset.h"
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
		BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogFilterPageClass, CPropertyPage)
			//{{AFX_MSG_MAP(SVToolAdjustmentDialogFilterPageClass)
			ON_BN_CLICKED(IDC_BUTTON4, OnButtonProperties)
			ON_BN_CLICKED(IDC_BUTTON3, OnButtonClearAll)
			ON_BN_CLICKED(IDC_BUTTON2, OnButtonDeleteCurrentFilter)
			ON_BN_CLICKED(IDC_BUTTON1, OnButtonInsertNewFilter)
			ON_LBN_SELCHANGE(IDC_LIST1, OnSelchangeList1)
			//}}AFX_MSG_MAP
		END_MESSAGE_MAP()

		SVToolAdjustmentDialogFilterPageClass::SVToolAdjustmentDialogFilterPageClass(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID) 
		: CPropertyPage(SVToolAdjustmentDialogFilterPageClass::IDD)
		, m_ImageController(rInspectionID, rTaskObjectID)
		, m_InspectionID(rInspectionID)
		, m_TaskObjectID(rTaskObjectID)
		{
			// Get Instance GUID for the Mask Operator...
			typedef GuiCmd::GetTaskObjectInstanceID Command;
			typedef SVSharedPtr<Command> CommandPtr;

			SVObjectTypeInfoStruct info(SVUnaryImageOperatorListObjectType/*SVUnaryImageOperatorObjectType*/, SVNotSetSubObjectType);

			CommandPtr commandPtr = CommandPtr(new Command(m_TaskObjectID, info));
			SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
			HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
			if (S_OK == hr)
			{
				m_UnaryImageOperatorID = commandPtr->GetInstanceID();
			}
		}

		SVToolAdjustmentDialogFilterPageClass::~SVToolAdjustmentDialogFilterPageClass()
		{
		}

		HRESULT SVToolAdjustmentDialogFilterPageClass::SetInspectionData()
		{
			UpdateData( TRUE ); // get data from dialog

			GuiCmd::InspectionRunOncePtr commandPtr = new GuiCmd::InspectionRunOnce( m_InspectionID, m_TaskObjectID );
			SVObjectSynchronousCommandTemplate< GuiCmd::InspectionRunOncePtr > command( m_InspectionID, commandPtr );

			HRESULT hrOk = command.Execute( TWO_MINUTE_CMD_TIMEOUT );

			UpdateData( FALSE );

			return hrOk;
		}
		
		bool SVToolAdjustmentDialogFilterPageClass::setImages()
		{
			bool retVal = false;
			const SvUl::NameGuidList guidList = m_ImageController.GetResultImages();

			if (guidList.size()>0)
			{
				IPictureDisp* pResultImage = m_ImageController.GetImage(guidList[0].second.ToGUID());
				// Set dialog image...
				dialogImage.setImage(pResultImage);
				dialogImage.Refresh();
				retVal = true;
			}

			return retVal;
		}

		void SVToolAdjustmentDialogFilterPageClass::refresh()
		{
			typedef GuiCmd::GetAvailableObjects Command;
			typedef SVSharedPtr<Command> CommandPtr;

			SvUl::NameGuidList availableList;
			CommandPtr commandPtr = new Command(m_UnaryImageOperatorID, SVObjectTypeInfoStruct(SVFilterObjectType, SVNotSetSubObjectType));
			SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
			HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
			if (S_OK == hr)
			{
				availableList = commandPtr->AvailableObjects();
			}

			// Populate filter list box and run filter...
			m_filterListBox.init( availableList, Filter_NoFilter );

			SetInspectionData();

			// Refresh dialog image...
			setImages();
		}

		void SVToolAdjustmentDialogFilterPageClass::OnSelchangeList1() 
		{
			int index = m_filterListBox.GetCurSel();
			SVGUID filterGUID	= m_filterListBox.getGUID(index);
			if( SVInvalidGUID != filterGUID ) 
			{
				typedef GuiCmd::GetObjectTypeInfo Command;
				typedef SVSharedPtr<Command> CommandPtr;
				CommandPtr commandPtr = new Command(filterGUID);
				SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
				HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
				if (S_OK == hr)
				{
					SVObjectTypeInfoStruct info = commandPtr->GetTypeInfo();
				
					m_btnProperties.EnableWindow( SVCustomFilterObjectType == info.SubType 
					|| SVCustom2FilterObjectType == info.SubType
					|| SVRankingFilterObjectType == info.SubType
					|| SVThinningFilterObjectType == info.SubType
					|| SVThickeningFilterObjectType == info.SubType
					|| SVWatershedFilterObjectType == info.SubType );
				}
			}// end if
		}// end OnSelchangeList1

		void SVToolAdjustmentDialogFilterPageClass::DoDataExchange(CDataExchange* pDX)
		{
			CPropertyPage::DoDataExchange(pDX);
			//{{AFX_DATA_MAP(SVToolAdjustmentDialogFilterPageClass)
			DDX_Control(pDX, IDC_BUTTON4, m_btnProperties);
			DDX_Control(pDX, IDC_BUTTON1, insertFilter);
			DDX_Control(pDX, IDC_LIST1, m_filterListBox);
			DDX_Control(pDX, IDC_DIALOGIMAGE, dialogImage);
			DDX_Control(pDX, IDC_COMBO1, m_availableFilterCB);
			//}}AFX_DATA_MAP
		}

		BOOL SVToolAdjustmentDialogFilterPageClass::OnSetActive() 
		{
			setImages();
			return CPropertyPage::OnSetActive();
		}

		BOOL SVToolAdjustmentDialogFilterPageClass::OnInitDialog() 
		{
			CPropertyPage::OnInitDialog();

			m_btnProperties.EnableWindow( FALSE );

			dialogImage.AddTab(_T("Tool Result"));  
			m_ImageController.Init();

			bool res = setImages();
			if(res)
			{
				typedef GuiCmd::GetCreatableObjects Command;
				typedef SVSharedPtr<Command> CommandPtr;

				SvUl::NameGuidList availableList;
				CommandPtr commandPtr = new Command(m_UnaryImageOperatorID, SVObjectTypeInfoStruct(SVFilterObjectType, SVNotSetSubObjectType));
				SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
				HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
				if (S_OK == hr)
				{
					availableList = commandPtr->AvailableObjects();
				}

				m_availableFilterCB.Init(availableList, _T(""), Filter_NoFilterAvailable );

				UpdateData( FALSE );

				refresh();
				return TRUE;
			}

			// Not valid call...
			if( GetParent() )
				GetParent()->SendMessage( WM_CLOSE );
			else
				SendMessage( WM_CLOSE );

			return TRUE;  // return TRUE unless you set the focus to a control
			// EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
		}


		////////////////////////////////////////////////////////////////////////////////
		// .Title       : OnButtonInsertNewFilter
		// -----------------------------------------------------------------------------
		// .Description : Insert New Filter in Current Filter Table
		////////////////////////////////////////////////////////////////////////////////
		// .History
		//	 Date		Author		Comment                                       
		//  :27.05.1997 RO			First Implementation
		//	:20.03.2000 RO			Revised to use new refresh() to update dialog.
		////////////////////////////////////////////////////////////////////////////////
		void SVToolAdjustmentDialogFilterPageClass::OnButtonInsertNewFilter() 
		{ 
			const SVGUID classID = m_availableFilterCB.getSelectedGUID();

			if( SVInvalidGUID != classID )
			{
				int destinyIndex	= m_filterListBox.GetCurSel();

				if( LB_ERR == destinyIndex  ) // First Entryitem
				{
					destinyIndex = 0;
				}

				// Construct and Create the Filter Class Object
				typedef GuiCmd::ConstructAndInsertTaskObject Command;
				typedef SVSharedPtr<Command> CommandPtr;
				CommandPtr commandPtr = new Command(m_UnaryImageOperatorID, classID, destinyIndex);
				SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
				HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
				if (S_OK != hr)
				{
					AfxMessageBox("Creation of Filter Failed");
				}

				// Refresh Dialog...
				refresh();
			}
		}

		////////////////////////////////////////////////////////////////////////////////
		// .Title       : OnButtonClearAll
		// -----------------------------------------------------------------------------
		// .Description : Clear Filter Table
		////////////////////////////////////////////////////////////////////////////////
		// .History
		//	 Date		Author		Comment                                       
		//  :27.05.1997 RO			First Implementation
		//	:20.03.2000 RO			Revised to use new refresh() to update dialog.
		////////////////////////////////////////////////////////////////////////////////
		void SVToolAdjustmentDialogFilterPageClass::OnButtonClearAll() 
		{
			// For all Items in the Selected (Instantiated) Filter list
			typedef GuiCmd::GetAvailableObjects Command;
			typedef SVSharedPtr<Command> CommandPtr;
			SvUl::NameGuidList availableList;
			CommandPtr commandPtr = new Command(m_UnaryImageOperatorID, SVObjectTypeInfoStruct(SVFilterObjectType, SVNotSetSubObjectType));
			SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
			HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
			if (S_OK == hr)
			{
				availableList = commandPtr->AvailableObjects();
			}

			// Delete them
			bool bReset = false;

			// remove all filter items (instantiated)
			size_t listSize = availableList.size();
			for (int i=0; i<listSize; ++i)
			{
				SVGUID filterGUID = availableList[i].second;
				if( SVInvalidGUID != filterGUID )
				{
					typedef GuiCmd::ShouldInspectionReset ResetCommand;
					typedef SVSharedPtr<ResetCommand> ResetCommandPtr;
					ResetCommandPtr resetCommandPtr = new ResetCommand(filterGUID);
					SVObjectSynchronousCommandTemplate<ResetCommandPtr> resetCmd(m_InspectionID, resetCommandPtr);
					HRESULT hr = resetCmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
					bReset |= resetCommandPtr->shouldResetInspection();

					GuiCmd::DestroyChildObject::FlagEnum flag = GuiCmd::DestroyChildObject::Flag_None;
					//if last object and it should be reset, set flag
					if (bReset && listSize-1==i )
					{
						flag = GuiCmd::DestroyChildObject::Flag_SetDefaultInputs_And_ResetInspection;
					}
					// Close, Disconnect and Delete it
					typedef GuiCmd::DestroyChildObject DestroyCommand;
					typedef SVSharedPtr<DestroyCommand> DestroyCommandPtr;
					DestroyCommandPtr destroyCommandPtr = new DestroyCommand(m_UnaryImageOperatorID, filterGUID, flag);
					SVObjectSynchronousCommandTemplate<DestroyCommandPtr> destroyCmd(m_InspectionID, destroyCommandPtr);
					destroyCmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
				}
			}

			m_filterListBox.SetCurSel( -1 );

			// Refresh Dialog...
			refresh();
		}

		////////////////////////////////////////////////////////////////////////////////
		// .Title       : OnButtonDeleteCurrentFilter
		// -----------------------------------------------------------------------------
		// .Description : Delete Current Filter in Current Filter Table
		////////////////////////////////////////////////////////////////////////////////
		// .History
		//	 Date		Author		Comment                                       
		//  :27.05.1997 RO			First Implementation
		//	:20.03.2000 RO			Revised to use new refresh() to update dialog.
		////////////////////////////////////////////////////////////////////////////////
		void SVToolAdjustmentDialogFilterPageClass::OnButtonDeleteCurrentFilter() 
		{
			int index = m_filterListBox.GetCurSel();
			SVGUID filterGUID = m_filterListBox.getGUID(index);
			if( SVInvalidGUID != filterGUID ) 
			{
				typedef GuiCmd::ShouldInspectionReset ResetCommand;
				typedef SVSharedPtr<ResetCommand> ResetCommandPtr;
				ResetCommandPtr resetCommandPtr = new ResetCommand(filterGUID);
				SVObjectSynchronousCommandTemplate<ResetCommandPtr> resetCmd(m_InspectionID, resetCommandPtr);
				HRESULT hr = resetCmd.Execute(TWO_MINUTE_CMD_TIMEOUT);

				if( S_OK == hr )
				{
					GuiCmd::DestroyChildObject::FlagEnum flag = GuiCmd::DestroyChildObject::Flag_None;

					if( resetCommandPtr->shouldResetInspection() )
					{
						flag = GuiCmd::DestroyChildObject::Flag_SetDefaultInputs_And_ResetInspection;
					}

					// Close, Disconnect and Delete it
					typedef GuiCmd::DestroyChildObject DestroyCommand;
					typedef SVSharedPtr<DestroyCommand> DestroyCommandPtr;
					DestroyCommandPtr destroyCommandPtr = new DestroyCommand(m_UnaryImageOperatorID, filterGUID, flag);
					SVObjectSynchronousCommandTemplate<DestroyCommandPtr> destroyCmd(m_InspectionID, destroyCommandPtr);
					destroyCmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
				}

				// Refresh Dialog...
				refresh();
			}
		}


		////////////////////////////////////////////////////////////////////////////////
		// .Title       : OnButtonProperties
		// -----------------------------------------------------------------------------
		// .Description : Change properties of the Current custom Filter in Filter Table
		////////////////////////////////////////////////////////////////////////////////
		// .History
		//	 Date		Author		Comment                                       
		////////////////////////////////////////////////////////////////////////////////
		void SVToolAdjustmentDialogFilterPageClass::OnButtonProperties() 
		{
			int index = m_filterListBox.GetCurSel();
			SVGUID filterGUID	= m_filterListBox.getGUID(index);
			if( SVInvalidGUID != filterGUID ) 
			{
				typedef GuiCmd::GetObjectTypeInfo Command;
				typedef SVSharedPtr<Command> CommandPtr;
				CommandPtr commandPtr = new Command(filterGUID);
				SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
				HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
				if (S_OK == hr)
				{
					SVObjectTypeInfoStruct info = commandPtr->GetTypeInfo();

					switch (info.SubType)
					{
					case SVCustomFilterObjectType:
						{
							SVCustomFilterDlg l_svDlg( m_InspectionID, filterGUID, this );
							l_svDlg.DoModal();
						}
						break;
					case SVCustom2FilterObjectType:
						{
							Custom2FilterDlg l_svDlg( m_InspectionID, filterGUID, this );
							l_svDlg.DoModal();
						}
						break;
					case SVRankingFilterObjectType:
						{
							SVRankingFilterDlg l_svDlg( m_InspectionID, filterGUID, this );
							l_svDlg.DoModal();
						}
						break;
					case SVThinningFilterObjectType:
						{
							SVThinningFilterDlg l_svDlg( m_InspectionID, filterGUID, this );
							l_svDlg.DoModal();
						}
						break;
					case SVThickeningFilterObjectType:
						{
							SVThickeningFilterDlg l_svDlg( m_InspectionID, filterGUID, this );
							l_svDlg.DoModal();
						}
						break;
					case SVWatershedFilterObjectType:
						{
							SVWatershedFilterDlg l_svDlg( m_InspectionID, m_TaskObjectID, filterGUID, this );
							l_svDlg.DoModal();
						}
						break;
					default: //nothing to do
						break;
					} 
				}
			}// end if

			// Refresh Dialog...
			refresh();
		}
	}  //end namespace SVOGUI
}  //end namespace Seidenader

