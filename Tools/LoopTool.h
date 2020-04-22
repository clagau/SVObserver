#pragma once
//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file LoopTool.h
/// All Rights Reserved 
//*****************************************************************************
/// Implements the LoopTool
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVTool.h"
#include "SVValueObjectLibrary\LinkedValue.h"
#pragma endregion Includes

namespace SvTo
{

/// Implements the LoopTool
class LoopTool :public SVToolClass
{
	SV_DECLARE_CLASS(SVToolClass);
public:
	LoopTool(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_LOOPTOOL);
	virtual ~LoopTool();
	bool AddTool(SVToolClass *pTool);
	bool RemoveTool(SVToolClass *pTool);
	virtual bool CreateObject(const SVObjectLevelCreateStruct& rCreateStructure) override;
	virtual long  setToolPosition(long ToolPosition) override;
	virtual bool Run(SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages) override;
	virtual int InsertBefore(uint32_t objectBeforeID, SVTaskObjectClass* pTaskObject) override;
	virtual bool propagateSizeAndPosition() override;
	virtual bool usePropagateSizeAndPosition() const override;

private:
	void BuildEmbeddedObjectList();
	void Initialize();
	/// Similar to SVTaskObjectListClass::Run but avoid to propagate ChildRunStatus.IsDisabled()
	bool RunAllChildren(SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages);
private:
	static const int DefaultMaxLoops = 10;
	static const int DefaultLinkedLoops = 1;
	static const int DefaultLinkedBreak = 0;

	//embedded parameter
	SvVol::SVLongValueObjectClass m_MaxLoops; //!  Maximum for the Number of Loops
	SvVol::LinkedValue	m_LinkedLoops; //!  Linked value for the number of Loops
	SvVol::LinkedValue m_LinkedBreak; //! Linked value break the loop
	SvVol::SVLongValueObjectClass m_LoopCounter; //! Loop counter
};

} //namespace SvTo
