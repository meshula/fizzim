
// standard libraries
#include <vector>

using std::vector;

// Core includes
#include "TextScanner.h"
#include "ModelReader.h"

ModelReader :: ~ModelReader() {
}

void ModelReader::ReadOBJ(char const*const pSource, int dataSize)
{
	char const* pCurr = pSource;
	char const* pEnd  = pSource + dataSize;

	while (pCurr < pEnd)
	{
		pCurr = TextScanner::ScanForNonWhiteSpace(pCurr, pEnd);

		switch (*pCurr)
		{
			case 'u':		// u is the first character of the usemtl command
			case 'g':		// group
			case '#':		// comment - skip over 'em
			default:		// unknown tokens - skip over 'em
				pCurr = TextScanner::ScanForEndOfLine(pCurr, pEnd);
				pCurr = TextScanner::ScanForNonWhiteSpace(pCurr, pEnd);
				break;

			case 'v':		// vertex
				{
					float x, y, z;

					++pCurr;	// skip past v
					switch (*pCurr)
					{
						case 'n':	// vertex normal
							++pCurr;	// skip n
							pCurr = TextScanner::GetFloat(pCurr, pEnd, x);
							pCurr = TextScanner::GetFloat(pCurr, pEnd, y);
							pCurr = TextScanner::GetFloat(pCurr, pEnd, z);
							normals.push_back(x);
							normals.push_back(y);
							normals.push_back(z);
							break;
						
						case 't':	// vertex texture
							++pCurr;	// skip t
							pCurr = TextScanner::GetFloat(pCurr, pEnd, x);
							pCurr = TextScanner::GetFloat(pCurr, pEnd, y);
							pCurr = TextScanner::GetFloat(pCurr, pEnd, z);
							mapping.push_back(x);
							mapping.push_back(y);
							mapping.push_back(z);
							break;
						
						default:	// vertex position
							pCurr = TextScanner::GetFloat(pCurr, pEnd, x);
							pCurr = TextScanner::GetFloat(pCurr, pEnd, y);
							pCurr = TextScanner::GetFloat(pCurr, pEnd, z);
							vertices.push_back(x);
							vertices.push_back(y);
							vertices.push_back(z);
							break;
					}
				}
				break;

			case 'f':	// face
				{
					++pCurr;	// skip past f
					char const* eol = TextScanner::ScanForEndOfLine(pCurr, pEnd);
					eol = TextScanner::ScanFromEndForLastNonWhiteSpace(pCurr, eol) + 1;
					int v[3];
					int t[3];
					int n[3];
					int index = 0;
					while (pCurr < eol)
					{
						// get index
						pCurr = TextScanner::GetInt(pCurr, eol, v[index]);
						v[index] -= 1;
						
						++pCurr;
						if (*pCurr == '/')	// no tex coord?
						{
							t[index] = -1;
						}
						else
						{
							pCurr = TextScanner::GetInt(pCurr, eol, t[index]);
							t[index] -= 1;
						}
						++pCurr;
						if (*pCurr == '/')	// no normal?
						{
							n[index] = -1;
						}
						else
						{
							pCurr = TextScanner::GetInt(pCurr, eol, n[index]);
							n[index] -= 1;
						}
						
						++index;
						if (index == 3)
						{
							faceIndices.push_back(v[0]);
							faceIndices.push_back(v[1]);
							faceIndices.push_back(v[2]);

							normalIndices.push_back(n[0]);
							normalIndices.push_back(n[1]);
							normalIndices.push_back(n[2]);
							
							uvIndices.push_back(t[0]);
							uvIndices.push_back(t[1]);
							uvIndices.push_back(t[2]);

							// if a polygon was encountered, turn it into triangles
							// by creating a fan
							v[1] = v[2];
							n[1] = n[2];
							t[1] = t[2];
							index = 2;
						}
					}
				}
				break;
				
			case 'o':	// indicates an object name. skip it!
			case 'm':	// material library m is first character of mtllib
				{
					pCurr = TextScanner::ScanForEndOfLine(pCurr, pEnd);	// skip it for the moment
				}
				break;

			// bmf extension t
			case 't':	// beginning a triangle strip
				break;
				
			// bmf extension q
			case 'q':	// continuing a triangle strip
				break;
		}
	}
}

