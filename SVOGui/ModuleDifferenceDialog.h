//*****************************************************************************
/// \copyright (c) 2023,2023 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
//*****************************************************************************
/// This is the dialog for show the different between the old and new Module
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
#include "SVOResource/resource.h"
#include <google/protobuf/repeated_ptr_field.h>
#include "SVProtoBuf/InspectionCommands.h"
#pragma endregion Includes

namespace SvOg
{
class ModuleDifferenceDialog : public CDialog
{
#pragma region Declarations
private:
	enum { IDD = IDD_MODULE_DIFFERENCE };
#pragma endregion Declarations

#pragma region Constructor
public:
	explicit ModuleDifferenceDialog(CString currentHistory, CString newHistory, const std::string& rModuleName, CWnd* pParent = nullptr);
	virtual ~ModuleDifferenceDialog();
#pragma endregion Constructor

#pragma region Protected Methods
protected:
	DECLARE_MESSAGE_MAP()

	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;
	virtual void OnYes();
	virtual void OnNo();

#pragma endregion Protected Methods
	#pragma region Member variables
private:
	CString m_strHistory;
	CString m_strHistoryNew;
	const std::string& m_rModuleName;
#pragma endregion Member variables
};

CString createHistoryText(const std::vector<std::pair<time_t, std::string>>& rHistoryList, const std::string& guid);
CString createHistoryText(const ::google::protobuf::RepeatedPtrField< ::SvPb::History >& rHistoryList, const std::string& guid);
} //namespace SvOg