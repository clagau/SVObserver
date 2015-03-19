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
#include <list>
#include "SVObjectLibrary/SVObjectWriter.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#include "SVTimerLibrary/SVClock.h"
#include "ResultViewItemDef.h"
#pragma endregion Includes

#pragma region Declarations
struct SVIPResultData;
class SVIPResultItemDefinition;
class SVResultsWrapperClass;
#pragma endregion Declarations

/**
	Class contains a list of GUIDS of and references to variables which are selected to be shown in the result view.
**/
class ResultViewReferences
{
public:
#pragma region Constructor
	typedef std::deque< SVIPResultItemDefinition > SVResultDefinitionDeque;

	//************************************
	// Description:  Default constructor for the ResultViewReferences class.
	//************************************
	ResultViewReferences();

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
	// Parameter:  htiParent:  
	// Returns:  bool:  
	//************************************
	virtual bool Load( SVXMLMaterialsTree& rTree, SVXMLMaterialsTree::SVBranchHandle htiParent );

	//************************************
	// Description:  Rebuild the reference vector from the list of GUIDs.
	//************************************
	virtual void RebuildReferenceVector();

	//************************************
	// Description:  Build SVResultDefinitionDeque from the reference vector.  (Defines the list of viewed variables.)
	// Parameter:  rDefinitions <out>:  object containing data from the class's variables.
	// Returns:  bool:  
	//************************************
	virtual bool SetResultDefinitions( SVResultDefinitionDeque& rDefinitions ) const; // BRW - How can a Set method be const?  Should it be called GetResultDefinitions?

	//************************************
	// Description:  Build SVIPResultData from the reference vector.  (Get the values of the variables.)
	// Parameter:  rResultData <out>:  object containing data from the class's variables.
	// Parameter:  getColor <in>:  true (default) if the color for the item in the result view should be fetched.
	// Returns:  bool:  
	//************************************
	virtual bool SetResultData( SVIPResultData& rResultData, bool getColor = true ) const; // BRW - This method should be changed to return HRESULT (as setPPQInputResultData did), not bool.  How can a Set method be const?  Should it be called GetResultData?

	//************************************
	// Description:  Build SVResultsWrapperClass from reference vector.  (Get values of variables for remote interface.)
	// Parameter:  pSVRWC:  
	// Parameter:  lptitle:  
	// Returns:  int:  
	//************************************
	int AddResults( SVResultsWrapperClass* pSVRWC, LPCTSTR lptitle );

	//************************************
	// Description:  Clears the lists.
	//************************************
	void Clear();

	//************************************
	// Description:  Add complete Name of variables to the stringset.
	// Parameter:  stringSet:  
	// Returns:  int:  
	//************************************
	int GetNameSet( SVStringSet& stringSet ) const;

	//************************************
	// Description:  
	// Parameter:  dottedName:  
	// Returns:  bool:  
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
#pragma endregion Public Methods

protected:
#pragma region Protected Methods
	//************************************
	// Description:  Helper function for Load.  Loads a single item.
	// Parameter:  rTree:  
	// Parameter:  htiLeaf:  
	// Returns:  bool:  
	//************************************
	bool LoadResultViewItemDef( SVXMLMaterialsTree& rTree, SVXMLMaterialsTree::SVLeafHandle htiLeaf );
#pragma endregion Protected Methods

#pragma region Member Variables
	CString m_BranchName; // Branch name in the config file
	std::vector<SVObjectReference> m_ReferenceVector;
	std::list<ResultViewItemDef> m_ResultViewItemDefList;
	SVClock::SVTimeStamp m_LastUpdateTimeStamp; // Most recent time when m_ReferenceVector changed.
#pragma endregion Member Variables
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\ResultViewReferences.h_v  $
 * 
 *    Rev 1.0   07 Jan 2015 17:35:06   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  980
 * SCR Title:  Add Non-Inspection Objects to the Result View
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/