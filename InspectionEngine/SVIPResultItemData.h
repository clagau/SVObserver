//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIPResultItemData
//* .File Name       : $Workfile:   SVIPResultItemData.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:16:16  $
//******************************************************************************
#pragma once

#pragma region Includes
#pragma endregion Includes

namespace SvIe
{

class SVIPResultItemData
{
public:
	SVIPResultItemData();
	SVIPResultItemData( const std::string& p_rValue, unsigned long p_Color );
	SVIPResultItemData( const std::string& p_rValue, unsigned long p_Color, unsigned long p_IOType );
	SVIPResultItemData( const SVIPResultItemData& p_rObject );

	const std::string& GetValue() const;
	unsigned long GetColor() const;
	bool IsIOTypePresent() const;
	unsigned long GetIOType() const;

protected:
	std::string m_Value;
	unsigned long m_Color;

	bool m_IOTypePresent;
	long m_IOType;
};

} //namespace SvIe
