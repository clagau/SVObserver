// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVDeviceParamCollection
// * .File Name       : $Workfile:   SVDeviceParamCollection.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.3  $
// * .Check In Date   : $Date:   15 May 2013 19:42:26  $
// ******************************************************************************

#ifndef SVDEVICEPARAMCOLLECTION_H
#define SVDEVICEPARAMCOLLECTION_H

#pragma warning( disable: 4786 )	// identifier truncation 255 chars

#include <map>
#include <vector>

#include "SVLibrary/SVVisitor.h"
#include "SVTemplate.h"
#include "SVDeviceParam.h"
#include "SVUtilityLibrary/SVString.h"

typedef std::map <SVDeviceParamEnum, SVDeviceParamWrapper> SVDeviceParamMap;

class SVDeviceParamCollection
{
public:
	SVDeviceParamCollection() {};
	SVDeviceParamCollection( const SVDeviceParamCollection& rhs );
	virtual ~SVDeviceParamCollection() {}	// this class is inheritable to provide access to runtime device-specific information
	HRESULT Clear();
	SVDeviceParamCollection& operator = ( const SVDeviceParamCollection& rhs );

	HRESULT CreateParameter( SVDeviceParamEnum e, const VARIANT& rv );

	template <typename DERIVED> HRESULT SetParameter( SVDeviceParamEnum e, const DERIVED& rParam )
	{
		mapParameters[e] = rParam;
		mapParameters[e]->SetType( e );
		return S_OK;
	}

	HRESULT SetParameter( const SVDeviceParamWrapper& rw )
	{
		if ( ((const SVDeviceParam*) rw ) != NULL )
		{
			mapParameters[rw->Type()] = rw;
			return S_OK;
		}
		return S_FALSE;
	}

	bool ParameterExists( SVDeviceParamEnum e ) const
	{
		return (mapParameters.find(e) != mapParameters.end()) && mapParameters.find(e)->second != NULL;
	}

	const SVDeviceParamWrapper& Parameter( SVDeviceParamEnum e ) const
	{
		static SVDeviceParamWrapper dummy;
		SVDeviceParamMap::const_iterator iter = mapParameters.find(e);
		if ( iter == mapParameters.end() )
			return dummy;

		return iter->second;
	}

	SVDeviceParamWrapper& GetParameter( SVDeviceParamEnum e )
	{
		return mapParameters[e];
	}

	template <typename DERIVED> HRESULT GetParameter( SVDeviceParamEnum e, DERIVED*& rpParam )
	{
		if ( mapParameters.find(e) != mapParameters.end() )
		{
			return mapParameters[e].GetDerivedValue( rpParam );
		}
		return ERROR_NOT_FOUND;
	}

	// For custom parameters - uses visual name
	bool ParameterExists( const SVString& rVisualName ) const
	{
		bool bRetVal = false;

		for (SVDeviceParamMap::const_iterator it = mapParameters.begin(); !bRetVal && it != mapParameters.end();++it)
		{
			if (rVisualName == it->second->VisualName())
			{
				bRetVal = true;
			}
		}
		return bRetVal;
	}

	// For custom parameters - uses visual name
	const SVDeviceParamWrapper& Parameter( const SVString& rVisualName ) const
	{
		static SVDeviceParamWrapper dummy;
		for (SVDeviceParamMap::const_iterator iter = mapParameters.begin(); iter != mapParameters.end();++iter)
		{
			if (rVisualName == iter->second->VisualName())
				return iter->second;
		}
		return dummy;
	}

	// For custom parameters - uses visual name
	SVDeviceParamWrapper& GetParameter( const SVString& rVisualName )
	{
		static SVDeviceParamWrapper dummy;
		for (SVDeviceParamMap::iterator iter = mapParameters.begin(); iter != mapParameters.end();++iter)
		{
			if (rVisualName == iter->second->VisualName())
				return iter->second;
		}
		return dummy;
	}

	// For custom parameters - uses visual name
	template <typename DERIVED> HRESULT GetParameter( const SVString& rVisualName, DERIVED*& rpParam )
	{
		for (SVDeviceParamMap::iterator iter = mapParameters.begin(); iter != mapParameters.end();++iter)
		{
			if (rVisualName == iter->second->VisualName())
				return iter->second.GetDerivedValue( rpParam );
		}
		return ERROR_NOT_FOUND;
	}

	/* const DERIVED*& doesn't seems to want to work correctly, so we can't implement this function.
	template <typename DERIVED> HRESULT GetParameter( SVDeviceParamEnum e, const DERIVED*& rpParam ) const
	{
		SVDeviceParamMap::const_iterator iter = mapParameters.find(e);
		if ( iter != mapParameters.end() )
		{
			rpParam = iter->second.DerivedValue( rpParam );
			return rpParam != NULL ? S_OK : TYPE_E_TYPEMISMATCH;
		}
		else
		{
			return ERROR_NOT_FOUND;
		}
	}
	*/

public:	// needed for iterators
	SVDeviceParamMap mapParameters;
};

typedef TDoubleFactorySingleton<SVDeviceParamEnum, CString, SVDeviceParam>::factory TheDeviceParamFactory;

class SVDeviceParamIndexer : public std::vector<SVDeviceParamEnum>
{
public:
	SVDeviceParamIndexer(const SVDeviceParamMap& map);

private:
	class keycomp : public std::binary_function<SVDeviceParamEnum, SVDeviceParamEnum, bool>
	{
	public:
		keycomp(const SVDeviceParamMap& map) : m_mapIndexed(map) {}
		bool operator () (const SVDeviceParamEnum& a, const SVDeviceParamEnum& b) const;

	private:
		const SVDeviceParamMap& m_mapIndexed;
	};
	const SVDeviceParamMap& m_mapIndexed;
};

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOMFCLibrary\SVDeviceParamCollection.h_v  $
 * 
 *    Rev 1.3   15 May 2013 19:42:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 016.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   10 May 2013 10:09:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  828
 * SCR Title:  Camera Binning
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed unnecessary whitespace
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 May 2013 11:10:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   03 May 2013 12:59:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  820
 * SCR Title:  Baumer Camera Trigger Debounce
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added non const methods to get the custom parameter by visual name
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 May 2013 15:39:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 013.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   01 May 2013 10:34:18   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  820
 * SCR Title:  Baumer Camera Trigger Debounce
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added method to check if parameters exists by visual name (for custom parameters). Added method to get parameter by visual name (for custom parameters). 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 13:02:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   01 Apr 2013 13:41:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  820
 * SCR Title:  Baumer Camera Trigger Debounce
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Moved the definition and implenetation for the SVDeviceParam class to SVDeviceParam..h/.cpp
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   18 Feb 2011 09:49:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  700
 * SCR Title:  Remove String Buffer Problems
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to remove duplicate string class, and fixed string conversion issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   15 Dec 2009 13:26:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   20 Jun 2007 14:48:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   03 Feb 2004 17:17:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added support for named device params
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   12 Jan 2004 11:08:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  404
 * SCR Title:  Change Camera Parameters that represent time to use standard units
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added virtual HRESULT SetMetaData( .. pBaseParam )
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   03 Nov 2003 16:19:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed logic for ParameterExists function
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   17 Sep 2003 08:56:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added Clear function and VisualName property
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   29 Aug 2003 13:10:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added support for NullDeviceParam
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   27 Aug 2003 10:06:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added Visitor support
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   12 Aug 2003 08:52:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added NULL check
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   07 Aug 2003 12:24:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added SetParameter for SVDeviceParamWrapper
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Jul 2003 09:03:22   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed Level to DetailLevel
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Jul 2003 08:41:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   renamed Level to DetailLevel in SVDeviceParam
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Jul 2003 10:19:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   transfered from SVObserver project
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
