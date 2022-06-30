//******************************************************************************
//* COPYRIGHT (c) 2004 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVProtoBuf/SVO-Enum.h"
#include "SVClassRegisterClass.h"
#include "SVClassRegisterListClass.h"
#pragma endregion Includes

class SVObjectClass;

//******************************************************************************
//* MACRO(S):
//******************************************************************************

// Should be in the class definition...
#define SV_DECLARE_CLASS					\
		public:	\
			virtual SvPb::ClassIdEnum GetClassID() const override; \
			LPCTSTR GetClassName() const;


// Should be in the implementation file... ( .cpp )
#define SV_IMPLEMENT_CLASS( XCLASSNAME, XCLASSID )	\
		class XCLASSNAME##RegisterClass : public SVClassRegisterClass \
		{ \
			public: \
				XCLASSNAME##RegisterClass() \
				{ \
				SVClassRegisterListClass::Instance().Add( this ); \
				} \
				virtual SvPb::ClassIdEnum GetClassID() const override { return XCLASSID; } \
				virtual SVObjectClass* Construct() override { return new ( XCLASSNAME ); } \
				virtual LPCTSTR GetClassName() const override { return #XCLASSNAME; } \
		};	\
		XCLASSNAME##RegisterClass The##XCLASSNAME##Register; \
		SvPb::ClassIdEnum XCLASSNAME::GetClassID() const { return XCLASSID; } \
		LPCTSTR XCLASSNAME::GetClassName() const { return #XCLASSNAME; }

