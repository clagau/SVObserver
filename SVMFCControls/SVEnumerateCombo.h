//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEnumerateCombo
//* .File Name       : $Workfile:   SVEnumerateCombo.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:26:34  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "Definitions/NameValueVector.h"
#pragma endregion Includes

namespace SvMc
{
	class SVEnumerateCombo : public CComboBox
	{
	public:
		SVEnumerateCombo();
		virtual ~SVEnumerateCombo();

		bool SetEnumTypes(const SvDef::NameValueVector& rEnumList);

		int SetCurSelItemData( DWORD_PTR dwItemData );
		DWORD_PTR  GetCurSelItemData();

	protected:
		DECLARE_MESSAGE_MAP()
	};
} //namespace SvMc

