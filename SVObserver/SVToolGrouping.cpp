//******************************************************************************
//* .Module Name     : SVToolGrouping
//* .File Name       : $Workfile:   SVToolGrouping.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.11  $
//* .Check In Date   : $Date:   12 Feb 2015 13:45:42  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <algorithm>
//Moved to precompiled header: #include <sstream>
#include "SVToolGrouping.h"
#include "ObjectInterfaces/IObjectWriter.h"
#include "ObjectInterfaces/ObjectInfo.h"
#include "SVMessage/SVMessage.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVXMLLibrary/SVConfigurationTags.h"
#include "SVXMLLibrary/SVNavigateTree.h"
#pragma endregion Includes

constexpr const char* c_DefaultToolGroupName = _T("Group");
constexpr const char* c_EndPrefix = _T("End ");

bool SVToolGrouping::Correct(const SvOi::ObjectInfoVector& toolsetinfo, int& rNchanged)
{
	bool res{ false };
	rNchanged = 0;
	int toolsetindex{ 0 };
	const int toolsetSize = static_cast<int>( toolsetinfo.size());
	int ToolsCountInGrouping = static_cast<int>(std::count_if(m_list.begin(), m_list.end(),
		[](const ToolGroup& p) {return (p.second.m_type == ToolGroupData::Tool); }));


	for (auto it = m_list.begin(); it != m_list.end(); )
	{
		if (it->second.m_type != ToolGroupData::Tool)
		{
			//no tool 
			it++;
		}
		else
		{
			
			if (toolsetindex < toolsetSize)
			{
				std::string name = toolsetinfo.at(toolsetindex).DisplayName;
				if (name == it->first.c_str())
				{
					//ok  
					it++;
					toolsetindex++;
				}
				else
				{
					if (ToolsCountInGrouping == toolsetSize)
					{
						//rename tool in toolgrouping 
						TRACE2("SVToolGrouping Rename %s to %s ", it->first.c_str(), name.c_str());
						it->first = name;
						it->second.m_name = name;
						res = true;
						it++;
						toolsetindex++;

					}
					else if (ToolsCountInGrouping > toolsetSize)
					{
						//delete  tool from  toolgrouping
						TRACE1("SVToolGrouping Delete %s", it->first.c_str());
						it = m_list.erase(it);
						ToolsCountInGrouping--;
						rNchanged--;
						res = true;
					}
					else
					{
						///insert tool to toolgrouping
						TRACE1("SVToolGrouping Insert %s ", name.c_str());
						m_list.emplace(it, std::make_pair(name, ToolGroupData(ToolGroupData::Tool, name)));
						ToolsCountInGrouping++;
						rNchanged++;
						res = true;
						toolsetindex++;
					}
				}
			}
			else //  (toolsetindex >= toolsetSize)
			{
				//tool in grouping but no more tools in toolset
				//delete all following tools in   grouping 
				TRACE1("SVToolGrouping Delete %s", it->first.c_str());
				it = m_list.erase(it);
				ToolsCountInGrouping--;
				rNchanged--;
				res = true;
			}
		}
	}
	
	//tools toolset add to grouping 
	for (int i = toolsetindex; i < toolsetSize; i++)
	{
		std::string name = toolsetinfo.at(i).DisplayName;
		TRACE1("SVToolGrouping Insert %s ", name.c_str());
		m_list.emplace_back(std::make_pair(name, ToolGroupData(ToolGroupData::Tool, name)));
		ToolsCountInGrouping++;
		rNchanged++;
		res = true;
	}
	return res;
}



std::string SVToolGrouping::GetDefaultName() const
{
	std::string name = c_DefaultToolGroupName;
	int cnt = 0;
	while (!IsNameUnique(name))
	{
		std::basic_stringstream<TCHAR> os;
		os << ++cnt;
		name = c_DefaultToolGroupName + os.str();
	}
	return name;
}

bool SVToolGrouping::IsNameUnique(const std::string& rName, LPCTSTR pExclude) const
{
	std::string nameLower;
	std::transform(rName.begin(), rName.end(), std::back_inserter(nameLower), [](unsigned char c) { return static_cast<char> (std::tolower(c)); });

	bool bRetVal = std::none_of(m_list.begin(), m_list.end(),
		[&nameLower, pExclude, rName](const ToolGroup& rGroup)->bool
	{
		if (nullptr != pExclude)
		{
			if (0 == rGroup.first.compare(pExclude))
			{
				return  false;
			}
		}
		// case insensitive compare
		std::string itemNameLower;
		std::transform(rGroup.first.begin(), rGroup.first.end(), std::back_inserter(itemNameLower), [](unsigned char c) { return static_cast<char> (std::tolower(c)); });
		return (itemNameLower == nameLower);
	});

	return bRetVal;
}

std::string SVToolGrouping::determineToolnameWithUniqueIndex(const std::string& rName, std::map<std::string, int>* pHighestUsedIndexForBaseToolname) const
{
	std::string newName = rName;
	std::string lowercaseBaseName;
	std::transform(rName.begin(), rName.end(), std::back_inserter(lowercaseBaseName), [](unsigned char c) { return static_cast<char> (std::tolower(c)); });

	//This strips any numbers at the end of the name
	size_t last_char_pos = lowercaseBaseName.find_last_not_of(_T("0123456789"));
	if (last_char_pos != std::string::npos)
	{
		lowercaseBaseName = lowercaseBaseName.substr(0, last_char_pos + 1);
	}

	int lowestUnusedToolnumber = 0;
	// find all names that start with this name (case insensitive)
	std::for_each(m_list.begin(), m_list.end(), [&lowercaseBaseName, &lowestUnusedToolnumber](const ToolGroup& rGroup)->void
	{
		std::string itemNameLower;
		std::transform(rGroup.first.begin(), rGroup.first.end(), std::back_inserter(itemNameLower), [](unsigned char c) { return static_cast<char> (std::tolower(c)); });
		size_t pos = itemNameLower.find(lowercaseBaseName);
		if (0 == pos)
		{
			// check if the base name is augmented with numeric values
			if (itemNameLower == lowercaseBaseName)
			{
				// check for trailing digits
				size_t last_char_pos = itemNameLower.find_last_not_of(_T("0123456789"));
				if (last_char_pos != std::string::npos)
				{
					std::string val = itemNameLower.substr(last_char_pos + 1);
					if (!val.empty())
					{
						int lastNum = atoi(val.c_str());
						lowestUnusedToolnumber = std::max(lowestUnusedToolnumber, lastNum + 1);
					}
					else
					{
						lowestUnusedToolnumber = std::max(lowestUnusedToolnumber, 1);
					}
				}
				else
				{
					lowestUnusedToolnumber = std::max(lowestUnusedToolnumber, 1);
				}
			}
			else
			{
				// check for trailing digits
				size_t last_char_pos = itemNameLower.find_last_not_of(_T("0123456789"));
				if (last_char_pos != std::string::npos)
				{
					std::string val = itemNameLower.substr(last_char_pos + 1);
					if (!val.empty())
					{
						// convert and set max
						int lastNum = atoi(val.c_str());
						lowestUnusedToolnumber = std::max(lowestUnusedToolnumber, lastNum + 1);
					}
				}
			}
		}
	});

	if (nullptr != pHighestUsedIndexForBaseToolname)
	{
		auto iter = pHighestUsedIndexForBaseToolname->find(lowercaseBaseName);

		if(iter != pHighestUsedIndexForBaseToolname->end())
		{
			if (lowestUnusedToolnumber <= iter->second)
			{
				lowestUnusedToolnumber = iter->second + 1;
			}
		}
		(*pHighestUsedIndexForBaseToolname)[lowercaseBaseName] = lowestUnusedToolnumber;
	}

	// build new name
	if (lowestUnusedToolnumber)
	{
		std::stringstream ss;
		// Get Base Name (can't end in a number)
		std::string baseName = rName;
		last_char_pos = rName.find_last_not_of(_T("0123456789"));
		if (last_char_pos != std::string::npos)
		{
			baseName = rName.substr(0, last_char_pos + 1);
		}
		ss << baseName << lowestUnusedToolnumber;
		newName = ss.str().c_str();
	}
	return newName;
}

std::string SVToolGrouping::GetToolToInsertBefore(const std::string& rName) const
{
	std::string toolName;
	if (!rName.empty())
	{
		ToolGroupList::const_iterator it = std::find_if(m_list.begin(), m_list.end(),
			[&rName](const ToolGroup& rGroup)->bool { return rName == rGroup.first; });
		if (it != m_list.end())
		{
			if (ToolGroupData::StartOfGroup == it->second.m_type || ToolGroupData::EndOfGroup == it->second.m_type)
			{
				// Start of Group or End of Group selected - get first tool after selection
				while (++it != m_list.end() && toolName.empty())
				{
					if (it->second.m_type == ToolGroupData::Tool)
					{
						toolName = it->first;
					}
				}
			}
			else
			{
				toolName = it->first;
			}
		}
	}
	return toolName;
}

void SVToolGrouping::AddGroup(const std::string& rName, const std::string& rInsertBefore)
{
	ToolGroupList::iterator it = m_list.end();
	if (!rInsertBefore.empty())
	{
		it = std::find_if(m_list.begin(), m_list.end(),
			[&rInsertBefore](const ToolGroup& rGroup)->bool { return rInsertBefore == rGroup.first; });
	}
	ToolGroupList::iterator curIt = m_list.insert(it, std::make_pair(rName, ToolGroupData(ToolGroupData::StartOfGroup, rName)));

	// check if end group needs to be reassigned to this newly added group (split group)
	it = std::find_if(curIt, m_list.end(),
		[&rName](const ToolGroup& rGroup)->bool { return rName != rGroup.first && (ToolGroupData::EndOfGroup == rGroup.second.m_type || ToolGroupData::StartOfGroup == rGroup.second.m_type); });
	if (it != m_list.end())
	{
		if (ToolGroupData::EndOfGroup == it->second.m_type)
		{
			std::string endName = it->first;
			// find current start group for end group and unassign
			it = std::find_if(m_list.begin(), m_list.end(),
				[&endName](const ToolGroup& rGroup)->bool { return endName == rGroup.second.m_endName && ToolGroupData::StartOfGroup == rGroup.second.m_type; });
			if (it != m_list.end())
			{
				it->second.m_endName.clear();
			}
			// reassign the end to the newly inserted group
			curIt->second.m_endName = endName;
		}
	}
}

bool SVToolGrouping::AddEndGroup(const std::string& rGroupName, const std::string& rInsertBefore)
{
	bool bRetVal = false;

	if (!rGroupName.empty())
	{
		std::string endName = c_EndPrefix + rGroupName;
		if (!IsNameUnique(endName))
		{
			endName = determineToolnameWithUniqueIndex(endName);
		}
		if (!rInsertBefore.empty())
		{
			auto it = std::find_if(m_list.begin(), m_list.end(),
				[&rInsertBefore](const ToolGroup& rGroup)->bool { return rInsertBefore == rGroup.first; });
			m_list.insert(it, std::make_pair(endName, ToolGroupData(ToolGroupData::EndOfGroup, rGroupName, endName, false)));

			// Find Start Group
			it = std::find_if(m_list.begin(), m_list.end(),
				[&rGroupName](const ToolGroup& rGroup)->bool { return ToolGroupData::StartOfGroup == rGroup.second.m_type && rGroupName == rGroup.first; });
			if (it != m_list.end())
			{
				// Update Start group with End group name
				it->second.m_endName = endName;
			}
		}
		else
		{
			// Find End of this group
			auto it = std::find_if(m_list.begin(), m_list.end(),
				[&rGroupName](const ToolGroup& rGroup)->bool { return rGroupName == rGroup.first; });
			if (it != m_list.end())
			{
				// Update Start group with End group name
				it->second.m_endName = endName;
				// Find the End of the group (next Start, or End of the list)
				it = std::find_if(it, m_list.end(), [&rGroupName](const ToolGroup& rGroup)->bool { return ToolGroupData::StartOfGroup == rGroup.second.m_type && rGroupName != rGroup.first; });
				m_list.insert(it, std::make_pair(endName, ToolGroupData(ToolGroupData::EndOfGroup, rGroupName, endName, false)));
			}
		}
	}
	return bRetVal;
}

void SVToolGrouping::AddTool(const std::string& rName, const std::string& rInsertBefore)
{
	ToolGroupList::iterator it = m_list.end();
	if (!rInsertBefore.empty())
	{
		it = std::find_if(m_list.begin(), m_list.end(),
			[&rInsertBefore](const ToolGroup& rGroup)->bool { return rInsertBefore == rGroup.first; });
	}
	m_list.insert(it, std::make_pair(rName, ToolGroupData(ToolGroupData::Tool, rName)));
}

bool SVToolGrouping::RemoveGroup(const std::string& rName)
{
	bool bRetVal = false;
	if (IsEndTag(rName))
	{
		ToolGroupList::iterator it = std::find_if(m_list.begin(), m_list.end(),
			[&rName](const ToolGroup& rGroup)->bool { return (rName == rGroup.first && ToolGroupData::EndOfGroup == rGroup.second.m_type); });
		if (it != m_list.end())
		{
			// Remove End 
			m_list.erase(it);
			bRetVal = true;

			it = std::find_if(m_list.begin(), m_list.end(),
				[&rName](const ToolGroup& rGroup)->bool { return (rName == rGroup.second.m_endName && ToolGroupData::StartOfGroup == rGroup.second.m_type); });
			if (it != m_list.end())
			{
				// Update Start
				it->second.m_endName.clear();
			}
		}
	}
	else if (IsStartTag(rName))
	{
		// Remove Start and End
		ToolGroupList::iterator it = std::find_if(m_list.begin(), m_list.end(),
			[&rName](const ToolGroup& rGroup)->bool { return (rName == rGroup.first && ToolGroupData::StartOfGroup == rGroup.second.m_type); });
		if (it != m_list.end())
		{
			std::string endName = it->second.m_endName;
			m_list.erase(it);
			it = std::find_if(m_list.begin(), m_list.end(),
				[&endName](const ToolGroup& rGroup)->bool { return (endName == rGroup.first && ToolGroupData::EndOfGroup == rGroup.second.m_type); });
			if (it != m_list.end())
			{
				m_list.erase(it);
				bRetVal = true;
			}
		}
	}
	return bRetVal;
}

bool SVToolGrouping::RemoveTool(const std::string& rName)
{
	bool bRetVal = false;
	ToolGroupList::iterator it = std::find_if(m_list.begin(), m_list.end(),
		[&rName](const ToolGroup& rGroup)->bool { return (rName == rGroup.first && ToolGroupData::Tool == rGroup.second.m_type); });
	if (it != m_list.end())
	{
		m_list.erase(it);
	}
	return bRetVal;
}

bool SVToolGrouping::RenameItem(const std::string& rOldName, const std::string& rNewName)
{
	bool bRetVal = false;
	if (IsEndTag(rOldName))
	{
		// Update End name in Start Group
		ToolGroupList::iterator it = std::find_if(m_list.begin(), m_list.end(),
			[&rOldName](const ToolGroup& rGroup)->bool { return (rOldName == rGroup.second.m_endName && ToolGroupData::StartOfGroup == rGroup.second.m_type); });
		if (it != m_list.end())
		{
			// Update End name in Start Group
			it->second.m_endName = rNewName;
		}
	}
	ToolGroupList::iterator it = std::find_if(m_list.begin(), m_list.end(),
		[&rOldName](const ToolGroup& rGroup)->bool { return rOldName == rGroup.first; });
	if (it != m_list.end())
	{
		it->first = rNewName;
		it->second.m_name = rNewName;
		bRetVal = true;
	}
	return bRetVal;
}

std::string SVToolGrouping::FindCandidateStartGroup(const std::string& rName) const
{
	std::string group;
	ToolGroupList::const_reverse_iterator it = m_list.rbegin();
	if (!rName.empty())
	{
		it = std::find_if(it, m_list.rend(), [&rName](const ToolGroup& rToolGroup)->bool { return rName == rToolGroup.first; });
	}
	if (it != m_list.rend())
	{
		it = std::find_if(it, m_list.rend(), [&rName](const ToolGroup& rToolGroup)->bool { return ToolGroupData::StartOfGroup == rToolGroup.second.m_type && rName != rToolGroup.first; });
		if (it != m_list.rend() && it->second.m_endName.empty())
		{
			group = it->first;
		}
	}
	return group;
}

std::string SVToolGrouping::FindGroup(const std::string& rName) const
{
	std::string group;
	ToolGroupList::const_reverse_iterator it = m_list.rbegin();
	if (!rName.empty())
	{
		it = std::find_if(it, m_list.rend(), [&rName](const ToolGroup& rToolGroup)->bool { return rName == rToolGroup.first; });
	}
	if (it != m_list.rend())
	{
		it = std::find_if(it, m_list.rend(), [&rName](const ToolGroup& rToolGroup)->bool { return ToolGroupData::StartOfGroup == rToolGroup.second.m_type && rName != rToolGroup.first; });
		if (it != m_list.rend())
		{
			group = it->first;
		}
	}
	return group;
}

bool SVToolGrouping::IsStartTag(const std::string& rName, bool& bState) const
{
	bool bRetVal = false;
	bState = false;
	ToolGroupList::const_iterator it = std::find_if(m_list.begin(), m_list.end(),
		[&rName](const ToolGroup& rGroup)->bool { return rName == rGroup.first && ToolGroupData::StartOfGroup == rGroup.second.m_type; });
	if (it != m_list.end())
	{
		bState = it->second.m_bCollapsed;
		bRetVal = true;
	}
	return bRetVal;
}

bool SVToolGrouping::IsStartTag(const std::string& rName) const
{
	bool bRetVal = false;
	ToolGroupList::const_iterator it = std::find_if(m_list.begin(), m_list.end(),
		[&rName](const ToolGroup& rGroup)->bool { return rName == rGroup.first && ToolGroupData::StartOfGroup == rGroup.second.m_type; });
	if (it != m_list.end())
	{
		bRetVal = true;
	}
	return bRetVal;
}

bool SVToolGrouping::IsEndTag(const std::string& rName) const
{
	bool bRetVal = false;
	ToolGroupList::const_iterator it = std::find_if(m_list.begin(), m_list.end(),
		[&rName](const ToolGroup& rGroup)->bool { return rName == rGroup.first && ToolGroupData::EndOfGroup == rGroup.second.m_type; });
	if (it != m_list.end())
	{
		bRetVal = true;
	}
	return bRetVal;
}

bool SVToolGrouping::IsCollapsed(const std::string& rName) const
{
	bool bCollapsed = false;
	ToolGroupList::const_iterator it = std::find_if(m_list.begin(), m_list.end(),
		[&rName](const ToolGroup& rGroup)->bool
	{
		return rName == rGroup.first;
		// && ToolGroupData::StartOfGroup == rGroup.second.m_type;
	});
	if (it != m_list.end())
	{
		bCollapsed = it->second.m_bCollapsed;
	}
	return bCollapsed;
}

bool SVToolGrouping::Collapse(const std::string& rName, bool bCollapse)
{
	bool bRetVal = false;
	ToolGroupList::iterator it = std::find_if(m_list.begin(), m_list.end(),
		[&rName](const ToolGroup& rGroup)->bool { return rName == rGroup.first; }); /*&& ToolGroupData::StartOfGroup == rGroup.second.m_type; }); */
	if (it != m_list.end())
	{
		it->second.m_bCollapsed = bCollapse;
		bRetVal = true;
	}
	return bRetVal;
}

HRESULT SVToolGrouping::LoadTools(SVTreeType& rTree, SVTreeType::SVBranchHandle htiTools, SVToolGrouping& rGroupings)
{
	HRESULT hr = S_OK;
	if (htiTools)
	{
		// Get the Tools
		SVToolGrouping::SVTreeType::SVLeafHandle htiLeaf;
		htiLeaf = rTree.getFirstLeaf(htiTools);
		while (S_OK == hr && rTree.isValidLeaf(htiTools, htiLeaf))
		{
			std::string leafName = rTree.getLeafName(htiLeaf);
			if (SvXml::CTAG_TOOL == leafName)
			{
				_variant_t Value;
				rTree.getLeafData(htiLeaf, Value);
				if (VT_EMPTY != Value.vt)
				{
					std::string toolName = SvUl::createStdString(Value.GetVARIANT());
					bool collapse = false;
					if (0 < toolName.size() && '+' == toolName[0])
					{
						toolName.erase(0,1);
						collapse = true;
					}
					if (!toolName.empty())
					{
						rGroupings.m_list.insert(rGroupings.m_list.end(), std::make_pair(toolName, ToolGroupData(ToolGroupData::Tool, toolName, collapse)));
					}
					else
					{
						hr = SVMSG_SVO_49_LOAD_CONFIGURATION_TOOLGROUPING;
					}
				}
				else
				{
					hr = SVMSG_SVO_49_LOAD_CONFIGURATION_TOOLGROUPING;
				}
			}
			else
			{
				hr = SVMSG_SVO_49_LOAD_CONFIGURATION_TOOLGROUPING;
			}
			htiLeaf = rTree.getNextLeaf(htiTools, htiLeaf);
		}
	}
	return  hr;
}

// Load the Groupings
HRESULT SVToolGrouping::SetParameters(SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent)
{
	HRESULT hr = S_OK;
	SVToolGrouping groupings;
	SVTreeType::SVBranchHandle htiChild = nullptr;
	if (SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::CTAG_TOOLGROUPINGS, htiParent, htiChild))
	{
		_variant_t svValue;
		SVTreeType::SVBranchHandle htiSubChild(rTree.getFirstBranch(htiChild));
		
		
		while (S_OK == hr && nullptr != htiSubChild)
		{
			// Will be either Tools or Group
			std::string name = rTree.getBranchName(htiSubChild);
			if (SvXml::CTAG_GROUP == name)
			{
				// Read the Group data
				if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_STARTGROUP, htiSubChild, svValue))
				{
					std::string groupName = SvUl::createStdString(svValue);
					std::string endGroupName;
					std::string startGroupComment;
					std::string endGroupComment;
					if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_COLLAPSED, htiSubChild, svValue))
					{
						bool bCollapsed = (VARIANT_TRUE == svValue.boolVal);

						if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_STARTGROUP_COMMENT, htiSubChild, svValue))
						{
							startGroupComment = SvUl::createStdString(svValue);
							SvUl::RemoveEscapedSpecialCharacters(startGroupComment, true);
						}
						if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_ENDGROUP, htiSubChild, svValue))
						{
							endGroupName = SvUl::createStdString(svValue);
							if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_ENDGROUP_COMMENT, htiSubChild, svValue))
							{
								endGroupComment = SvUl::createStdString(svValue);
								SvUl::RemoveEscapedSpecialCharacters(endGroupComment, true);
							}
						}
						groupings.m_list.insert(groupings.m_list.end(), std::make_pair(groupName, ToolGroupData(ToolGroupData::StartOfGroup, groupName, endGroupName, bCollapsed)));
						// Update Comments
						if (!startGroupComment.empty())
						{
							groupings.SetComment(groupName, startGroupComment);
						}
						// Get The Tools...
						SVTreeType::SVBranchHandle htiTools(rTree.getFirstBranch(htiSubChild));
						if (nullptr != htiTools)
						{
							hr = LoadTools(rTree, htiTools, groupings);
						}
						if (S_OK == hr)
						{
							// End Group is optional
							if (!endGroupName.empty())
							{
								groupings.m_list.insert(groupings.m_list.end(), std::make_pair(endGroupName, ToolGroupData(ToolGroupData::EndOfGroup, groupName, endGroupName, false)));
								if (!endGroupComment.empty())
								{
									groupings.SetComment(endGroupName, endGroupComment);
								}
							}
						}
					}
					else
					{
						hr = SVMSG_SVO_49_LOAD_CONFIGURATION_TOOLGROUPING;
					}
				}
				else
				{
					hr = SVMSG_SVO_49_LOAD_CONFIGURATION_TOOLGROUPING;
				}
			}
			else if (SvXml::CTAG_TOOLS == name)
			{
				// Read The Tools
				hr = LoadTools(rTree, htiSubChild, groupings);
			}
			htiSubChild = rTree.getNextBranch(htiChild, htiSubChild);
		}
		if (S_OK == hr)
		{
			m_list.swap(groupings.m_list);
		}
	}
	return hr;
}

// Save the Groupings
/*
<NODE Name="SvXml::CTAG_TOOLGROUPINGS">
  <NODE Name="SvXml::CTAG_TOOLS">
	<DATA Name=SvXml::CTAG_TOOL Type="VT_BSTR">Dotted.Name</DATA>
	<DATA Name=SvXml::CTAG_TOOL Type="VT_BSTR">Dotted.Name</DATA>
  </NODE>
  <NODE Name="SvXml::CTAG_GROUP">
	<DATA Name=SvXml::CTAG_STARTGROUP Type="VT_BSTR">Group1</DATA>
	<DATA Name=SvXml::CTAG_COLLAPSED Type="VT_BOOL">TRUE</DATA>
	<DATA Name=SvXml::CTAG_STARTGROUP_COMMENT Type="VT_BSTR">Optional Comment</DATA>
	<NODE Name="SvXml::CTAG_TOOLS">
	  <DATA Name=SvXml::CTAG_TOOL Type="VT_BSTR">Dotted.Name</DATA>
	  <DATA Name=SvXml::CTAG_TOOL Type="VT_BSTR">Dotted.Name</DATA>
	</NODE>
	<DATA Name=SvXml::CTAG_ENDGROUP Type="VT_BSTR">EndGroup1</DATA>
	<DATA Name=SvXml::CTAG_ENDGROUP_COMMENT Type="VT_BSTR">Optional Comment</DATA>
  </NODE>
  <NODE Name="SvXml::CTAG_GROUP">
	<DATA Name=SvXml::CTAG_NAME Type="VT_BSTR">Group2</DATA>
	<DATA Name=SvXml::CTAG_COLLAPSED Type="VT_BOOL">TRUE</DATA>
	<NODE Name="SvXml::CTAG_TOOLS">
	  <DATA Name=SvXml::CTAG_TOOL Type="VT_BSTR">Dotted.Name</DATA>
	  <DATA Name=SvXml::CTAG_TOOL Type="VT_BSTR">Dotted.Name</DATA>
	</NODE>
  <NODE Name="SvXml::CTAG_GROUP">
	<DATA Name=SvXml::CTAG_STARTGROUP Type="VT_BSTR">Group3</DATA>
	<DATA Name=SvXml::CTAG_COLLAPSED Type="VT_BOOL">FALSE</DATA>
	<NODE Name="SvXml::CTAG_TOOLS">
	  <DATA Name=SvXml::CTAG_TOOL Type="VT_BSTR">Dotted.Name</DATA>
  </NODE>
</NODE>

*/
bool SVToolGrouping::GetParameters(SvOi::IObjectWriter& rWriter)
{
	bool bRetVal = true;
	if (m_list.size())
	{
		bool bGroupActive = false;
		bool bToolListActive = false;
		rWriter.StartElement(SvXml::CTAG_TOOLGROUPINGS);
		for (ToolGroupList::const_iterator it = m_list.begin(); it != m_list.end(); ++it)
		{
			if (ToolGroupData::StartOfGroup == it->second.m_type)
			{
				if (bToolListActive)
				{
					rWriter.EndElement();
					bToolListActive = false;
				}
				if (bGroupActive)
				{
					rWriter.EndElement();
				}
				rWriter.StartElement(SvXml::CTAG_GROUP);
				bGroupActive = true;

				_bstr_t name(it->first.c_str());
				_variant_t value(name);
				rWriter.WriteAttribute(SvXml::CTAG_STARTGROUP, value);

				std::string tmp(it->second.m_comment);
				SvUl::AddEscapeSpecialCharacters(tmp, true);
				_variant_t commentValue(tmp.c_str());
				rWriter.WriteAttribute(SvXml::CTAG_STARTGROUP_COMMENT, commentValue);

				_variant_t collapsedValue(it->second.m_bCollapsed);
				rWriter.WriteAttribute(SvXml::CTAG_COLLAPSED, collapsedValue);
			}
			else if (ToolGroupData::EndOfGroup == it->second.m_type)
			{
				if (bToolListActive)
				{
					rWriter.EndElement();
					bToolListActive = false;
				}

				_bstr_t name(it->second.m_endName.c_str());
				_variant_t value(name);
				rWriter.WriteAttribute(SvXml::CTAG_ENDGROUP, value);

				std::string tmp(it->second.m_comment);
				SvUl::AddEscapeSpecialCharacters(tmp, true);
				_variant_t commentValue(tmp.c_str());
				rWriter.WriteAttribute(SvXml::CTAG_ENDGROUP_COMMENT, commentValue);

				if (bGroupActive)
				{
					rWriter.EndElement();
					bGroupActive = false;
				}
			}
			else if (ToolGroupData::Tool == it->second.m_type)
			{
				if (!bToolListActive)
				{
					rWriter.StartElement(SvXml::CTAG_TOOLS);
					bToolListActive = true;
				}
				_bstr_t name(it->first.c_str());
				if (it->second.m_bCollapsed)
				{
					name = "+" + name;
				}
				_variant_t value(name);
				rWriter.WriteAttribute(SvXml::CTAG_TOOL, value);
			}
		}
		if (bToolListActive)
		{
			rWriter.EndElement();
		}
		if (bGroupActive)
		{
			rWriter.EndElement();
		}
		// End Element for Toolgroupings
		rWriter.EndElement();
	}
	return bRetVal;
}

SVToolGrouping::iterator SVToolGrouping::begin()
{
	return m_list.begin();
}

SVToolGrouping::iterator SVToolGrouping::end()
{
	return m_list.end();
}


SVToolGrouping::iterator SVToolGrouping::erase(SVToolGrouping::iterator it)
{
	return m_list.erase(it);
}

SVToolGrouping::iterator SVToolGrouping::find(const std::string& rName)
{
	SVToolGrouping::iterator it = std::find_if(begin(), end(),
		[&rName](const ToolGroup& rGroup)->bool { return rGroup.first == rName; });
	return it;
}

bool SVToolGrouping::empty() const
{
	return m_list.empty();
}

size_t SVToolGrouping::size() const
{
	return m_list.size();
}

std::string SVToolGrouping::GetComment(const std::string& rName) const
{
	std::string comment;
	if (IsEndTag(rName))
	{
		ToolGroupList::const_iterator it = std::find_if(m_list.begin(), m_list.end(),
			[&rName](const ToolGroup& rGroup)->bool { return (rName == rGroup.first && ToolGroupData::EndOfGroup == rGroup.second.m_type); });
		if (it != m_list.end())
		{
			comment = it->second.m_comment;
		}
	}
	else if (IsStartTag(rName))
	{
		// Remove Start and End
		ToolGroupList::const_iterator it = std::find_if(m_list.begin(), m_list.end(),
			[&rName](const ToolGroup& rGroup)->bool { return (rName == rGroup.first && ToolGroupData::StartOfGroup == rGroup.second.m_type); });
		if (it != m_list.end())
		{
			comment = it->second.m_comment;
		}
	}
	return comment;
}

void SVToolGrouping::SetComment(const std::string& rName, const std::string& rComment)
{
	if (IsEndTag(rName))
	{
		ToolGroupList::iterator it = std::find_if(m_list.begin(), m_list.end(),
			[&rName](const ToolGroup& rGroup)->bool { return (rName == rGroup.first && ToolGroupData::EndOfGroup == rGroup.second.m_type); });
		if (it != m_list.end())
		{
			it->second.m_comment = rComment;
		}
	}
	else if (IsStartTag(rName))
	{
		// Remove Start and End
		ToolGroupList::iterator it = std::find_if(m_list.begin(), m_list.end(),
			[&rName](const ToolGroup& rGroup)->bool { return (rName == rGroup.first && ToolGroupData::StartOfGroup == rGroup.second.m_type); });
		if (it != m_list.end())
		{
			it->second.m_comment = rComment;
		}
	}
}

int SVToolGrouping::GetNumberOfVisibleItems()
{
	int res = 0;
	bool isCollapsed = false;

	for (auto const &tgr : m_list)
	{
		switch (tgr.second.m_type)
		{
		case ToolGroupData::Tool:
			if (!isCollapsed)
			{
				res++;
			}
			break;
		case ToolGroupData::EndOfGroup:
			if (!isCollapsed)
			{
				res++;
			}
			isCollapsed = false;
			break;

		case ToolGroupData::StartOfGroup:
			res++;
			isCollapsed = tgr.second.m_bCollapsed;
			break;
		}
	}

	return res;
}




