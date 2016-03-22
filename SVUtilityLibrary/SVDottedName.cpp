//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDottedName
//* .File Name       : $Workfile:   SVDottedName.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   03 Jul 2014 16:27:28  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/algorithm/string.hpp>
#include "SVDottedName.h"
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

