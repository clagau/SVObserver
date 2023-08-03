//*****************************************************************************
/// \copyright (c) 2023,2023 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
//*****************************************************************************
/// This is the dialog of the Module Settings.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
#include "SVOResource/resource.h"
#include "GridCtrlLibrary/GridCtrl.h"
#include "SVUtilityLibrary/SVGUID.h"
#pragma endregion Includes

namespace SvOg
{
struct ModuleData
{
	std::string m_name;
	SVGUID m_guid;
	int m_numberOfUse;
	std::string m_comment;
	std::vector<std::pair<time_t, std::string>> m_historyList;
};
using ModuleDataList = std::vector<ModuleData>;

class ModuleManagerDialog : public CDialog
{
#pragma region Declarations
private:
	enum { IDD = IDD_MODULE_MANAGER };
#pragma endregion Declarations

#pragma region Constructor
public:
	explicit ModuleManagerDialog(CWnd* pParent = nullptr);
	virtual ~ModuleManagerDialog();
#pragma endregion Constructor

#pragma region Protected Methods
protected:
	DECLARE_MESSAGE_MAP()

	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;
	afx_msg void OnSelchangeList();
	void OnDeleteModule();
	void OnImportModule();
	void OnExportModule();
	void OnRenameModule();
	void OnHelpButton();

	void initModuleListControl();
	int getSelectedIndex();
#pragma endregion Protected Methods
	#pragma region Member variables
private:
	ModuleDataList m_moduleList;
	CListBox m_moduleListBox;
	CString m_strComment;
	CString m_strHistory;
#pragma endregion Member variables
};
} //namespace SvOg