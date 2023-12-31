// ******************************************************************************
// * COPYRIGHT (c) 2005 by K�rber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVRegressionFileSelectSheet
// * .File Name       : $Workfile:   SVRegressionFileSelectSheet.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 14:41:56  $
// ******************************************************************************
#pragma once

#pragma region Includes
#include "Definitions/SVRegressionTestStruct.h"
#include "InspectionEngine/SVVirtualCamera.h"
#include "FilesystemUtilities/ImageFileFormats.h"
#include "SVMFCControls/ResizablePropertySheet.h"
#pragma endregion Includes


class SVRegressionFileSelectSheet : public SvMc::ResizablePropertySheet
{
// Construction
public:
	explicit SVRegressionFileSelectSheet(UINT nIDCaption, uint32_t inspectionID, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);
	explicit SVRegressionFileSelectSheet(LPCTSTR pszCaption, uint32_t inspectionID, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSVRegressionFileSelectSheet)
	public:
	virtual BOOL OnInitDialog() override;
	//}}AFX_VIRTUAL
	void OnOK();
	void OnAddImage();

// Implementation
public:
	virtual ~SVRegressionFileSelectSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSVRegressionFileSelectSheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void CreatePages(std::vector<RegressionTestStruct>* rRegessionList, std::vector<RegressionTestStruct>* pRegessionImageList, const SvIe::SVVirtualCameraPtrVector&  rCameraVector);
	void removeFilePage(CPropertyPage* pPage);

private: //methods
	std::string MakeFileNameMask( const std::string& rFileName );
	/// Validate the regression data and fill the list of files for all structs. If it valid it throw an exception.
	void ValidateAndFillFileList();

	bool ValidateAndFillFileList(RegressionTestStruct& rStruct, long& lListCountSize);

	/// Fill the list of files with the RegressionFileEnum::RegFileList mode for one struct
	/// \param rStruct [in,out] The struct which should to be filled
	/// \returns int Number of images
	int FillFileList(RegressionTestStruct& rStruct);

	/// Fill the list of files with directory-modi (Main method)
	/// \param rStruct [in,out] The struct to be filled
	/// \returns int Number of images
	int FillFileListFromDirectory(RegressionTestStruct& rStruct);

	/// Fill the list of files with directory-modi for recursive call of the subdirectories.
	/// \param rStruct [in,out] The struct to be filled
	/// \param rParentDirectory [in] The current directory path of this recursive call.
	/// \returns int Number of images
	int collectMatchingFilesInDirectories(RegressionTestStruct& rStruct, const std::string& rParentDirectory, ImageFileFormat fileFormat);

	int collectMatchingFilesInDirectory(RegressionTestStruct& rStruct, const std::string& rParentDirectory, ImageFileFormat fileFormat);

	SvPb::GetToolsWithReplaceableSourceImageResponse createToolNameList() const;

	void updateActiveButton();

private: //data members
	CList<CString, CString&> m_listMasks;
	std::vector<RegressionTestStruct>* m_pRegressionCameraList = nullptr;
	std::vector<RegressionTestStruct>* m_pRegressionImageList = nullptr;
	CButton m_addImageButton;
	uint32_t m_InspectionID;
};

