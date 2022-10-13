//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file Ranking2FilterDlg.h
/// All Rights Reserved
//*****************************************************************************
//* This class is the dialog for the ranking2 filter
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
#include "SVOResource/resource.h"
#include "GridCtrlLibrary\GridCtrl.h"
#include "SVOGuiUtility/DataController.h"
#pragma endregion Includes

namespace SvOg
{
class Ranking2FilterDlg : public CDialog
{
public:
#pragma region Declarations

	enum { IDD = IDD_RANKING2_FILTER };
#pragma endregion Declarations

public:
#pragma region Constructor
	Ranking2FilterDlg(uint32_t inspectionId, uint32_t filterId, CWnd* pParent = nullptr);

	virtual ~Ranking2FilterDlg();
#pragma endregion Constructor

public:
#pragma region Public Methods
	/// Sets the kernel values for the inspection
	/// \returns HRESULT S_OK on success
	HRESULT SetInspectionData();
#pragma endregion Public Methods

protected:
#pragma region Protected Methods
	DECLARE_MESSAGE_MAP()

	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;

	afx_msg void OnCbnSelchangeKernelWidth();
	afx_msg void OnCbnSelchangeKernelHeight();
	afx_msg void OnSelchangeRankingRank();
	afx_msg void OnGridSelChanged(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	//OnBnMfcStandardOk is used to avoid Enter key calling CDialog::OnOK
	afx_msg void OnBnMfcStandardOk() {};
	afx_msg void OnBnClickedOk();

	/// Initializes the values from the filter object to the local values
	void initializeFilter();

	/// This handles key input to the grid control
	/// \param Character [in] The key to be input to the control
	/// \returns bool True when key has been handled
	bool inputGridCtrlKey(WPARAM Character);

	/// The method recalculates the kernel with a new width and height
	/// \param Width [in] The kernel width
	/// \param Height [in] The kernel height
	void recalculateKernel(int Width, int Height);

	/// The method initializes the kernel with a new width and height
	/// \param Width [in] The kernel width
	/// \param Height [in] The kernel height
	void initializeKernel(int Width, int Height);

	/// The method copies the kernel with a new width and height
	/// \param SourceKernelWidth [in] The kernel width of the source kernel
	/// \param SourceKernelHeight [in] The kernel height of the source kernel
	void copyKernel(long SourceKernelWidth, long SourceKernelHeight);

	/// Updates all selected cells of the grid with the given value
	void updateGridSelection();

	/// This method is used to update the Edit Cell control and the Grid Status
	void updateEditCellandStatus();

	/// This method checks if the kernel data is valid. (Throw an exception if data invalid)
	void isDataValid() const;

	/// This method determines if the control has the focus currently
	/// \param ControlID [in] The ID of the control which we would like to control
	/// \returns bool True if the ID of the control has the focus
	bool doesControlHaveFocus(int ControlID) const;

	void enableCells();

	void setKernelFromControl();
#pragma endregion Protected Methods

private:
#pragma region Member Variables
	std::vector<long> m_KernelArray;				//List of values for the kernel
	SvGcl::GridCtrl m_Grid;						//The grid displaying the kernel
	CComboBox m_WidthCtrl;							//The kernel width control
	CComboBox m_HeightCtrl;							//The kernel height control
	CComboBox m_ctlRankingRank;
	long m_KernelWidth;								//The kernel width value
	long m_KernelHeight;							//The kernel  value
	long m_RankingRank;
	CString m_GridStatus;							//Grid status string

	const uint32_t m_InspectionID;
	const uint32_t m_filterID;
	SvOgu::ValueController m_values;
#pragma endregion Member Variables
};
}  //namespace SvOg
