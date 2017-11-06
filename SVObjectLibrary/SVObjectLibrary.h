//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectLibrary
//* .File Name       : $Workfile:   SVObjectLibrary.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 09:42:28  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVUtilityLibrary/SVGUID.h"
#include "SVClassRegisterListClass.h"
#include "Definitions/ObjectDefines.h"
#pragma endregion Includes

class SVObjectClass;

//******************************************************************************
//* MACRO(S):
//******************************************************************************

// Should be in the class definition...
#define SV_DECLARE_CLASS( XCLASSNAME )					\
		public:	\
			virtual SVGUID GetClassID(); \
			virtual LPCTSTR GetClassName();


// Should be in the implementation file... ( .cpp )
#define SV_IMPLEMENT_CLASS( XCLASSNAME, XCLASSID )	\
		class XCLASSNAME##RegisterClass : public SVClassRegisterClass \
		{ \
			public: \
				XCLASSNAME##RegisterClass() \
				{ \
				SVClassRegisterListClass::Instance().Add( this ); \
					OutputDebugString( "SV_IMPLEMENT: " #XCLASSNAME ", " ); \
					OutputDebugString( SVGUID( XCLASSID ).ToString().c_str() ); \
					OutputDebugString( _T( "\n" ) ); \
				} \
				virtual SVGUID GetClassID() override { return XCLASSID; } \
				virtual SVObjectClass* Construct() override { return new ( XCLASSNAME ); } \
				virtual LPCTSTR GetClassName() override { return #XCLASSNAME; } \
		};	\
		XCLASSNAME##RegisterClass The##XCLASSNAME##Register; \
		SVGUID XCLASSNAME::GetClassID() { return XCLASSID; } \
		LPCTSTR XCLASSNAME::GetClassName() { return #XCLASSNAME; }

