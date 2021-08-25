//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file TableLimitAnalyzer.h
/// All Rights Reserved 
//*****************************************************************************
/// Class coordinated the analyzer for limiting the rows of the table.
//******************************************************************************
#pragma once

#pragma region Includes
#include "InspectionEngine/SVTaskObject.h"
#include "SVValueObjectLibrary\LinkedValue.h"
#pragma endregion Includes

namespace SvAo
{

class TableLimitAnalyzer : public SvIe::SVTaskObjectClass
{
#pragma region Constructor
	SV_DECLARE_CLASS
public:
	explicit TableLimitAnalyzer( LPCTSTR ObjectName );
	TableLimitAnalyzer( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_TABLEANALYZER_LIMIT );

	virtual ~TableLimitAnalyzer();
#pragma endregion Constructor

#pragma region Public Methods
public:
	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	virtual bool onRun( SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	void Initialize();

	/// Build the embedded object list for this task.
	void	BuildEmbeddedObjectList();
#pragma endregion Private Methods

#pragma region Member Variables
protected:
	SvVol::LinkedValue m_LimitValue;
#pragma endregion Member Variables
};

} //namespace SvAo
