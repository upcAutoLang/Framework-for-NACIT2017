/******************************************************************************/
/* This header file exports all FGCamera functions as standard C functions    */
/* First parameter of each function is a this pointer to FGamera object.      */
/* c. kuehnel, intek, 18.4.2005                                               */
/******************************************************************************/

#ifndef FGWRAP_H
#define FGWRAP_H

#include <FGCamera.h>

/* Include this header to get defines and constants. The CFGCamera class is   */
/* excluded in standard C projects. We also import the following declarations */
/* from there:                                                                */
/* UINT32 FLINK FGInitModule(FGINIT *pArg);                                   */
/* void   FLINK FGExitModule();                                               */
/* UINT32 FLINK FGGetNodeList(FGNODEINFO *pInfo,UINT32 MaxCnt,                */
/*                            UINT32 *pRealCnt);                              */

#ifdef FGWEXPORT
#define FEXP __declspec(dllexport) 
#define FLINK __stdcall
#else
#define FEXP __declspec(dllimport) 
#define FLINK __stdcall
#endif

#ifdef __cplusplus
extern "C" {
#endif

FEXP UINT32   FLINK FGSetInitialPhySpeed(UINT8 Speed);

FEXP UINT32   FLINK FGGetCameraHandle(FGHANDLE *phCamera,UINT32HL *pGuid,void* IsoContext);
FEXP void     FLINK FGPutCameraHandle(FGHANDLE hCamera);

FEXP UINT32   FLINK FGWriteRegister(FGHANDLE hCamera,UINT32 Address,UINT32 Value);
FEXP UINT32   FLINK FGReadRegister(FGHANDLE hCamera,UINT32 Address,UINT32 *pValue);

FEXP UINT32   FLINK FGWriteBlock(FGHANDLE hCamera,UINT32 Address,UINT8 *pData,UINT32 Length);
FEXP UINT32   FLINK FGReadBlock(FGHANDLE hCamera,UINT32 Address,UINT8 *pData,UINT32 Length);

FEXP UINT32   FLINK FGSetParameter(FGHANDLE hCamera,UINT16 Which,UINT32 Value);
FEXP UINT32   FLINK FGGetParameter(FGHANDLE hCamera,UINT16 Which,UINT32 *pValue);
FEXP UINT32   FLINK FGGetParameterInfo(FGHANDLE hCamera,UINT16 Which,FGPINFO *pInfo);

FEXP UINT32   FLINK FGOpenCapture(FGHANDLE hCamera);
FEXP UINT32   FLINK FGCloseCapture(FGHANDLE hCamera);

FEXP UINT32   FLINK FGStartDevice(FGHANDLE hCamera);
FEXP UINT32   FLINK FGStopDevice(FGHANDLE hCamera);

FEXP UINT32   FLINK FGGetFrame(FGHANDLE hCamera,FGFRAME *pFrame,UINT32 TimeoutInMs);
FEXP UINT32   FLINK FGPutFrame(FGHANDLE hCamera,FGFRAME *pFrame);
FEXP UINT32   FLINK FGDiscardFrames(FGHANDLE hCamera);

FEXP UINT32   FLINK FGGetDeviceName(FGHANDLE hCamera,char *pBuf,UINT32 MaxLength);
FEXP FGHANDLE FLINK FGGetContext(FGHANDLE hCamera);

FEXP UINT32   FLINK FGAssignUserBuffers(FGHANDLE hCamera,UINT32 Cnt,UINT32 Size,void* *ppMemArray);

#ifdef __cplusplus
}
#endif

#endif
