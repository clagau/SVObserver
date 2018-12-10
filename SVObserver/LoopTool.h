#pragma once
//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file LoopTool.h
/// All Rights Reserved 
//*****************************************************************************
/// Implements the LoopTool
//******************************************************************************
#pragma once
#include "InspectionEngine\SVTool.h"
#include "SVValueObjectLibrary\LinkedValue.h"
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
	virtual int InsertAfter(const SVGUID& rGuid, SVTaskObjectClass* pTaskObject) override;
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
	SVLongValueObjectClass m_MaxLoops; //!  Maximum for the Number of Loops
	LinkedValue	m_LinkedLoops; //!  Linked value for the number of Loops
	LinkedValue m_LinkedBreak; //! Linked value break the loop
	SVLongValueObjectClass m_LoopCounter; //! Loop counter
};

