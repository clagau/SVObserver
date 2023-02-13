//*****************************************************************************
/// \copyright (c) 2022,2022 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
//*****************************************************************************
/// This is the dialog to solve Inputs values conflicts after import tools.
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
class InputConflictDlg : public CDialog
{
#pragma region Declarations
private:
	enum	{ IDD = IDD_INPUT_CONFLICT };
#pragma endregion Declarations

#pragma region Constructor
public:
	explicit InputConflictDlg(uint32_t inspectionId, const ::google::protobuf::RepeatedPtrField<SvPb::FixedInputData>& rInputDataVector, const std::vector<uint32_t>& rToolIds, LPCTSTR title = "Input Conflict", CWnd* pParent = nullptr);
	virtual ~InputConflictDlg();
#pragma endregion Constructor

#pragma region Protected Methods
protected:
	DECLARE_MESSAGE_MAP()

	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;

	void setLinkedValueColumns(int pos);
	void setInputObjectColumn(int pos);
	void LoadGlobalData();
	void OnGridClick(NMHDR* pNotifyStruct, LRESULT* /*pResult*/);
	void OnGridEndEdit(NMHDR* pNotifyStruct, LRESULT* pResult);
	void OnGridValueSelectionChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
	bool setLinkedValue(const std::string& rNewStr, const std::string& name, SvOgu::LinkedValueData& rData);
	void commitLinkedData(int pos, const SvOgu::LinkedValueData& rData);
	void commitInputObject(int pos, const std::string& rText);
	void updateSelections();
#pragma endregion Protected Methods

#pragma region Member variables
private:
	uint32_t m_inspectionId {SvDef::InvalidObjectId};
	SvGcl::GridCtrl m_Grid;						//The grid displaying the conflicting values
	const ::google::protobuf::RepeatedPtrField<SvPb::FixedInputData>& m_rInputDataVector;  //This is the conflict Global Data as data pair
	const std::vector<uint32_t>& m_rToolIds;
	CBitmap m_downArrowBitmap;
	CImageList m_ImageList;
	LPCTSTR m_title;

	std::map<uint32_t, std::unique_ptr<SvOgu::ValueController>> m_valuesControllerMap;
	std::map<uint32_t, std::vector<SvPb::ObjectNameIdPair>> m_inputObjectValueMap;
#pragma endregion Member variables
};
} //namespace SvOg