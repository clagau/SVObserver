//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMap
//* .File Name       : $Workfile:   SVMap.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 07:44:14  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <map>

template <typename SVT_KEY, typename SVT_VALUE>
class SVMap
{
public:
	typedef std::map< SVT_KEY, SVT_VALUE > SVMapBase;
	typedef typename SVMapBase::key_type key_type;
	typedef typename SVMapBase::mapped_type mapped_type;
	typedef typename SVMapBase::value_type value_type;
	typedef typename SVMapBase::size_type size_type;
	typedef typename SVMapBase::const_iterator const_iterator;
	typedef typename SVMapBase::iterator iterator;
	typedef typename SVMapBase::_Pairib SVIteratorBoolPair;

	SVMap();
	SVMap( const SVMap& p_rObject );
	SVMap( const SVMapBase& p_rObject );

	virtual ~SVMap();

	operator SVMapBase const & () const;
	operator SVMapBase & ();

	const SVMap& operator=( const SVMap& p_rObject );
	const SVMap& operator=( const SVMapBase& p_rObject );

	virtual bool empty() const;

	virtual void clear();

	virtual size_type size() const;

	virtual iterator begin();
	virtual const_iterator begin() const;

	virtual iterator end();
	virtual const_iterator end() const;

	virtual iterator find( const key_type& p_rKey );
	virtual const_iterator find( const key_type& p_rKey ) const;

	virtual SVIteratorBoolPair insert( const value_type& p_rKeyValuePair );
	virtual SVIteratorBoolPair insert( const key_type& p_rKey, const mapped_type& p_rValue );

	template< class InputIterator >
	void insert( InputIterator p_First, InputIterator p_Last );

	virtual size_type erase( const key_type& p_rKey );
	virtual iterator erase( iterator p_Where );


	// MFC Style Operators
	virtual bool IsEmpty() const;

	virtual int GetCount() const;
	virtual int GetSize() const;

	virtual iterator GetStartPosition() const;

	virtual void GetNextAssoc( iterator& rNextPosition, key_type& rKey, mapped_type& rValue ) const;

	virtual bool Lookup( const key_type& key, mapped_type& rValue ) const;

	virtual void SetAt( const key_type& key, mapped_type& newElement );

	virtual mapped_type& operator[]( const key_type& key );

	virtual void RemoveKey( const key_type& key );

	virtual void RemoveAll();

protected:
	SVMapBase m_Map;

};

#include "SVMap.inl"
