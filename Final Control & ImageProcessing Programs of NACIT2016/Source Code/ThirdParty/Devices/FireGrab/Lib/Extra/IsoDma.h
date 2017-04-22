////////////////////////////////////////////////////////////////////////////////
// Headerfile for IsoDma stuff.
// intek, C. Kuehnel, 29.1.2008
////////////////////////////////////////////////////////////////////////////////

#ifndef ISODMA_H
#define ISODMA_H

#include <fgcamera.h>

////////////////////////////////////////////////////////////////////////////////
// Class for a DMA.
////////////////////////////////////////////////////////////////////////////////

class CSPEC CIsoDma
{
private:
  class CFGNodeMgr     *m_pNodeMgr;             // Our node manager

  class CPhysMemList   *m_pPhysMemStack;        // Physical memory
  class CHandleList    *m_pStandbyQueue;        // Mirror for drivers standby queue
  class CHandleList    *m_pInputQueue;          // DMA input queue for LIMP mode
  class CPtrList       *m_pUserMem;             // User buffer memory

  UINT32                m_Reserved;             // Unused
  UINT32                m_UbSize;               // Payload size of user buffers

  void*                 m_hIsoDma;              // ISO DMA from driver

public:
  FGISODMAPARMS         m_Parms;                // Our parameters
  CFGIsoDma            *m_pFGIsoDma;            // Pointer parent DMA

protected:
  virtual UINT32        AllocateFrameBuffers();
  virtual void          FreeFrameBuffers();

public:
                        CIsoDma(CFGNodeMgr *pNodeMgr);
  virtual               ~CIsoDma();

  virtual UINT32        OpenCapture(FGISODMAPARMS *pParms);
  virtual UINT32        CloseCapture();

  virtual UINT32        AssignUserBuffers(UINT32 Cnt,UINT32 Size,void* *ppMemArray);
  virtual UINT32        GetFrame(FGFRAME *pFrame,UINT32 TimeoutInMs);
  virtual UINT32        PutFrame(FGFRAME *pFrame);
  virtual UINT32        DiscardFrames();
  virtual UINT32        Resize(UINT32 PktCnt,UINT32 PktSize);

  virtual UINT8         IsOpen()                { return m_hIsoDma?true:false; }
  virtual void          RemoveNodeMgr();

  virtual UINT32        AnnounceBuffer(UINT8 *pMem,UINT32 Size,FGHANDLE *phBuffer);
  virtual UINT32        RevokeBuffer(FGHANDLE hBuffer);
};

#endif
