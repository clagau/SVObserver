//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Seidenader Maschinenbau GmbH. All Rights Reserved
/// \file BlobAnalyzer2Draw.h
/// This is the class for the draw tab of Blob Analyzer 2
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "ISVPropertyPageDialog.h"
#include "DataController.h"
#include "SVMFCControls/SVEditableListBox.h"
#include "SVMFCControls/SVEnumerateCombo.h"
#pragma endregion Includes

namespace SvOg
{
	class BlobAnalyzer2Draw : public CPropertyPage, public ISVPropertyPageDialog
	{
#pragma region Declarations
	protected:
		//{{AFX_DATA(BlobAnalyzer2Draw)
		enum { IDD = IDD_BLOB2_DRAW_DIALOG };

		SvMc::SVEnumerateCombo m_cbBlobType;
		SvMc::SVEnumerateCombo m_cbDrawTypeAdditionalImage;
		SvMc::SVEnumerateCombo m_cbBlobTypeAdditionalImage;
		//}}AFX_DATA
#pragma endregion Declarations

#pragma region Constructor
	public:
		// Standard constructor
		BlobAnalyzer2Draw(uint32_t inspectionId, uint32_t analyzerId, uint32_t drawTaskId);

		// Standard destructor
		virtual ~BlobAnalyzer2Draw();
#pragma endregion Constructor

#pragma region Public Methods
	public:
		virtual bool QueryAllowExit() override; //from ISVPropertyPageDialog
		virtual BOOL OnSetActive() override;
#pragma endregion Public Methods

#pragma region Protected Methods
		DECLARE_MESSAGE_MAP()

		//{{AFX_VIRTUAL(BlobAnalyzer2Draw)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV-Unterstützung
																	 //}}AFX_VIRTUAL
	protected:
		//{{AFX_MSG(BlobAnalyzer2Draw)
		virtual BOOL OnInitDialog() override;
		void OnCheckEnable();
		void OnChangeData();
		void OnBnClickedInsert();
		void OnBnClickedDelete();
		void OnBnClickedDeleteAll();
		void OnSelchangeList1();
		void OnKillFocusColor1();
		void OnKillFocusColor2();
		void OnKillFocusColor3();
		void OnChangeDrawType();
		void OnChangeBlobType2();
		LRESULT  OnRenameSteps(WPARAM, LPARAM);
		//}}AFX_MSG

#pragma endregion Protected Methods

#pragma region Private Methods
	private:
		void init();
		bool setInspectionData();

		void enableControls();

		void fillList();
		void setNumberOfSteps();
		void fillStepNames();

		void OnKillFocusColor(byte value, std::vector<byte>& rValueArray);
		void reset();

		bool SetComboValue(int index, const std::vector<long>& rArray, SvMc::SVEnumerateCombo& rControl);

		template <typename T>
		void insertValue(int index, T value, std::vector<T>& rArray)
		{
			if (0 <= index && m_numberOfSteps > index)
			{
				if (rArray.size() <= m_numberOfSteps)
				{
					rArray.insert(rArray.begin() + index, value);
				}
			}
			else
			{
				if (rArray.size() <= m_numberOfSteps)
				{
					rArray.push_back(value);
				}
			}
		}
#pragma endregion Private Methods

#pragma region Member Variables
	private:
		const uint32_t m_InspectionID;
		const uint32_t m_analyzerID;
		const uint32_t m_drawTaskId;

		ValueController m_Values;

		int m_fillColor = 0;
		BOOL	m_useFillBlob = false;

		BOOL m_useAdditionalImage = false;
		int m_AdditionalImageColor = 0;
		int m_background_color1 = 0;
		int m_background_color2 = 0;
		int m_background_color3 = 0;
		int m_color1 = 0;
		int m_color2 = 0;
		int m_color3 = 0;
		int m_numberOfSteps = 0;
		std::vector<CString> m_StepNames;
		SvOi::NameValueVector m_DrawTypeList;
		std::vector<long> m_DrawTypeArray;
		SvOi::NameValueVector m_BlobType2List;
		std::vector<long> m_BlobTypeArray;
		std::vector<byte> m_Color1Array;
		std::vector<byte> m_Color2Array;
		std::vector<byte> m_Color3Array;
		SvMc::SVEditableListBox m_AdditionalImageListBox;

#pragma endregion Member Variables
	};
} //namespace SvOg
#pragma once
