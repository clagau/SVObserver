// ******************************************************************************
// * COPYRIGHT (c) 2013 by Seidenader, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVDataItemManagerTemplate
// * .File Name       : $Workfile:   SVDataItemManagerTemplate.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   02 Oct 2013 08:55:16  $
// ******************************************************************************

#ifndef SVDATAITEMMANAGERTEMPLATE_H
#define SVDATAITEMMANAGERTEMPLATE_H

#include <map>
#include <boost/config.hpp>
#include <boost/function.hpp>

template< typename SVItemData, typename SVIndexType = int >
class SVDataItemManagerTemplate
{
public:
	typedef boost::function< DWORD_PTR ( SVIndexType ) > SVGetItemDataFunctor;
	typedef boost::function< BOOL ( SVIndexType, DWORD_PTR ) > SVSetItemDataFunctor;
	typedef std::map< unsigned long, SVItemData > SVItemDataMap;
	typedef typename SVItemDataMap::iterator iterator;
	typedef typename SVItemDataMap::const_iterator const_iterator;
	typedef typename SVItemDataMap::key_type key_type;
	typedef typename SVItemDataMap::size_type size_type;

	SVDataItemManagerTemplate( SVGetItemDataFunctor p_GetFunctor, SVSetItemDataFunctor p_SetFunctor );
	SVDataItemManagerTemplate( const SVDataItemManagerTemplate& p_rObject );
	virtual ~SVDataItemManagerTemplate();

	bool empty() const;
	void clear();

	iterator end();
	const_iterator end() const;

	iterator find( const key_type& p_Keyval );
	const_iterator find( const key_type& p_Keyval ) const;

	void erase( iterator p_Where );
	size_type erase( const key_type& p_Keyval );

	iterator GetItemData( const SVIndexType& p_IndexVal );
	const_iterator GetItemData( const SVIndexType& p_IndexVal ) const;
	bool SetItemData( const SVIndexType& p_IndexVal, const SVItemData& p_rItem );
	void ClearItemData( const SVIndexType& p_IndexVal );

protected:
	SVDataItemManagerTemplate();

	SVGetItemDataFunctor m_GetFunctor;
	SVSetItemDataFunctor m_SetFunctor;

	unsigned long m_LastUsedIdentifier;
	SVItemDataMap m_Items;

};

#include "SVDataItemManagerTemplate.inl"

#endif

