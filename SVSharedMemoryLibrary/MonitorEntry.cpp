#pragma region Includes
#include "StdAfx.h"
#include "MonitorEntry.h"
#include "Definitions/SVObjectTypeInfoStruct.h"
#include "SVMatroxLibrary\MatroxImageProps.h"

#include "SVProtoBuf\BasicStructureHelper.h"

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
	void MonitorEntryData::SetMatroxImageProps(const MatroxImageProps  &rImageProps)
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
	void MonitorEntryData::BuildProtoMessage(MesMonitorEntryData &rprotoMessage) const
	{
		rprotoMessage.set_inspectionstoreid(InspectionStoreId);
		rprotoMessage.set_itemid(ItemId);
		rprotoMessage.set_objecttype(ObjectType);
		rprotoMessage.set_variant_type(variant_type);
		rprotoMessage.set_store_offset(Store_Offset);
		rprotoMessage.set_isarray(isArray);
		rprotoMessage.set_wholearray(wholeArray);
		rprotoMessage.set_arrayindex(arrayIndex);
		rprotoMessage.set_monitorlistflag(m_MonitorListFlag);
		rprotoMessage.set_sizey(sizeY);
		rprotoMessage.set_sizex(sizeX);
		rprotoMessage.set_pitchbyte(PitchByte);
		rprotoMessage.set_pitch(Pitch);
		rprotoMessage.set_matrox_type(Matrox_type);
		rprotoMessage.set_attrib(Attrib);
		rprotoMessage.set_bandsize(BandSize);
		rprotoMessage.set_bytesize(ByteSize);
	}

	void MonitorEntryData::BuildFromProtoMessage(const MesMonitorEntryData& rprotoMessage)
	{

		InspectionStoreId = rprotoMessage.inspectionstoreid();
		ItemId = rprotoMessage.itemid();
		ObjectType = rprotoMessage.objecttype();
		variant_type = rprotoMessage.variant_type();
		Store_Offset = rprotoMessage.store_offset();
		isArray = rprotoMessage.isarray();
		arrayIndex = rprotoMessage.arrayindex();
		wholeArray = rprotoMessage.wholearray();
		m_MonitorListFlag = rprotoMessage.monitorlistflag();
		sizeY = rprotoMessage.sizey();
		sizeX = rprotoMessage.sizex();
		PitchByte = rprotoMessage.pitchbyte();
		Pitch = rprotoMessage.pitch();
		Matrox_type = rprotoMessage.matrox_type();
		Attrib = rprotoMessage.attrib();
		BandSize = rprotoMessage.bandsize();
		ByteSize = rprotoMessage.bytesize();
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

	
	
	void  MonitorEntry::BuildProtoMessage(MesMonitorEntry& rmesMonitorEntry) const
	{
		SvPB::SetGuidInProtoBytes(rmesMonitorEntry.mutable_guid(), m_Guid);
		rmesMonitorEntry.set_name(name.c_str());
		auto pEntryDataMessage = rmesMonitorEntry.mutable_entrydata();
		data.BuildProtoMessage(*pEntryDataMessage);
	}

	
	void MonitorEntry::BuildFromProtoMessage(const MesMonitorEntry& rmesMonitorEntry)
	{
		SvPB::GetGuidFromProtoBytes(rmesMonitorEntry.guid(), m_Guid);
		name = rmesMonitorEntry.name();
		data.BuildFromProtoMessage(rmesMonitorEntry.entrydata());
	}

	void MonitorEntry::AddListItem(RRWS::QueryListItemResponse& resp) const
	{
		if (IsImage())
		{
			auto pImDef = resp.add_imagedefinition();
			pImDef->set_name(name.c_str());
			pImDef->set_sizex(static_cast<INT32>(data.sizeX));
			pImDef->set_sizey(static_cast<INT32>(data.sizeY));
			pImDef->set_storeid(data.InspectionStoreId);
			pImDef->set_imageid(data.ItemId);
		}
		else
		{
			auto pValueDef = resp.add_valuedefinition();
			pValueDef->set_name(name.c_str());
			pValueDef->set_type(data.variant_type);
			pValueDef->set_size (static_cast<INT32>(data.ByteSize));
			pValueDef->set_storeid (data.InspectionStoreId);
			pValueDef->set_offset(data.Store_Offset);
		}

	}

}
