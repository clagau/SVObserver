//******************************************************************************
//* COPYRIGHT (c) 2005 by Körber Pharma Inspection GmbH. All Rights Reserved
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
	m_eResetItem = SvOi::SVResetItemNone;
};

inline SVInspectionProcessResetStruct::SVInspectionProcessResetStruct( const SVInspectionProcessResetStruct& rRhs )
{
	*this = rRhs;
};

inline const SVInspectionProcessResetStruct &SVInspectionProcessResetStruct::operator=( const SVInspectionProcessResetStruct& rRhs )
{
	if( &rRhs != this  )
	{
		m_eResetItem = rRhs.m_eResetItem;
		m_ObjectSet = rRhs.m_ObjectSet;
	}

	return *this;
};

inline SvOi::SVResetItemEnum SVInspectionProcessResetStruct::SetResetData( SvOi::SVResetItemEnum eResetItem, SVObjectClass *pObject )
{
	switch( eResetItem )
	{
	case SvOi::SVResetItemIP:
		{
			if( eResetItem < m_eResetItem )
			{
				m_ObjectSet.clear();
			}

			m_eResetItem = SvOi::SVResetItemIP;

			break;
		}
		case SvOi::SVResetItemTool:
		{
			if( eResetItem < m_eResetItem )
			{
				m_ObjectSet.clear();

				m_eResetItem = SvOi::SVResetItemTool;
			}

			break;
		}
		case SvOi::SVResetItemOwner:
		{
			if( eResetItem <= m_eResetItem )
			{
				m_ObjectSet.insert( pObject->GetParent() );

				m_eResetItem = SvOi::SVResetItemOwner;
			}

			break;
		}
		case SvOi::SVResetItemNone:
		{
			break;
		}
		default:
		{
			m_ObjectSet.clear();

			m_eResetItem = SvOi::SVResetItemIP;

			break;
		}
	}

	return m_eResetItem;
};

