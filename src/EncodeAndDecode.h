#pragma once

#ifndef IN
#define IN
#define OUT
#endif//IN

int Base64_Decode(char *Inbuf, char *Outbuf, int nInLen, int nOutLen);

bool Base64Encode(IN unsigned char* pSrc, IN unsigned int  cbSize, OUT char* strEncode, IN OUT unsigned int & dwEncode);


