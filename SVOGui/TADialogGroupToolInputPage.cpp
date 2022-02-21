//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Seidenader Maschinenbau GmbH. All Rights Reserved
/// \file TADialogGroupToolInputPage.cpp
/// This is the class for the input tab of Group Tool
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TADialogGroupToolInputPage.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageManager.h"
#include "GridCtrlLibrary/GridCellCombo.h"
#include "DisplayHelper.h"
#include "Definitions/GlobalConst.h"
#include "LinkedValueSelectorDialog.h"
#include "SVShowDependentsDialog.h"
#include "ObjectInterfaces/IDependencyManager.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
	namespace
	{
		constexpr int cHeaderSize = 1;
		constexpr int cTypeColumnSize = 100;
		constexpr int cNameColumnSize = 150;
		constexpr int cIndirectNameColumnSize = 215;
		constexpr int cBoxColumnSize = 25;

		struct ColumnDef
		{
			std::string m_name;
			int m_columnSize;

			ColumnDef(const std::string& name, int columnSize) : m_name(name), m_columnSize(columnSize) {};
		};

		enum ColumnPos
		{
			DependencyColumn = 0,
			NameColumn,
			TypeColumn,
			DefaultColumn,
			ValueColumn,
			ValueButtonColumn,
		};

		std::map<int, ColumnDef> g_columnInputObjectsDefArray = {
			{DependencyColumn, {"", 15}},
			{NameColumn, ColumnDef{"Name", cNameColumnSize}},
			{TypeColumn, {"Type", cTypeColumnSize }},
			{DefaultColumn, {"Default", cNameColumnSize}},
			{ValueColumn, {"Value", cIndirectNameColumnSize }},
			{ValueButtonColumn, {"", cBoxColumnSize}},
		};



		variant_t checkText(SvPb::InputTypeEnum type, const std::string& text)
		{
			variant_t result{};
			try
			{
				switch (type)
				{
				case SvPb::InputTypeEnum::TypeDecimal:
					result = std::stod(text);
					break;
				case SvPb::InputTypeEnum::TypeText:
					result = text.c_str();
					break;
				default: //do nothing, empty variant
					break;
				}
			}
			catch (...)
			{
				SvDef::StringVector msgList;
				msgList.push_back(text);
				SvStl::MessageManager msg(SvStl::MsgType::Data);
				msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ConvertTextToVariantFailed, msgList, SvStl::SourceFileParams(StdMessageParams));
				msg.Throw();
			}

			return result;
		}

		variant_t checkText(SvPb::InputTypeEnum type, const variant_t& value)
		{
			variant_t result{};
			bool isValid{ true };
			switch (type)
			{
			case SvPb::InputTypeEnum::TypeDecimal:
				isValid = (S_OK == ::VariantChangeTypeEx(&result, &value, SvDef::LCID_USA, VARIANT_ALPHABOOL, VT_R8));
				break;
			case SvPb::InputTypeEnum::TypeText:
				isValid = (S_OK == ::VariantChangeTypeEx(&result, &value, SvDef::LCID_USA, VARIANT_ALPHABOOL, VT_BSTR));
				break;
			default: //do nothing, empty variant
				break;
			}

			if (false == isValid)
			{
				SvDef::StringVector msgList;
				msgList.push_back(static_cast<std::string>(static_cast<CString>(value)));
				SvStl::MessageManager msg(SvStl::MsgType::Data);
				msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ConvertTextToVariantFailed, msgList, SvStl::SourceFileParams(StdMessageParams));
				msg.Throw();
			}

			return result;
		}

		bool isOk(const GroupInputData& rData)
		{
			return (SvDef::InvalidObjectId == rData.m_errorData.m_ObjectId);
		}
	}

	SvStl::MessageContainerVector getErrorMessage(uint32_t inspectionId, uint32_t toolId)
	{
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pGetMessageListRequest = requestCmd.mutable_getmessagelistrequest();
		pGetMessageListRequest->set_objectid(toolId);

		HRESULT hr = SvCmd::InspectionCommands(inspectionId, requestCmd, &responseCmd);
		if (hr == S_OK && responseCmd.has_standardresponse())
		{
			return SvPb::convertProtobufToMessageVector(responseCmd.standardresponse().errormessages());
		}
		assert(false);
		return {};
	}

	SvStl::MessageData getMessage(const SvStl::MessageContainerVector& rErrorMessages, uint32_t objectId)
	{
		auto iter = std::ranges::find_if(rErrorMessages, [objectId](const auto& rEntry) { return rEntry.getObjectId() == objectId; });
		if (rErrorMessages.end() != iter)
		{
			return iter->getMessage();
		}
		return {};
	}

	SvDef::StringPairVector getDependency(uint32_t valueId)
	{
		SvDef::StringPairVector dependencyList;
		SvOi::getObjectDependency(std::back_inserter(dependencyList), {valueId}, SvOi::ToolDependencyEnum::Client);
		return dependencyList;
	}

	bool setValue(GroupInputData& data, const std::string& newStr)
	{
		if (SvPb::LinkedSelectedType::DirectValue == data.m_data.m_type && SvPb::isValueType(data.m_type))
		{
			variant_t tmp{ newStr.c_str() };
			SvStl::MessageContainer msgContainer;
			bool isValid = (S_OK == ::VariantChangeTypeEx(&data.m_data.m_directValue, &tmp, SvDef::LCID_USA, VARIANT_ALPHABOOL, data.m_data.m_defaultValue.vt));
			if (false == isValid)
			{
				SvDef::StringVector msgList;
				msgList.push_back(data.m_name);
				msgContainer.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_LinkedValue_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams));
				SvStl::MessageManager Msg(SvStl::MsgType::Display);
				Msg.setMessage(msgContainer.getMessage());
			}
			else
			{
				data.m_data.m_Value = data.m_data.m_directValue;
			}
			return isValid;
		}
		return false;
	}

	BEGIN_MESSAGE_MAP(TADialogGroupToolInputPage, CPropertyPage)
		//{{AFX_MSG_MAP(TaDlgGroupToolInput)
		ON_BN_CLICKED(IDC_BUTTON_REMOVE, OnBnClickedButtonRemove)
		ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
		ON_BN_CLICKED(IDC_BUTTON_MOVEUP, OnBnClickedMoveUp)
		ON_BN_CLICKED(IDC_BUTTON_MOVEDOWN, OnBnClickedMoveDown)
		ON_NOTIFY(NM_CLICK, IDC_GRID, OnGridClick)
		ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID, OnGridEndEdit)
		ON_NOTIFY(GVN_SELCHANGED, IDC_GRID, OnSelectionChanged)
		ON_COMMAND(ID_ADD_COLUMN, OnBnClickedButtonAdd)
		ON_COMMAND(ID_REMOVE_COLUMNS, OnBnClickedButtonRemove)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

#pragma region Constructor
	TADialogGroupToolInputPage::TADialogGroupToolInputPage(uint32_t inspectionId, uint32_t toolId, uint32_t taskId)
		: CPropertyPage(TADialogGroupToolInputPage::IDD)
		, m_InspectionID(inspectionId)
		, m_toolId(toolId)
		, m_TaskObjectID(taskId)
		, m_Values{ SvOg::BoundValues{ inspectionId, taskId }}
	{
	}
#pragma endregion Constructor

#pragma region Public Methods
	bool TADialogGroupToolInputPage::QueryAllowExit()
	{
		return setInspectionData();
	}
#pragma endregion Public Methods

#pragma region Protected Methods
	void TADialogGroupToolInputPage::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(TADialogGroupToolInputPage)
		DDX_Control(pDX, IDC_GRID, m_Grid);
		//}}AFX_DATA_MAP
	}

	constexpr int c_maxValues = 50;

	BOOL TADialogGroupToolInputPage::OnInitDialog()
	{
		CPropertyPage::OnInitDialog();

		DisplayHelper::setIconListToGrid(m_ImageList, m_downArrowBitmap, m_Grid);

		m_errorMessages = getErrorMessage(m_InspectionID, m_toolId);

		loadDataList();
		initGridControl();
		FillGridControl();

		UpdateData(FALSE);

		return TRUE;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
	}

	BOOL TADialogGroupToolInputPage::OnSetActive()
	{
		BOOL bOk = CPropertyPage::OnSetActive();
		if (bOk)
		{
			loadDataList();
			FillGridControl();
		}
		return bOk;
	}

	BOOL TADialogGroupToolInputPage::OnKillActive()
	{
		SvGcl::CCellID focusId = m_Grid.GetFocusCell();
		if (focusId.IsValid())
		{
			SvGcl::GridCellBase* pCell = m_Grid.GetCell(focusId.row, focusId.col);
			if (nullptr != pCell)
			{
				pCell->EndEdit();
			}
		}

		bool ret = setInspectionData();
		if (ret)
		{
			return CPropertyPage::OnKillActive();
		}
		return false;
	}

	void TADialogGroupToolInputPage::OnGridClick(NMHDR* pNotifyStruct, LRESULT*)
	{
		auto* pItem = reinterpret_cast<SvGcl::NM_GRIDVIEW*>(pNotifyStruct);
		if (1 > pItem->iRow || m_inputData.size() < pItem->iRow)
		{
			return;
		}

		switch (pItem->iColumn)
		{
		case DependencyColumn:
		{
			if (false == isOk(m_inputData[pItem->iRow - 1]))
			{
				SvStl::MessageManager Msg(SvStl::MsgType::Display);
				Msg.setMessage(m_inputData[pItem->iRow - 1].m_errorData);
			}
			if (0 < m_inputData[pItem->iRow - 1].m_dependencies.size())
			{
				SVShowDependentsDialog Dlg(m_inputData[pItem->iRow - 1].m_dependencies);
				Dlg.DoModal();
			}
			break;
		}
		case ValueButtonColumn:
		{
			ObjectSelectorData data {m_InspectionID};
			LinkedValueSelectorTypesEnum possibleType = LinkedValueSelectorTypesEnum::None;
			switch (m_inputData[pItem->iRow - 1].m_type)
			{
			case SvPb::InputTypeEnum::TypeDecimal:
			{
				possibleType = LinkedValueSelectorTypesEnum::All;
				data.m_type = SvPb::allNumberValueObjects;
				data.m_attribute = SvPb::selectableForEquation;
				data.m_searchArea = { SvPb::SearchArea::globalConstantItems, SvPb::SearchArea::cameraObject, SvPb::SearchArea::ppqItems, SvPb::SearchArea::toolsetItems };
				break;
			}
			case SvPb::InputTypeEnum::TypeText:
			{
				possibleType = LinkedValueSelectorTypesEnum::DirectIndirect;
				data.m_type = SvPb::allValueObjects;
				data.m_attribute = SvPb::ObjectAttributes::viewable;
				data.m_searchArea = { SvPb::SearchArea::globalConstantItems, SvPb::SearchArea::cameraObject, SvPb::SearchArea::ppqItems, SvPb::SearchArea::toolsetItems };
				break;
			}
			case SvPb::InputTypeEnum::TypeTable:
			{
				possibleType = LinkedValueSelectorTypesEnum::Indirect;
				data.m_type = SvPb::tableObjects;
				data.m_attribute = SvPb::ObjectAttributes::taskObject;
				data.m_searchArea = { SvPb::SearchArea::toolsetItems };
				break;
			}
			case SvPb::InputTypeEnum::TypeGrayImage:
			{
				possibleType = LinkedValueSelectorTypesEnum::Indirect;
				data.m_type = SvPb::grayImageObjects;
				data.m_attribute = SvPb::ObjectAttributes::archivableImage;
				data.m_searchArea = { SvPb::SearchArea::toolsetItems };
				break;
			}
			case SvPb::InputTypeEnum::TypeColorImage:
			{
				possibleType = LinkedValueSelectorTypesEnum::Indirect;
				data.m_type = SvPb::colorImageObjects;
				data.m_attribute = SvPb::ObjectAttributes::archivableImage;
				data.m_searchArea = { SvPb::SearchArea::toolsetItems };
				break;
			}
			case SvPb::InputTypeEnum::TypeImage:
			{
				possibleType = LinkedValueSelectorTypesEnum::Indirect;
				data.m_type = SvPb::allImageObjects;
				data.m_attribute = SvPb::ObjectAttributes::archivableImage;
				data.m_searchArea = { SvPb::SearchArea::toolsetItems };
				break;
			}
			}

			data.m_excludeSameLineageVector = { m_toolId };
			LinkedValueSelectorDialog dlg(m_InspectionID, m_Values.GetObjectID(SvPb::ExternalInputEId+(pItem->iRow - 1)), m_inputData[pItem->iRow - 1].m_name, m_inputData[pItem->iRow - 1].m_data, m_inputData[pItem->iRow - 1].m_data.m_defaultValue.vt, data, nullptr, possibleType);
			if (IDOK == dlg.DoModal())
			{
				m_inputData[pItem->iRow - 1].m_data = dlg.getData();
				setValueColumn(pItem->iRow - 1);
				setInspectionData(false);
				m_Grid.Refresh();
			}
		}
		break;
		default:
			//nothing to do
			return;
		}
	}

	void TADialogGroupToolInputPage::OnGridEndEdit(NMHDR* pNotifyStruct, LRESULT* pResult)
	{
		SvGcl::NM_GRIDVIEW* pItem = (SvGcl::NM_GRIDVIEW*) pNotifyStruct;
		bool bAcceptChange = true;

		if (0 < pItem->iRow && m_inputData.size() >= pItem->iRow)
		{
			std::string cellText = m_Grid.GetCell(pItem->iRow, pItem->iColumn)->GetText();
			switch (pItem->iColumn)
			{
			case NameColumn:
				if (m_inputData[pItem->iRow - 1].m_name != cellText)
				{
					if (m_inputData.end() != std::find_if(m_inputData.begin(), m_inputData.end(), [cellText](const auto& rEntry) { return rEntry.m_name == cellText; }))
					{
						SvDef::StringVector msgList;
						msgList.emplace_back(cellText);
						SvStl::MessageManager Msg(SvStl::MsgType::Display);
						Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_RenameError_DuplicateName, msgList, SvStl::SourceFileParams(StdMessageParams));
						bAcceptChange = false;
					}
					else
					{
						m_inputData[pItem->iRow - 1].m_name = cellText;
					}
				}
				break;
			case TypeColumn:
			{
				const auto& type = getType(cellText);
				m_inputData[pItem->iRow - 1].m_type = static_cast<SvPb::InputTypeEnum>(type.second);
				FillGridControl();
				break;
			}
			case DefaultColumn:
			{
				if (SvPb::isValueType(m_inputData[pItem->iRow - 1].m_type))
				{
					try
					{
						//check if text a correct variant	
						m_inputData[pItem->iRow - 1].m_data.m_defaultValue = checkText(m_inputData[pItem->iRow - 1].m_type, cellText);
					}
					catch (const SvStl::MessageContainer& rSvE)
					{
						SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
						Msg.setMessage(rSvE.getMessage());
						bAcceptChange = false;
						m_inputData[pItem->iRow - 1].m_data.m_defaultValue = SvPb::getDefaultString(m_inputData[pItem->iRow - 1].m_type);
						m_Grid.SetItemText(pItem->iRow, DefaultColumn, static_cast<CString>(m_inputData[pItem->iRow - 1].m_data.m_defaultValue));
					}
				}
				else
				{
					m_inputData[pItem->iRow - 1].m_data.m_defaultValue = _variant_t{};
				}
				break;
			}
			case ValueColumn:
			{
				bAcceptChange = setValue(m_inputData[pItem->iRow - 1], cellText);
				break;
			}
			}
		}

		*pResult = (bAcceptChange) ? 0 : -1;
	}

	void TADialogGroupToolInputPage::OnSelectionChanged(NMHDR*, LRESULT*)
	{
		setInspectionData(false);
		UpdateEnableButtons();
	}

	void TADialogGroupToolInputPage::OnBnClickedButtonAdd()
	{
		if (m_inputData.size() < c_maxValues)
		{
			GroupInputData data;
			if (0 < std::count_if(m_inputData.begin(), m_inputData.end(), [data](const auto& rEntry) { return rEntry.m_name == data.m_name; }))
			{ //duplicate
				std::string newName;
				int additionalValue = 1;
				do
				{
					newName = c_inputName + std::to_string(additionalValue++);
				} while (0 < std::count_if(m_inputData.begin(), m_inputData.end(), [newName](const auto& rEntry) { return rEntry.m_name == newName; }));
				data.m_name = newName;
			}
			m_inputData.emplace_back(std::move(data));
		}
		setInspectionData(false);
		FillGridControl();
	}

	void TADialogGroupToolInputPage::OnBnClickedButtonRemove()
	{
		SvGcl::CellRange Selection = m_Grid.GetSelectedCellRange();
		for (int i = Selection.GetMaxRow(); i >= Selection.GetMinRow() && i <= m_inputData.size(); --i)
		{
			for (int j = Selection.GetMinCol(); j <= Selection.GetMaxCol(); j++)
			{
				if (m_Grid.IsCellSelected(i, j) && 0 < i && m_inputData.size() > i -1)
				{
					m_inputData.erase(m_inputData.begin() + (i - 1));
					break;
				}
			}
		}
		FillGridControl();
	}

	void TADialogGroupToolInputPage::OnBnClickedMoveUp()
	{
		SvGcl::CellRange Selection = m_Grid.GetSelectedCellRange();
		if (Selection.GetMinRow() == Selection.GetMaxRow() && 1 < Selection.GetMinRow())
		{
			std::swap(m_inputData[Selection.GetMinRow() - 1], m_inputData[Selection.GetMinRow() - 2]);
			Selection.SetMinRow(Selection.GetMinRow() - 1);
			Selection.SetMaxRow(Selection.GetMaxRow() - 1);
			m_Grid.SetSelectedRange(Selection, true);
			m_Grid.SetFocusCell(Selection.GetMaxRow(), Selection.GetMaxCol());
			FillGridControl();
		}
	}

	void TADialogGroupToolInputPage::OnBnClickedMoveDown()
	{
		SvGcl::CellRange Selection = m_Grid.GetSelectedCellRange();
		if (Selection.GetMinRow() == Selection.GetMaxRow() && 0 < Selection.GetMinRow() && Selection.GetMinRow() + 1 < m_Grid.GetRowCount())
		{
			std::swap(m_inputData[Selection.GetMinRow() - 1], m_inputData[Selection.GetMinRow()]);
			Selection.SetMinRow(Selection.GetMinRow() + 1);
			Selection.SetMaxRow(Selection.GetMaxRow() + 1);
			m_Grid.SetSelectedRange(Selection, true);
			m_Grid.SetFocusCell(Selection.GetMaxRow(), Selection.GetMaxCol());
			FillGridControl();
		}
	}
#pragma endregion Protected Methods

#pragma region Private Methods
	bool TADialogGroupToolInputPage::setInspectionData(bool displayMessageBox)
	{
		UpdateData(TRUE);
		try
		{
			SvPb::InspectionCmdRequest requestCmd;
			SvPb::InspectionCmdResponse responseCmd;
			auto* pRequest = requestCmd.mutable_setandsortembeddedvaluerequest();
			pRequest->set_ownerid(m_TaskObjectID);
			for (const auto& data : m_inputData)
			{
				auto* pTmp = pRequest->add_embeddedlist();
				pTmp->set_name(data.m_name);
				pTmp->set_oldembeddedid(data.m_oldEmbeddedId);
				pTmp->set_type(data.m_type);
				*pTmp->mutable_value() = convertLinkedValue(data.m_data);
			}

			HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
			if (S_OK != hr)
			{
				if (responseCmd.has_setandsortembeddedvalueresponse())
				{
					auto& rMsg = responseCmd.setandsortembeddedvalueresponse();
					if (rMsg.has_errormessages())
					{
						m_errorMessages = convertProtobufToMessageVector(rMsg.errormessages());
					}
				}

				if (displayMessageBox)
				{
					SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
					if (0 < m_errorMessages.size())
					{
						Msg.setMessage(m_errorMessages[0].getMessage());
					}
					else
					{
						Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_UnknownException, SvStl::SourceFileParams(StdMessageParams));
					}
				}
			}
			else
			{
				m_errorMessages.clear();
			}
			loadDataList();
			FillGridControl();
			return S_OK == hr;
		}
		catch (const SvStl::MessageContainer& rSvE)
		{
			if (displayMessageBox)
			{
				SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
				Msg.setMessage(rSvE.getMessage());
			}
			return false;
		}
	}

	void TADialogGroupToolInputPage::initGridControl()
	{
		m_Grid.SetFixedRowCount(cHeaderSize);
		m_Grid.SetFixedColumnCount(0);
		m_Grid.SetRowResize(FALSE);
		m_Grid.SetColumnResize(true);
		m_Grid.AllowReorderColumn(false);
		m_Grid.EnableDragAndDrop(FALSE);
		m_Grid.SetEditable(true);
		m_Grid.SetColumnCount(static_cast<int>(g_columnInputObjectsDefArray.size()));
		SvGcl::GV_ITEM Item;
		Item.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR;
		Item.row = 0;
		Item.crBkClr = CLR_DEFAULT;
		Item.nFormat = DT_LEFT | DT_VCENTER | DT_WORDBREAK;
		for (const auto& rData : g_columnInputObjectsDefArray)
		{
			m_Grid.SetColumnWidth(rData.first, rData.second.m_columnSize);
			Item.col = rData.first;
			Item.strText = rData.second.m_name.c_str();
			m_Grid.SetItem(&Item);
		}
	}

	void TADialogGroupToolInputPage::FillGridControl()
	{
		CStringArray typeOptions;
		SvOi::NameValueVector typePairs;
		int numberOfObjects = static_cast<int>(m_inputData.size());
		if (0 < numberOfObjects)
		{
			typePairs = m_Values.GetEnumTypes(SvPb::InputObjectTypeEId);
			for (const auto& entry : typePairs)
			{
				typeOptions.Add(entry.first.c_str());
			}
		}
		m_Grid.SetRowCount(numberOfObjects + 1);
		for (int i = 0; numberOfObjects > i; ++i)
		{
			auto row = i + 1;

			bool hasDependencies = (0 < m_inputData[i].m_dependencies.size());
			m_Grid.SetItemBkColour(row, DependencyColumn,  isOk(m_inputData[i]) ? SvDef::White : SvDef::Black);
			m_Grid.SetItemFgColour(row, DependencyColumn, isOk(m_inputData[i]) ? SvDef::Black : SvDef::White);
			m_Grid.SetItemText(row, DependencyColumn, hasDependencies ? "D" : "");
			m_Grid.SetItemState(row, DependencyColumn, m_Grid.GetItemState(row, DependencyColumn) | GVIS_READONLY);

			m_Grid.SetItemText(row, NameColumn, m_inputData[i].m_name.c_str());
			m_Grid.SetItemState(row, NameColumn, m_Grid.GetItemState(row, NameColumn) & (~GVIS_READONLY));
			using namespace SvGcl;
			m_Grid.SetCellType(row, TypeColumn, RUNTIME_CLASS(GridCellCombo));
			auto* pCell = dynamic_cast<SvGcl::GridCellCombo*>(m_Grid.GetCell(row, TypeColumn));
			
			pCell->SetOptions(typeOptions);
			pCell->SetStyle(CBS_DROPDOWNLIST);
			auto type = m_inputData[i].m_type;
			auto iter = std::find_if(typePairs.begin(), typePairs.end(), [type](const auto& rEntry) { return rEntry.second == type; });
			if (typePairs.end() != iter)
			{
				pCell->SetText(iter->first.c_str());
			}
			else
			{
				pCell->SetText(typePairs[0].first.c_str());
			}

			//check if defaultValue correct
			try
			{
				m_inputData[i].m_data.m_defaultValue = checkText(m_inputData[i].m_type, m_inputData[i].m_data.m_defaultValue);
			}
			catch(...)
			{
				m_inputData[i].m_data.m_defaultValue = SvPb::getDefaultString(type);
			}
			
			if (SvPb::isValueType(type)) 
			{
				m_Grid.SetItemText(row, DefaultColumn, static_cast<CString>(m_inputData[i].m_data.m_defaultValue));
				m_Grid.SetItemState(row, DefaultColumn, m_Grid.GetItemState(row, DefaultColumn) & (~GVIS_READONLY));
			}
			else
			{
				m_Grid.SetItemText(row, DefaultColumn, "");
				m_Grid.SetItemState(row, DefaultColumn, m_Grid.GetItemState(row, DefaultColumn) | GVIS_READONLY);
			}

			setValueColumn(i);
			
			SvGcl::GV_ITEM buttonItem;
			buttonItem.mask = GVIF_IMAGE;
			buttonItem.iImage = 0;
			buttonItem.row = row;
			buttonItem.col = ValueButtonColumn;
			m_Grid.SetItem(&buttonItem);
		}

		m_Grid.Refresh();
		UpdateEnableButtons();
	}

	void TADialogGroupToolInputPage::setValueColumn(int pos)
	{
		bool isChangeable{ false };
		CString valueString;
		if (SvPb::isValueType(m_inputData[pos].m_type))
		{
			valueString = static_cast<CString>(m_inputData[pos].m_data.m_Value);
			isChangeable = SvPb::LinkedSelectedType::DirectValue == m_inputData[pos].m_data.m_type;
			if (valueString.IsEmpty())
			{
				valueString = static_cast<CString>(SvPb::getDefaultString(m_inputData[pos].m_type));
			}
		}
		else
		{
			valueString = SvCmd::getDottedName(m_InspectionID, m_inputData[pos].m_data.m_indirectIdName).c_str();
		}
		m_Grid.SetItemText(pos+1, ValueColumn, valueString);
		if (isChangeable)
		{
			m_Grid.SetItemBkColour(pos + 1, ValueColumn, SvDef::White);
			m_Grid.SetItemState(pos+1, ValueColumn, m_Grid.GetItemState(pos+1, ValueColumn) & (~GVIS_READONLY));
		}
		else
		{
			m_Grid.SetItemBkColour(pos + 1, ValueColumn, SvDef::WhiteSmoke);
			m_Grid.SetItemState(pos+1, ValueColumn, m_Grid.GetItemState(pos+1, ValueColumn) | GVIS_READONLY);
		}
	}

	void TADialogGroupToolInputPage::showContextMenu(CPoint point)
	{
		CMenu menu;
		if (menu.LoadMenu(IDR_TABLE_DEFINE_MOUSE_MENU))
		{
			if (CMenu* pPopupMenu = menu.GetSubMenu(0))
			{
				ClientToScreen(&point);
				pPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
			}
		}
	}

	SvOi::NameValuePair TADialogGroupToolInputPage::getType(const std::string& rTypeName)
	{
		const auto& rTypePairs = m_Values.GetEnumTypes(SvPb::InputObjectTypeEId);
		auto iter = std::find_if(rTypePairs.begin(), rTypePairs.end(), [rTypeName](const auto& rEntry) { return rTypeName == rEntry.first; });
		if (rTypePairs.end() != iter)
		{
			return *iter;
		}
		else
		{
			assert(false);
			return {};
		}
	}

	void TADialogGroupToolInputPage::loadDataList()
	{
		m_inputData.clear();
		m_Values.Init();

		for (int i = 0; c_maxValues > i; ++i)
		{
			if (0 == m_Values.GetAllowedAttributeFlags(SvPb::ExternalInputEId + i))
			{	//stop loading values, because it is hidden.
				break;
			}

			GroupInputData data;
			data.m_name = m_Values.GetName(SvPb::ExternalInputEId + i);
			data.m_oldEmbeddedId = SvPb::ExternalInputEId + i;
			data.m_type = static_cast<SvPb::InputTypeEnum>(m_Values.Get<int>(SvPb::InputObjectTypeEId + i));
			data.m_data = m_Values.Get<LinkedValueData>(SvPb::ExternalInputEId + i);
			
			auto valueId = m_Values.GetObjectID(SvPb::ExternalInputEId + i);
			data.m_dependencies = getDependency(valueId);
			data.m_errorData = getMessage(m_errorMessages, valueId);;
			m_inputData.emplace_back(std::move(data));
		}
	}

	void TADialogGroupToolInputPage::UpdateEnableButtons()
	{
		SvGcl::CellRange Selection = m_Grid.GetSelectedCellRange();
		bool bMoveUpEnable = false;
		bool bMoveDownEnable = false;
		if (Selection.GetMinRow() == Selection.GetMaxRow())
		{
			int pos = Selection.GetMinRow(); //GetMaxRow identically
			bMoveUpEnable = (1 < pos && pos <= m_inputData.size());
			bMoveDownEnable = (0 < pos && pos + 1 < m_Grid.GetRowCount());
		}

		GetDlgItem(IDC_BUTTON_MOVEUP)->EnableWindow(bMoveUpEnable);
		GetDlgItem(IDC_BUTTON_MOVEDOWN)->EnableWindow(bMoveDownEnable);

		bool bRemoveEnable = -1 != Selection.GetMaxRow() && Selection.GetMinRow() <= m_inputData.size();
		if (bRemoveEnable)
		{
			for (int i = Selection.GetMaxRow(); i >= Selection.GetMinRow() && i <= m_inputData.size(); --i)
			{
				for (int j = Selection.GetMinCol(); j <= Selection.GetMaxCol(); j++)
				{
					if (m_Grid.IsCellSelected(i, j) && 0 < i && m_inputData.size() > i - 1)
					{
						bRemoveEnable = bRemoveEnable && 0 == m_inputData[i-1].m_dependencies.size();
					}
				}
			}
		}
		GetDlgItem(IDC_BUTTON_REMOVE)->EnableWindow(bRemoveEnable);

		if (SvDef::c_maxTableColumn > m_inputData.size())
		{
			GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(true);
		}
		else
		{
			GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(false);
		}
	}
#pragma endregion Private Mehods
} //namespace SvOg
