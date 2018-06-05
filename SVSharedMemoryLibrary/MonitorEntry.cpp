#pragma region Includes
#include "StdAfx.h"
#include "MonitorEntry.h"
#include "Definitions/SVObjectTypeInfoStruct.h"
#include "SVMatroxLibrary\MatroxImageProps.h"

#include "SVProtoBuf\ConverterHelper.h"

#pragma endregion Includes

namespace SvSml
{
	
	bool MonitorEntryData::GetMatroxImageProps(MatroxImageProps& rImageProps) const
	{
		if (ObjectType != SvDef::SVObjectTypeEnum::SVImageObjectType)
		{
			return false;
		}
		rImageProps.Pitch = Pitch;
		rImageProps.sizeX = sizeX;
		rImageProps.sizeY = sizeY;
		rImageProps.PitchByte = PitchByte;
		rImageProps.Matrox_type = Matrox_type;
		rImageProps.Attrib = Attrib;
		rImageProps.Bandsize = BandSize;
		rImageProps.Bytesize = ByteSize;
		return true;
	}
	void MonitorEntryData::SetMatroxImageProps(const MatroxImageProps& rImageProps)
	{
		ObjectType = SvDef::SVImageObjectType;
		Pitch = rImageProps.Pitch;
		sizeX = rImageProps.sizeX;
		sizeY = rImageProps.sizeY;
		PitchByte = rImageProps.PitchByte;
		Matrox_type = rImageProps.Matrox_type;
		Attrib = rImageProps.Attrib;
		ByteSize = rImageProps.Bytesize;
		BandSize = rImageProps.Bandsize;
	}
	void MonitorEntryData::BuildProtoMessage(SvPml::MesMonitorEntryData& rProtoMessage) const
	{
		rProtoMessage.set_inspectionstoreid(InspectionStoreId);
		rProtoMessage.set_itemid(ItemId);
		rProtoMessage.set_objecttype(ObjectType);
		rProtoMessage.set_variant_type(variant_type);
		rProtoMessage.set_store_offset(Store_Offset);
		rProtoMessage.set_isarray(isArray);
		rProtoMessage.set_wholearray(wholeArray);
		rProtoMessage.set_arrayindex(arrayIndex);
		rProtoMessage.set_monitorlistflag(m_MonitorListFlag);
		rProtoMessage.set_sizey(sizeY);
		rProtoMessage.set_sizex(sizeX);
		rProtoMessage.set_pitchbyte(PitchByte);
		rProtoMessage.set_pitch(Pitch);
		rProtoMessage.set_matrox_type(Matrox_type);
		rProtoMessage.set_attrib(Attrib);
		rProtoMessage.set_bandsize(BandSize);
		rProtoMessage.set_bytesize(ByteSize);
	}

	void MonitorEntryData::BuildFromProtoMessage(const SvPml::MesMonitorEntryData& rProtoMessage)
	{

		InspectionStoreId = rProtoMessage.inspectionstoreid();
		ItemId = rProtoMessage.itemid();
		ObjectType = rProtoMessage.objecttype();
		variant_type = rProtoMessage.variant_type();
		Store_Offset = rProtoMessage.store_offset();
		isArray = rProtoMessage.isarray();
		arrayIndex = rProtoMessage.arrayindex();
		wholeArray = rProtoMessage.wholearray();
		m_MonitorListFlag = rProtoMessage.monitorlistflag();
		sizeY = rProtoMessage.sizey();
		sizeX = rProtoMessage.sizex();
		PitchByte = rProtoMessage.pitchbyte();
		Pitch = rProtoMessage.pitch();
		Matrox_type = rProtoMessage.matrox_type();
		Attrib = rProtoMessage.attrib();
		BandSize = rProtoMessage.bandsize();
		ByteSize = rProtoMessage.bytesize();
	}

	MonitorEntry::MonitorEntry() :data(), m_Guid(GUID_NULL)
	{
	};

	MonitorEntry::MonitorEntry(const std::string& na) :data(), m_Guid(GUID_NULL)
	{
		name = na;
	};

	bool MonitorEntry::GetMatroxImageProps(MatroxImageProps &ImageProps) const
	{
		return data.GetMatroxImageProps(ImageProps);
	}

	void MonitorEntry::SetMatroxImageProps(const MatroxImageProps  &rImageProps) 
	{
		 data.SetMatroxImageProps(rImageProps);
	}

	bool MonitorEntry::GetValue(std::string& string, BYTE* ptr) const 
	{
		TCHAR Text[100];
		switch (data.variant_type)
		{
		case VT_I1: //CHAR
		{
			assert(data.ByteSize == 1);
			char* pValue = (char*)ptr;
			sprintf_s(Text, 100, _T("%c"), *pValue);
			string = Text;
			break;
		}
		case VT_UI1: //BYTE
		{
			assert(data.ByteSize == 1);
			BYTE value = *((BYTE*)ptr);
			sprintf_s(Text, 100, _T("0x%2.2x"), value);
			string = Text;
			break;
		}
		case VT_I2:
		{
			short* pShort = (short*)ptr;
			assert(data.ByteSize == 2);
			short   value = *pShort;
			string = std::to_string(value);
			break;
		}
		case VT_UI2:
		{
			assert(data.ByteSize == 2);
			WORD*   pWord = (WORD*)(ptr);
			sprintf_s(Text, 100, _T("0x%4.4x"), *pWord);
			string = Text;
			break;
		}
		case VT_I4:
		{
			int* pInt = (int*)ptr;
			assert(data.ByteSize == 4);
			INT32   value = *pInt;
			string = std::to_string(value);
			break;
		}
		case VT_UI4:
		{
			assert(data.ByteSize == 4);
			DWORD*   pDWord = (DWORD*)(ptr);
			sprintf_s(Text, 100, _T("0x%8.8x"), *pDWord);
			string = Text;
			break;
		}
		case VT_R4:
		{
			assert(data.ByteSize == 4);
			float* pfloat = (float*)ptr;
			string = std::to_string(*pfloat);
			break;
		}
		
		case VT_R8:
		{
			assert(data.ByteSize == 8);
			double* pDouble = (double*)ptr;
			string = std::to_string(*pDouble);
			break;
		}
		case VT_BSTR:
		{
			string.assign((char*)ptr, data.ByteSize - 1);
			break;
		}

		case VT_I8:
		{
			assert(data.ByteSize == 8);
			INT64*  pvalue = (INT64*)(ptr);
			sprintf_s(Text, 100, _T("%I64d"), *pvalue);
			string = Text;
			break;

		}
		case VT_UI8:
		{
			assert(data.ByteSize == 8);
			UINT64* pvalue = (UINT64*)(ptr);
			sprintf_s(Text, 100, _T("%I64d"), *pvalue);
			string = Text;
			break;
		}

		case VT_INT:
		{
			// BasicvalueObject converts VT_INT to VT_BOOL
			assert(data.ByteSize == 4);
			int*  pValue = (int*)(ptr);
			if (*pValue)
				string = "TRUE";
			else
				string = "FALSE";
			break;
		}
		case VT_UINT:
		{
			assert(data.ByteSize == 4);
			DWORD*   pDWord = (DWORD*)(ptr);
			sprintf_s(Text, 100, _T("0x%8.8x"), *pDWord);
			string = Text;
			break;
		}
		
		case VT_BOOL:
		{
			assert(data.ByteSize == 4);
			int*  pValue = (int*)(ptr);
			if (*pValue)
				string = "TRUE";
			else
				string = "FALSE";
			break;
		}
		case SvDef::VT_DPOINT:
		{
			assert(data.ByteSize == 16);
			double* px = (double*)(ptr);
			double* py = (double*)(ptr + 8);
			sprintf_s(Text, 100, _T("( %lf, %lf)"), *px, *py);
			string = Text;
			break;
		}
		case SvDef::VT_POINT:
		{
			assert(data.ByteSize == 8);
			int* px = (int*)(ptr);
			int* py = (int*)(ptr + 4);

			sprintf_s(Text, 100, _T("( %i, %i)"), *px, *py);
			string = Text;
			break;
		}

		default:
			string = "???";
			break;
		}
		return true;
	}
	bool MonitorEntry::GetValue(_variant_t& val, BYTE* offset) const
	{
		//@Todo[MEC][7.50] [17.07.2017] not implemented yet
		return false;
	}

	
	
	void  MonitorEntry::BuildProtoMessage(SvPml::MesMonitorEntry& rMesMonitorEntry) const
	{
		SvPb::SetGuidInProtoBytes(rMesMonitorEntry.mutable_guid(), m_Guid);
		rMesMonitorEntry.set_name(name.c_str());
		auto pEntryDataMessage = rMesMonitorEntry.mutable_entrydata();
		data.BuildProtoMessage(*pEntryDataMessage);
	}

	
	void MonitorEntry::BuildFromProtoMessage(const SvPml::MesMonitorEntry& rMesMonitorEntry)
	{
		SvPb::GetGuidFromProtoBytes(rMesMonitorEntry.guid(), m_Guid);
		name = rMesMonitorEntry.name();
		data.BuildFromProtoMessage(rMesMonitorEntry.entrydata());
	}

	void MonitorEntry::AddListItem(SvPb::QueryListItemResponse& resp) const
	{
		if (IsImage())
		{
			auto pImageDef = resp.add_imagedeflist();
			pImageDef->set_name(name.c_str());
			pImageDef->set_width(static_cast<INT32>(data.sizeX));
			pImageDef->set_height(static_cast<INT32>(data.sizeY));
			pImageDef->set_storeid(data.InspectionStoreId);
			pImageDef->set_imageid(data.ItemId);
		}
		else
		{
			auto pValueDef = resp.add_valuedeflist();
			pValueDef->set_name(name.c_str());
			pValueDef->set_type(data.variant_type);
			pValueDef->set_size (static_cast<INT32>(data.ByteSize));
			pValueDef->set_storeid (data.InspectionStoreId);
			pValueDef->set_offset(data.Store_Offset);
		}

	}

}
