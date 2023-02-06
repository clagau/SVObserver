//*****************************************************************************
/// \copyright (c) 2020,2020 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file GroupTool.h
/// All Rights Reserved 
//*****************************************************************************
/// Implements the GroupTool
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVTool.h"
#pragma endregion Includes

namespace SvOp
{
	class ParameterTask;
	class ResultParameterTask;
}

namespace SvTo
{

/// Implements the GroupTool
class GroupTool : public SVToolClass
{
	SV_DECLARE_CLASS
public:
	GroupTool(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_GROUPTOOL);
	virtual ~GroupTool();
	virtual bool CreateObject(const SVObjectLevelCreateStruct& rCreateStructure) override;

	virtual long setToolPosition(long ToolPosition) override;
	virtual int InsertBefore(uint32_t objectBeforeID, SVTaskObjectClass* pTaskObject) override;
	virtual bool propagateSizeAndPosition() override;
	virtual bool usePropagateSizeAndPosition() const override;
	virtual void addOverlays(const SvIe::SVImageClass* p_Image, SvPb::OverlayDesc& rOverlay) const override;
	virtual bool ResetObject(SvStl::MessageContainerVector* pErrorMessages = nullptr) override;
	virtual void resetCounters() override;
	virtual bool Run(SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector* pErrorMessages = nullptr) override;

	virtual bool areAuxExtentsAvailable() const override { return false; };
	virtual SvPb::InspectionCmdResponse getInvalidDependencies() const override;
	virtual void fillSelectorList(std::back_insert_iterator<std::vector<SvPb::TreeItem>> treeInserter, SvOi::IsObjectAllowedFunc pFunctor, UINT attribute, bool wholeArray, SvPb::SVObjectTypeEnum nameToType, SvPb::ObjectSelectorType requiredType, bool stopIfClosed = false, bool firstObject = false) const override;
	virtual void fillObjectList(std::back_insert_iterator<std::vector<SvOi::IObjectClass*>> inserter, const SvDef::SVObjectTypeInfoStruct& rObjectInfo, bool addHidden = false, bool stopIfClosed = false, bool firstObject = false) override;

	virtual uint32_t getFirstClosedParent(uint32_t stopSearchAtObjectId) const override;
	virtual bool isValidDependency(const std::pair<std::string, std::string>& rEntry) const override;
	virtual std::vector<std::string> getToolAdjustNameList() const override;
	virtual void OnObjectRenamed(const SVObjectClass& rRenamedObject, const std::string& rOldName) override;
	virtual int getToolDepth(bool goUpwards = true) const override;
	bool isClosed() const;

	///  Set all Inputs to the Default Value. For some Type (e.g. Image) Inputs become invalid because the default Value is empty.
	void setInputsToDefault();

	/// Moved FriendList from this tool to the parameter and clear it in this tool.
	/// \param friendList [out]
	void movedAndDeleteFriends(SVThreadSafeList<SVTaskObjectClass*>& rFriendList);
	void movedAndDeleteTaskObjects(SvIe::SVTaskObjectPtrVector& taskList);
	///  Move the embeddedObjects to the attached list: switch objectId, copy value and move connection.
	/// \param rEmbeddedObject [inout] List of embeddedObjects to move to.
	void moveEmbeddedObjects(SVObjectPtrVector& rEmbeddedObjects);
	void moveObjectToThis(GroupTool& rGroupTool);

protected:
	virtual bool useOverlayColorTool() const override { return false; };

private:
	void Initialize();
	SvDef::StringPairVector getInvalidDependenciesList() const;
	bool isValid(const std::pair<std::string, std::string>& rEntry) const;
	void updateValidCheckStrings();

private:
	SvVol::SVBoolValueObjectClass m_isClosed;
	SvOp::ParameterTask* m_pInputTask = nullptr;
	SvOp::ResultParameterTask* m_pResultTask = nullptr;

	std::string m_groupStr;
	std::string m_inputStr;
	std::string m_resultStr;
	std::vector<std::string> m_embeddedNameList;
};

} //namespace SvTo
