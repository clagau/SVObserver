//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
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
#include <vector>
#include "GridCtrlLibrary\GridCtrl.h"
#include "SVTaskObjectInterfaceClass.h"
#include "SVXMLLibrary\SVXMLMaterialsTree.h"
#pragma endregion Includes

namespace SvGcl = Seidenader::GridCtrlLibrary;

//Forward declarations
class Custom2Filter;
class SVObjectXMLWriter;

class Custom2FilterDlg : public CDialog, public SVTaskObjectInterfaceClass
{
public:
#pragma region Declarations
	typedef std::vector<long> LongArray;

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
	// Description: The class constructor
	// Parameter: pParent <in> The parent window of the dialog
	//************************************
	Custom2FilterDlg(CWnd* pParent = NULL);

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
	virtual HRESULT SetInspectionData() override;
#pragma endregion Public Methods

protected:
#pragma region Protected Methods
	DECLARE_MESSAGE_MAP()

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

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
	// Description: This returns the custom filter object
	// Return: A pointer to the custom filter object 
	//************************************
	Custom2Filter* getCustom2Filter();

	//************************************
	// Description: This handles character input to the grid control
	// Parameter: Character <in> The character to be input to the control
	// Return: True when character has been handled 
	//************************************
	BOOL inputGridCtrlCharacter( WPARAM Character );

	//************************************
	// Description: This handles key input to the grid control
	// Parameter: Key <in> The key to be input to the control
	// Return: True when key has been handled 
	//************************************
	BOOL inputGridCtrlKey( WPARAM Character );

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
	// Parameter: rValue <in> The new value for all selected cells
	//************************************
	void updateGridSelection( const CString& rValue );

	//************************************
	// Description: This method is used to update the Edit Cell control and the Grid Status
	//************************************
	void updateEditCellandStatus();

	//************************************
	// Description: The method checks if the values of the cells are valid and changes them if not
	//************************************
	void checkCellsValid();

	//************************************
	// Description: This method reads the Custom2 Filter branch from the XML tree
	// Parameter: rTree <in> The tree object
	// Parameter: rMessage <out> Reference to the message to display if not valid
	// Return: True if read was successful
	//************************************
	bool readCustom2FilterBranch( SVXMLMaterialsTree& rTree, SVString& rMessage );

	//************************************
	// Description: This method reads the element in the tree and branch of the given resource ID
	// Parameter: rTree <in> Reference to the tree object
	// Parameter: rBranch <in> Reference to the branch in which the value is
	// Parameter: ResourceID <in> Resource ID containing the tag to the value in the branch
	// Return: The value read from the XML entry as a variant
	//************************************
	variant_t readElement( SVXMLMaterialsTree& rTree, const SVXMLMaterialsTree::SVBranchHandle& rBranch, UINT ResourceID ) const;

	//************************************
	// Description: This method writes an element into the XML file
	// Parameter: rXmlWriter <in> Reference to the XML writer
	// Parameter: ResourceID <in> Resource ID containing the tag for the value
	// Parameter: Value <in> Variant value to write into the file
	//************************************
	void writeElement( SVObjectXMLWriter& rXmlWriter, UINT ResourceID, variant_t Value );

	//************************************
	// Description: This method checks if the kernel data is valid
	// Parameter: rMessage <out> Reference to the message to display if not valid
	// Return: True if data valid
	//************************************
	bool isDataValid( SVString& rMessage ) const;

	//************************************
	// Description: This method determines if the control has the focus currently
	// Parameter: ControlID <in> The ID of the control which we would like to control
	// Return: True if the ID of the control has the focus
	//************************************
	bool doesControlHaveFocus( UINT ControlID ) const;
#pragma endregion Protected Methods

private:
#pragma region Member Variables
	SvGcl::CGridCtrl m_Grid;						//The grid displaying the kernel
	LongArray m_KernelArray;						//List of values for the kernel
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
#pragma endregion Member Variables
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\Custom2FilterDlg.h_v  $
 * 
 *    Rev 1.1   17 Nov 2014 09:11:14   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  942
 * SCR Title:  Create new Custom2 Filter SVO-324 SVO-67 SVO-74
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Added Method inputGridCtrlCharacter and inputGridCtrlKey
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   24 Oct 2014 11:15:00   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  942
 * SCR Title:  Create new Custom2 Filter SVO-324 SVO-67 SVO-74
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 */