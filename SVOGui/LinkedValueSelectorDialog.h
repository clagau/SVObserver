//*****************************************************************************
// \copyright COPYRIGHT (c) 2021,2021 by Seidenader Maschinenbau GmbH. All Rights Reserved
/// \file LinkedValueSelectorDialog.h
/// Dialog to set the LinkedValue settings.
//*****************************************************************************
#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "ObjectInterfaces/IFormulaController.h"
#include "ObjectSelectorLibrary/ObjectSelectorPpg.h"
#include "ObjectSelectorLibrary/ObjectTreeGenerator.h"
#include "SVProtoBuf/SVO-Enum.h"
#include "DataController.h"
#pragma endregion Includes

namespace SvOg
{
	class SVFormulaEditorPageClass;
	using ValidCheckCallback = std::function<bool(const variant_t&, SvStl::MessageContainer&)>;
	using ConvertValueCallback = std::function<CString(const variant_t&)>;

	struct ObjectSelectorData
	{
		ObjectSelectorData() {};
		explicit ObjectSelectorData(uint32_t stopAtId) : m_stopAtId(stopAtId) {};

		std::vector<SvPb::SearchArea> m_searchArea{ SvPb::SearchArea::globalConstantItems, SvPb::SearchArea::toolsetItems };
		SvPb::ObjectSelectorType m_type{ SvPb::allNumberValueObjects };
		SvPb::ObjectAttributes m_attribute = SvPb::selectableForEquation;
		std::vector<uint32_t> m_excludeSameLineageVector;
		uint32_t m_stopAtId = SvDef::InvalidObjectId;
	};

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
		LinkedValueSelectorDialog(uint32_t inspectionId, uint32_t objectId, const std::string& rName, const LinkedValueData& data, VARTYPE vtType, ObjectSelectorData objectSelectorData = {}, ValidCheckCallback validCallback = nullptr, LinkedValueSelectorTypesEnum possibleTypes = LinkedValueSelectorTypesEnum::All);
		
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

		//virtual BOOL OnKillActive() override;
		void OnSelchangeValueType();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	private:
		void createObjectPage();
		void createFormulaPage();
		void updateShownControls();
		bool checkAndSetDirectValue();

	protected:
		int m_iType;

		//Value
		CString m_directValue;

	private:
		CComboBox m_valueTypeCtrl;
		uint32_t m_inspectionId;
		uint32_t m_objectId;
		const std::string m_ObjectName;
		LinkedValueData m_data;
		VARTYPE m_vtType;
		LinkedValueSelectorTypesEnum m_possibleTypes = LinkedValueSelectorTypesEnum::All;

		ValidCheckCallback m_validCheckCallback = nullptr;

		//linked 
		SvOsl::ObjectTreeGenerator m_linkedTreeGenerator;
		CPropertySheet m_dlgLinkedSheet;
		std::unique_ptr<SvOsl::ObjectSelectorPpg> m_pDlgLinkedPage;
		ObjectSelectorData m_objectSelectorData;

		//For Formula
		CPropertySheet m_dlgFormulaSheet;
		std::unique_ptr<SVFormulaEditorPageClass> m_pDlgFormulaPage;
		SvOi::IFormulaControllerPtr m_FormulaController;
	};
} //namespace SvOg
