//******************************************************************************
//* COPYRIGHT (c) 2014 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "Definitions\ObjectDefines.h"
#pragma endregion Includes

class SVObjectReference;

namespace SvIe
{

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
	ResultViewItemDef() = default;

	//************************************
	// Description:  Constructor for the ResultViewItemDef class.
	// Parameter:  objectId <in>:  Initial value for the object ID.
	//************************************
	explicit ResultViewItemDef(uint32_t objectId);

	//************************************
	// Description:  Constructor for the ResultViewItemDef class.
	// Parameter:  objectId <in>:  Initial value for the object ID.
	// Parameter:  index <in>:  Initial value for the index.
	//************************************
	ResultViewItemDef(uint32_t objectId, int index);

	//************************************
	// Description:  Constructor for the ResultViewItemDef class.
	// Parameter:  objRef <in>:  a reference to the object whose data should be used to populate member variables
	//************************************
	explicit ResultViewItemDef(const SVObjectReference& objRef);

	//************************************
	// Description:  Destructor for the ResultViewItemDef class.
	//************************************
	virtual ~ResultViewItemDef();
#pragma endregion Constructor

#pragma region Public Methods
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
	// Returns:  uint32_t objectId:  the value of the member variable m_ObjectID
	//************************************
	uint32_t getObjectId() const { return m_ObjectID; };
#pragma endregion Public Methods

protected:
#pragma region Member Variables
	uint32_t m_ObjectID = SvDef::InvalidObjectId;
	bool m_hasIndex = false;
	int m_Index = 0;
#pragma endregion Member Variables
};

} //namespace SvIe
