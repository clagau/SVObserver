//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : BasicValueObjects
//* .File Name       : $Workfile:   BasicValueObjects.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Jun 2014 08:55:52  $
//******************************************************************************

#pragma region Public Methods
template <typename ELEMENT_TYPE>
HRESULT BasicValueObjects::getValue( LPCTSTR DottedName, ELEMENT_TYPE& rValue ) const
{
	HRESULT Result = S_FALSE;

	BasicValueObjectPtr pValue = getValueObject( DottedName );
	if( !pValue.empty() )
	{
		Result = pValue->getValue( rValue );
	}

	return Result;
}

template <typename ELEMENT_TYPE>
BasicValueObjectPtr BasicValueObjects::setValueObject( LPCTSTR DottedName, const ELEMENT_TYPE Value, SVObjectClass* pOwner, SVObjectTypeEnum ObjectType )
{
	vt_const_iterator Iter = findValueObject( DottedName );
	if( m_Tree.end() == Iter )
	{
		Iter = createValueObject( DottedName, Value, pOwner, ObjectType );
	}
	else
	{
		(*Iter)->setValue( Value );
	}

	return *Iter;
}

inline const BasicValueObjects::ValueTree& BasicValueObjects::getTree() const
{
	return m_Tree;
}
#pragma endregion Public Methods

#pragma region Private Methods
template <typename ELEMENT_TYPE>
BasicValueObjects::vt_const_iterator BasicValueObjects::createValueObject( LPCTSTR DottedName, const ELEMENT_TYPE Value, SVObjectClass* pOwner, SVObjectTypeEnum ObjectType )
{
	vt_const_iterator Iter( m_Tree.end() );
	vt_iterator IterParent( m_Tree.end() );
	vt_iterator IterStart( m_Tree.begin() );
	vt_iterator IterEnd( m_Tree.end() );
	SVObjectNameInfo ParsedName;
	SVObjectClass* pParent = pOwner;

	if( S_OK == ParsedName.ParseObjectName( DottedName ) )
	{
		for( size_t i=0; i < ParsedName.m_NameArray.size(); i++ )
		{
			SVString Name( ParsedName.m_NameArray[i] );

			Iter = findChildObject( IterStart, IterEnd, Name.c_str() );
			if( IterEnd == Iter )
			{
				//If default object type then check parent
				if( nullptr != pParent && SVBasicValueObjectType == ObjectType )
				{
					ObjectType = pParent->GetObjectInfo().ObjectTypeInfo.ObjectType;
				}
				BasicValueObjectPtr pValue(nullptr);
				bool Node( false );
				//Check if last name entry
				if( ParsedName.m_NameArray.size() != i + 1 )
				{
					//Generate node and node becomes the new parent object
					Node = true;
				}
				pValue = new BasicValueObject( Name.c_str(), pParent, Node, ObjectType );
				pValue->setValue(Value);

				if( m_Tree.end() != IterParent )
				{
					Iter = IterParent.node()->insert( pValue );
					IterStart = IterParent.node()->begin();
				}
				else
				{
					Iter = m_Tree.insert( pValue );
					IterStart = m_Tree.begin();
				}
			}
			if( (*Iter)->isNode() )
			{
				IterParent = IterStart;
				std::advance ( IterParent, std::distance<vt_const_iterator>( IterStart, Iter ) );
				IterStart = IterParent.node()->begin();
				IterEnd = IterParent.node()->end();

				pParent = dynamic_cast<SVObjectClass*> ( (*Iter).get() );
			}
		}
	}

	return Iter;
}
#pragma endregion Private Methods

