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
#include "SVObjectLibrary/SVObjectWriter.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVOCore/ResultViewItemDef.h"
#include "TableObject.h"
#pragma endregion Includes

#pragma region Declarations
struct SVIPResultData;
class SVIPResultItemDefinition;
class SVInspectionProcess;
#pragma endregion Declarations

/**
	Class contains a list of GUIDS of and references to variables which are selected to be shown in the result view.
**/
class ResultViewReferences
{
public:
	typedef std::deque< SVIPResultItemDefinition > SVResultDefinitionDeque;
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
	bool Save(SVObjectWriter& rWriter);

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
	void GetResultData( SVIPResultData& rResultData) const; 

	/// Fill the table data to the result data.
	/// \param p_rResultData [in,out]
	void GetResultTableData(SVIPResultData &p_rResultData) const;	

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
	// Returns:  SvTl::SVTimeStamp:  the value of the member variable m_LastUpdateTimeStamp
	//************************************
	SvTl::SVTimeStamp getUpdateTimeStamp() { return m_LastUpdateTimeStamp;  };

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
	SVGUID m_resultTableGuid; //The Guid of the table to display in result table view (if no table should displayed it is set to SV_GUID_NULL)
	TableObject* m_resultTable;
	SVObjectReferenceVector m_ReferenceVector;
	std::list<ResultViewItemDef> m_ResultViewItemDefList;
	SvTl::SVTimeStamp m_LastUpdateTimeStamp; // Most recent time when m_ReferenceVector changed.
#pragma endregion Member Variables
};

