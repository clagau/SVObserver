//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Seidenader Maschinenbau GmbH. All Rights Reserved
/// \file BlobAnalyzer2Draw.cpp
/// This is the class for the draw tab of Blob Analyzer 2
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "BlobAnalyzer2Draw.h"
#include "BoundValue.h"
#include "GuiValueHelper.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
	BEGIN_MESSAGE_MAP(BlobAnalyzer2Draw, CPropertyPage)
		//{{AFX_MSG_MAP(BlobAnalyzer2Draw)
		ON_BN_CLICKED(IDC_FILL_BLOBS, OnCheckEnable)
		ON_CBN_SELCHANGE(IDC_COMBO_BLOB_TYPE, OnChangeData)
		ON_EN_CHANGE(IDC_BLOB_COLOR, OnChangeData)
		ON_BN_CLICKED(IDC_ADIITONAL_IMAGE, OnCheckEnable)
		ON_BN_CLICKED(IDC_BLACK_RADIO, OnCheckEnable)
		ON_BN_CLICKED(IDC_NONBLACK_RADIO, OnCheckEnable)
		ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedInsert)
		ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedDelete)
		ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedDeleteAll)
		ON_LBN_SELCHANGE(IDC_LIST1, OnSelchangeList1)
		ON_CBN_SELCHANGE(IDC_COMBO_DRAW_TYPE, OnChangeDrawType)
		ON_CBN_SELCHANGE(IDC_COMBO_BLOB_TYPE2, OnChangeBlobType2)
		ON_EN_KILLFOCUS(IDC_EDIT_COLOR1, OnKillFocusColor1)
		ON_EN_KILLFOCUS(IDC_EDIT_COLOR2, OnKillFocusColor2)
		ON_EN_KILLFOCUS(IDC_EDIT_COLOR3, OnKillFocusColor3)
		ON_MESSAGE(WM_APP_LB_ITEM_EDITED, OnRenameSteps)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

#pragma region Constructor
	BlobAnalyzer2Draw::BlobAnalyzer2Draw(uint32_t inspectionId, uint32_t analyzerId, uint32_t drawTaskId)
		: CPropertyPage(BlobAnalyzer2Draw::IDD)
		, m_InspectionID(inspectionId)
		, m_analyzerID(analyzerId)
		, m_drawTaskId(drawTaskId)
		, m_Values{ SvOg::BoundValues{ inspectionId, drawTaskId } }
	{
	}

	BlobAnalyzer2Draw::~BlobAnalyzer2Draw()
	{
	}
#pragma endregion Constructor

#pragma region Public Methods
	bool BlobAnalyzer2Draw::QueryAllowExit()
	{
		return setInspectionData();
	}

	BOOL BlobAnalyzer2Draw::OnSetActive()
	{
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_resetobjectrequest();
		pRequest->set_objectid(m_analyzerID);
		SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);

		init();
		return true;
	}
#pragma endregion Public Methods

#pragma region Protected Methods
	void BlobAnalyzer2Draw::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(BlobAnalyzer2Draw)
		DDX_Check(pDX, IDC_FILL_BLOBS, m_useFillBlob);
		DDX_Control(pDX, IDC_COMBO_BLOB_TYPE, m_cbBlobType);
		DDX_Text(pDX, IDC_BLOB_COLOR, m_fillColor);
		DDV_MinMaxLong(pDX, m_fillColor, 0, 255);

		DDX_Check(pDX, IDC_ADIITONAL_IMAGE, m_useAdditionalImage);
		DDX_Control(pDX, IDC_COMBO_DRAW_TYPE, m_cbDrawTypeAdditionalImage);
		DDX_Control(pDX, IDC_COMBO_BLOB_TYPE2, m_cbBlobTypeAdditionalImage);
		DDX_Radio(pDX, IDC_BLACK_RADIO, m_AdditionalImageColor);
		DDX_Control(pDX, IDC_LIST1, m_AdditionalImageListBox);
		DDX_Text(pDX, IDC_EDIT_BC_RED, m_background_color1);
		DDV_MinMaxLong(pDX, m_background_color1, 0, 255);
		DDX_Text(pDX, IDC_EDIT_BC_GREEN, m_background_color2);
		DDV_MinMaxLong(pDX, m_background_color2, 0, 255);
		DDX_Text(pDX, IDC_EDIT_BC_BLUE, m_background_color3);
		DDV_MinMaxLong(pDX, m_background_color3, 0, 255);
		DDX_Text(pDX, IDC_EDIT_COLOR1, m_color1);
		DDV_MinMaxLong(pDX, m_color1, 0, 255);
		DDX_Text(pDX, IDC_EDIT_COLOR2, m_color2);
		DDV_MinMaxLong(pDX, m_color2, 0, 255);
		DDX_Text(pDX, IDC_EDIT_COLOR3, m_color3);
		DDV_MinMaxLong(pDX, m_color3, 0, 255);
		//}}AFX_DATA_MAP
	}

	BOOL BlobAnalyzer2Draw::OnInitDialog()
	{
		CPropertyPage::OnInitDialog();

		init();

		return TRUE;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
	}

	void BlobAnalyzer2Draw::OnCheckEnable()
	{
		UpdateData();
		reset();
		setInspectionData();
	}

	void BlobAnalyzer2Draw::OnChangeData()
	{
		UpdateData();
		setInspectionData();
	}

	void BlobAnalyzer2Draw::OnBnClickedInsert()
	{
		constexpr byte defaultColor = 255;
		int index = m_AdditionalImageListBox.GetCurSel();
		insertValue<CString>(index, ("Draw " + std::to_string(m_StepNames.size())).c_str(), m_StepNames);
		insertValue(index, m_DrawTypeList[0].second, m_DrawTypeArray);
		insertValue(index, m_BlobType2List[0].second, m_BlobTypeArray);
		insertValue(index, defaultColor, m_Color1Array);
		insertValue(index, defaultColor, m_Color2Array);
		insertValue(index, defaultColor, m_Color3Array);

		reset();
	}

	void BlobAnalyzer2Draw::OnBnClickedDelete()
	{
		int sel = m_AdditionalImageListBox.GetCurSel();
		if (0 <= sel && sel < m_numberOfSteps)
		{
			m_StepNames.erase(m_StepNames.begin() + sel);
			m_DrawTypeArray.erase(m_DrawTypeArray.begin() + sel);
			m_BlobTypeArray.erase(m_BlobTypeArray.begin() + sel);
			m_Color1Array.erase(m_Color1Array.begin() + sel);
			m_Color2Array.erase(m_Color2Array.begin() + sel);
			m_Color3Array.erase(m_Color3Array.begin() + sel);
		}
		reset();
	}

	void BlobAnalyzer2Draw::OnBnClickedDeleteAll()
	{
		m_StepNames.clear();
		m_DrawTypeArray.clear();
		m_BlobTypeArray.clear();
		m_Color1Array.clear();
		m_Color2Array.clear();
		m_Color3Array.clear();
		reset();
	}

	void BlobAnalyzer2Draw::OnSelchangeList1()
	{
		int index = m_AdditionalImageListBox.GetCurSel();
		if (false == SetComboValue(index, m_DrawTypeArray, m_cbDrawTypeAdditionalImage))
		{
			m_cbDrawTypeAdditionalImage.SetCurSel(-1);
		}
		if (false == SetComboValue(index, m_BlobTypeArray, m_cbBlobTypeAdditionalImage))
		{
			m_cbBlobTypeAdditionalImage.SetCurSel(-1);
		}

		if (0 <= index && m_Color1Array.size() > index)
		{
			m_color1 = m_Color1Array[index];
		}
		if (0 <= index && m_Color2Array.size() > index)
		{
			m_color2 = m_Color2Array[index];
		}
		if (0 <= index && m_Color3Array.size() > index)
		{
			m_color3 = m_Color3Array[index];
		}
		enableControls();
		UpdateData(FALSE);
	}

	void BlobAnalyzer2Draw::OnKillFocusColor1()
	{
		UpdateData();
		OnKillFocusColor(static_cast<byte>(m_color1), m_Color1Array);
	}

	void BlobAnalyzer2Draw::OnKillFocusColor2()
	{
		UpdateData();
		OnKillFocusColor(static_cast<byte>(m_color2), m_Color2Array);
	}

	void BlobAnalyzer2Draw::OnKillFocusColor3()
	{
		UpdateData();
		OnKillFocusColor(static_cast<byte>(m_color3), m_Color3Array);
	}

	void BlobAnalyzer2Draw::OnChangeDrawType()
	{
		UpdateData();
		int index = m_AdditionalImageListBox.GetCurSel();
		int CurrentSelection = m_cbDrawTypeAdditionalImage.GetCurSel();
		if (0 <= index && m_DrawTypeArray.size() > index && 0 <= CurrentSelection)
		{
			m_DrawTypeArray[index] = static_cast<long> (m_cbDrawTypeAdditionalImage.GetItemData(CurrentSelection));
		}
	}

	void BlobAnalyzer2Draw::OnChangeBlobType2()
	{
		UpdateData();
		int index = m_AdditionalImageListBox.GetCurSel();
		int CurrentSelection = m_cbBlobTypeAdditionalImage.GetCurSel();
		if (0 <= index && m_BlobTypeArray.size() > index && 0 <= CurrentSelection)
		{
			m_BlobTypeArray[index] = static_cast<long> (m_cbBlobTypeAdditionalImage.GetItemData(CurrentSelection));
		}
	}

	LRESULT BlobAnalyzer2Draw::OnRenameSteps(WPARAM, LPARAM)
	{
		int sel = m_AdditionalImageListBox.GetCurSel();
		if (0 <= sel && sel < m_numberOfSteps)
		{
			m_AdditionalImageListBox.GetText(sel, m_StepNames[sel]);
		}
		return 0;
	}
#pragma endregion Protected Methods

#pragma region Private Methods
	void BlobAnalyzer2Draw::init()
	{
		m_Values.Init();
		m_useFillBlob = m_Values.Get<bool>(SvPb::BlobUseFillEId);

		const SvOi::NameValueVector& rBlobTypeList = m_Values.GetEnumTypes(SvPb::BlobFillTypeEId);
		m_cbBlobType.SetEnumTypes(rBlobTypeList);
		long CurrentSelection = m_Values.Get<long>(SvPb::BlobFillTypeEId);
		m_cbBlobType.SetCurSelItemData(CurrentSelection);
		m_fillColor = m_Values.Get<long>(SvPb::BlobFillColorEId);

		m_useAdditionalImage = m_Values.Get<bool>(SvPb::UseAdditionalImageEId);
		m_AdditionalImageColor = (m_Values.Get<bool>(SvPb::IsColorAdditionalImageEId) ? 1 : 0);
		m_DrawTypeList = m_Values.GetEnumTypes(SvPb::DrawTypesEId);
		m_cbDrawTypeAdditionalImage.SetEnumTypes(m_DrawTypeList);
		m_BlobType2List = m_Values.GetEnumTypes(SvPb::BlobTypesEId);
		m_cbBlobTypeAdditionalImage.SetEnumTypes(m_BlobType2List);
		m_background_color1 = m_Values.Get<long>(SvPb::BackgroundColor1EId);
		m_background_color2 = m_Values.Get<long>(SvPb::BackgroundColor2EId);
		m_background_color3 = m_Values.Get<long>(SvPb::BackgroundColor3EId);
		fillStepNames();
		m_DrawTypeArray = ConvertVariantSafeArrayToVector<long>(m_Values.Get<_variant_t>(SvPb::DrawTypesEId));
		m_BlobTypeArray = ConvertVariantSafeArrayToVector<long>(m_Values.Get<_variant_t>(SvPb::BlobTypesEId));
		m_Color1Array = ConvertVariantSafeArrayToVector<byte>(m_Values.Get<_variant_t>(SvPb::Color1EId));
		m_Color2Array = ConvertVariantSafeArrayToVector<byte>(m_Values.Get<_variant_t>(SvPb::Color2EId));
		m_Color3Array = ConvertVariantSafeArrayToVector<byte>(m_Values.Get<_variant_t>(SvPb::Color3EId));

		reset();

		UpdateData(FALSE);
	}

	bool BlobAnalyzer2Draw::setInspectionData()
	{
		UpdateData(TRUE);

		m_Values.Set<bool>(SvPb::BlobUseFillEId, (TRUE == m_useFillBlob));
		int CurrentSelection = m_cbBlobType.GetCurSel();
		if (0 <= CurrentSelection)
		{
			long Value = static_cast<long> (m_cbBlobType.GetItemData(CurrentSelection));
			m_Values.Set<long>(SvPb::BlobFillTypeEId, Value);
		}
		m_Values.Set<byte>(SvPb::BlobFillColorEId, static_cast<byte>(m_fillColor));

		m_Values.Set<bool>(SvPb::UseAdditionalImageEId, (TRUE == m_useAdditionalImage));
		m_Values.Set<bool>(SvPb::IsColorAdditionalImageEId, (1 == m_AdditionalImageColor));
		m_Values.Set<byte>(SvPb::BackgroundColor1EId, static_cast<byte>(m_background_color1));
		m_Values.Set<byte>(SvPb::BackgroundColor2EId, static_cast<byte>(m_background_color2));
		m_Values.Set<byte>(SvPb::BackgroundColor3EId, static_cast<byte>(m_background_color3));
		CString tmpString;
		if (m_StepNames.size() > 0)
		{
			tmpString = m_StepNames[0];
			for (int i = 1; i < m_StepNames.size(); ++i)
			{
				tmpString = tmpString + "|" + m_StepNames[i];
			}
		}
		m_Values.Set<CString>(SvPb::DrawAdditionalImageStepNamesEId, tmpString);
		_variant_t value = ConvertVectorToVariantSafeArray<std::vector<long>>(m_DrawTypeArray);
		m_Values.Set<_variant_t>(SvPb::DrawTypesEId, value);
		value = ConvertVectorToVariantSafeArray<std::vector<long>>(m_BlobTypeArray);
		m_Values.Set<_variant_t>(SvPb::BlobTypesEId, value);
		value = ConvertVectorToVariantSafeArray<std::vector<byte>>(m_Color1Array);
		m_Values.Set<_variant_t>(SvPb::Color1EId, value);
		value = ConvertVectorToVariantSafeArray<std::vector<byte>>(m_Color2Array);
		m_Values.Set<_variant_t>(SvPb::Color2EId, value);
		value = ConvertVectorToVariantSafeArray<std::vector<byte>>(m_Color3Array);
		m_Values.Set<_variant_t>(SvPb::Color3EId, value);
		m_Values.Commit();
		
		return true;// (S_OK == hResult);
	}

	void BlobAnalyzer2Draw::enableControls()
	{
		GetDlgItem(IDC_COMBO_BLOB_TYPE)->EnableWindow(m_useFillBlob);
		GetDlgItem(IDC_BLOB_COLOR)->EnableWindow(m_useFillBlob);	

		//Additional Image Controls
		int selectedIndex = m_AdditionalImageListBox.GetCurSel();
		bool isValueSelected = (0 <= selectedIndex && m_numberOfSteps > selectedIndex);
		//@TODO[MZA][10.10][18.02.2021] Color Selection is disabled, because it works not yet
		GetDlgItem(IDC_BLACK_RADIO)->EnableWindow(false);// m_useAdditionalImage);
		GetDlgItem(IDC_NONBLACK_RADIO)->EnableWindow(false);// m_useAdditionalImage);
		GetDlgItem(IDC_EDIT_BC_RED)->EnableWindow(m_useAdditionalImage);
		GetDlgItem(IDC_EDIT_BC_GREEN)->ShowWindow(0 != m_AdditionalImageColor ? SW_SHOW : SW_HIDE );
		GetDlgItem(IDC_EDIT_BC_GREEN)->EnableWindow(m_useAdditionalImage);
		GetDlgItem(IDC_EDIT_BC_BLUE)->ShowWindow(0 != m_AdditionalImageColor ? SW_SHOW : SW_HIDE);
		GetDlgItem(IDC_EDIT_BC_BLUE)->EnableWindow(m_useAdditionalImage);
		GetDlgItem(IDC_STATIC_DRAW)->ShowWindow(isValueSelected ? SW_SHOW : SW_HIDE);
		GetDlgItem(IDC_COMBO_DRAW_TYPE)->ShowWindow(isValueSelected ? SW_SHOW : SW_HIDE);
		GetDlgItem(IDC_COMBO_DRAW_TYPE)->EnableWindow(m_useAdditionalImage);
		GetDlgItem(IDC_STATIC_BLOB)->ShowWindow(isValueSelected ? SW_SHOW : SW_HIDE);
		GetDlgItem(IDC_COMBO_BLOB_TYPE2)->ShowWindow(isValueSelected ? SW_SHOW : SW_HIDE);
		GetDlgItem(IDC_COMBO_BLOB_TYPE2)->EnableWindow(m_useAdditionalImage);
		GetDlgItem(IDC_STATIC_COLOR)->ShowWindow(isValueSelected ? SW_SHOW : SW_HIDE);
		GetDlgItem(IDC_EDIT_COLOR1)->ShowWindow(isValueSelected ? SW_SHOW : SW_HIDE);
		GetDlgItem(IDC_EDIT_COLOR1)->EnableWindow(m_useAdditionalImage);
		GetDlgItem(IDC_EDIT_COLOR2)->ShowWindow(0 != m_AdditionalImageColor && isValueSelected ? SW_SHOW : SW_HIDE);
		GetDlgItem(IDC_EDIT_COLOR2)->EnableWindow(m_useAdditionalImage);
		GetDlgItem(IDC_EDIT_COLOR3)->ShowWindow(0 != m_AdditionalImageColor && isValueSelected ? SW_SHOW : SW_HIDE);
		GetDlgItem(IDC_EDIT_COLOR3)->EnableWindow(m_useAdditionalImage);
		GetDlgItem(IDC_LIST1)->EnableWindow(m_useAdditionalImage);
		GetDlgItem(IDC_BUTTON1)->EnableWindow(m_useAdditionalImage);
		GetDlgItem(IDC_BUTTON2)->EnableWindow(m_useAdditionalImage && isValueSelected);
		GetDlgItem(IDC_BUTTON3)->EnableWindow(m_useAdditionalImage && 0 < m_numberOfSteps);
	}

	void BlobAnalyzer2Draw::fillList()
	{
		m_AdditionalImageListBox.ResetContent();
		for (int i = 0; i < m_numberOfSteps; ++i)
		{
			m_AdditionalImageListBox.AddString(m_StepNames[i]);
		}
		m_AdditionalImageListBox.AddString("-------------");
	}

	void BlobAnalyzer2Draw::setNumberOfSteps()
	{
		m_numberOfSteps = static_cast<int>(std::min({ m_StepNames.size(), m_DrawTypeArray.size(), m_BlobTypeArray.size(), m_Color1Array.size(), m_Color2Array.size(), m_Color3Array.size() }));
	}

	void BlobAnalyzer2Draw::fillStepNames()
	{
		auto fullString = m_Values.Get<CString>(SvPb::DrawAdditionalImageStepNamesEId);
		int curPos = 0;
		auto resToken = fullString.Tokenize(_T("|"), curPos);
		while (resToken != _T(""))
		{
			m_StepNames.emplace_back(resToken);
			resToken = fullString.Tokenize(_T("|"), curPos);
		};
	}

	void BlobAnalyzer2Draw::OnKillFocusColor(byte value, std::vector<byte>& rValueArray)
	{
		int index = m_AdditionalImageListBox.GetCurSel();
		if (0 <= index && rValueArray.size() > index)
		{
			rValueArray[index] = static_cast<byte>(value);
		}
	}

	void BlobAnalyzer2Draw::reset()
	{
		setNumberOfSteps();
		fillList();
		enableControls();
	}

	bool BlobAnalyzer2Draw::SetComboValue(int index, const std::vector<long>& rArray, SvMc::SVEnumerateCombo& rControl)
	{
		if (0 <= index && rArray.size() > index)
		{
			return -1 < rControl.SetCurSelItemData(rArray[index]);
		}

		return false;
	}

#pragma endregion Private Mehods
} //namespace SvOg
