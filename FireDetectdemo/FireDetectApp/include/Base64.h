
#ifndef _BASE_64_H
#define _BASE_64_H

// 从双字中取单字节
#define B0(a)           (a & 0xFF)
#define B1(a)           (a >> 8 & 0xFF)
#define B2(a)           (a >> 16 & 0xFF)
#define B3(a)           (a >> 24 & 0xFF)
#define BYTE  char
void Byte2HexStr(const unsigned char* source, char* dest, int sourcelen);

int EncodeBase64(char * base64code, const char * src, int src_len);
int DecodeBase64(char * buf, const char * base64code, int src_len/* = 0*/);


/**  
 * bytes字符串转换为Byte值   
* @param String src Byte字符串，每个Byte之间没有分隔符   
* @return byte[]   
*/   
void Bytes2HexStr(BYTE *src,int srcLen,BYTE *des);
void base64_encode(const unsigned char *src, int src_len, char *dst);
#endif
