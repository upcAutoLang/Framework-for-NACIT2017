////////////////////////////////////////////////////////////////////////////////
// Headerfile for conversion module for video data.
// C. Kuehnel, intek, 11.5.2001
////////////////////////////////////////////////////////////////////////////////

#ifndef CONVERT_H
#define CONVERT_H

#include <basetype.h>

#ifdef __cplusplus
extern "C" {
#endif

void InitTables();

void ConvertRGB(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR);
void ConvertRGB16(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR,UINT8 Swap);
void Convert444(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR);
void Convert422(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR);
void Convert411(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR);
void ConvertY8(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR);
void ConvertY16(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR);
void ConvertY16a(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR);
void ConvertRawY8(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR,UINT8 BayerPattern);
void ConvertRawY16(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR,UINT8 BayerPattern);
void ConvertRawY16a(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR,UINT8 BayerPattern);

void XPlane444(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pR,UINT8 *pG,UINT8 *pB);
void XPlane422(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pR,UINT8 *pG,UINT8 *pB);
void XPlane411(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pR,UINT8 *pG,UINT8 *pB);

#ifdef __cplusplus
}
#endif

#endif
