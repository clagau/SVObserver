//******************************************************************************
//* COPYRIGHT (c) 2014 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : Custom2 Filter dialog
//* .File Name       : $Workfile:   Custom2FilterDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   17 Nov 2014 09:11:14  $
//* ----------------------------------------------------------------------------
//* This class is the dialog for the custom2 filter
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
#include "SVOResource/resource.h"
#include "GridCtrlLibrary/GridCtrl.h"
#include "SVOGuiUtility/DataController.h"
#pragma endregion Includes

namespace SvOg
{
	class Custom2FilterDlg : public CDialog
	{
	public:
#pragma region Declarations
		enum SumType
		{
			TotalSum,
			PositiveSum,
			NegativeSum,
			AbsoluteSum,
			LastSumType
		};	

		enum { IDD = IDD_CUSTOM2_FILTER };
#pragma endregion Declarations

	public:
#pragma region Constructor
		//************************************
		/// The class constructor
		/// \param rFilterClass [in] the interface to the filter class.
		/// \param pParent [in]
		//************************************
		Custom2FilterDlg(uint32_t inspectionId, uint32_t filterId, CWnd* pParent = nullptr );

		//************************************
		// Description: The class destructor
		//************************************
		virtual ~Custom2FilterDlg();
#pragma endregion Constructor

	public:
#pragma region Public Methods
		//************************************
		// Description: Sets the kernel values for the inspection
		// Return: S_OK on success
		//************************************
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
		afx_msg void OnCbnSelchangeSumSelector();
		afx_msg void OnBnClickedApplySum();
		afx_msg void OnEnChangeNormilizationFactor();
		afx_msg void OnEnChangeEditCell();
		afx_msg void OnEnSetfocusEditCell();
		afx_msg void OnGridSelChanged(NMHDR *pNotifyStruct, LRESULT* pResult);
		afx_msg void OnBnClickedImportFilter();
		afx_msg void OnBnClickedExportFilter();
		//OnBnMfcStandardOk is used to avoid Enter key calling CDialog::OnOK
		afx_msg void OnBnMfcStandardOk(){};
		afx_msg void OnBnClickedOk();

		//************************************
		// Description: Initializes the values from the filter object to the local values
		//************************************
		void initializeFilter();

		//************************************
		// Description: This handles character input to the grid control
		// Parameter: Character <in> The character to be input to the control
		// Return: True when character has been handled 
		//************************************
		bool inputGridCtrlCharacter( WPARAM Character );

		//************************************
		// Description: This handles key input to the grid control
		// Parameter: Key <in> The key to be input to the control
		// Return: True when key has been handled 
		//************************************
		bool inputGridCtrlKey( WPARAM Character );

		//************************************
		// Description: The method recalculates the kernel with a new width and height
		// Parameter: Width <in> The kernel width
		// Parameter: Height <in> The kernel height
		//************************************
		void recalculateKernel( int Width, int Height );

		//************************************
		// Description: The method initializes the kernel with a new width and height
		// Parameter: Width <in> The kernel width
		// Parameter: Height <in> The kernel height
		//************************************
		void initializeKernel( int Width, int Height );

		//************************************
		// Description: The method copies the kernel with a new width and height
		// Parameter: SourceKernelWidth <in> The kernel width of the source kernel
		// Parameter: SourceKernelHeight <in> The kernel height of the source kernel
		//************************************
		void copyKernel( long SourceKernelWidth, long SourceKernelHeight );

		//************************************
		// Description: Adds the kernel array values depending on the sum type and displays the value
		//************************************
		void addKernelSum();

		//************************************
		// Description: Calculates the Normalization Result using the Kernel Sum and Normalization Factor
		//************************************
		void calculateNormalizationResult();

		//************************************
		// Description: Updates all selected cells of the grid with the given value
		// Parameter: Value <in> The new value for all selected cells
		//************************************
		void updateGridSelection( LPCTSTR Value );

		//************************************
		// Description: This method is used to update the Edit Cell control and the Grid Status
		//************************************
		void updateEditCellandStatus();

		//************************************
		// Description: The method checks if the values of the cells are valid and changes them if not
		//************************************
		void checkCellsValid();

		//************************************
		// Description: This method checks if the kernel data is valid. (Throw an exception if data invalid)
		//************************************
		void isDataValid() const;

		//************************************
		// Description: This method determines if the control has the focus currently
		// Parameter: ControlID <in> The ID of the control which we would like to control
		// Return: True if the ID of the control has the focus
		//************************************
		bool doesControlHaveFocus( int ControlID ) const;
#pragma endregion Protected Methods

	private:
#pragma region Member Variables
		std::vector<long> m_KernelArray;						//List of values for the kernel
		SvGcl::GridCtrl m_Grid;						//The grid displaying the kernel
		CComboBox m_WidthCtrl;							//The kernel width control
		CComboBox m_HeightCtrl;							//The kernel height control
		CComboBox m_SumSelectorCtrl;					//The sum selector control
		long m_KernelWidth;								//The kernel width value
		long m_KernelHeight;							//The kernel  value
		long m_KernelSum;								//The sum of the kernel
		long m_NormalizationFactor;						//Normalization factor
		CString m_NormilizationResult;					//Normalization result
		BOOL m_AbsoluteValue;							//Absolute value flag
		BOOL m_ClippingEnabled;							//Clipping enabled flag
		bool m_GridEditMode;							//Grid edit mode flag
		CString m_EditCell;								//Edit cell value
		CString m_GridStatus;							//Grid status string

		const uint32_t m_InspectionID;
		const uint32_t m_filterID;
		SvOgu::ValueController m_values;
#pragma endregion Member Variables
	};
}  //namespace SvOg
