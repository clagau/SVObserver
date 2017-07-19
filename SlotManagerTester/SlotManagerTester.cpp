//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file SlotManagerTester.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Defines the entry point for the console application.
//******************************************************************************


#include "stdafx.h"
#include <memory.h>
#include "SVSharedMemoryLibrary\SMRingbuffer.h"


void Init( std::set<int> &prim, int max)
{
	std::vector<bool>  Sieb;
	Sieb.reserve(max +1);
	for (int i = 0; i <= max; i++)
	{
		Sieb.push_back(TRUE);
	}
	
	for (int div = 2; div < max -1; )
	{
		for (int n = 2 * div; n <= max; n += div)
		{
			Sieb[n] = FALSE;
		}
		while (div < max -1 )
		{
			if (Sieb[++div] == TRUE)
				break;
		}
	}
	for (int i = 1; i < max; i++)
	{
		if (Sieb[i])
		{
			prim.insert(i);
		}
			
	}
}


bool isPrim(int number)
{
	static std::set<int> primSet;
	static bool isinit = false;
	if (!isinit)
	{
		Init(primSet, 1000);
	}
	if (primSet.find(number) != primSet.end())
	{
		return true;
	}
	return false;
}

void WaitForInput()
{
	int in;
	do
	{
		in = getchar();
	} while (in != '\n');

}


bool  DisplayTrigger(SvSml::SMRingBuffer &Ringbuffer, SvSml::SMRingBuffer::SlotType type)
{

	int readerslot  = Ringbuffer.GetReaderSlotLastWritten(type);
	if ( readerslot <0 )
	{
		std::cout << "FEHLER: no ReaderSlot:" << std::endl;
		return false;
	}
	DWORD TriggerNumber = Ringbuffer.GetTriggerNumber(readerslot);

	std::cout << "Trigger: " << TriggerNumber  << std::endl;
	Ringbuffer.ReleaseReaderSlot(readerslot);
	return true;
}

size_t DisplayAllRejects(SvSml::SMRingBuffer &Ringbuffer)
{
	std::vector<DWORD> rejects;
	Ringbuffer.GetRejects(rejects);
	for (const auto &j : rejects)
	{
		std::cout << j << " ";

	}
	std::cout << std::endl;
	return rejects.size();
}

int main(int argc, char* argv[])
{
	SVString name;
	bool bServer(false);
	int nRejects(20), nTotal(40),ClientThreads(1);
	int delay(20);
	const char* Usage = "USAGE: server Name  Rejects  nTotal  delay \nUSAGE: client Name   delay";
	if (argc > 2 && strcmp("server", argv[1]) == 0)
	{

		bServer = true;
	}
	else if (argc > 2 && strcmp("client", argv[1]) == 0)
	{
		bServer = false;
	}
	else
	{
		std::cout << Usage;
		WaitForInput();
		return 0;
	}
		
	name = argv[2];

	if (bServer)
	{
		if (argc > 3)
		{
			nRejects = atoi(argv[3]);
		}
		if (argc > 4)
		{
			nTotal = atoi(argv[4]);
			nTotal = max(nTotal, nRejects + 20);
		}
		if (argc > 5)
		{
			delay = atoi(argv[5]);
		}
	}
	else 
	{
		if (argc > 3)
		{
			delay = atoi(argv[3]);
		}
	}

	if (bServer)
	{
		DWORD failer(0);
		SvSml::SMRingBuffer Ringbufffer;
		try
		{
			Ringbufffer.CreateConnection(name.c_str(), nTotal,nRejects );
			for (UINT Triggercount = 0;  Triggercount < MAXDWORD; Triggercount++)
			{
				int  wslot = Ringbufffer.GetNextWriteSlot();
				if ( wslot <0 )
				{
					failer++; 
					std::cout << "No Write Slot for Trigger: " << Triggercount << "ErrorCount: " << failer << std::endl;
 					continue;
				}
				bool isReject = (Triggercount < 1000) &&  isPrim(Triggercount);
				isReject = isReject || Triggercount == 1234;
				if (isReject)
				{
					Ringbufffer.SetToReject(wslot);
				}
				Ringbufffer.ReleaseWriteSlot(wslot, Triggercount, TRUE);
				Sleep(delay);
			}
		}

		catch (std::exception& e)
		{
			std::cout << "An Exception occured: " << e.what();
			WaitForInput();
		}

	}//server 
	else //client 
	{
		SvSml::SMRingBuffer RingbuffferClient;
		try
		{
			DWORD failer(0);
			RingbuffferClient.OpenConnection(name.c_str());
			std::cout << "L last Trigger" << std::endl;
			std::cout << "R last Reject" << std::endl;
			std::cout << "A  Reject list" << std::endl;
			std::cout << "P XX  del Poll last trigger" << std::endl;
			std::cout << "Q XX  del Poll last Reject" << std::endl;
			std::cout << "X  quit" << std::endl;
			bool cont(true);
			while (cont)
			{
				int loop = 100;
				int in = getchar();
				switch (in)
				{
				case 'L':
				case 'l':
					DisplayTrigger(RingbuffferClient, SvSml::SMRingBuffer::Last );
					break;
				case 'r':
				case 'R':
					DisplayTrigger(RingbuffferClient, SvSml::SMRingBuffer::Reject);
					break;
				case 'p':
				case 'P':
					loop = 100;
					std::cin >> loop;
					std::cin >> delay;
					for (int i = 0; i < loop; i++)
					{
						DisplayTrigger(RingbuffferClient, SvSml::SMRingBuffer::Last);
						Sleep(delay);
					}

					break;
				case 'q':
				case 'Q':
					 loop = 100;
					std::cin >> loop;
					std::cin >> delay;
					for (int i = 0; i < loop; i++)
					{
						DisplayTrigger(RingbuffferClient, SvSml::SMRingBuffer::Reject);
						Sleep(delay);
					}
					break;
				
				case 'A':
				case 'a':
					DisplayAllRejects(RingbuffferClient);
					break;
				
				case 'x':
				case 'X':
					cont = false;
					break;
				default:
					break;

				}
			}
		
		}
		catch (std::exception& e)
		{
			std::cout << "An Exception occured: " << e.what();
		}

	}
	return 0;

}

