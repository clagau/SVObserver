//******************************************************************************
/// COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
///  declare  ToolSizeAdjustTask class
//******************************************************************************
#pragma once
 
#pragma region Includes
#include "Definitions/SVExtentEnums.h"
#include "InspectionEngine/SVTaskObject.h"
#include "ObjectInterfaces/IToolSizeAdjustTask.h"
#include "SVValueObjectLibrary\SVEnumerateValueObjectClass.h"
#pragma endregion Includes

namespace SvTo
{
class SVToolClass;
}

namespace SvVol
{
class SVDoubleValueObjectClass;
}

namespace SvOp
{

//! \brief Set Tool position and Size of a tool during resetObject
//! Set the tool position and size to full Image or in accordance with a formula
//! class is in friendlist  of corresponding tool 
//! the friendlist of this class holds the Svevaluationobjects 
class ToolSizeAdjustTask : public SvIe::SVTaskObjectClass, public SvOi::IToolSizeAdjustTask
{
	SV_DECLARE_CLASS( ToolSizeAdjustTask );
public:
    ToolSizeAdjustTask( bool AllowFullsize = true, bool AllowAdjustSize = true, bool AllowAdjustPosition = true, SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_TOOLSIZEADJUSTTASK );
	virtual ~ToolSizeAdjustTask();

public:
	//************************************
	//! Add a ToolSizeAdjustTask object to the friendlist of pTool 
	//! \param pTool [in]
	//! \returns HRESULT S_OK if successfully
	//************************************
	static ToolSizeAdjustTask* AddToFriendlist(SvTo::SVToolClass *pTool, bool AllowFullsize = true, bool AllowAdjustSize = true, bool AllowAdjustPosition = true);

	//************************************
	//! Insert a a ToolSizeAdjustTask object to the friendlist of pTool if it is not there
	//! Correct  the extend Property of pTool 
	//! \param pTool [in]
	//! \returns HRESULT S_OK if successfully
	//************************************
	static HRESULT EnsureInFriendList(SvTo::SVToolClass *pTool, bool AllowFullsize = true, bool AllowAdjustSize = true, bool AllowAdjustPosition = true);
	
	//************************************
	//! Search and return the first ToolSizeAdjustTask”bject in friendlist of pObject
	//! \param pObject [in]
	//! \returns ToolSizeAdjustTask*
	//************************************
	static ToolSizeAdjustTask* GetToolSizeAdjustTask(const SVObjectClass *pObject);
	
	//************************************
	//! returns True if pObject has a ToolSizeAdjustTaskObject in the friendlist, which formula or AutoSize  
	//! \param pObject [in]
	//! \returns bool
	//************************************
	static bool UseSizeAdjust(const SVObjectClass *pObject);

	//************************************
	//! returns True if a formula or Autosize is used 
	//! \returns bool
	//************************************
	bool UseAutoMode() const;
	
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	//! DoNothing in onRun
	virtual bool onRun(SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr) override;
	
	//************************************
	//! Get the result of the formula 
	//! \param val [in] TSPositionX etc,
	//! \param value [out]
	//! \returns HRESULT S_OK if successfully 
	//************************************
	HRESULT GetResultValue(SvDef::ToolSizeAdjustEnum val, long &value) const;
	
	//************************************
	//! Get the ValueObjects with the results of the Equations 
	//! \param val [in]  TSPositionX etc,
	//! \returns SVDoubleValueObjectClass* or nullptr 
	//************************************
	SvVol::SVDoubleValueObjectClass* GetDResultObjects(SvDef::ToolSizeAdjustEnum val) const;
	
	//************************************
	// Set the extend property of the tool to bSetByReset if ruled by formula 
	//! \returns HRESULT  S_OK if successfully 
	//************************************
	HRESULT SetExtendPropertyAutoReset();

	//************************************
	//! return white color
	//! \returns DWORD
	//************************************
	virtual DWORD GetObjectColor() const override; 

	virtual bool IsFullSizeAllowed() const override;
	virtual bool IsAdjustSizeAllowed() const override;
	virtual bool IsAdjustPositionAllowed() const override;
	virtual SvOi::ITool* getTool() const override;

protected:
	//************************************
	//! Add SVEquationObject to the Friendlist 
	//! \param pInfo [in,out] inObjectinfostruct 
	//! \param classId [in] ClassId of the SVEquationObject  
	//! \param Name [in] InputObjectname
	//! \returns bool true if sucessfull 
	//************************************
	bool AddEvaluationObject(SvOl::SVInObjectInfoStruct* pInfo, SvPb::ClassIdEnum classId, LPCTSTR Name);
	
	//************************************
	//! DoNothing in Run
	//! \param rRunStatus [in,out]
	//! \returns TRUE
	//************************************
	virtual bool Run( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;
	
	//************************************
	//! retrieves the inputmodes. Check for consistency 
	//! \param rModeWidth [out]
	//! \param rModeHeight [out]
	//! \param rModePosX [out]
	//! \param rModePosY [out]
	//! \returns S_OK or errornumber 
	//************************************
	HRESULT GetModes(long &rModeWidth, long &rModeHeight, long &rModePosX, long &rModePosY);

	//************************************
	//! Get the Owner. The Owner is the tool which we want to resize  
	//! \returns SVToolClass*
	//************************************
	SvTo::SVToolClass* GetTool() const;
	
public:
	const static long MaxToolSize; //< Maximum allowed WindowSize 
	const static long MinToolSize; //< Maximum allowed WindowSize 

protected:
	SvVol::SVEnumerateValueObjectClass m_InputModes[static_cast<int>(SvDef::ToolSizeAdjustEnum::TSValuesCount)]; //< the four input modes None=0,Full Size,Formula
	SvOl::SVInObjectInfoStruct m_InObjectInfoDResult[static_cast<int>(SvDef::ToolSizeAdjustEnum::TSValuesCount)]; //<The four inputs from the Equations 
	
	bool m_AllowFullSize;
	bool m_AllowAdjustSize;
	bool m_AllowAdjustPosition;
};
 
} //namespace SvOp
