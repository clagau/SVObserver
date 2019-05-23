//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ResultViewReferences
//* .File Name       : $Workfile:   ResultViewReferences.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   07 Jan 2015 17:35:06  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <list>
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#include "InspectionEngine/ResultViewItemDef.h"
#include "InspectionEngine/SVIPResultItemDefinition.h"
#include "InspectionEngine/SVIPResultData.h"
#include "SVObjectLibrary/SVObjectReference.h"
#include "TriggerRecordController/ITriggerRecordR.h"
#pragma endregion Includes

namespace SvOi
{
class IObjectWriter;
}
namespace SvOp
{
class TableObject;
}
namespace SvIe
{
struct SVIPResultData;
}
class SVInspectionProcess;

/**
	Class contains a list of GUIDS of and references to variables which are selected to be shown in the result view.
**/
class ResultViewReferences
{
public:
	typedef std::deque<SvIe::SVIPResultItemDefinition> SVResultDefinitionDeque;
	typedef SvXml::SVXMLMaterialsTree SVTreeType;

#pragma region Constructor
public:
	//************************************
	//! constructor for the ResultViewReferences class.
	//! \param tagname [in] default tagname for entrie in cfg file 
	//************************************
	ResultViewReferences(LPCTSTR tagname);

	//************************************
	// Description:  Destructor for the ResultViewReferences class.
	//************************************
	~ResultViewReferences();
#pragma endregion Constructor

#pragma region Public Methods
	//************************************
	// Description:  Save the item in the m_ReferenceVector as complete name to the config file
	// Parameter:  rWriter:  
	// Returns:  bool:  
	//************************************
	bool Save(SvOi::IObjectWriter& rWriter);

	//************************************
	// Description:  Load GUIDs and references from the configuration file.
	// Parameter:  rTree:  
	// Parameter:  hParent:  
	// Parameter:  tagname tagname in cfgfile 
	// Returns:  bool: return true if the tagnane was found in the cfg file   
	//************************************
	bool Load( SVTreeType& rTree, SVTreeType::SVBranchHandle hParent, LPCTSTR tagname = nullptr );

	
	//************************************
	//! Description:  Rebuild the reference vector from the list of GUIDs.
	//! \param pInspection [in] if not null the References will be checked if the are  a Disabled PPQVariables
	//! \returns void
	//************************************
	void RebuildReferenceVector(SVInspectionProcess* pInspection = nullptr);

	//************************************
	// Description:  Build SVResultDefinitionDeque from the reference vector.  (Defines the list of viewed variables.)
	// Parameter:  rDefinitions <out>:  object containing data from the class's variables.
	// Returns:  S_OK if no error occurs   
	//************************************
	HRESULT GetResultDefinitions( SVResultDefinitionDeque& rDefinitions ) const; 

	const SVGUID& getTableGuid() const { return m_resultTableGuid; }
	void setTableGuid(const SVGUID& guid) { m_resultTableGuid = guid; }

	//************************************
	// Description:  Build SVIPResultData from the reference vector.  (Get the values of the variables.)
	// Parameter:  rResultData <out>:  object containing data from the class's variables.
	// Parameter:  getColor <in>:  true (default) if the color for the item in the result view should be fetched.
	//************************************
	void GetResultData(SvIe::SVIPResultData& rResultData) const; 

	std::vector <SvIe::IPResultTableData> getResultTableData(SvTrc::ITriggerRecordRPtr pTriggerRecord);

	//************************************
	// Description:  Clears the lists.
	//************************************
	void Clear();

	//************************************
	// Description: Gets the selected result view objects
	// Returns:  int:  number of entries in the set
	//************************************
	const SVObjectReferenceVector& GetSelectedObjects() const;

	//************************************
	// Description:  Insert a new entry
	// Parameter:  reference to DottedName of new object
	// Returns:  true if successful
	//************************************
	bool Insert( const std::string& rDottedName );

	//************************************
	// Description:  Gets the value of the member variable m_LastUpdateTimeStamp
	// Returns:  double:  the value of the member variable m_LastUpdateTimeStamp
	//************************************
	double getUpdateTimeStamp() { return m_LastUpdateTimeStamp;  };

	//************************************
	/// Check if the Object is Viewable
	/// this is only called when loading as the original ObjectAttributesAllowed may have changed
	/// \param reference to the SVObjectRefernce
	/// \return boolean true=viewable, false=not viewable
	//************************************
	bool IsViewable(const SVObjectReference& objectRef) const;

	//************************************
	//! To read deprecated Configuration gets all variable from tooloutputlist with  with attribute IsView
	//! \param pInspection [in]
	//! \returns void
	//************************************
	void InsertFromOutputList(SVInspectionProcess* pInspection);
	
	
	//************************************
	//!To read deprecated Configuration gets all variable from m_PPQInputs with attribute IsView
	//! \param pInspection [in]
	//! \returns void
	//************************************
	void InsertFromPPQInputs(SVInspectionProcess* pInspection);

#pragma endregion Public Methods

protected:
#pragma region Protected Methods
	//************************************
	// Description:  Helper function for Load.  Loads a single item.
	// Parameter:  rTree:  
	// Parameter:  hLeaf:  
	// Returns:  bool:  
	//************************************
	bool LoadResultViewItemDef( SVTreeType& rTree, SVTreeType::SVLeafHandle hLeaf );
#pragma endregion Protected Methods

#pragma region Member Variables
	std::string m_TagName; // Branch name in the config file
	SVGUID m_resultTableGuid; //The Guid of the table to display in result table view (if no table should displayed it is set to GUID_NULL)
	SvOp::TableObject* m_resultTable;
	SVObjectReferenceVector m_ReferenceVector;
	std::list<SvIe::ResultViewItemDef> m_ResultViewItemDefList;
	double m_LastUpdateTimeStamp; // Most recent time when m_ReferenceVector changed.
#pragma endregion Member Variables
};

