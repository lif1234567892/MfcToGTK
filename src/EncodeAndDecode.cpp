#ifdef _WIN32
#include "stdafx.h"
#endif//_WIN32

#include "EncodeAndDecode.h"
#include <new>
#include <ctype.h>
#include <string.h>

using namespace std;

int Base64_Decode(char *Inbuf, char *Outbuf, int nInLen, int nOutLen)
{
	int nbytesdecoded;
	char *pBufin;
	char *pszBufout = Outbuf;
	int nprbytes;
	unsigned int dwCoded = nInLen;
	char *pszCoded = Inbuf;
	char *pszDecoded = Outbuf;

	const int pr2six[256] =	{
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
		52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64, 64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
		10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64, 64, 26, 27,
		28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
	};

	char* szBufTmp = new(nothrow) char[nInLen+4];	// 后面4个字节补齐0，用来简化操作
	if(!szBufTmp)
		return -1;	// 内存不足
	//
	// Remove leading whitespace.
	//
	while((dwCoded > 0) && (isspace(*pszCoded)))
	{
		pszCoded++; dwCoded--;
	}


	strncpy(szBufTmp, pszCoded, dwCoded);
	szBufTmp[dwCoded] = szBufTmp[dwCoded + 1] = szBufTmp[dwCoded + 2]
	= szBufTmp[dwCoded + 3] = 0;

	//
	// Determine how many characters are in the input buffer.
	// If this would decode into more bytes than would fit into
	// the output buffer, return 2
	//

	pBufin = szBufTmp;
	while(pr2six[ *(pBufin++) ] <= 63);
	nprbytes = pBufin - szBufTmp - 1;
	nbytesdecoded = ((nprbytes + 3) / 4) * 3;

	if(nOutLen < nbytesdecoded){
		delete []szBufTmp;
		return -1;
	}

	pBufin = szBufTmp;

	while(nprbytes > 0){
		*(pszBufout++) =
			(unsigned char)(pr2six[*pBufin] << 2 | pr2six[pBufin[1]] >> 4);
		*(pszBufout++) =
			(unsigned char)(pr2six[pBufin[1]] << 4 | pr2six[pBufin[2]] >> 2);
		*(pszBufout++) =
			(unsigned char)(pr2six[pBufin[2]] << 6 | pr2six[pBufin[3]]);
		pBufin += 4;
		nprbytes -= 4;
	}

	if(nprbytes & 03){
		if(pr2six[pBufin[-2]] > 63)
			nbytesdecoded -= 2;
		else
			nbytesdecoded -= 1;
	}

	Outbuf[nbytesdecoded] = '\0';

	delete []szBufTmp;

	return nbytesdecoded;
}

/*这里定义BASE64用到的字符转换对照表*/
static char base64_alphabet[] =
{
	('A'), ('B'), ('C'), ('D'), ('E'), ('F'), ('G'), ('H'), ('I'),
	('J'), ('K'), ('L'), ('M'), ('N'), ('O'), ('P'), ('Q'), ('R'),
	('S'), ('T'), ('U'), ('V'), ('W'), ('X'), ('Y'), ('Z'), ('a'),
	('b'), ('c'), ('d'), ('e'), ('f'), ('g'), ('h'), ('i'), ('j'),
	('k'), ('l'), ('m'), ('n'), ('o'), ('p'), ('q'), ('r'), ('s'),
	('t'), ('u'), ('v'), ('w'), ('x'), ('y'), ('z'), ('0'), ('1'),
	('2'), ('3'), ('4'), ('5'), ('6'), ('7'), ('8'), ('9'), ('+'),
	('/')
};
static char cmove_bits(unsigned char src, unsigned lnum, unsigned rnum)
{
	src <<= lnum;
	src >>= rnum;
	return src;
}
#ifndef BASE64_PAD64
#define BASE64_PAD64 '='
#endif//BASE64_PAD64
bool Base64Encode(IN unsigned char* pSrc, IN unsigned int cbSize, OUT char* strEncode, IN OUT unsigned int& dwEncode)
{
	if(pSrc && cbSize && strEncode && dwEncode)
	{
		int nPadNum = 0;

		if((cbSize % 3) != 0)
			nPadNum = 3 - cbSize % 3;

		unsigned int dwReSize = (cbSize + nPadNum) + ((cbSize + nPadNum) / 3) + 1;

		if(dwReSize > dwEncode)
		{
			//Debug_View(_T("[Base64Encode]Memory Out Of Range!\n"));
			return false;
		}

		char* p = strEncode;
		unsigned char* pDat = pSrc;

		for(unsigned int m = 0; m < (cbSize + nPadNum); m += 3)
		{
			*p = base64_alphabet[cmove_bits(*pDat, 0, 2)];/*得到编码后的字符串*/

			/*以下处理结尾对齐的情况*/
			if(m == cbSize + nPadNum - 3 && nPadNum != 0)
			{
				if(nPadNum == 1)
				{
					*(p + 1) = base64_alphabet[cmove_bits(*pDat, 6, 2) + cmove_bits(*(pDat + 1), 0, 4)];
					*(p + 2) = base64_alphabet[cmove_bits(*(pDat + 1), 4, 2)];
					*(p + 3) = BASE64_PAD64;
				}
				else if(nPadNum == 2)
				{
					*(p + 1) = base64_alphabet[cmove_bits(*pDat, 6, 2)];
					*(p + 2) = BASE64_PAD64;
					*(p + 3) = BASE64_PAD64;
				}
			}
			else
			{
				*(p + 1) = base64_alphabet[cmove_bits(*pDat, 6, 2) + cmove_bits(*(pDat + 1), 0, 4)];
				*(p + 2) = base64_alphabet[cmove_bits(*(pDat + 1), 4, 2) + cmove_bits(*(pDat + 2), 0, 6)];
				*(p + 3) = base64_alphabet[*(pDat + 2) & 0x3f];
			}

			p += 4;
			pDat += 3;
		}

		dwEncode = (p - strEncode);
		return true;
	}

	return false;
}



