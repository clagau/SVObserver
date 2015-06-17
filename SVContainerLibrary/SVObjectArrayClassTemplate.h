//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectArrayClassTemplate
//* .File Name       : $Workfile:   SVObjectArrayClassTemplate.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 11:27:02  $
//******************************************************************************

#pragma once

#ifdef SV_SHOW_INCLUDES
	#pragma message( "Including " __FILE__ )
#endif

#include <comdef.h>
#include "SVVector.h"

namespace Seidenader
{
	namespace SVContainerLibrary
	{
		template <class TYPE>
		class SVObjectArrayClassTemplate : public SVVector<TYPE>
		{
		public:
			SVObjectArrayClassTemplate();
			SVObjectArrayClassTemplate(SVObjectArrayClassTemplate<TYPE>& orig);
			virtual ~SVObjectArrayClassTemplate();

			// this allows assigning from an array of one type to an array of another type
			template <typename T>
			SVObjectArrayClassTemplate<TYPE>& operator = ( const SVObjectArrayClassTemplate<T>& rhs )
			{
				SetSize( static_cast< int >( rhs.GetSize() ) );
				for ( int i=0; i < rhs.GetSize(); i++ )
					ElementAt(i) = rhs.GetAt(i);
				return *this;
			}

			// native type assignment for faster operation
			SVObjectArrayClassTemplate<TYPE>& operator = ( const SVObjectArrayClassTemplate<TYPE>& rhs );
		};

		template <class TYPE>
		SVObjectArrayClassTemplate<TYPE>::SVObjectArrayClassTemplate()
		{
		}

		template <class TYPE>
		SVObjectArrayClassTemplate<TYPE>::SVObjectArrayClassTemplate(SVObjectArrayClassTemplate<TYPE>& orig)
		{
			(*this) = orig;
		}

		template <class TYPE>
		SVObjectArrayClassTemplate<TYPE>::~SVObjectArrayClassTemplate()
		{
		}

		template <class TYPE>
		SVObjectArrayClassTemplate<TYPE>& SVObjectArrayClassTemplate<TYPE>::operator = (const SVObjectArrayClassTemplate<TYPE>& rhs)
		{
			if ( this != &rhs )
			{
				Copy( rhs );
			}

			return *this;
		}


		typedef SVObjectArrayClassTemplate<CPoint> SVObjectCPointArrayClass;
		typedef SVObjectArrayClassTemplate<BOOL> SVObjectBoolArrayClass;
		typedef SVObjectArrayClassTemplate<BYTE> SVObjectByteArrayClass;
		typedef SVObjectArrayClassTemplate<long> SVObjectLongArrayClass;
		typedef SVObjectArrayClassTemplate<double> SVObjectDoubleArrayClass;
		typedef SVObjectArrayClassTemplate<DWORD> SVObjectDWordArrayClass;
		typedef SVObjectArrayClassTemplate<__int64> SVObjectInt64ArrayClass;
		typedef SVObjectArrayClassTemplate< _variant_t > SVObjectVariantArrayClass;
		typedef SVObjectArrayClassTemplate<CString> SVObjectCStringArrayClass;
	} //SVContainerLibrary
} //Seidenader

namespace SvCl = Seidenader::SVContainerLibrary;


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObjectLibrary\SVObjectArrayClassTemplate.h_v  $
 * 
 *    Rev 1.1   01 Oct 2013 11:27:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 16:38:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   08 Nov 2010 14:26:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container types.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   05 Nov 2010 10:05:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove redundent data objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   15 Dec 2009 12:55:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Jun 2005 12:33:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   used GetAt instead of ElementAt to work with const
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   20 Jun 2005 15:13:22   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added template operator =
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   19 May 2005 14:41:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   made operator = const-correct
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Feb 2005 14:52:50   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   First version of file for SVObjectLibrary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   06 Jul 2004 15:16:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  416
 * SCR Title:  Add Variant Value Object
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
