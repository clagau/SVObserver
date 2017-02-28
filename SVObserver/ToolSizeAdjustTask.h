//******************************************************************************
/// COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
///  declare  ToolSizeAdjustTask class
//******************************************************************************
#pragma once
 
#pragma region Includes
#include "SVOCore/SVTaskObject.h"
#include "SVTool.h"
#pragma endregion Includes

//! \brief Set Tool position and Size of a tool during resetObject
//! Set the tool position and size to full Image or in accordance with a formula
//! class is in friendlist  of corresponding tool 
//! the friendlist of this class holds the Svevaluationobjects 
class ToolSizeAdjustTask : public SVTaskObjectClass
{
	SV_DECLARE_CLASS( ToolSizeAdjustTask );
public:
    ToolSizeAdjustTask( bool AllowFullsize = true, bool AllowAdjustSize = true, bool AllowAdjustPosition = true, SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_TOOLSIZEADJUSTTASK );
	virtual ~ToolSizeAdjustTask();
	enum TSModes {TSNone =0,TSFullSize, TSFormula, TSModeCount }; 
	enum TSValues {TSPositionX=0, TSPositionY, TSWidth, TSHeight, TSValuesCount};

public:
	//************************************
	//! Add a ToolSizeAdjustTask object to the friendlist of pTool 
	//! \param pTool [in]
	//! \returns HRESULT S_OK if successfully
	//************************************
	static ToolSizeAdjustTask* AddToFriendlist(SVToolClass *pTool, bool AllowFullsize = true, bool AllowAdjustSize = true, bool AllowAdjustPosition = true);

	//************************************
	//! Insert a a ToolSizeAdjustTask object to the friendlist of pTool if it is not there
	//! Correct  the extend Property of pTool 
	//! \param pTool [in]
	//! \returns HRESULT S_OK if successfully
	//************************************
	static HRESULT EnsureInFriendList(SVToolClass *pTool, bool AllowFullsize = true, bool AllowAdjustSize = true, bool AllowAdjustPosition = true);
	
	//************************************
	//! Search and return the first ToolSizeAdjustTask”bject in friendlist of pObject
	//! \param pObject [in]
	//! \returns ToolSizeAdjustTask*
	//************************************
	static ToolSizeAdjustTask* GetToolSizeAdjustTask(SVObjectClass *pObject);
	
	//************************************
	//! returns True if pObject has a ToolSizeAdjustTaskObject in the friendlist, which formula or AutoSize  
	//! \param pObject [in]
	//! \returns bool
	//************************************
	static BOOL UseSizeAdjust( SVObjectClass *pObject);

	//************************************
	//! returns True if a formula or Autosize is used 
	//! \returns BOOL
	//************************************
	BOOL UseAutoMode() const;
	
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	//! DoNothing in onRun
	virtual bool onRun(SVRunStatusClass& RRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr) override;
	
	//************************************
	//! Get the result of the formula 
	//! \param val [in] TSPositionX etc,
	//! \param value [out]
	//! \returns HRESULT S_OK if successfully 
	//************************************
	HRESULT GetResultValue( TSValues val, long &value) const;
	
	//************************************
	//! Get the extend Values of the tool
	//! \param val [in] TSPositionX etc,
	//! \param value [out]
	//! \returns HRESULT
	//************************************
	HRESULT GetExtentValues( TSValues val, long &value) const;
	
	//************************************
	//! Get Width and height of the parent 
	//! \param width [out]
	//! \param height [out]
	//! \returns HRESULT
	//************************************
	HRESULT GetParentExtentOutputValues( TSValues val, long &value) const;
	
	//************************************
	//! Get the ValueObjects with the results of the Equations 
	//! \param val [in]  TSPositionX etc,
	//! \returns SVDoubleValueObjectClass* or nullptr 
	//************************************
	SVDoubleValueObjectClass* GetDResultObjects(TSValues val) const;
	
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

	bool IsFullSizeAllowed() const;
	bool IsAdjustSizeAllowed() const;
	bool IsAdjustPositionAllowed() const;

	SVEnumerateValueObjectClass* GetInputMode(TSValues mode);

protected:
	//************************************
	//! Result data Index of inspection
	//! \returns int
	//************************************
	int GetIndex();
	
	//************************************
	//! Add SVEquationObject to the Friendlist 
	//! \param pInfo [in,out] inObjectinfostruct 
	//! \param GuidClass [in] Guid of the SVEquationObject  
	//! \param Name [in] InputObjectname
	//! \returns BOOL TRUE if sucessfull 
	//************************************
	BOOL AddEvaluationObject(SVInObjectInfoStruct* pInfo, GUID const &GuidClass, LPCTSTR Name);
	
	//************************************
	//! DoNothing in Run
	//! \param RRunStatus [in,out]
	//! \returns TRUE
	//************************************
	virtual bool Run( SVRunStatusClass& RRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;
	
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
	SVToolClass* GetTool() const; 
	
public:
	const static long MaxToolSize; //< Maximum allowed WindowSize 
	const static long MinToolSize; //< Maximum allowed WindowSize 

protected:
	SVEnumerateValueObjectClass m_InputModes[TSValuesCount]; //< the four input modes None=0,Full Size,Formula
	SVInObjectInfoStruct m_InObjectInfoDResult[TSValuesCount]; //<The four inputs from the Equations 
	
	bool m_AllowFullSize;
	bool m_AllowAdjustSize;
	bool m_AllowAdjustPosition;
};
 
