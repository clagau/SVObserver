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

//Moved to precompiled header: #include <comdef.h>
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

