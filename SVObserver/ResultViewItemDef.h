//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ResultViewItemDef
//* .File Name       : $Workfile:   ResultViewItemDef.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   07 Jan 2015 17:35:06  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVObjectLibrary/SVObjectReference.h"
#include "SVUtilityLibrary/SVGUID.h"
#pragma endregion Includes

/*
	Class describes one item in the result view.
*/
class ResultViewItemDef
{
public:
#pragma region Constructor
	//************************************
	// Description:  Default constructor for the ResultViewItemDef class.
	//************************************
	ResultViewItemDef();

	//************************************
	// Description:  Constructor for the ResultViewItemDef class.
	// Parameter:  guid <in>:  Initial value for the object ID.
	//************************************
	ResultViewItemDef(const SVGUID& guid);

	//************************************
	// Description:  Constructor for the ResultViewItemDef class.
	// Parameter:  guid <in>:  Initial value for the object ID.
	// Parameter:  index <in>:  Initial value for the index.
	//************************************
	ResultViewItemDef(const SVGUID& guid, const SVString& index);

	//************************************
	// Description:  Constructor for the ResultViewItemDef class.
	// Parameter:  objRef <in>:  a reference to the object whose data should be used to populate member variables
	//************************************
	ResultViewItemDef(const SVObjectReference& objRef);

	//************************************
	// Description:  Destructor for the ResultViewItemDef class.
	//************************************
	virtual ~ResultViewItemDef();
#pragma endregion Constructor

#pragma region Public Methods
	//************************************
	// Description:  Sets the member variable m_Index.
	// Parameter:  index <in>:  The desired value for the index.
	//************************************
	void setIndex(const SVString& index);

	//************************************
	// Description:  Gets the member variable m_Index.
	// Returns:  const SVString&:  the value of the member variable m_Index
	//************************************
	const SVString& getIndex() const;

	//************************************
	// Description:  Gets the value of the member variable m_Index as a long
	// Returns:  long:  the value of the member variable m_Index, if possible.  0 otherwise.
	//************************************
	long getIndexValue() const;

	//************************************
	// Description:  Gets the value of the member variable m_hasIndex
	// Returns:  bool:  the value of the member variable m_bIndex
	//************************************
	bool hasIndex() const;

	//************************************
	// Description:  Gets the value of the member variable m_ObjectID
	// Returns:  const SVString&:  the value of the member variable m_ObjectID
	//************************************
	const SVGUID& getGuid() const;
#pragma endregion Public Methods

protected:
#pragma region Member Variables
	SVGUID m_ObjectID;
	bool m_hasIndex;
	SVString m_Index;
#pragma endregion Member Variables
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\ResultViewItemDef.h_v  $
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