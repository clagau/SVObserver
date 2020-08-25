//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRange
//* .File Name       : $Workfile:   SVRange.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.7  $
//* .Check In Date   : $Date:   17 Feb 2015 18:14:06  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "Definitions/RangeEnum.h"
#include "InspectionEngine/SVTaskObject.h"
#include "ObjectInterfaces/ITool.h"
#include "SVLibrary/ISVCancel.h"
#include "SVValueObjectLibrary/LinkedValue.h"
#pragma endregion Includes

namespace SvOp
{

enum Range_defaults
{
	lowDef = 0, // Warn/Fail Low Default
	highDef = 9999999 // Warn/Fail High Default
};
static const VARTYPE cVarType_Value = VT_R8;

////////////////////////////////////////////////////////////////////////////////
// .Title       : Class SVRange
// -----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
class SVRange : public SvIe::SVTaskObjectClass, public ISVCancel
{
	SV_DECLARE_CLASS( SVRange );

public:
	SVRange( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVRANGE );
	virtual ~SVRange();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	// ISVCancel interface
	virtual HRESULT GetCancelData(SVCancelData*& rpData) override;
	virtual HRESULT SetCancelData(SVCancelData* pData) override;

	virtual HRESULT SetValuesForAnObject(uint32_t aimObjectID, SVObjectAttributeClass* pDataObject) override;

	void setHighValues(double failHigh, double warnHigh);
	void setLowValues(double failLow, double warnLow);
	void setDefaultLowValues(double failLow, double warnLow);

	//************************************
	// Description:  retrieve the direct value string for Erange
	// Parameter: enum ERange
	// Parameter: double rValue reference to the value 
	// Returns:  HRESULT:  S_OK if successful
	//************************************
	HRESULT getValue(RangeEnum::ERange, double& rValue);
	
	/// Add the Range-Values to the ParameterList for the needed in monitor list.
	/// \param retList [in,out] The ParameterList
	void addEntriesToMonitorList(std::back_insert_iterator<SvOi::ParametersForML> inserter) const;

#pragma region Methods to replace processMessage
#pragma endregion Methods to replace processMessage
	
protected:
	void init();
	bool getInputValue( double& RVal );
	virtual bool onRun(RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	bool checkLinkedValues(RangeEnum::ERange firstType, RangeEnum::ERange secondType, SvStl::MessageContainerVector * pErrorMessages);

protected:
	
	SvVol::LinkedValue m_LinkedValues[RangeEnum::ER_COUNT];
	SvOl::SVInObjectInfoStruct m_inputObjectInfo;
};

} //namespace SvOp
