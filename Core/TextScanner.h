
/*
 __  __           _           _       
|  \/  | ___  ___| |__  _   _| | __ _ 
| |\/| |/ _ \/ __| '_ \| | | | |/ _` |
| |  | |  __/\__ \ | | | |_| | | (_| |
|_|  |_|\___||___/_| |_|\__,_|_|\__,_|
 _____         _   ____                                  
|_   _|____  _| |_/ ___|  ___ __ _ _ __  _ __   ___ _ __ 
  | |/ _ \ \/ / __\___ \ / __/ _` | '_ \| '_ \ / _ \ '__|
  | |  __/>  <| |_ ___) | (_| (_| | | | | | | |  __/ |   
  |_|\___/_/\_\\__|____/ \___\__,_|_| |_|_| |_|\___|_|   
                                                         
This parser was written in the distant past by Nick Porcino for the public domain
and is freely available on an as is basis. It is meant for educational
purposes and is not suitable for any particular purpose. No warranty is expressed
or implied. Use at your own risk. Do not operate heavy machinery or governments
while using this code.

version 1.0 released as part of SuperDuperFramework, 1995
version 1.1 released as COTD on flipcode, 2001, major clean up
version 1.2 released as part of COTD on flipcode, April 2003
			removed C99 types, 
			added setting to treat quoted strings as whitespace
			removed some GetInts

*/

#pragma once

#ifndef TEXTSCANNER_H
#define TEXTSCANNER_H

namespace TextScanner
{
	// Get Token
	char const* GetToken						(char delim, char const* pCurr, char const* pEnd, char const*& resultStringBegin, int& stringLength);
	char const* GetTokenWSDelimited				(char const* pCurr, char const* pEnd, char const*& resultStringBegin, int& stringLength);
	char const* GetTokenAlphaNumeric			(char const* pCurr, char const* pEnd, char const*& resultStringBegin, int& stringLength);
	char const* GetTokenNameSpacedAlphaNumeric	(char const* pCurr, char const* pEnd, char const*& resultStringBegin, int& stringLength);

	// Get Value
	char const* GetString						(char const* pCurr, char const* pEnd, char const*& resultStringBegin, int& stringLength);
	char const* GetInt							(char const* pCurr, char const* pEnd, unsigned int& result);
	char const* GetInt							(char const* pCurr, char const* pEnd, int& result);
	char const* GetHex							(char const* pCurr, char const* pEnd, unsigned int& result);
	char const* GetFloat						(char const* pcurr, char const* pEnd, float& result);

	// Scan forwards
	char const* ScanForCharacter				(char delim, char const* pCurr, char const* pEnd);
	char const* ScanForWhiteSpace				(char const* pCurr, char const* pEnd);
	char const* ScanForNonWhiteSpace			(char const* pCurr, char const* pEnd);
	char const* ScanForQuote					(char const* pCurr, char const* pEnd);
	char const* ScanForEndOfLine				(char const* pCurr, char const* pEnd);
	char const* ScanForLastCharacterOnLine		(char const* pCurr, char const* pEnd);
	char const* ScanForBeginningOfNextLine		(char const* pCurr, char const* pEnd);
	char const* ScanPastCPPComments				(char const* pCurr, char const* pEnd);

	// scan backwards
	char const* ScanBackwardsForCharacter		(char delim, char const* pCurr, char const* pEnd);
	char const* ScanBackwardsForWhiteSpace		(char const* pCurr, char const* pStart);
	char const* ScanFromEndForLastNonWhiteSpace	(char const* pCurr, char const* pEnd);

	// Configuration
	void TreatQuotedStringsAsWhiteSpace			(bool setting);

	// Simple Tests
	template <class Type> inline bool IsWhiteSpace(Type test)	{ return (test == 9 || test == ' ' || test == 13 || test == 10);	}
	template <class Type> inline bool IsNumeric(Type test)		{ return (test >= '0' && test <= '9');	}
	template <class Type> inline bool IsAlpha(Type test)		{ return ((test >= 'a' && test <= 'z') || (test >= 'A' && test <= 'Z'));	}
}

#endif
