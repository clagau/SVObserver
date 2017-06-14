// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
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
#include "SVRegressionTestStruct.h"
#include "SVVirtualCamera.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

/////////////////////////////////////////////////////////////////////////////
// CSVRegressionFileSelectSheet

class CSVRegressionFileSelectSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CSVRegressionFileSelectSheet)

// Construction
public:
	CSVRegressionFileSelectSheet(UINT nIDCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);
	CSVRegressionFileSelectSheet(LPCTSTR pszCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSVRegressionFileSelectSheet)
	public:
	virtual BOOL OnInitDialog() override;
	//}}AFX_VIRTUAL
	void OnOK();

// Implementation
public:
	virtual ~CSVRegressionFileSelectSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSVRegressionFileSelectSheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void CreatePages(CList<RegressionTestStruct*,RegressionTestStruct*>*RegessionList,SVVirtualCameraPtrSet CameraList );

private: //methods
	SVString MakeFileNameMask( const SVString& rFileName );
	/// Validate the regression data and fill the list of files for all structs. If it valid it throw an exception.
	void ValidateAndFillFileList();
	void ClearRegressionList();

	/// Fill the list of files with the RegressionFileEnum::RegFileList mode for one struct
	/// \param rStruct [in,out] The struct which should to be filled
	/// \returns int Number of images
	int FillFileList(RegressionTestStruct& rStruct);

	/// Fill the list of files with directory-modi (Main method)
	/// \param rStruct [in,out] The struct which should to be filled
	/// \returns int Number of images
	int FillFileListFromDirectory(RegressionTestStruct& rStruct);

	/// Fill the list of files with directory-modi for recursive call of the subdirectories.
	/// \param rStruct [in,out] The struct which should to be filled
	/// \param rCurrentPath [in] The current path of this recursive call.
	/// \returns int Number of images
	int FillFileListFromDirectory(RegressionTestStruct& rStruct, const SVString& rCurrentPath);

private: //data members
	CList<CString, CString&> m_listMasks;
	CList<RegressionTestStruct*,RegressionTestStruct*> *m_pRegressionList;
};

