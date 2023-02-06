//*****************************************************************************
// \copyright COPYRIGHT (c) 2021,2021 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file LinkedValueSelectorDialog.h
/// Dialog to set the LinkedValue settings.
//*****************************************************************************
#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "ObjectInterfaces/IFormulaController.h"
#include "ObjectSelectorLibrary/ObjectSelectorPpg.h"
#include "ObjectSelectorLibrary/ObjectTreeGenerator.h"
#include "DataController.h"
#pragma endregion Includes
#include "GroupToolHelper.h"

namespace SvOgu
{
	class SVFormulaEditorPageClass;
	using ValidCheckCallback = std::function<bool(const variant_t&, SvStl::MessageContainer&)>;
	using ConvertValueCallback = std::function<CString(const variant_t&)>;

	enum class LinkedValueSelectorTypesEnum
	{
		None,
		Indirect,
		DirectIndirect,
		All
	};

	class LinkedValueSelectorDialog : public CDialog
	{
	protected:
		//{{AFX_DATA(LinkedValueSelectorDialog)
		enum {	IDD = IDD_LINKEDVALUE_SELECTOR_DIALOG	};
		//}}AFX_DATA
	public:
		LinkedValueSelectorDialog(uint32_t inspectionId, uint32_t objectId, const std::string& rName, const LinkedValueData& data, VARTYPE vtType, ValidCheckCallback validCallback = nullptr);
		LinkedValueSelectorDialog(uint32_t inspectionId, uint32_t objectId, const SvOgu::GroupInputResultData& resultdata);
		
		virtual ~LinkedValueSelectorDialog();

		LinkedValueData getData() { return m_data; };
		static bool createVariantFromString(VARTYPE vtType, const std::string& rStrValue, variant_t& rVariantValue);

	protected:

		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(LinkedValueSelectorDialog)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
		//}}AFX_VIRTUAL

		// Generated message map functions
		//{{AFX_MSG(LinkedValueSelectorDialog)
		virtual BOOL OnInitDialog() override;
		virtual void OnOK() override;
		void OnValue();
		void OnLinked();
		void OnFormula();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	private:
		void changeType(SvPb::LinkedSelectedOption newType);
		void createObjectPage();
		void createFormulaPage();
		void updateShownControls();
		bool checkAndSetDirectValue();

	protected:
		SvPb::LinkedSelectedOption m_type;

		//Value
		CString m_directValue;

	private:
		SvPb::LinkedValueTypeEnum m_LinkedValueType = SvPb::LinkedValueTypeEnum::TypeDecimal;
		uint32_t m_inspectionId;
		uint32_t m_objectId;
		const std::string m_ObjectName;
		LinkedValueData m_data;
		VARTYPE m_vtType;
		LinkedValueSelectorTypesEnum m_possibleTypes = LinkedValueSelectorTypesEnum::All;

		ValidCheckCallback m_validCheckCallback = nullptr;

		//linked 
		CPropertySheet m_dlgLinkedSheet;
		SvOsl::ObjectTreeGenerator m_linkedTreeGenerator;
		std::unique_ptr<SvOsl::ObjectSelectorPpg> m_pDlgLinkedPage;

		//For Formula
		std::unique_ptr<SVFormulaEditorPageClass> m_pDlgFormulaPage;
		SvOi::IFormulaControllerPtr m_FormulaController;
	};
} //namespace SvOgu
