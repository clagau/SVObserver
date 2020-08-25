//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file TableExcludeAnalyzer.h
/// All Rights Reserved 
//*****************************************************************************
/// Class coordinated the analyzer for excluding rows of the table.
//******************************************************************************
#pragma once

#pragma region Includes
#include "InspectionEngine/SVTaskObject.h"
#include "SVValueObjectLibrary\LinkedValue.h"
#pragma endregion Includes

namespace SvAo
{

class TableExcludeAnalyzer : public SvIe::SVTaskObjectClass
{
#pragma region Constructor
	SV_DECLARE_CLASS( TableExcludeAnalyzer );
public:
	explicit TableExcludeAnalyzer( LPCTSTR ObjectName );
	TableExcludeAnalyzer( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_TABLEANALYZER_EXCLUDE );

	virtual ~TableExcludeAnalyzer();
#pragma endregion Constructor

#pragma region Public Methods
public:
	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	virtual bool onRun( RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	void Initialize();

	/// Build the embedded object list for this tool.
	void	BuildEmbeddedObjectList();

	void BuildInputObjectList ();
#pragma endregion Private Methods

#pragma region Member Variables
protected:
	SvOl::SVInObjectInfoStruct m_excludeColumnObjectInfo;
	SvVol::LinkedValue m_excludeHigh;
	SvVol::LinkedValue m_excludeLow;
#pragma endregion Member Variables
};

} //namespace SvAo
