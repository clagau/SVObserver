//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBlobAnalyzerDialog
//* .File Name       : $Workfile:   SVBlobAnalyzerDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.7  $
//* .Check In Date   : $Date:   18 Nov 2014 11:13:46  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVBlobAnalyzerDialog.h"

#include "SVFillBlobDlg.h"
#include "SVBlobFeaturePropertiesDlg.h"
#include "ObjectInterfaces/ISVOApp_Helper.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVProtoBuf/SVO-Enum.h"
#pragma endregion Includes

namespace SvOg
{
	BEGIN_MESSAGE_MAP(SVBlobAnalyzerDialog, CDialog)
		//{{AFX_MSG_MAP(SVBlobAnalyzerDialog)
		ON_BN_CLICKED(IDC_BUTTON1, OnButtonAdd)
		ON_BN_CLICKED(IDC_BUTTON5, OnButtonRemove)
		ON_BN_CLICKED(IDC_BUTTON4, OnButtonSetRange) 
		ON_BN_CLICKED(IDC_SET_SORT_BTN, OnSetSortBtn)
		ON_BN_CLICKED(IDC_ASCENDING, OnAscending)
		ON_WM_DESTROY()
		ON_LBN_SELCHANGE(IDC_LIST2, OnSelchangeList2)
		ON_BN_CLICKED(IDC_NO_OF_BLOBS, OnNbrOfBlobs)
		ON_BN_CLICKED(IDC_EXCLUDE_BLOB, OnExcludeBlob)
		ON_BN_CLICKED(IDC_BTN_FILL_BLOBS, OnBtnFillBlobs)
		ON_BN_CLICKED(IDC_FILL_BLOBS, OnFillBlobs)
		ON_CBN_SELCHANGE(IDC_BLOB_COLOR, OnBlobColor)
		ON_EN_CHANGE(IDC_EDIT_MAX_NBR_BLOBS, OnChangeEditMaxNbrBlobs)
		ON_EN_KILLFOCUS(IDC_EDIT_MAX_NBR_BLOBS, OnKillFocusMaxNbrBlobs)
		ON_BN_CLICKED(IDC_BUTTON_SET_FEATURE_PROPERTIES, OnButtonSetFeatureProperties)
		ON_EN_CHANGE(IDC_EDIT_MAX_BLOB_DATA_ARRAY_SIZE, OnChangeEditMaxBlobDataArraySize)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	////////////////////////////////////////////////////////////////////////////
	//
	//
	SVBlobAnalyzerDialog::SVBlobAnalyzerDialog(uint32_t InspectionID, uint32_t TaskObjectID, CWnd* pParent)
	: CDialog( SVBlobAnalyzerDialog::IDD, pParent )
	, m_InspectionID{ InspectionID }
	, m_TaskObjectID{ TaskObjectID }
	, m_values{ SvOgu::BoundValues{ InspectionID, TaskObjectID } }
	{
	}

	SVBlobAnalyzerDialog::~SVBlobAnalyzerDialog()
	{
	}

	void SVBlobAnalyzerDialog::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(SVBlobAnalyzerDialog)
		DDX_Control(pDX, IDC_EDIT_MAX_BLOB_DATA_ARRAY_SIZE, m_ebMaxBlobDataArraySize);
		DDX_Control(pDX, IDC_EDIT_MAX_NBR_BLOBS, m_ctlMaxNbrBlobs);
		DDX_Control(pDX, IDC_FILL_BLOBS, m_chkFillBlob);
		DDX_Control(pDX, IDC_BTN_FILL_BLOBS, m_btnFillBlobs);
		DDX_Control(pDX, IDC_LIST2, m_lbSelectedFeatures);
		DDX_Control(pDX, IDC_LIST1, m_lbAvailableFeatures);
		DDX_Control(pDX, IDC_BLOB_COLOR, m_cbBlobColor);
		DDX_Text(pDX, IDC_SORT_FEATURE_EDT, m_SortFeatureEdt);
		DDX_Check(pDX, IDC_ASCENDING, m_Ascending);
		DDX_Check(pDX, IDC_EXCLUDE_BLOB, m_bExclude);
		DDX_Text(pDX, IDC_EDIT_MAX_NBR_BLOBS, m_lMaxNumberBlobs);
		DDV_MinMaxLong(pDX, m_lMaxNumberBlobs, 1, 10000);
		DDX_Text(pDX, IDC_EDIT_MAX_BLOB_DATA_ARRAY_SIZE, m_lMaxBlobDataArraySize);
		DDV_MinMaxLong(pDX, m_lMaxBlobDataArraySize, 1, m_lMaxNumberBlobs);
		//}}AFX_DATA_MAP
	}

	//
	// Saved location of dialog.
	//
	static CPoint gPtPosition(0, 0);

	BOOL SVBlobAnalyzerDialog::OnInitDialog() 
	{
		CDialog::OnInitDialog();
	
		m_values.Init();

		// Remove Close Button
		ModifyStyle( WS_SYSMENU, 0, SWP_FRAMECHANGED );


		//      List of not enabled.
		initFeatureListBox(m_lbAvailableFeatures, false);

		//      List of enabled.
		initFeatureListBox(m_lbSelectedFeatures, true);

		const SvDef::NameValueVector& rBlobColorList = m_values.GetEnumTypes(SvPb::BlobColorEId);
		m_cbBlobColor.SetEnumTypes(rBlobColorList);
		long CurrentSelection = m_values.Get<long>(SvPb::BlobColorEId);
		m_cbBlobColor.SetCurSelItemData(CurrentSelection);

		m_FeaturesEnabled = std::string(m_values.Get<CString>(SvPb::BlobEnabledFeaturesEId));
		assert(SvPb::SVBlobFeatureEnum::SV_NUMBER_OF_BLOB_FEATURES == m_FeaturesEnabled.size());

		m_Ascending = m_values.Get<bool>(SvPb::SortAscendingEId);
		m_bExclude = m_values.Get<bool>(SvPb::ExcludeFailedEId);
		m_lMaxNumberBlobs = m_values.Get<long>(SvPb::NbrOfBlobsEId);
		m_lMaxBlobDataArraySize = m_values.Get<long>(SvPb::MaxBlobDataArraySizeEId);

		long SortFeature = m_values.Get<long>(SvPb::SortFeatureEId);
		if (0 <= SortFeature && SvPb::SVBlobFeatureEnum::SV_NUMBER_OF_BLOB_FEATURES > SortFeature)
		{
			m_SortFeatureEdt = m_values.GetName(SvPb::BlobFeatureEId + SortFeature).c_str();
		}
		else
		{
			m_SortFeatureEdt.Empty();
		}

		EnableButtons();

		bool bFillBlobValue = m_values.Get<bool>(SvPb::BlobUseFillEId);
		m_btnFillBlobs.EnableWindow(bFillBlobValue);
		m_chkFillBlob.SetCheck(bFillBlobValue);

		UpdateData (false);

		//
		// Position this dialog at saved (or initial) location.
		//
		BOOL bResult2 = SetWindowPos(nullptr,          // Z order not used
									 gPtPosition.x,
									 gPtPosition.y,
									 0,0,           // Width and Height Not Used
									 SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW 
									);
		assert(bResult2);

		return TRUE;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
	}

	HRESULT SVBlobAnalyzerDialog::SetInspectionData()
	{
		HRESULT Result{E_FAIL};

		UpdateData(true);


		int CurrentSelection = m_cbBlobColor.GetCurSel();
		if (0 <= CurrentSelection)
		{
			long Value = static_cast<long> (m_cbBlobColor.GetItemData(CurrentSelection));
			m_values.Set<long>(SvPb::BlobColorEId, Value);
		}

		m_values.Set<CString>(SvPb::BlobEnabledFeaturesEId, m_FeaturesEnabled.c_str());
		m_values.Set<bool>(SvPb::SortAscendingEId, m_Ascending ? true : false);
		m_values.Set<bool>(SvPb::ExcludeFailedEId, m_bExclude ? true : false);
		bool bFillBlobValue = m_chkFillBlob.GetCheck() ? true : false;
		m_values.Set<bool>(SvPb::BlobUseFillEId, bFillBlobValue);
		m_values.Set<long>(SvPb::NbrOfBlobsEId, m_lMaxNumberBlobs);
		m_values.Set<long>(SvPb::MaxBlobDataArraySizeEId, m_lMaxBlobDataArraySize);

		Result = m_values.Commit();

		return Result;
	}

	/////////////////////////////////////////////////////////////////////////////
	//
	//
	void SVBlobAnalyzerDialog::EnableButtons()
	{
		// Check for features selected
		int count = m_lbSelectedFeatures.GetCount();
		bool l_bSetRange = true;
	
		CWnd* pAscendingCheckBoxWnd = GetDlgItem( IDC_ASCENDING );
		CWnd* pSetSortButtonWnd = GetDlgItem( IDC_SET_SORT_BTN );
		CWnd* pSetRangeButtonWnd = GetDlgItem( IDC_BUTTON4 );

		BOOL state = false;

		if( count ) 
		{
			// Check for the "No Feature" string
			long newIndex = static_cast< long >( m_lbSelectedFeatures.GetItemData( 0 ) );
			if( newIndex != LB_ERR)
			{
				state = true;
				int iIndex = m_lbSelectedFeatures.GetCurSel();
				SvPb::SVBlobFeatureEnum index = static_cast<SvPb::SVBlobFeatureEnum> (m_lbSelectedFeatures.GetItemData(iIndex));

				if (SvPb::SVBlobFeatureEnum::SV_AREA == index || SvPb::SVBlobFeatureEnum::SV_BOXX_MAX == index || SvPb::SVBlobFeatureEnum::SV_BOXX_MIN == index 
					|| SvPb::SVBlobFeatureEnum::SV_BOXY_MAX == index || SvPb::SVBlobFeatureEnum::SV_BOXY_MIN == index)
				{
					GetDlgItem(IDC_BUTTON5)->EnableWindow(false);
				}
				else
				{
					GetDlgItem(IDC_BUTTON5)->EnableWindow(true);
				}

				if (SvPb::SVBlobFeatureEnum::SV_CENTER_X_SOURCE == index || SvPb::SVBlobFeatureEnum::SV_CENTER_Y_SOURCE == index)
				{
					pSetSortButtonWnd->EnableWindow(false);
					pSetRangeButtonWnd->EnableWindow(false);
					l_bSetRange = false;
				}
				else
				{
					pSetSortButtonWnd->EnableWindow(true);
					pSetRangeButtonWnd->EnableWindow(true);
				}
			}
		}

		pAscendingCheckBoxWnd->EnableWindow( state );
		pAscendingCheckBoxWnd->ShowWindow( state ? SW_SHOW : SW_HIDE );

		if ( l_bSetRange )
		{
			pSetRangeButtonWnd->EnableWindow( state );
			pSetRangeButtonWnd->ShowWindow( state ? SW_SHOW : SW_HIDE );
		}
	}

	void SVBlobAnalyzerDialog::OnButtonAdd() 
	{
		long lAvailableIndex = static_cast<long> (m_lbAvailableFeatures.GetItemData( m_lbAvailableFeatures.GetCurSel()));
		long lCurrentIndex = static_cast<long> (m_lbSelectedFeatures.GetItemData( m_lbSelectedFeatures.GetCurSel()));

		if(0 <= lAvailableIndex && SvPb::SVBlobFeatureEnum::SV_NUMBER_OF_BLOB_FEATURES > lAvailableIndex)
		{
			if (LB_ERR == lCurrentIndex)
			{
				m_values.Set(SvPb::SortFeatureEId, lAvailableIndex);

				m_SortFeatureEdt = m_values.GetName(SvPb::BlobFeatureEId + lAvailableIndex).c_str();
			}

			m_FeaturesEnabled[lAvailableIndex] = _T('1');
			m_values.Set<CString>(SvPb::BlobEnabledFeaturesEId, m_FeaturesEnabled.c_str());

			//! Reset the Blob Analyzer
			m_values.Commit(SvOgu::PostAction::doReset | SvOgu::PostAction::doRunOnce);

			//List of not enabled.
			initFeatureListBox(m_lbAvailableFeatures, false);

			//List of enabled.
			initFeatureListBox(m_lbSelectedFeatures, true);
		}
		EnableButtons();

		UpdateData(false);
	}

	void SVBlobAnalyzerDialog::OnButtonRemove() 
	{
		long index = static_cast<long> (m_lbSelectedFeatures.GetItemData(m_lbSelectedFeatures.GetCurSel()));

		if(0 <= index && SvPb::SVBlobFeatureEnum::SV_NUMBER_OF_BLOB_FEATURES > index)
		{
			m_FeaturesEnabled[index] = _T('0');
			m_values.Set<CString>(SvPb::BlobEnabledFeaturesEId, m_FeaturesEnabled.c_str());

			// need to reset the sort index
			long sortIndex = m_values.Get<long>(SvPb::SortFeatureEId);
			int count = m_lbSelectedFeatures.GetCount();

			if (index == sortIndex)
			{
				// Only one Entry and it may be the "No Feature" string
				if( count )
				{
					long newIndex = static_cast<long> ( m_lbSelectedFeatures.GetItemData(0) );
					if( newIndex == LB_ERR)
					{
						m_values.Set<long>(SvPb::SortFeatureEId, 1L);
						m_SortFeatureEdt.Empty ();
					}
					else
					{
						m_values.Set<long>(SvPb::SortFeatureEId, newIndex);
						m_SortFeatureEdt = m_values.GetName(SvPb::BlobFeatureEId + newIndex).c_str();
					}
				}
			}
			else
			{
				if(0 == count) // truly empty list
				{
					m_values.Set<long>(SvPb::SortFeatureEId, -1L);
					m_SortFeatureEdt.Empty();
				}
				// Only one Entry and it may be the "No Feature" string
				else if( count == 1 )
				{
					long newIndex = static_cast<long> ( m_lbSelectedFeatures.GetItemData( 0 ) );
					if(LB_ERR == newIndex)
					{
						m_values.Set<long>(SvPb::SortFeatureEId, -1L);
						m_SortFeatureEdt.Empty();
					}
				}
			}

			//! Reset the Blob Analyzer and then call initFeatureListBox to display the new feature lists
			m_values.Commit(SvOgu::PostAction::doReset | SvOgu::PostAction::doRunOnce);

			//List of not enabled.
			initFeatureListBox(m_lbAvailableFeatures, false);

			//List of enabled.
			initFeatureListBox(m_lbSelectedFeatures, true);
		}

		EnableButtons();

		UpdateData (false);
	}

	/////////////////////////////////////////////////////////////////////////////
	//
	//
	void SVBlobAnalyzerDialog::OnButtonSetRange() 
	{

		int Feature = static_cast<int> (m_lbSelectedFeatures.GetItemData( m_lbSelectedFeatures.GetCurSel()));
	
		if (0 <= Feature)
		{
			SvPb::InspectionCmdRequest requestCmd;
			SvPb::InspectionCmdResponse responseCmd;
			auto* pRequest = requestCmd.mutable_getblobanalyzerinforequest();
			pRequest->set_feature(Feature);
			pRequest->set_featureenabled(false);
			pRequest->set_taskobjectid(m_TaskObjectID);

			HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
			if (S_OK == hr && responseCmd.has_getblobanalyzerinforesponse())
			{
				SvOi::SetupDialogManager(responseCmd.getblobanalyzerinforesponse().resultobjectclassid(), m_InspectionID, responseCmd.getblobanalyzerinforesponse().resultobjectid(), GetSafeHwnd());
			}
			else
			{
				SvStl::MessageManager MesMan(SvStl::MsgType::Log);
				MesMan.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams));
			}
		}
		else
		{
			SvStl::MessageManager MesMan(SvStl::MsgType::Log);
			MesMan.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams));
		}
	}

	void SVBlobAnalyzerDialog::OnOK() 
	{
		SetInspectionData();

		//Reset the tool
		SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		try
		{
			m_values.ResetObject(m_InspectionID, m_TaskObjectID);
			const SvStl::MessageContainerVector& rMessages = m_values.getFailedMessageList();
			if (rMessages.size() > 0 && 0 != rMessages[0].getMessage().m_MessageCode)
			{
				Msg.setMessage(rMessages[0].getMessage());
			}
		}
		catch (const SvStl::MessageContainerVector& rSvE)
		{
			if (0 < rSvE.size())
			{
				Msg.setMessage(rSvE[0].getMessage());
			}
		}
		catch (const SvStl::MessageContainer& rSvE)
		{
			Msg.setMessage(rSvE.getMessage());
		}

		CDialog::OnOK();
	}

	void SVBlobAnalyzerDialog::OnCancel() 
	{
		CDialog::OnCancel();
	}

	void SVBlobAnalyzerDialog::OnSetSortBtn() 
	{
		long lCurrentIndex = static_cast< long >( m_lbSelectedFeatures.GetItemData( m_lbSelectedFeatures.GetCurSel() ) );

		if( lCurrentIndex != LB_ERR && lCurrentIndex >= 0)
		{
			m_values.Set<long>(SvPb::SortFeatureEId, lCurrentIndex);
			m_SortFeatureEdt = m_values.GetName(SvPb::BlobFeatureEId + lCurrentIndex).c_str();
		}
		else
		{
			m_values.Set<long>(SvPb::SortFeatureEId, -1L);
			m_SortFeatureEdt.Empty();
		}
		m_values.Commit();
		UpdateData(false);
	}

	void SVBlobAnalyzerDialog::OnAscending() 
	{
		UpdateData(true);
	}

	void SVBlobAnalyzerDialog::OnDestroy() 
	{
		CDialog::OnDestroy();
	
		//
		// Save the current location of the property sheet.
		//
		CRect rectWindow;
		GetWindowRect(&rectWindow);
		gPtPosition.x = rectWindow.left;
		gPtPosition.y = rectWindow.top;

		// TODO: Add your message handler code here
	}

	// Features Area, Box X Max, Box X Min, Box Y Max and Box Y Min should not be removed from the list. 
	// So if the selection is one of these features, disable the remove button.
	void SVBlobAnalyzerDialog::OnSelchangeList2() 
	{
		int Index = m_lbSelectedFeatures.GetCurSel();

		if( LB_ERR != Index )
		{
			int Feature = static_cast<SvPb::SVBlobFeatureEnum>(m_lbSelectedFeatures.GetItemData(Index));

			if (SvPb::SVBlobFeatureEnum::SV_AREA == Feature || SvPb::SVBlobFeatureEnum::SV_BOXX_MAX == Feature || SvPb::SVBlobFeatureEnum::SV_BOXX_MIN == Feature 
				|| SvPb::SVBlobFeatureEnum::SV_BOXY_MAX == Feature || SvPb::SVBlobFeatureEnum::SV_BOXY_MIN == Feature)
			{
				GetDlgItem(IDC_BUTTON5)->EnableWindow(false);
			}
			else
			{
				GetDlgItem(IDC_BUTTON5)->EnableWindow(true);
			}
			if (SvPb::SVBlobFeatureEnum::SV_CENTER_X_SOURCE == Feature || SvPb::SVBlobFeatureEnum::SV_CENTER_Y_SOURCE == Feature)
			{
				GetDlgItem(IDC_SET_SORT_BTN)->EnableWindow(false);
				GetDlgItem(IDC_BUTTON4)->EnableWindow(false);
			}
			else
			{
				GetDlgItem(IDC_SET_SORT_BTN)->EnableWindow(true);
				GetDlgItem(IDC_BUTTON4)->EnableWindow(true);
			}
		}
	}

	void SVBlobAnalyzerDialog::OnNbrOfBlobs() 
	{
		//needed here only to fill-in the request to inspection commands
		int Feature = static_cast<int> (m_lbSelectedFeatures.GetItemData(m_lbSelectedFeatures.GetCurSel()));

		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_getblobanalyzerinforequest();
		pRequest->set_feature(Feature);
		pRequest->set_featureenabled(false);
		pRequest->set_taskobjectid(m_TaskObjectID);

		HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
		if (S_OK == hr && responseCmd.has_getblobanalyzerinforesponse())
		{
			SvOi::SetupDialogManager(responseCmd.getblobanalyzerinforesponse().resultblobclassid(), m_InspectionID, responseCmd.getblobanalyzerinforesponse().resultblobid(), GetSafeHwnd());
		}
		else
		{
		}
	}

	void SVBlobAnalyzerDialog::OnExcludeBlob() 
	{
		SetInspectionData();
	}

	void SVBlobAnalyzerDialog::OnBtnFillBlobs() 
	{
		SVFillBlobDlg dlg(m_values);
		dlg.DoModal();
	}

	void SVBlobAnalyzerDialog::OnFillBlobs() 
	{
		m_btnFillBlobs.EnableWindow(m_chkFillBlob.GetCheck());
	
		SetInspectionData();
	}

	void SVBlobAnalyzerDialog::OnBlobColor()
	{
		SetInspectionData();
	}

	void SVBlobAnalyzerDialog::OnChangeEditMaxNbrBlobs() 
	{
		CString Temp;
		m_ctlMaxNbrBlobs.GetWindowText( Temp);

		m_lMaxNumberBlobs = atol(Temp);
	}

	void SVBlobAnalyzerDialog::OnKillFocusMaxNbrBlobs()
	{
		UpdateData(true);
	}

	void SVBlobAnalyzerDialog::OnChangeEditMaxBlobDataArraySize() 
	{
		SetInspectionData();
	}

	void SVBlobAnalyzerDialog::OnButtonSetFeatureProperties() 
	{
		int Index = m_lbSelectedFeatures.GetCurSel();
		if (LB_ERR != Index)
		{
			int Feature = static_cast<int> (m_lbSelectedFeatures.GetItemData(Index) );

			double DefaultValue = m_values.GetDefault<double>(SvPb::BlobFeatureEId + Feature);
			std::string FeatureName = m_values.GetName(SvPb::BlobFeatureEId + Feature);
			
			SVBlobFeaturePropertiesDlg DlgProperties(FeatureName, DefaultValue);
			if (DlgProperties.DoModal() == IDOK)
			{
				m_values.SetDefault<double>(SvPb::BlobFeatureEId + Feature, DlgProperties.getDefaultValue());
				m_values.Commit();
			}
		}
	}

	void SVBlobAnalyzerDialog::initFeatureListBox(CListBox& listBox, bool EnabledFeatures)
	{
		listBox.ResetContent();

		//needed here only to fill-in the request to inspection commands
		int Feature = static_cast<int> (m_lbSelectedFeatures.GetItemData(m_lbSelectedFeatures.GetCurSel()));

		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_getblobanalyzerinforequest();
		pRequest->set_feature(Feature);
		pRequest->set_featureenabled(EnabledFeatures);
		pRequest->set_taskobjectid(m_TaskObjectID);

		HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
		if (S_OK == hr && responseCmd.has_getblobanalyzerinforesponse())
		{
			auto featureList = responseCmd.getblobanalyzerinforesponse().list();
			for (auto const& feature : featureList)
			{
				if (!feature.name().empty())
				{
					listBox.SetItemData(listBox.AddString(feature.name().c_str()), static_cast<DWORD_PTR>(feature.value()));
				}
			}
		}
		else
		{
		}

		if (0 >= listBox.GetCount())
		{
			listBox.SetItemData(listBox.AddString(_T("(No Feature)")), static_cast<DWORD_PTR> (LB_ERR));
		}

		listBox.SetCurSel(0);
	}
} //namespace SvOg