//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
#include "ObjectInterfaces/IBlobAnalyzer.h"
#include "ObjectInterfaces/IObjectManager.h"
#include "ObjectInterfaces/ISVOApp_Helper.h"
#include "SVObjectLibrary/SVClsids.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#pragma endregion Includes

namespace SvOg
{
	BEGIN_MESSAGE_MAP(SVBlobAnalyzeFeatureDialogClass, CDialog)
		//{{AFX_MSG_MAP(SVBlobAnalyzeFeatureDialogClass)
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
	SVBlobAnalyzeFeatureDialogClass::SVBlobAnalyzeFeatureDialogClass(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, CWnd* pParent )
	: CDialog( SVBlobAnalyzeFeatureDialogClass::IDD, pParent )
	, m_rInspectionID{ rInspectionID }
	, m_rTaskObjectID{ rTaskObjectID }
	, m_Values{ SvOg::BoundValues{ rInspectionID, rTaskObjectID } }
	{
	}

	SVBlobAnalyzeFeatureDialogClass::~SVBlobAnalyzeFeatureDialogClass()
	{
	}

	void SVBlobAnalyzeFeatureDialogClass::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(SVBlobAnalyzeFeatureDialogClass)
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

	BOOL SVBlobAnalyzeFeatureDialogClass::OnInitDialog() 
	{
		CDialog::OnInitDialog();
	
		m_Values.Init();

		// Remove Close Button
		ModifyStyle( WS_SYSMENU, 0, SWP_FRAMECHANGED );


		//      List of not enabled.
		initFeatureListBox(m_lbAvailableFeatures, false);

		//      List of enabled.
		initFeatureListBox(m_lbSelectedFeatures, true);

		const SvOi::NameValueVector& rBlobColorList = m_Values.GetEnumTypes(SVBlobColorGuid);
		m_cbBlobColor.SetEnumTypes(rBlobColorList);
		long CurrentSelection = m_Values.Get<long>(SVBlobColorGuid);
		m_cbBlobColor.SetCurSelItemData(CurrentSelection);

		m_FeaturesEnabled = std::string(m_Values.Get<CString>(SVBlobEnabledFeaturesObjectGuid));
		assert(SvOi::SV_NUMBER_OF_BLOB_FEATURES == m_FeaturesEnabled.size());

		m_Ascending = m_Values.Get<bool>(SVSortAscendingObjectGuid);
		m_bExclude = m_Values.Get<bool>(SVExcludeFailedObjectGuid);
		m_lMaxNumberBlobs = m_Values.Get<long>(SVNbrOfBlobsObjectGuid);
		m_lMaxBlobDataArraySize = m_Values.Get<long>(SVMaxBlobDataArraySizeObjectGuid);

		long SortFeature = m_Values.Get<long>(SVSortFeatureObjectGuid);
		if (0 <= SortFeature && SvOi::SV_NUMBER_OF_BLOB_FEATURES > SortFeature)
		{
			m_SortFeatureEdt = m_Values.GetName(SVBlobFeatureGuids[SortFeature]).c_str();
		}
		else
		{
			m_SortFeatureEdt.Empty();
		}

		EnableButtons();

		bool bFillBlobValue = m_Values.Get<bool>(SVBlobUseFillGuid);
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
		ASSERT(bResult2);

		return TRUE;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zur�ckgeben
	}

	HRESULT SVBlobAnalyzeFeatureDialogClass::SetInspectionData()
	{
		HRESULT Result{E_FAIL};

		UpdateData(true);


		int CurrentSelection = m_cbBlobColor.GetCurSel();
		if (0 <= CurrentSelection)
		{
			long Value = static_cast<long> (m_cbBlobColor.GetItemData(CurrentSelection));
			m_Values.Set<long>(SVBlobColorGuid, Value);
		}

		m_Values.Set<CString>(SVBlobEnabledFeaturesObjectGuid, m_FeaturesEnabled.c_str());
		m_Values.Set<bool>(SVSortAscendingObjectGuid, m_Ascending ? true : false);
		m_Values.Set<bool>(SVExcludeFailedObjectGuid, m_bExclude ? true : false);
		bool bFillBlobValue = m_chkFillBlob.GetCheck() ? true : false;
		m_Values.Set<bool>(SVBlobUseFillGuid, bFillBlobValue);
		m_Values.Set<long>(SVNbrOfBlobsObjectGuid, m_lMaxNumberBlobs);
		m_Values.Set<long>(SVMaxBlobDataArraySizeObjectGuid, m_lMaxBlobDataArraySize);

		Result = m_Values.Commit();

		return Result;
	}

	/////////////////////////////////////////////////////////////////////////////
	//
	//
	void SVBlobAnalyzeFeatureDialogClass::EnableButtons()
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
				SvOi::SVBlobFeatureEnum index = static_cast<SvOi::SVBlobFeatureEnum> (m_lbSelectedFeatures.GetItemData(iIndex));

				if (SvOi::SV_AREA == index || SvOi::SV_BOXX_MAX == index || SvOi::SV_BOXX_MIN == index || SvOi::SV_BOXY_MAX == index || SvOi::SV_BOXY_MIN == index)
				{
					GetDlgItem(IDC_BUTTON5)->EnableWindow(false);
				}
				else
				{
					GetDlgItem(IDC_BUTTON5)->EnableWindow(true);
				}

				if (SvOi::SV_CENTER_X_SOURCE == index || SvOi::SV_CENTER_Y_SOURCE == index)
				{
					GetDlgItem(IDC_SET_SORT_BTN)->EnableWindow(false);
					GetDlgItem(IDC_BUTTON4)->EnableWindow(false);
					l_bSetRange = false;
				}
				else
				{
					GetDlgItem(IDC_SET_SORT_BTN)->EnableWindow(true);
					GetDlgItem(IDC_BUTTON4)->EnableWindow(true);
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

	void SVBlobAnalyzeFeatureDialogClass::OnButtonAdd() 
	{
		long lAvailableIndex = static_cast<long> (m_lbAvailableFeatures.GetItemData( m_lbAvailableFeatures.GetCurSel()));
		long lCurrentIndex = static_cast<long> (m_lbSelectedFeatures.GetItemData( m_lbSelectedFeatures.GetCurSel()));

		if(0 <= lAvailableIndex && SvOi::SV_NUMBER_OF_BLOB_FEATURES > lAvailableIndex)
		{
			if (LB_ERR == lCurrentIndex)
			{
				m_Values.Set(SVSortFeatureObjectGuid, lAvailableIndex);

				m_SortFeatureEdt = m_Values.GetName(SVBlobFeatureGuids[lAvailableIndex]).c_str();
			}

			m_FeaturesEnabled[lAvailableIndex] = _T('1');
			m_Values.Set<CString>(SVBlobEnabledFeaturesObjectGuid, m_FeaturesEnabled.c_str());

			//! Reset the Blob Analyzer
			m_Values.Commit(SvOg::doResetRunOnce);
			//! Initialize the embedded values
			m_Values.Init();

			//List of not enabled.
			initFeatureListBox(m_lbAvailableFeatures, false);

			//List of enabled.
			initFeatureListBox(m_lbSelectedFeatures, true);
		}
		EnableButtons();

		UpdateData(false);
	}

	void SVBlobAnalyzeFeatureDialogClass::OnButtonRemove() 
	{
		long index = static_cast<long> (m_lbSelectedFeatures.GetItemData(m_lbSelectedFeatures.GetCurSel()));

		if(0 <= index && SvOi::SV_NUMBER_OF_BLOB_FEATURES > index)
		{
			m_FeaturesEnabled[index] = _T('0');
			m_Values.Set<CString>(SVBlobEnabledFeaturesObjectGuid, m_FeaturesEnabled.c_str());

			// need to reset the sort index
			long sortIndex = m_Values.Get<long>(SVSortFeatureObjectGuid);
			int count = m_lbSelectedFeatures.GetCount();

			if (index == sortIndex)
			{
				// Only one Entry and it may be the "No Feature" string
				if( count )
				{
					long newIndex = static_cast<long> ( m_lbSelectedFeatures.GetItemData(0) );
					if( newIndex == LB_ERR)
					{
						m_Values.Set<long>(SVSortFeatureObjectGuid, 1L);
						m_SortFeatureEdt.Empty ();
					}
					else
					{
						m_Values.Set<long>(SVSortFeatureObjectGuid, newIndex);
						m_SortFeatureEdt = m_Values.GetName(SVBlobFeatureGuids[newIndex]).c_str();
					}
				}
			}
			else
			{
				if(0 == count) // truly empty list
				{
					m_Values.Set<long>(SVSortFeatureObjectGuid, -1L);
					m_SortFeatureEdt.Empty();
				}
				// Only one Entry and it may be the "No Feature" string
				else if( count == 1 )
				{
					long newIndex = static_cast<long> ( m_lbSelectedFeatures.GetItemData( 0 ) );
					if(LB_ERR == newIndex)
					{
						m_Values.Set<long>(SVSortFeatureObjectGuid, -1L);
						m_SortFeatureEdt.Empty();
					}
				}
			}

			//! Reset the Blob Analyzer and then call initFeatureListBox to display the new feature lists
			m_Values.Commit(SvOg::doResetRunOnce);
			//! Initialize the embedded values
			m_Values.Init();

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
	void SVBlobAnalyzeFeatureDialogClass::OnButtonSetRange() 
	{

		int Feature = static_cast<int> (m_lbSelectedFeatures.GetItemData( m_lbSelectedFeatures.GetCurSel()));

		if (0 <= Feature)
		{
			SvOi::IBlobAnalyzer* pBlobAnalyzer = dynamic_cast<SvOi::IBlobAnalyzer*> (SvOi::getObject(m_rTaskObjectID));
			if (nullptr != pBlobAnalyzer)
			{
				SvOi::IObjectClass* pResult = pBlobAnalyzer->getResultObject(Feature);
				if (nullptr != pResult)
				{
					SvOi::SetupDialogManager(pResult->GetClassID(), pResult->GetUniqueObjectID(), GetSafeHwnd());
				}
				else
				{
					SvStl::MessageMgrStd MesMan(SvStl::LogOnly);
					MesMan.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16088);
				}
			}
		}
		else
		{
			SvStl::MessageMgrStd MesMan(SvStl::LogOnly);
			MesMan.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16087);
		}
	}

	void SVBlobAnalyzeFeatureDialogClass::OnOK() 
	{
		SetInspectionData();

		//Reset the inspection
		m_Values.ResetObject(m_rInspectionID, m_rInspectionID);
		const SvStl::MessageContainerVector& rMessages = m_Values.getFailedMessageList();
		if (rMessages.size() > 0 && 0 != rMessages[0].getMessage().m_MessageCode)
		{
			SvStl::MessageMgrStd Msg(SvStl::LogAndDisplay);
			Msg.setMessage(rMessages[0].getMessage());
		}

		CDialog::OnOK();
	}

	void SVBlobAnalyzeFeatureDialogClass::OnCancel() 
	{
		CDialog::OnCancel();
	}

	void SVBlobAnalyzeFeatureDialogClass::OnSetSortBtn() 
	{
		long lCurrentIndex = static_cast< long >( m_lbSelectedFeatures.GetItemData( m_lbSelectedFeatures.GetCurSel() ) );

		if( lCurrentIndex != LB_ERR && lCurrentIndex >= 0)
		{
			m_Values.Set<long>(SVSortFeatureObjectGuid, lCurrentIndex);
			m_SortFeatureEdt = m_Values.GetName(SVBlobFeatureGuids[lCurrentIndex]).c_str();
		}
		else
		{
			m_Values.Set<long>(SVSortFeatureObjectGuid, -1L);
			m_SortFeatureEdt.Empty();
		}
		m_Values.Commit();
		UpdateData(false);
	}

	void SVBlobAnalyzeFeatureDialogClass::OnAscending() 
	{
		UpdateData(true);
	}

	void SVBlobAnalyzeFeatureDialogClass::OnDestroy() 
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
	void SVBlobAnalyzeFeatureDialogClass::OnSelchangeList2() 
	{
		int Index = m_lbSelectedFeatures.GetCurSel();

		if( LB_ERR != Index )
		{
			int Feature = static_cast<SvOi::SVBlobFeatureEnum>(m_lbSelectedFeatures.GetItemData(Index));

			if (SvOi::SV_AREA == Feature || SvOi::SV_BOXX_MAX == Feature || SvOi::SV_BOXX_MIN == Feature || SvOi::SV_BOXY_MAX == Feature || SvOi::SV_BOXY_MIN == Feature)
			{
				GetDlgItem(IDC_BUTTON5)->EnableWindow(false);
			}
			else
			{
				GetDlgItem(IDC_BUTTON5)->EnableWindow(true);
			}
			if (SvOi::SV_CENTER_X_SOURCE == Feature || SvOi::SV_CENTER_Y_SOURCE == Feature)
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

	void SVBlobAnalyzeFeatureDialogClass::OnNbrOfBlobs() 
	{
		SvOi::IBlobAnalyzer* pBlobAnalyzer = dynamic_cast<SvOi::IBlobAnalyzer*> (SvOi::getObject(m_rTaskObjectID));
		if (nullptr != pBlobAnalyzer)
		{
			SvOi::IObjectClass* pResultBlob = pBlobAnalyzer->getResultBlob();
			if (nullptr != pResultBlob)
			{
				SvOi::SetupDialogManager(pResultBlob->GetClassID(), pResultBlob->GetUniqueObjectID(), GetSafeHwnd());
			}
		}
	}

	void SVBlobAnalyzeFeatureDialogClass::OnExcludeBlob() 
	{
		SetInspectionData();
	}

	void SVBlobAnalyzeFeatureDialogClass::OnBtnFillBlobs() 
	{
		SVFillBlobDlg dlg(m_Values);
		dlg.DoModal();
	}

	void SVBlobAnalyzeFeatureDialogClass::OnFillBlobs() 
	{
		m_btnFillBlobs.EnableWindow(m_chkFillBlob.GetCheck());
	
		SetInspectionData();
	}

	void SVBlobAnalyzeFeatureDialogClass::OnBlobColor()
	{
		SetInspectionData();
	}

	void SVBlobAnalyzeFeatureDialogClass::OnChangeEditMaxNbrBlobs() 
	{
		CString Temp;
		m_ctlMaxNbrBlobs.GetWindowText( Temp);

		m_lMaxNumberBlobs = atol(Temp);
	}

	void SVBlobAnalyzeFeatureDialogClass::OnKillFocusMaxNbrBlobs()
	{
		UpdateData(true);
	}

	void SVBlobAnalyzeFeatureDialogClass::OnChangeEditMaxBlobDataArraySize() 
	{
		SetInspectionData();
	}

	void SVBlobAnalyzeFeatureDialogClass::OnButtonSetFeatureProperties() 
	{
		int Index = m_lbSelectedFeatures.GetCurSel();
		if (LB_ERR != Index)
		{
			int Feature = static_cast<int> (m_lbSelectedFeatures.GetItemData(Index) );

			double DefaultValue = m_Values.GetDefault<double>(SVBlobFeatureGuids[Feature]);
			std::string FeatureName = m_Values.GetName(SVBlobFeatureGuids[Feature]);
			
			SVBlobFeaturePropertiesDlg DlgProperties(FeatureName, DefaultValue);
			if (DlgProperties.DoModal() == IDOK)
			{
				m_Values.SetDefault<double>(SVBlobFeatureGuids[Feature], DlgProperties.getDefaultValue());
				m_Values.Commit();
			}
		}
	}

	void SVBlobAnalyzeFeatureDialogClass::initFeatureListBox(CListBox& listBox, bool EnabledFeatures)
	{
		listBox.ResetContent();

		SvOi::IBlobAnalyzer* pBlobAnalyzer = dynamic_cast<SvOi::IBlobAnalyzer*> (SvOi::getObject(m_rTaskObjectID));
		if (nullptr != pBlobAnalyzer)
		{
			const SvOi::NameValueVector& rList = pBlobAnalyzer->getFeatureList(EnabledFeatures);
			for (auto const& rEntry : rList)
			{
				if (!rEntry.first.empty())
				{
					listBox.SetItemData(listBox.AddString(rEntry.first.c_str()), static_cast<DWORD_PTR>(rEntry.second));
				}
			}
		}

		if (0 >= listBox.GetCount())
		{
			listBox.SetItemData(listBox.AddString(_T("(No Feature)")), LB_ERR);
		}

		listBox.SetCurSel(0);
	}
} //namespace SvOg