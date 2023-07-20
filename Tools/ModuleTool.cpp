//*****************************************************************************
/// \copyright (c) 2020,2020 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file ModuleTool.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Implements the ModuleTool
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "ModuleTool.h"
#include "ObjectInterfaces/IConfigurationObject.h"
#include "ObjectInterfaces/IModuleTool.h"
#pragma endregion Includes

namespace SvTo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

	SV_IMPLEMENT_CLASS(ModuleTool, SvPb::ModuleToolClassId);

	ModuleTool::ModuleTool(SVObjectClass* pOwner, int StringResourceID)
		:GroupTool(pOwner, StringResourceID)
	{
		Initialize();
	}

	ModuleTool::~ModuleTool()
	{
		SvOi::unregisterModuleInstance(m_moduleGuid, getObjectId());
	}

	bool ModuleTool::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
	{
		m_isCreated = __super::CreateObject(rCreateStructure);

		m_moduleComment.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);

		return m_isCreated;
	}

	bool ModuleTool::ResetObject(SvStl::MessageContainerVector* pErrorMessages)
	{
		bool result = __super::ResetObject(pErrorMessages);

		setViewFlag();

		try
		{
			SvOi::registerModuleInstance(m_moduleGuid, getObjectId(), getModuleComment(), m_historyList, false == m_isEditing);
		}
		catch (SvStl::MessageContainer& rErr)
		{
			result = false;
			if (pErrorMessages)
			{
				pErrorMessages->push_back(rErr);
			}
		}

		return result;
	}

	std::vector<std::string> ModuleTool::getToolAdjustNameList() const
	{
		constexpr std::array<LPCTSTR, 4> cToolAdjustNameList
		{
			_T("Input"),
			_T("Conditional"),
			_T("Module Comment"),
			_T("Comment"),
		};
		return { cToolAdjustNameList.begin(), cToolAdjustNameList.end() };
	}

	void ModuleTool::Persist(SvOi::IObjectWriter& rWriter, bool closeObject/* = true*/) const
	{
		__super::Persist(rWriter, false);

		rWriter.WriteAttribute(scModuleGuidTag, m_moduleGuid.ToString().c_str());
		rWriter.StartElement(scHistoryTag);
		for (const auto& rValue : m_historyList)
		{
			rWriter.StartElement(std::to_string(rValue.first).c_str());
			rWriter.WriteAttribute(scCommentTag, rValue.second.c_str());
			rWriter.EndElement();
		}
		rWriter.EndElement();

		if (closeObject)
		{
			rWriter.EndElement();
		}
	}

	HRESULT ModuleTool::SetObjectValue(SVObjectAttributeClass* pDataObject)
	{
		SvCl::SVObjectStdStringArrayClass stringList;
		if (std::string(pDataObject->GetName()) == scModuleGuidTag)
		{
			auto data = pDataObject->getData();
			if (VT_BSTR == data.vt)
			{
				m_moduleGuid = SVGUID{pDataObject->getData()};
				return S_OK;
			}
			
			Log_Assert(false);
			return E_FAIL;
		}
		//else if (pDataObject->GetAttributeData(scLinkedIndirectValueTag, stringList))
		//{
		//	m_indirectValueRef = SVObjectReference(0 < stringList.size() ? stringList[0] : "");
		//	return S_OK;
		//}
		//else if (pDataObject->GetAttributeData(scLinkedFormulaTag, stringList))
		//{
		//	m_formulaString = (0 < stringList.size() ? stringList[0] : "");
		//	SvUl::RemoveEscapedSpecialCharacters(m_formulaString, true);
		//	return S_OK;
		//}
		else
		{
			return __super::SetObjectValue(pDataObject);
		}
	}

	SVGUID ModuleTool::renewModuleGuid()
	{
		auto status = (CoCreateGuid(&m_moduleGuid) == S_OK);
		Log_Assert(status);
		return m_moduleGuid;
	}

	std::string ModuleTool::getModuleComment() const
	{
		std::string text;
		m_moduleComment.GetValue(text);
		return text;
	}

	void ModuleTool::setModuleComment(const std::string& rText)
	{
		m_moduleComment.setValue(rText);
	}

	void ModuleTool::Initialize()
	{
		m_canResizeToParent = false;
		m_ObjectTypeInfo.m_ObjectType = SvPb::SVToolObjectType;
		m_ObjectTypeInfo.m_SubType = SvPb::ModuleToolObjectType;

		renewModuleGuid();
		m_historyList.emplace_back(std::time(nullptr), "Init");

		RegisterEmbeddedObject(&m_moduleComment, SvPb::ModuleCommentEId, IDS_OBJECTNAME_MODULE_COMMENT, false, SvOi::SVResetItemNone, true);
		m_moduleComment.SetDefaultValue(_T(""));
	}

	void ModuleTool::setViewFlag()
	{
		for (auto* pTask : m_TaskObjectVector)
		{
			if (pTask && SvPb::ParameterTaskObjectType != pTask->GetObjectType())
			{
				if (m_isEditing)
				{
					pTask->SetObjectAttributesAllowed(SvPb::viewable, SvOi::SetAttributeType::AddAttribute);
				}
				else
				{
					pTask->SetObjectAttributesAllowed(SvPb::viewable, SvOi::SetAttributeType::RemoveAttribute);
				}
			}
		}
	}
} //namespace SvTo

bool SvOi::setHistory(uint32_t objectID, const std::vector<std::pair<time_t, std::string>>& rHistoryVector)
{
	SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject(objectID);
	auto* pModule = dynamic_cast<SvTo::ModuleTool*>(pObject);
	if (pModule)
	{
		pModule->setHistory(rHistoryVector);
		return true;
	}
	
	return false;
}