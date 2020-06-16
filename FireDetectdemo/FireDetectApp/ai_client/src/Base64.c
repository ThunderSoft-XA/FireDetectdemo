
//#include <Base64.h>
//#include <string.h>
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "Base64.h"



void Byte2HexStr(const unsigned char* source, char* dest, int sourcelen)
{
    short i;
    unsigned char highByte, lowByte;
    for (i = 0; i < sourcelen; i++)
    {
        highByte = source[i] >> 4;
        lowByte = source[i] & 0x0f;

        highByte += 0x03;

        if (highByte > 0x39)
        {
            dest[2*i] = highByte + 0x07;
        }
        else
        {
            dest[2*i] = highByte;
        }

        lowByte += 0x03;

        if (lowByte > 0x39)
        {
            dest[2*i + 1] = lowByte + 0x07;
        }
        else
        {
            dest[2*i + 1] = lowByte;
        }
    }
}


#define B0(a)           (a & 0xFF)
#define B1(a)           (a >> 8 & 0xFF)
#define B2(a)           (a >> 16 & 0xFF)
#define B3(a)           (a >> 24 & 0xFF)
static char GetB64Char(int iIndex)
{
    const char szBase64Table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    if (iIndex >= 0 && iIndex < 64) {
        return szBase64Table[iIndex];
    }

    return '=';
}

static int GetB64Index(char ch) {
    int index = -1;
    if (ch >= 'A' && ch <= 'Z') {
        index = ch - 'A';
    } else if (ch >= 'a' && ch <= 'z') {
        index = ch - 'a' + 26;
    } else if (ch >= '0' && ch <= '9') {
        index = ch - '0' + 52;
    } else if (ch == '+') {
        index = 62;
    } else if (ch == '/') {
        index = 63;
    }

    return index;
}

int EncodeBase64(char * base64code, const char * src, int src_len)
{
    if (src_len == 0)
        src_len = strlen(src);

    int len = 0;
    unsigned char* psrc = (unsigned char*)src;
    char * p64 = base64code;
    int i;
    for (i = 0; i < src_len - 3; i += 3) {
        unsigned long ulTmp = *(unsigned long*)psrc;
        register int b0 = GetB64Char((B0(ulTmp) >> 2) & 0x3F);
        register int b1 = GetB64Char((B0(ulTmp) << 6 >> 2 | B1(ulTmp) >> 4) & 0x3F);
        register int b2 = GetB64Char((B1(ulTmp) << 4 >> 2 | B2(ulTmp) >> 6) & 0x3F);
        register int b3 = GetB64Char((B2(ulTmp) << 2 >> 2) & 0x3F);
        *((unsigned long*)p64) = b0 | b1 << 8 | b2 << 16 | b3 << 24;
        len += 4;
        p64  += 4;
        psrc += 3;
    }

    // 处理最后余下的不足3字节的饿数据
    if (i < src_len) {
        int rest = src_len - i;
        unsigned long ulTmp = 0;
	int j = 0;
        for (j = 0; j < rest; ++j) {
            *(((unsigned char*)&ulTmp) + j) = *psrc++;
        }

        p64[0] = GetB64Char((B0(ulTmp) >> 2) & 0x3F);
        p64[1] = GetB64Char((B0(ulTmp) << 6 >> 2 | B1(ulTmp) >> 4) & 0x3F);
        p64[2] = rest > 1 ? GetB64Char((B1(ulTmp) << 4 >> 2 | B2(ulTmp) >> 6) & 0x3F) : '=';
        p64[3] = rest > 2 ? GetB64Char((B2(ulTmp) << 2 >> 2) & 0x3F) : '=';
        p64 += 4;
        len += 4;
    }

    *p64 = '\0';

    return len;
}

int DecodeBase64(char * buf, const char * base64code, int src_len/* = 0*/)
{
	if (src_len == 0) {
		src_len = strlen(base64code);
    }

	int len = 0;
	unsigned char* psrc = (unsigned char*)base64code;
	char * pbuf = buf;
	int i;
	for (i = 0; i < src_len - 4; i += 4) {
		unsigned long ulTmp = *(unsigned long*)psrc;

		register int b0 = (GetB64Index((char)B0(ulTmp)) << 2 | GetB64Index((char)B1(ulTmp)) << 2 >> 6) & 0xFF;
		register int b1 = (GetB64Index((char)B1(ulTmp)) << 4 | GetB64Index((char)B2(ulTmp)) << 2 >> 4) & 0xFF;
		register int b2 = (GetB64Index((char)B2(ulTmp)) << 6 | GetB64Index((char)B3(ulTmp)) << 2 >> 2) & 0xFF;

		*((unsigned long*)pbuf) = b0 | b1 << 8 | b2 << 16;
		psrc  += 4;
		pbuf += 3;
		len += 3;
	}

	// 处理最后余下的不足4字节的饿数据
	if (i < src_len) {
		int rest = src_len - i;
		unsigned long ulTmp = 0;
		int j = 0;
		for (j = 0; j < rest; ++j) {
			*(((unsigned char*)&ulTmp) + j) = *psrc++;
		}

		register int b0 = (GetB64Index((char)B0(ulTmp)) << 2 | GetB64Index((char)B1(ulTmp)) << 2 >> 6) & 0xFF;
		*pbuf++ = b0;
		len  ++;

		if ('=' != B1(ulTmp) && '=' != B2(ulTmp)) {
			register int b1 = (GetB64Index((char)B1(ulTmp)) << 4 | GetB64Index((char)B2(ulTmp)) << 2 >> 4) & 0xFF;
			*pbuf++ = b1;
			len  ++;
		}

		if ('=' != B2(ulTmp) && '=' != B3(ulTmp)) {
			register int b2 = (GetB64Index((char)B2(ulTmp)) << 6 | GetB64Index((char)B3(ulTmp)) << 2 >> 2) & 0xFF;
			*pbuf++ = b2;
			len  ++;
		}
	}

	*pbuf = '\0';

	return len;
}


/*   Byte值转换为bytes字符串 
*   @param src：Byte指针 srcLen:src长度 des:转换得到的bytes字符串   
**/  
void Bytes2HexStr(BYTE *src,int srcLen,BYTE *des)
{
	BYTE *res;
	int i=0;
 
	res = des;
	while(srcLen>0)
	{
		sprintf((char*)(res+i*2),"%02x",*(src+i));
		i++;
		srcLen--;
	}
}

#define BASE64_ENCODE_BODY                                                \
  static const char *b64 =                                                \
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"; \
  int i, j, a, b, c;                                                      \
                                                                          \
  for (i = j = 0; i < src_len; i += 3) {                                  \
    a = src[i];                                                           \
    b = i + 1 >= src_len ? 0 : src[i + 1];                                \
    c = i + 2 >= src_len ? 0 : src[i + 2];                                \
                                                                          \
    BASE64_OUT(b64[a >> 2]);                                              \
    BASE64_OUT(b64[((a & 3) << 4) | (b >> 4)]);                           \
    if (i + 1 < src_len) {                                                \
      BASE64_OUT(b64[(b & 15) << 2 | (c >> 6)]);                          \
    }                                                                     \
    if (i + 2 < src_len) {                                                \
      BASE64_OUT(b64[c & 63]);                                            \
    }                                                                     \
  }                                                                       \
                                                                          \
  while (j % 4 != 0) {                                                    \
    BASE64_OUT('=');                                                      \
  }                                                                       \
  BASE64_FLUSH()

#define BASE64_OUT(ch) \
  do {                 \
    dst[j++] = (ch);   \
  } while (0)

#define BASE64_FLUSH() \
  do {                 \
    dst[j++] = '\0';   \
  } while (0)

void cs_base64_encode(const unsigned char *src, int src_len, char *dst) {
  BASE64_ENCODE_BODY;
}

#undef BASE64_OUT
#undef BASE64_FLUSH

void base64_encode(const unsigned char *src, int src_len, char *dst) {
  cs_base64_encode(src, src_len, dst);
}

