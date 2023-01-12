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
	}

	void ModuleTool::MoveObjectToThis(GroupTool& rGroupTool)
	{
		DestroyFriends();
		rGroupTool.movedAndDeleteFriends(m_friendList);
		for (int i = 0; m_friendList.size() > i; ++i)
		{
			if (m_friendList[i])
			{
				m_friendList[i]->SetObjectOwner(this);
			}
		}
	
		DeleteAll();
		rGroupTool.movedAndDeleteTaskObjects(m_TaskObjectVector);
		for (auto* pObject : m_TaskObjectVector)
		{
			if (pObject)
			{
				pObject->SetObjectOwner(this);
			}
		}

		rGroupTool.moveEmbeddedObjects(m_embeddedList);
		//The inputs are not moved, because the GroupTool don't use the inputs.
		
		SetName(rGroupTool.GetName());
		m_moduleComment.setValue(rGroupTool.getComment());
		rGroupTool.moveObject(*this);
	}

	bool ModuleTool::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
	{
		m_isCreated = __super::CreateObject(rCreateStructure);

		m_moduleComment.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);

		for (auto* pTask : m_TaskObjectVector)
		{
			if (pTask && SvPb::ParameterTaskObjectType != pTask->GetObjectType())
			{
				pTask->SetObjectAttributesAllowed(SvPb::viewable, SvOi::SetAttributeType::RemoveAttribute);
			}
		}

		return m_isCreated;
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

	void ModuleTool::Initialize()
	{
		m_canResizeToParent = false;
		m_ObjectTypeInfo.m_ObjectType = SvPb::SVToolObjectType;
		m_ObjectTypeInfo.m_SubType = SvPb::ModuleToolObjectType;

		RegisterEmbeddedObject(&m_moduleComment, SvPb::ModuleCommentEId, IDS_OBJECTNAME_MODULE_COMMENT, false, SvOi::SVResetItemNone, false);
		m_moduleComment.SetDefaultValue(_T(""));
	}
} //namespace SvTo
