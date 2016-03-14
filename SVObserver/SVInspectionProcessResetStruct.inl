//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInspectionProcessResetStruct
//* .File Name       : $Workfile:   SVInspectionProcessResetStruct.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:02:56  $
//******************************************************************************

inline SVInspectionProcessResetStruct::SVInspectionProcessResetStruct()
{
	m_eResetItem = SVResetItemNone;
};

inline SVInspectionProcessResetStruct::SVInspectionProcessResetStruct( const SVInspectionProcessResetStruct &p_rsvObject )
{
	*this = p_rsvObject;
};

inline const SVInspectionProcessResetStruct &SVInspectionProcessResetStruct::operator=( const SVInspectionProcessResetStruct &p_rsvObject )
{
	if( this != &p_rsvObject )
	{
		m_eResetItem = p_rsvObject.m_eResetItem;
		m_svObjectSet = p_rsvObject.m_svObjectSet;
	}

	return *this;
};

inline SVResetItemEnum SVInspectionProcessResetStruct::SetResetData( SVResetItemEnum p_eResetItem, SVObjectClass *p_psvObject )
{
	switch( p_eResetItem )
	{
		default:
		{
			m_svObjectSet.clear();

			m_eResetItem = SVResetItemIP;

			break;
		}
		case SVResetItemIP:
		{
			if( p_eResetItem < m_eResetItem )
			{
				m_svObjectSet.clear();
			}

			m_eResetItem = SVResetItemIP;

			break;
		}
		case SVResetItemTool:
		{
			if( p_eResetItem < m_eResetItem )
			{
				m_svObjectSet.clear();

				m_eResetItem = SVResetItemTool;
			}

			break;
		}
		case SVResetItemOwner:
		{
			if( p_eResetItem <= m_eResetItem )
			{
				m_svObjectSet.insert( p_psvObject->GetOwner() );

				m_eResetItem = SVResetItemOwner;
			}

			break;
		}
		case SVResetItemNone:
		{
			break;
		}
	}

	return m_eResetItem;
};

