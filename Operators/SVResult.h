//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVResult.h
//* .File Name       : $Workfile:   SVResult.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   14 Jan 2015 16:43:46  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVLibrary/ISVCancel.h"
#include "InspectionEngine/SVTaskObjectList.h"
#include "SVValueObjectLibrary/SVBoolValueObjectClass.h"
#pragma endregion Includes

namespace SvOp
{
class SVRange;

enum
{
	SV_DEFAULT_MAX_BLOB_NUMBER			= 100,
	SV_DEFAULT_TEXT_HEIGHT				= 20,
	SV_DEFAULT_NUMBER_OF_INTENSITIES	= 256,
	SV_NUMBER_OF_MATCH_STRINGS			= 1,
};

////////////////////////////////////////////////////////////////////////////////
// .Title       : Base Class of module SVResult 
////////////////////////////////////////////////////////////////////////////////

class SVResult : public SvIe::SVTaskObjectListClass, public ISVCancel
{
	///This class does not need to call SV_DECLARE_CLASS as it is a base class and only derived classes are instantiated
	//SV_DECLARE_CLASS( SVResult );

public:
	SVResult ( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVRESULT );

	virtual ~SVResult();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	// ISVCancel interface
	virtual HRESULT GetCancelData(SVCancelData*& rpData) override;
	virtual HRESULT SetCancelData(SVCancelData* pData) override;

	bool IsFailed();
	bool IsWarned();
	bool IsGood();

	// derived class overrides...

	SVRange* GetResultRange();

	virtual bool Run(RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr) override;
	
	
	// Only valid for single input Results that can use the m_inputObjectInfo 
	// shortcut.
	const SVObjectClass* getInput() const;

protected:
	virtual bool onRun( RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	void init();
	
private:
	bool ValidateLocal(SvStl::MessageContainerVector *pErrorMessages=nullptr) const;

protected:
	// Input: Pointer to the Input Object
	// This gives an easy shortcut to the input object.  Really only useful 
	// with objects that KNOW they only have a single input object (otherwise walk 
	// the input object list).  Objects that can use this are Double and Long.
	SvOl::SVInObjectInfoStruct m_inputObjectInfo;

	// Passed, if TRUE ( Reset Value: FALSE )
	SvVol::SVBoolValueObjectClass	m_Passed;
	// Warned, if TRUE ( Reset Value: TRUE )
	SvVol::SVBoolValueObjectClass	m_Warned;
	// Failed, if TRUE ( Reset Value: TRUE )
	SvVol::SVBoolValueObjectClass	m_Failed;
};

} //namespace SvOp
