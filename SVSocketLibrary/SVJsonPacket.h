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

namespace SvSol
{
	class SVJsonPacket
	{
	private:
		std::basic_string<TCHAR> m_packet;
		std::basic_string<TCHAR> m_pending;
		int m_count;

	public:
		SVJsonPacket();
		void Add(const std::basic_string<TCHAR>& data);
		bool IsComplete() const;
		bool IsValid() const;
		const std::basic_string<TCHAR>& GetJsonData() const;
		void Clear();
		void Reset();
	};
} //namespace SvSol
