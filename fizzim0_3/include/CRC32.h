//CRC32.h

#ifndef CRC32_H
#define CRC32_H

// derived from http://rossm.net/Electronics/Computers/Software/C++/Algorithms/

/*
CRC is a "digital fingerprint" of data.
With CRC32 you can get a single 32-bit number that represents a string or file (of any size).
If the data changes in any way (even a single bit) the CRC32 calculation would yield a completely different value.
This algorithm is used by WinZip and PKZIP (ie the same numbers are generated) so:
pkunzip.exe -vt File.zip gives the same CRC value as you get by giving the file to this code.

A single 1kB table is allocated while instances of CCRC32 exist.

Usage:
  int CRC=CCRC32().From(Buffer,Size);

or use one instance for several CRCs:
  CCRC32 CRC32;
  int CRC1=CRC32.From(Buffer1,Size1);
  int CRC2=CRC32.From(Buffer2,Size2);

But since we want the fastest routines we should use CreateFile
and thats what the class uses internally for retrieving the CRC32 of a file:
  int CRC=CCRC32().From("C:\\CheckMe.txt");

Test CRC numbers:
resume = 60C1D0A0
resumé = 84CF1FAB
*/

class CRC32 
{
  static uint32*	Tbl;
  static int		Instances;
  
  uint32			LastCRC;

public:
	CRC32() 
	{
		if(Tbl) 
			++Instances;
		else 
		{
			LastCRC=-1;
			Instances=0;
			Tbl=new uint32[256];
			if(!Tbl) return;
			Instances=1;
			uint32* ptr=Tbl;
			for(uint32 n=0; n<256; ++n){
				uint32 c=n;
				for(int k=0; k<8; ++k) c=(c&1 ? 0xEDB88320^(c>>1) : c>>1);
				*ptr++=c;
			} 
		}
	}

	virtual ~CRC32() 
	{
		if(Instances) --Instances;
		if(Instances) return;
		if(Tbl) delete[] Tbl;
		Tbl=0;
	}

	/* This is the following method without the optimised looping:
	  uint32 From(const char* Buffer, uint32 Len, bool Partial=false) {
		if(!Tbl || !Buffer) return 0;
		register uint32 CRC=(Partial ? LastCRC : LastCRC=-1);
		while(Len--) CRC=(CRC>>8)^Tbl[(CRC^(*Buffer++))&0xFF];
		return LastCRC=~CRC;
	  }
	*/

	uint32 Calculate(char const* Buffer, uint32 Len, bool Partial=false) 
	{
		if(!Tbl || !Buffer) 
			return 0;

		register uint32 CRC = (Partial ? LastCRC : LastCRC=-1);
		uint32 ModLen=Len&7;
		Len>>=3;
		while(Len--) 
		{
			CRC = (CRC>>8)^Tbl[(CRC^(*Buffer++))&0xFF];
			CRC = (CRC>>8)^Tbl[(CRC^(*Buffer++))&0xFF];
			CRC = (CRC>>8)^Tbl[(CRC^(*Buffer++))&0xFF];
			CRC = (CRC>>8)^Tbl[(CRC^(*Buffer++))&0xFF];
			CRC = (CRC>>8)^Tbl[(CRC^(*Buffer++))&0xFF];
			CRC = (CRC>>8)^Tbl[(CRC^(*Buffer++))&0xFF];
			CRC = (CRC>>8)^Tbl[(CRC^(*Buffer++))&0xFF];
			CRC = (CRC>>8)^Tbl[(CRC^(*Buffer++))&0xFF];
		}

		while(ModLen--) 
			CRC=(CRC>>8)^Tbl[(CRC^(*Buffer++))&0xFF];

		return LastCRC=~CRC;
	}
};

uint32* CRC32::Tbl=0;       // Table of CRC values for every possible BYTE
int     CRC32::Instances=0; // Count of references to Tbl (Instances of CCRC32)

#endif
