//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMaterialData
//* .File Name       : $Workfile:   SVMaterialData.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:14:28  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <comutil.h>
//Moved to precompiled header: #include <memory>
#pragma endregion Includes

namespace  SvXml
{
	class SVMaterialData
	{
	public:
		SVMaterialData();
		SVMaterialData( const SVMaterialData& rObject );
		explicit SVMaterialData( const _variant_t& rVariant );

		virtual ~SVMaterialData();

		bool empty() const;

		void clear();

		VARIANT& GetVARIANT();

		operator const _variant_t & () const;

		SVMaterialData& operator=( const SVMaterialData& rObject );
		SVMaterialData& operator=( const _variant_t& rVariant );

	  bool operator==( const SVMaterialData& rObject ) const;

	  bool operator!=( const SVMaterialData& rObject ) const;

	protected:
		_variant_t m_Variant;

	};

	typedef std::shared_ptr< SVMaterialData > SVMaterialDataPtr;
} //namespace SvXml


