
/*
This parser was written in the distant past by Nick Porcino for the public domain
and is freely available on an as is basis. It is meant for educational
purposes and is not suitable for any particular purpose. No warranty is expressed
or implied. Use at your own risk. Do not operate heavy machinery or governments
while using this code.
*/

#include <cmath>
#include "TextScanner.h"

//! @todo replace Assert with proper error reporting mechanism

#include <assert.h>
#define Assert assert

/*----------------------------------------------------------------------------

*/

static bool sbTreatQuotedStringsAsWhiteSpace = false;

void TreatQuotedStringsAsWhiteSpace	(bool setting)
{
	sbTreatQuotedStringsAsWhiteSpace = setting;
}

char const* TextScanner::ScanForQuote(char const* pCurr, char const* pEnd)
{
	Assert(pCurr && pEnd && pEnd >= pCurr);
	
	while (pCurr < pEnd)
	{
		if (*pCurr == '\"')
			break;
				
		++pCurr;
	}

	return pCurr;
}

char const* TextScanner::ScanForWhiteSpace(char const* pCurr, char const* pEnd)
{
	Assert(pCurr && pEnd && pEnd >= pCurr);
	
	while (pCurr < pEnd)
	{
		if (sbTreatQuotedStringsAsWhiteSpace) {
			if (*pCurr == '\"') {
				pCurr = ScanForQuote(pCurr + 1, pEnd);
			}
		}

		if (TextScanner::IsWhiteSpace(*pCurr))
			break;
				
		++pCurr;
	}

	return pCurr;
}

char const* TextScanner::ScanForNonWhiteSpace(char const* pCurr, char const* pEnd)
{
	Assert(pCurr && pEnd && pEnd >= pCurr);
	
	while (pCurr < pEnd)
	{
		if (sbTreatQuotedStringsAsWhiteSpace) {
			if (*pCurr == '\"') {
				pCurr = ScanForQuote(pCurr + 1, pEnd);
			}
		}

		if (!TextScanner::IsWhiteSpace(*pCurr))
			break;
				
		++pCurr;
	}

	return pCurr;
}

char const* TextScanner::ScanBackwardsForWhiteSpace(char const* pCurr, char const* pStart)
{
	Assert(pCurr && pStart && pStart <= pCurr);
	Assert(sbTreatQuotedStringsAsWhiteSpace == false);	// scanning backwards over quoted strings not implemented
	
	while (pCurr >= pStart)
	{
		if (TextScanner::IsWhiteSpace(*pCurr))
			break;
				
		--pCurr;
	}

	return pCurr;
}

char const* TextScanner::ScanFromEndForLastNonWhiteSpace(char const* pCurr, char const* pEnd)
{
	Assert(pCurr && pEnd && pEnd >= pCurr);
	Assert(sbTreatQuotedStringsAsWhiteSpace == false);	// scanning backwards over quoted strings not implemented
	
	while (pCurr < pEnd)
	{
		if (!TextScanner::IsWhiteSpace(*pEnd))
			break;

		--pEnd;
	}

	return pEnd;
}

char const* TextScanner::ScanForCharacter(char delim, char const* pCurr, char const* pEnd)
{
	Assert(pCurr && pEnd);

	while (pCurr < pEnd)
	{
		if (sbTreatQuotedStringsAsWhiteSpace) {
			if (*pCurr == '\"') {
				pCurr = ScanForQuote(pCurr + 1, pEnd);
			}
		}

		if (*pCurr == delim)
			break;

		++pCurr;
	}
	return pCurr;
}

char const* TextScanner::ScanBackwardsForCharacter(char delim, char const* pCurr, char const* pStart)
{
	Assert(pCurr && pStart && pStart <= pCurr);
	Assert(sbTreatQuotedStringsAsWhiteSpace == false);	// scanning backwards over quoted strings not implemented
	
	while (pCurr >= pStart)
	{
		if (*pCurr == delim)
			break;
				
		--pCurr;
	}

	return pCurr;
}

char const* TextScanner::GetToken(char delim, char const* pCurr, char const* pEnd, char const*& resultStringBegin, int& stringLength)
{
	Assert(pCurr && pEnd);

	pCurr = ScanForNonWhiteSpace(pCurr, pEnd);
	resultStringBegin = pCurr;

	char const* pStringEnd = ScanForCharacter(delim, pCurr, pEnd);
	
	stringLength = (int)(pStringEnd - resultStringBegin);
	
	return pCurr;
}

char const* TextScanner::GetTokenAlphaNumeric(char const* pCurr, char const* pEnd, char const*& resultStringBegin, int& stringLength)
{
	Assert(pCurr && pEnd);

	pCurr = ScanForNonWhiteSpace(pCurr, pEnd);
	resultStringBegin = pCurr;
	stringLength = 0;

	while (pCurr < pEnd)
	{
		char test = pCurr[0];

		if (IsWhiteSpace(test))
			break;
			
		bool accept = ((test == '_') || TextScanner::IsNumeric(test) || TextScanner::IsAlpha(test));

		if (!accept)
			break;
			
		++pCurr;
		++stringLength;
	}

	return pCurr;
}

// same as above except token may also contain colons.

char const* TextScanner::GetTokenNameSpacedAlphaNumeric	(char const* pCurr, char const* pEnd, char const*& resultStringBegin, int& stringLength)
{
	Assert(pCurr && pEnd);

	pCurr = ScanForNonWhiteSpace(pCurr, pEnd);
	resultStringBegin = pCurr;
	stringLength = 0;

	while (pCurr < pEnd)
	{
		char test = pCurr[0];

		if (IsWhiteSpace(test))
			break;
			
		bool accept = ((test == ':') || (test == '_') || TextScanner::IsNumeric(test) || TextScanner::IsAlpha(test));

		if (!accept)
			break;
			
		++pCurr;
		++stringLength;
	}

	return pCurr;
}

char const* TextScanner::GetTokenWSDelimited(char const* pCurr, char const* pEnd, char const*& resultStringBegin, int& stringLength)
{
	Assert(pCurr && pEnd);

	pCurr = ScanForNonWhiteSpace(pCurr, pEnd);
	resultStringBegin = pCurr;

	char const* pStringEnd = ScanForWhiteSpace(pCurr, pEnd);
	
	stringLength = (int)(pStringEnd - resultStringBegin);
	
	return pStringEnd;
}

char const* TextScanner::GetString(char const* pCurr, char const* pEnd, char const*& resultStringBegin, int& stringLength)
{
	Assert(pCurr && pEnd && pEnd >= pCurr);
	Assert(sbTreatQuotedStringsAsWhiteSpace == false);	// can't be suppressing quoted strings!

	pCurr = ScanForQuote(pCurr, pEnd);
	
	if (pCurr < pEnd)
	{
		++pCurr;	// skip past quote
		resultStringBegin = pCurr;

		pCurr = ScanForQuote(pCurr, pEnd);

		if (pCurr <= pEnd)
		{
			stringLength = (int)(pCurr - resultStringBegin);
		}
		else
			stringLength = 0;

		++pCurr;	// point past closing quote
	}
	else
		stringLength = 0;

	return pCurr;
}


char const* TextScanner::ScanForEndOfLine(char const* pCurr, char const* pEnd)
{
	while (pCurr < pEnd)
	{
		if (*pCurr == '\r')
		{
			if (pCurr[1] == '\n')
				++pCurr;
			break;
		}
		if (*pCurr == '\n')
		{
			if (pCurr[1] == '\r')
				++pCurr;
			break;
		}

		++pCurr;
	}
	return pCurr;
}

char const* TextScanner::ScanForLastCharacterOnLine(char const* pCurr, char const* pEnd)
{
	while (pCurr < pEnd)
	{
		if (pCurr[1] == '\r' || pCurr[1] == '\n' || pCurr[1] == '\0')
		{
			break;
		}

		++pCurr;
	}
	return pCurr;
}


char const* TextScanner::GetInt(char const* pCurr, char const* pEnd, int& result)
{
	pCurr = ScanForNonWhiteSpace(pCurr, pEnd);

	result = 0;

	bool signFlip = false;
	
	if (*pCurr == '+')
	{
		++pCurr;
	}
	else
	if (*pCurr == '-')
	{
		++pCurr;
		signFlip = true;
	}
	
	while (pCurr < pEnd)
	{
		if (!TextScanner::IsNumeric(*pCurr))
		{
			break;
		}
		result = result * 10 + *pCurr - '0';
		++pCurr;
	}

	if (signFlip)
	{
		result = - result;
	}
	return pCurr;
}

char const* TextScanner::GetInt(char const* pCurr, char const* pEnd, unsigned int& result)
{
	pCurr = ScanForNonWhiteSpace(pCurr, pEnd);

	result = 0;

	while (pCurr < pEnd)
	{
		if (!TextScanner::IsNumeric(*pCurr))
		{
			break;
		}
		result = result * 10 + *pCurr - '0';
		++pCurr;
	}
	return pCurr;
}

char const* TextScanner::GetFloat(char const* pCurr, char const* pEnd, float& result)
{
	pCurr = ScanForNonWhiteSpace(pCurr, pEnd);

	result = float(0.0);

	bool signFlip = false;
	
	if (*pCurr == '+')
	{
		++pCurr;
	}
	else
	if (*pCurr == '-')
	{
		++pCurr;
		signFlip = true;
	}

	// get integer part
	int intPart;
	pCurr = GetInt(pCurr, pEnd, intPart);
	result = (float) intPart;

	// get fractional part
	if (*pCurr == '.')
	{
		++pCurr;

		float scaler = 0.1f;
		while (pCurr < pEnd)
		{
			if (!TextScanner::IsNumeric(*pCurr))
			{
				break;
			}
			result = result + (float(*pCurr - '0') * scaler);
			++pCurr;
			scaler *= 0.1f;
		}
	}

	// get exponent
	if (*pCurr == 'e' || *pCurr == 'E')
	{
		++pCurr;
		
		pCurr = GetInt(pCurr, pEnd, intPart);
		result *= std::powf(10.0, (float) intPart);
	}

	if (signFlip)
	{
		result = - result;
	}
	return pCurr;
}


char const* TextScanner::GetHex(char const* pCurr, char const* pEnd, unsigned int& result)
{
	pCurr = ScanForNonWhiteSpace(pCurr, pEnd);

	result = 0;
	
	while (pCurr < pEnd)
	{
		if (TextScanner::IsNumeric(*pCurr))
		{
			result = result * 16 + *pCurr - '0';
		}
		else if (*pCurr >= 'A' && *pCurr <= 'F')
		{
			result = result * 16 + *pCurr - 'A' + 10;
		}
		else if (*pCurr >= 'a' && *pCurr <= 'f')
		{
			result = result * 16 + *pCurr - 'a' + 10;
		}
		else
		{
			break;
		}
		++pCurr;
	}

	return pCurr;
}

char const* TextScanner::ScanForBeginningOfNextLine(char const* pCurr, char const* pEnd)
{
	pCurr = ScanForEndOfLine(pCurr, pEnd);
	return (ScanForNonWhiteSpace(pCurr, pEnd));
}


char const* TextScanner::ScanPastCPPComments(char const* pCurr, char const* pEnd)
{
	if (*pCurr == '/')
	{
		if (pCurr[1] == '/')
		{
			pCurr = ScanForEndOfLine(pCurr, pEnd);
		}
		else if (pCurr[1] == '*')
		{
			pCurr = &pCurr[2];
			while (pCurr < pEnd)
			{
				if (pCurr[0] == '*' && pCurr[1] == '/')
				{
					pCurr = &pCurr[2];
					break;
				}
				
				++pCurr;
			}
		}
	}
	
	return pCurr;
}
