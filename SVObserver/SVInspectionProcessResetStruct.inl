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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVInspectionProcessResetStruct.inl_v  $
 * 
 *    Rev 1.0   23 Apr 2013 11:02:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Sep 2005 15:38:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   correctly handle ItemOwner case
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Sep 2005 12:13:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified SetResetData
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Aug 2005 09:42:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a new structure to hold reset information to speed up the reset process.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/