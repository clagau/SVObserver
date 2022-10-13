//*****************************************************************************
/// \copyright (c) 2022,2022 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
//*****************************************************************************
/// This is the dialog to create Inputs and Results for groupTool to remove dependencies inside-out.
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
class CreateInputResultDlg : public CDialog
{
public:
	using DataMapType = std::map<std::string, std::vector<std::string>>;
#pragma region Declarations
private:
	enum	{ IDD = IDD_CREATE_INPUT_RESULT_DIALOG };
#pragma endregion Declarations

#pragma region Constructor
public:
	explicit CreateInputResultDlg(uint32_t inspectionId, uint32_t toolId, const ::google::protobuf::RepeatedPtrField< SvPb::NameNamePair >& rDependencyPairs, CWnd* pParent = nullptr);
	virtual ~CreateInputResultDlg();
#pragma endregion Constructor

#pragma region Protected Methods
protected:
	DECLARE_MESSAGE_MAP()

	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;

	virtual void OnOK() override;
	void SetGridData();
	void OnInputGridEndEdit(NMHDR* pNotifyStruct, LRESULT* pResult);
	void OnResultGridEndEdit(NMHDR* pNotifyStruct, LRESULT* pResult);
#pragma endregion Protected Methods

#pragma region Member variables
private:
	uint32_t m_inspectionId {SvDef::InvalidObjectId};
	uint32_t m_toolId {SvDef::InvalidObjectId};
	uint32_t m_inputId {SvDef::InvalidObjectId};
	SvOgu::ValueController m_inputValueCtrl;
	DataMapType m_inputMap;
	std::vector<std::string> m_inputParameterNames;
	SvGcl::GridCtrl m_InputGrid;
	uint32_t m_resultId {SvDef::InvalidObjectId}; 
	SvOgu::ValueController m_resultValueCtrl;
	DataMapType m_resultMap;
	std::vector<std::string> m_resultParameterNames;
	SvGcl::GridCtrl m_ResultGrid;
#pragma endregion Member variables
};
} //namespace SvOg