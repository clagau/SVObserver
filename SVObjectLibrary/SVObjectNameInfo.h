//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectNameInfo
//* .File Name       : $Workfile:   SVObjectNameInfo.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   17 Mar 2014 14:19:08  $
//******************************************************************************

#ifndef SVOBJECTNAMEINFO_H
#define SVOBJECTNAMEINFO_H

#pragma region Includes
#include <deque>
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

struct SVObjectNameInfo
{
	typedef std::deque< SVString > SVNameDeque;

	static HRESULT ParseObjectName( SVObjectNameInfo& p_rNameInfo, const SVString& p_rObjectName );

	SVObjectNameInfo();
	SVObjectNameInfo( const SVNameDeque& p_rNameArray );
	SVObjectNameInfo( const SVNameDeque& p_rNameArray, const SVString& p_rIndex );
	SVObjectNameInfo( const SVNameDeque& p_rNameArray, const SVString& p_rIndex, const SVString& p_rDefaultValue );
	SVObjectNameInfo( const SVObjectNameInfo& p_rObject );

	void clear();

	HRESULT ParseObjectName( const SVString& p_rObjectName );
	HRESULT UpdateObjectNameOnly( const SVString& p_rObjectName );

	SVString GetObjectName( size_t p_StartIndex = 0 ) const;
	SVString GetObjectArrayName( size_t p_StartIndex = 0 ) const;

	void RemoveTopName();
	void RemoveBottomName();
	long GetIndexValue() const;



	
	//************************************
	//! Set function for m_Index
	//! \param Index [in]
	//! \returns void
	//************************************
	void SetIndex(const SVString& Index ) {m_Index = Index;}
	
	//************************************
	//! Get Funtionn for m_Index 
	//! \returns const SVString&
	//************************************
	const SVString&      GetIndex() const {return m_Index;} 

	//************************************
	//! Get function for m_IndexPresent
	//! \returns bool
	//************************************
	bool IsIndexPresent() const  {return m_IndexPresent; } ;
	
	
	//************************************
	//! Set function for m_IndexPresent
	//! \param ispresent [in]
	//! \returns void
	//************************************
	void SetIsIndexPresent(bool ispresent) {m_IndexPresent = ispresent;}
	
	//************************************
	//! Clears the index variable
	//! \returns void
	//************************************
	void ClearIndex() {m_Index.clear();};
	
	//************************************
	//! true if m_index is empty 
	//! \returns bool
	//************************************
	bool IsIndexEmpty()  const {return m_Index.empty() ;}

	bool IsDefaultValuePresent() const  {return m_DefaultValuePresent; } ;
	void SetIsDefaultValuePresent(bool ispresent) {m_DefaultValuePresent = ispresent;}
	
	
	const SVString&      GetDefaultValue() const {return m_DefaultValue;} 
	void SetDefaultValue(const SVString& DefaultValue ) {m_DefaultValue = DefaultValue;}

	SVNameDeque m_NameArray;



private:
	
	bool m_IndexPresent; /// true for arrays 
	SVString m_Index; /// one based index or empty for non array names or whole arrays 

	bool m_DefaultValuePresent;
	SVString m_DefaultValue;
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObjectLibrary\SVObjectNameInfo.h_v  $
 * 
 *    Rev 1.2   17 Mar 2014 14:19:08   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  869
 * SCR Title:  Add PPQ and Environment Variables to Object Manager and Update Pickers
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added Includes region.
 *   Added methods to remove either the top or bottom part of an FQN.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Jun 2013 17:42:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 017.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   21 May 2013 12:48:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  824
 * SCR Title:  Add Remote Access to SVObserver Application Configuration Variables
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update Object Name Information class to allow for starting the name at a specified index position.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 16:58:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Jun 2012 17:25:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Separated dotted name parsing and management to its own class.  This allows for the use of this functionality without have to involve the Reference Class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Jun 2012 17:20:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Separated dotted name parsing and management to its own class.  This allows for the use of this functionality without have to involve the Reference Class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/