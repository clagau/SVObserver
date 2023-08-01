//*****************************************************************************
/// \copyright (c) 2020,2020 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file ModuleTool.h
/// All Rights Reserved 
//*****************************************************************************
/// Implements the ModuleTool
//******************************************************************************
#pragma once

#pragma region Includes
#include "GroupTool.h"
#include "SVUtilityLibrary/SVGUID.h"
#pragma endregion Includes

namespace SvTo
{

/// Implements the GroupTool
class ModuleTool : public GroupTool
{
	SV_DECLARE_CLASS
public:
	ModuleTool(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_MODULETOOL);
	virtual ~ModuleTool();

	virtual bool CreateObject(const SVObjectLevelCreateStruct& rCreateStructure) override;
	virtual bool ResetObject(SvStl::MessageContainerVector* pErrorMessages = nullptr) override;

	virtual std::vector<std::string> getToolAdjustNameList() const override;
	virtual void Persist(SvOi::IObjectWriter& rWriter, bool closeObject = true) const override;
	virtual HRESULT SetObjectValue(SVObjectAttributeClass* PDataObject) override;

	time_t getLastModify() const { return m_historyList.empty() ? 0 : m_historyList[m_historyList.size()-1].first; };
	SVGUID getModuleGuid() const { return m_moduleGuid; };
	SVGUID renewModuleGuid();
	std::string getModuleComment() const;
	void setModuleComment(const std::string& rText);
	void setHistory(const std::vector<std::pair<time_t, std::string>>& rHistoryVector) { m_historyList = rHistoryVector; };
	std::vector<std::pair<time_t, std::string>> getHistory() const { return m_historyList; };
	void setEditFlag(bool editFlag) { m_isEditing = editFlag; setViewFlag(); };

	virtual void getOverlays(const SvIe::SVImageClass&, SVExtentMultiLineStructVector&) const override { }; //Don't add overlays for ModuleTool by now.
	virtual void getOverlays(const SvIe::SVImageClass&, SvPb::OverlayDesc&) const override {}; //Don't add overlays for ModuleTool by now.
	virtual bool canHasChildren() const override { return m_isEditing; };

private:
	void Initialize();
	void setViewFlag();

private:
	SvVol::SVStringValueObjectClass m_moduleComment;
	std::vector<std::pair<time_t, std::string>> m_historyList; //first: time, second: comment
	SVGUID m_moduleGuid;
	bool m_isEditing = false;
};

} //namespace SvTo
