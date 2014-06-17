//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDottedName
//* .File Name       : $Workfile:   SVDottedName.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Jun 2014 10:26:12  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#pragma warning (push)
#pragma warning (disable : 4996)
#include <boost/config.hpp>
#include <boost/algorithm/string.hpp>
#include "SVDottedName.h"
#pragma warning (pop)
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

#pragma region Constructor
SVDottedName::SVDottedName(const std::string& name)
: m_name(name)
{
	boost::algorithm::split(m_nameTokens, name, boost::algorithm::is_any_of("."), boost::algorithm::token_compress_on);
}

SVDottedName::~SVDottedName()
{
}
#pragma endregion Constructor

#pragma region Public Methods
size_t SVDottedName::size() const
{
	return m_nameTokens.size();
}

const std::string& SVDottedName::operator[](size_t index) const
{
	return m_nameTokens[index];
}

SVDottedName::const_iterator SVDottedName::begin() const
{
	return m_nameTokens.begin();
}

SVDottedName::const_iterator SVDottedName::end() const
{
	return m_nameTokens.end();
}

SVDottedName::const_reverse_iterator SVDottedName::rbegin() const
{
	return m_nameTokens.rbegin();
}

SVDottedName::const_reverse_iterator SVDottedName::rend() const
{
	return m_nameTokens.rend();
}
#pragma endregion Public Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVUtilityLibrary\SVDottedName.cpp_v  $
 * 
 *    Rev 1.0   17 Jun 2014 10:26:12   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/