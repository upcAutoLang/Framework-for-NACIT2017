/********************************************************************************/
/* Module file to port FGamera class functions to standard C.                  */
/* c. kuehnel, intek, 18.4.2005                                                 */
/********************************************************************************/

#include <fgwrap.h>

static UINT8 PhySpeed=PS_AUTO;

UINT32 FLINK FGSetInitialPhySpeed(UINT8 Speed)
{
  if(Speed>PS_AUTO)
   return FCE_INPARMS;
   
  PhySpeed=Speed;
  
  return FCE_NOERROR;
}

UINT32 FLINK FGGetCameraHandle(FGHANDLE *phCamera,UINT32HL *pGuid,void* IsoContext)
{
  CFGCamera *pCamera;
  UINT32    Result;

  *phCamera=NULL;

  pCamera=new CFGCamera;
  if(!pCamera)
   return FCE_NOMEM;

  if(PhySpeed!=PS_AUTO)
   pCamera->SetParameter(FGP_PHYSPEED,(UINT32)PhySpeed);

  Result=pCamera->Connect(pGuid,IsoContext);
  if(Result!=FCE_NOERROR)
   delete pCamera;
  else
   *phCamera=(void*)pCamera;

  return Result;
}

void FLINK FGPutCameraHandle(FGHANDLE hCamera)
{
  ((CFGCamera*)hCamera)->Disconnect();
  delete (CFGCamera*)hCamera;
}

UINT32 FLINK FGWriteRegister(FGHANDLE hCamera,UINT32 Address,UINT32 Value)
{
  return ((CFGCamera*)hCamera)->WriteRegister(Address,Value);
}

UINT32 FLINK FGReadRegister(FGHANDLE hCamera,UINT32 Address,UINT32 *pValue)
{
  return ((CFGCamera*)hCamera)->ReadRegister(Address,pValue);
}

UINT32 FLINK FGWriteBlock(FGHANDLE hCamera,UINT32 Address,UINT8 *pData,UINT32 Length)
{
  return ((CFGCamera*)hCamera)->WriteBlock(Address,pData,Length);
}

UINT32 FLINK FGReadBlock(FGHANDLE hCamera,UINT32 Address,UINT8 *pData,UINT32 Length)
{
  return ((CFGCamera*)hCamera)->ReadBlock(Address,pData,Length);
}

UINT32 FLINK FGSetParameter(FGHANDLE hCamera,UINT16 Which,UINT32 Value)
{
  return ((CFGCamera*)hCamera)->SetParameter(Which,Value);
}

UINT32 FLINK FGGetParameter(FGHANDLE hCamera,UINT16 Which,UINT32 *pValue)
{
  return ((CFGCamera*)hCamera)->GetParameter(Which,pValue);
}

UINT32 FLINK FGGetParameterInfo(FGHANDLE hCamera,UINT16 Which,FGPINFO *pInfo)
{
  return ((CFGCamera*)hCamera)->GetParameterInfo(Which,pInfo);
}

UINT32 FLINK FGOpenCapture(FGHANDLE hCamera)
{
  return ((CFGCamera*)hCamera)->OpenCapture();
}

UINT32 FLINK FGCloseCapture(FGHANDLE hCamera)
{
  return ((CFGCamera*)hCamera)->CloseCapture();
}

UINT32 FLINK FGStartDevice(FGHANDLE hCamera)
{
  return ((CFGCamera*)hCamera)->StartDevice();
}

UINT32 FLINK FGStopDevice(FGHANDLE hCamera)
{
  return ((CFGCamera*)hCamera)->StopDevice();
}

UINT32 FLINK FGGetFrame(FGHANDLE hCamera,FGFRAME *pFrame,UINT32 TimeoutInMs)
{
  return ((CFGCamera*)hCamera)->GetFrame(pFrame,TimeoutInMs);
}

UINT32 FLINK FGPutFrame(FGHANDLE hCamera,FGFRAME *pFrame)
{
  return ((CFGCamera*)hCamera)->PutFrame(pFrame);
}

UINT32 FLINK FGDiscardFrames(FGHANDLE hCamera)
{
  return ((CFGCamera*)hCamera)->DiscardFrames();
}

UINT32 FLINK FGGetDeviceName(FGHANDLE hCamera,char *pBuf,UINT32 MaxLength)
{
  return ((CFGCamera*)hCamera)->GetDeviceName(pBuf,MaxLength);
}

FGHANDLE FLINK FGGetContext(FGHANDLE hCamera)
{
  return ((CFGCamera*)hCamera)->GetContext();
}

UINT32 FLINK FGAssignUserBuffers(FGHANDLE hCamera,UINT32 Cnt,UINT32 Size,
                           void* *ppMemArray)
{
  return ((CFGCamera*)hCamera)->AssignUserBuffers(Cnt,Size,ppMemArray);
}

