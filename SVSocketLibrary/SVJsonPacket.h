//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVJsonPacket
//* .File Name       : $Workfile:   SVJsonPacket.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:14:52  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <string>
#pragma endregion Includes

namespace Seidenader { namespace SVSocketLibrary
{
	class SVJsonPacket
	{
	private:
		std::string m_packet;
		std::string m_pending;
		int m_count;

	public:
		SVJsonPacket();
		void Add(const std::string& data);
		bool IsComplete() const;
		bool IsValid() const;
		const std::string& GetJsonData() const;
		void Clear();
		void Reset();
	};
} /*namespace SVSocketLibrary*/ } /*namespace Seidenader*/

namespace SvSol = Seidenader::SVSocketLibrary;
