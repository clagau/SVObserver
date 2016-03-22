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
#include "ResultViewItemDef.h"
#pragma endregion Includes

#pragma region Declarations
struct SVIPResultData;
class SVIPResultItemDefinition;
class SVResultsWrapperClass;
class SVInspectionProcess;
class SVValueObjectClass;
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
	virtual ~ResultViewReferences();
#pragma endregion Constructor

#pragma region Public Methods
	//************************************
	// Description:  Save the item in the m_ReferenceVector as complete name to the config file
	// Parameter:  rWriter:  
	// Returns:  bool:  
	//************************************
	virtual bool Save(SVObjectWriter& rWriter);

	//************************************
	// Description:  Load GUIDs and references from the configuration file.
	// Parameter:  rTree:  
	// Parameter:  hParent:  
	// Parameter:  tagname tagname in cfgfile 
	// Returns:  bool: return true if the tagnane was found in the cfg file   
	//************************************
	virtual bool Load( SVTreeType& rTree, SVTreeType::SVBranchHandle hParent, LPCTSTR tagname = nullptr );

	
	//************************************
	//! Description:  Rebuild the reference vector from the list of GUIDs.
	//! \param pInspection [in] if not null the References will be checked if the are  a Disabled PPQVariables
	//! \returns void
	//************************************
	virtual void RebuildReferenceVector(SVInspectionProcess* pInspection = nullptr);

	//************************************
	// Description:  Build SVResultDefinitionDeque from the reference vector.  (Defines the list of viewed variables.)
	// Parameter:  rDefinitions <out>:  object containing data from the class's variables.
	// Returns:  S_OK if no error occurs   
	//************************************
	virtual HRESULT GetResultDefinitions( SVResultDefinitionDeque& rDefinitions ) const; 

	//************************************
	// Description:  Build SVIPResultData from the reference vector.  (Get the values of the variables.)
	// Parameter:  rResultData <out>:  object containing data from the class's variables.
	// Parameter:  getColor <in>:  true (default) if the color for the item in the result view should be fetched.
	// Returns: S_OK if no error occurs   
	//************************************
	virtual HRESULT GetResultData( SVIPResultData& rResultData) const; 

	//************************************
	// Description:  Build SVResultsWrapperClass from reference vector.  (Get values of variables for remote interface.)
	// Parameter:  [out] pSVRWC:  
	// Parameter:  [in] lptitle:  
	// Returns:  int:  number of entries which are added to pSVRWC
	//************************************
	int AddResults( SVResultsWrapperClass* pSVRWC, LPCTSTR lptitle );

	//************************************
	// Description:  Clears the lists.
	//************************************
	void Clear();

	//************************************
	// Description:  Add complete Name of variables to the stringset.
	// Parameter:  stringSet:  
	// Returns:  int:  number of entries in the set
	//************************************
	int GetNameSet( SVStringSet& stringSet ) const;

	//************************************
	// Description:  Insert a new entry 
	// Parameter:  dottedName of new Variable   
	// Returns:  true if sucessfull  
	//************************************
	bool Insert( const SVString& dottedName );

	//************************************
	// Description:  Gets the value of the member variable m_LastUpdateTimeStamp
	// Returns:  SVClock::SVTimeStamp:  the value of the member variable m_LastUpdateTimeStamp
	//************************************
	SVClock::SVTimeStamp getUpdateTimeStamp();

	//************************************
	/// Check if the Object is Viewable
	/// this is only called when loading as the original ObjectAttributesAllowed may have changed
	/// \param reference to the SVObjectRefernce
	/// \return boolean true=viewable, false=not viewable
	//************************************
	virtual bool IsViewable(const SVObjectReference& objectRef) const;

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
	CString m_TagName; // Branch name in the config file
	std::vector<SVObjectReference> m_ReferenceVector;
	std::list<ResultViewItemDef> m_ResultViewItemDefList;
	SVClock::SVTimeStamp m_LastUpdateTimeStamp; // Most recent time when m_ReferenceVector changed.
#pragma endregion Member Variables
};

