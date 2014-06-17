//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDottedName
//* .File Name       : $Workfile:   SVDottedName.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Jun 2014 10:26:14  $
//******************************************************************************

#pragma once

#pragma region Includes
#include <deque>
#include <string>
#include "SVString.h"
#pragma endregion Includes

class SVDottedName
{
public:
#pragma region Declarations
	typedef std::deque<std::string> NameList;
	typedef NameList::const_iterator const_iterator;
	typedef NameList::const_reverse_iterator const_reverse_iterator;
#pragma endregion Declarations

#pragma region Constructor
	SVDottedName(const std::string& name);
	~SVDottedName();
#pragma endregion Constructor

#pragma region Public Methods
	size_t size() const;
	const std::string& operator[](size_t index) const;
	const_iterator begin() const;
	const_iterator end() const;
	const_reverse_iterator rbegin() const;
	const_reverse_iterator rend() const;
#pragma endregion Public Methods

#pragma region Member Variables
private:
	std::string m_name;
	NameList m_nameTokens;
#pragma endregion Member Variables
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVUtilityLibrary\SVDottedName.h_v  $
 * 
 *    Rev 1.0   17 Jun 2014 10:26:14   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/