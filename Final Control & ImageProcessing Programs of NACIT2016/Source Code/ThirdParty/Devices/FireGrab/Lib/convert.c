////////////////////////////////////////////////////////////////////////////////
// Module for converting videodata.
// c. kuehnel, intek, 12.5.2001
////////////////////////////////////////////////////////////////////////////////

#include <convert.h>
#include <string.h>

typedef struct
{
  union
  {
    SINT16    Vr;
    SINT16    Ug;
  };

  union
  {
    SINT16    Vg;
    SINT16    Ub;
  };
}TBLENTRY;

TBLENTRY VTable[256];
TBLENTRY UTable[256];

UINT8  TablesInitialized=FALSE;

////////////////////////////////////////////////////////////////////////////////
// Init lookup-tables.
// We use the following formulas:
// R = Y + 1.4022V
// G = Y - 0.3457U - 0.7144V
// B = Y + 1.771U
////////////////////////////////////////////////////////////////////////////////

void InitTables()
{
  SINT16  Vr,Vg,Ug,Ub;
  UINT32  i;
  SINT16  U,V;

  if(TablesInitialized)
   return;

  // Initialize Assemblertables
  for(i=0;i<256;i++)
  {
    U=(SINT16)i-(SINT16)128;
    V=(SINT16)i-(SINT16)128;

    Vr=(SINT16)(1.4022*V);
    Vg=(SINT16)(-0.7144*V);
    Ug=(SINT16)(-0.3457*U);
    Ub=(SINT16)(1.7710*U);

    VTable[i].Vr=Vr;
    VTable[i].Vg=Vg;

    UTable[i].Ug=Ug;
    UTable[i].Ub=Ub;
  }

  TablesInitialized=TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// Convert RGB to BGR in mode RGB.
// Is: R G B R G B ...
// Windows BITMAP-Format is B G R B G R ...
////////////////////////////////////////////////////////////////////////////////

void ConvertRGB(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR)
{
  UINT32 Cnt;

  Cnt=XSize*YSize;
  while(Cnt--)
  {
    pBGR[2]=pBuf[0];
    pBGR[1]=pBuf[1];
    pBGR[0]=pBuf[2];

    pBuf+=3;
    pBGR+=3;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Convert RGB16 to BGR in mode RGB.
// Is: R16 G16 B16 R16 G16 B16 ...
// Windows BITMAP-Format is B G R B G R ...
////////////////////////////////////////////////////////////////////////////////

void ConvertRGB16(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR,UINT8 Swap)
{
  UINT32 Cnt;

  Cnt=XSize*YSize;
  if(!Swap)
  {
    while(Cnt--)
    {
      pBGR[2]=pBuf[0];
      pBGR[1]=pBuf[2];
      pBGR[0]=pBuf[4];

      pBuf+=6;
      pBGR+=3;
    }
  }
  else
  {
    while(Cnt--)
    {
      pBGR[2]=pBuf[1];
      pBGR[1]=pBuf[3];
      pBGR[0]=pBuf[5];

      pBuf+=6;
      pBGR+=3;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
// Convert YUV to RGB in mode 4:4:4.
// Is: U Y V U Y V U Y V ...
// Windows BITMAP-Format is B G R B G R ...
////////////////////////////////////////////////////////////////////////////////

void Convert444(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR)
{
  UINT32    Cnt;
  TBLENTRY *pU,*pV;
  UINT16    Y;
  UINT16    Res;

  Cnt=(XSize*YSize);

  while(Cnt--)
  {
    pU=UTable+*pBuf++;
    Y=(UINT16)*pBuf++;
    pV=VTable+*pBuf++;

    // Blue
    if((Res=(UINT16)((SINT16)Y+pU->Ub))>255)
     Res=-(Res>>8);
    *pBGR++=(UINT8)Res;

    // Green
    if((Res=((UINT16)(SINT16)Y+pU->Ug+pV->Vg))>255)
     Res=-(Res>>8);
    *pBGR++=(UINT8)Res;

    // Red
    if((Res=((UINT16)(SINT16)Y+pV->Vr))>255)
     Res=-(Res>>8);
    *pBGR++=(UINT8)Res;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Convert YUV to RGB in mode 4:2:2.
// Is: U Y V Y U Y V Y ...
////////////////////////////////////////////////////////////////////////////////

void Convert422(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR)
{
  UINT32    Cnt;
  TBLENTRY *pU,*pV;
  UINT16    Y1,Y2;
  UINT16    Res;

  Cnt=(XSize*YSize)/2;
  while(Cnt--)
  {
    pU=UTable+*pBuf++;
    Y1=(UINT16)*pBuf++;
    pV=VTable+*pBuf++;
    Y2=(UINT16)*pBuf++;

    // Blue0
    if((Res=(UINT16)((SINT16)Y1+pU->Ub))>255)
     Res=-(Res>>8);
    *pBGR++=(UINT8)Res;

    // Green0
    if((Res=((UINT16)(SINT16)Y1+pU->Ug+pV->Vg))>255)
     Res=-(Res>>8);
    *pBGR++=(UINT8)Res;

    // Red0
    if((Res=((UINT16)(SINT16)Y1+pV->Vr))>255)
     Res=-(Res>>8);
    *pBGR++=(UINT8)Res;

    // Blue1
    if((Res=(UINT16)((SINT16)Y2+pU->Ub))>255)
     Res=-(Res>>8);
    *pBGR++=(UINT8)Res;

    // Green1
    if((Res=((UINT16)(SINT16)Y2+pU->Ug+pV->Vg))>255)
     Res=-(Res>>8);
    *pBGR++=(UINT8)Res;

    // Red1
    if((Res=((UINT16)(SINT16)Y2+pV->Vr))>255)
     Res=-(Res>>8);
    *pBGR++=(UINT8)Res;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Convert Y8 to RGB buffer.
////////////////////////////////////////////////////////////////////////////////

void ConvertY8(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR)
{
  const UINT32 cnt=XSize*YSize;
  UINT32 i;
  
  for(i=0;i<cnt;++i)
  {
    *pBGR = *pBuf; ++pBGR;
    *pBGR = *pBuf; ++pBGR;
    *pBGR = *pBuf; ++pBGR;
    ++pBuf;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Convert YUV to RGB in mode 4:1:1.
// Is: U Y Y V Y Y U Y Y V Y Y ...
////////////////////////////////////////////////////////////////////////////////

void Convert411(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR)
{
  UINT32    Cnt;
  TBLENTRY *pU,*pV;
  UINT16    Y1,Y2,Y3,Y4;
  UINT16    Res;

  Cnt=(XSize*YSize)/4;
  while(Cnt--)
  {
    pU=UTable+*pBuf++;
    Y1=(UINT16)*pBuf++;
    Y2=(UINT16)*pBuf++;
    pV=VTable+*pBuf++;
    Y3=(UINT16)*pBuf++;
    Y4=(UINT16)*pBuf++;

    // Blue0
    if((Res=(UINT16)((SINT16)Y1+pU->Ub))>255)
     Res=-(Res>>8);
    *pBGR++=(UINT8)Res;

    // Green0
    if((Res=((UINT16)(SINT16)Y1+pU->Ug+pV->Vg))>255)
     Res=-(Res>>8);
    *pBGR++=(UINT8)Res;

    // Red0
    if((Res=((UINT16)(SINT16)Y1+pV->Vr))>255)
     Res=-(Res>>8);
    *pBGR++=(UINT8)Res;

    // Blue1
    if((Res=(UINT16)((SINT16)Y2+pU->Ub))>255)
     Res=-(Res>>8);
    *pBGR++=(UINT8)Res;

    // Green1
    if((Res=((UINT16)(SINT16)Y2+pU->Ug+pV->Vg))>255)
     Res=-(Res>>8);
    *pBGR++=(UINT8)Res;

    // Red1
    if((Res=((UINT16)(SINT16)Y2+pV->Vr))>255)
     Res=-(Res>>8);
    *pBGR++=(UINT8)Res;

    // Blue2
    if((Res=(UINT16)((SINT16)Y3+pU->Ub))>255)
     Res=-(Res>>8);
    *pBGR++=(UINT8)Res;

    // Green2
    if((Res=((UINT16)(SINT16)Y3+pU->Ug+pV->Vg))>255)
     Res=-(Res>>8);
    *pBGR++=(UINT8)Res;

    // Red2
    if((Res=((UINT16)(SINT16)Y3+pV->Vr))>255)
     Res=-(Res>>8);
    *pBGR++=(UINT8)Res;

    // Blue3
    if((Res=(UINT16)((SINT16)Y4+pU->Ub))>255)
     Res=-(Res>>8);
    *pBGR++=(UINT8)Res;

    // Green3
    if((Res=((UINT16)(SINT16)Y4+pU->Ug+pV->Vg))>255)
     Res=-(Res>>8);
    *pBGR++=(UINT8)Res;

    // Red3
    if((Res=((UINT16)(SINT16)Y4+pV->Vr))>255)
     Res=-(Res>>8);
    *pBGR++=(UINT8)Res;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Convert Y16 to RGB (big endian).
////////////////////////////////////////////////////////////////////////////////

void ConvertY16(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR)
{
  UINT32 Cnt;
  UINT8  Val;

  Cnt=XSize*YSize;
  while(Cnt--)
  {
    Val=*pBGR++=*pBuf;
    *pBGR++=Val;
    *pBGR++=Val;
    pBuf+=2;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Convert Y16 to RGB byte reversed order (little endian).
////////////////////////////////////////////////////////////////////////////////

void ConvertY16a(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR)
{
  UINT32 Cnt;
  UINT8  Val;

  Cnt=XSize*YSize;
  pBuf++;
  while(Cnt--)
  {
    Val=*pBGR++=*pBuf;
    *pBGR++=Val;
    *pBGR++=Val;
    pBuf+=2;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Convert raw mode with 8 bit values pattern RG GB.
////////////////////////////////////////////////////////////////////////////////

void ConvertRawY8RGGB(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR)
{
  UINT8 *pR,*pB,*pG0,*pG1;
  UINT32 i,j;
  struct pdst
  {
    UINT8 B;
    UINT8 G;
    UINT8 R;
  }*pDst;

  pDst=(struct pdst*)pBGR;

  for(i=0;i<YSize-1;i++)
  {
    if(i&1)
    {
      pG1=pBuf+i*XSize;
      pB=pG1+1;
      pR=pG1+XSize;
      pG0=pR+1;
    }
    else
    {
      pR=pBuf+i*XSize;
      pG0=pR+1;
      pG1=pR+XSize;
      pB=pG1+1;
    }

    // Go through all pixels
    for(j=0;j<XSize-1;j++)
    {
      pDst->B=*pB;
      pDst->G=(UINT8)((*pG0+*pG1)/2);
      pDst->R=*pR;

      pDst++;

      if(j&1)
      {
        pB+=2;
        pG0+=2;
      }
      else
      {
        pR+=2;
        pG1+=2;
      }
    }
    pDst->B=0;
    pDst->G=0;
    pDst->R=0;

    pDst++;
  }

  memset(pBGR+(XSize*(YSize-1))*3,0,XSize*3);
}

////////////////////////////////////////////////////////////////////////////////
// Convert raw mode with 8 bit values pattern GR BG.
////////////////////////////////////////////////////////////////////////////////

void ConvertRawY8GRBG(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR)
{
  UINT8 *pR,*pB,*pG0,*pG1;
  UINT32 i,j;
  struct pdst
  {
    UINT8 B;
    UINT8 G;
    UINT8 R;
  }*pDst;

  pDst=(struct pdst*)pBGR;

  for(i=0;i<YSize-1;i++)
  {
    if(i&1)
    {
      pB=pBuf+i*XSize;
      pG1=pB+1;
      pG0=pB+XSize;
      pR=pG0+1;
    }
    else
    {
      pG0=pBuf+i*XSize;
      pR=pG0+1;
      pB=pG0+XSize;
      pG1=pB+1;
    }

    // Go through all pixels
    for(j=0;j<XSize-1;j++)
    {
      pDst->B=*pB;
      pDst->G=(UINT8)((*pG0+*pG1)/2);
      pDst->R=*pR;

      pDst++;

      if(j&1)
      {
        pR+=2;
        pG1+=2;
      }
      else
      {
        pG0+=2;
        pB+=2;
      }
    }

    pDst->B=0;
    pDst->G=0;
    pDst->R=0;

    pDst++;
  }

  memset(pBGR+(XSize*(YSize-1))*3,0,XSize*3);
}

////////////////////////////////////////////////////////////////////////////////
// Convert raw mode with 8 bit values pattern BG GR.
////////////////////////////////////////////////////////////////////////////////

void ConvertRawY8BGGR(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR)
{
  UINT8 *pR,*pB,*pG0,*pG1;
  UINT32 i,j;
  struct pdst
  {
    UINT8 B;
    UINT8 G;
    UINT8 R;
  }*pDst;

  pDst=(struct pdst*)pBGR;

  for(i=0;i<YSize-1;i++)
  {
    if(i&1)
    {
      pG1=pBuf+i*XSize;
      pR=pG1+1;
      pB=pG1+XSize;
      pG0=pB+1;
    }
    else
    {
      pB=pBuf+i*XSize;
      pG0=pB+1;
      pG1=pB+XSize;
      pR=pG1+1;
    }

    // Go through all pixels
    for(j=0;j<XSize-1;j++)
    {
      pDst->B=*pB;
      pDst->G=(UINT8)((*pG0+*pG1)/2);
      pDst->R=*pR;

      pDst++;

      if(j&1)
      {
        pG0+=2;
        pR+=2;
      }
      else
      {
        pB+=2;
        pG1+=2;
      }
    }
    pDst->B=0;
    pDst->G=0;
    pDst->R=0;

    pDst++;
  }

  memset(pBGR+(XSize*(YSize-1))*3,0,XSize*3);
}

////////////////////////////////////////////////////////////////////////////////
// Convert raw mode with 8 bit values pattern GB RG.
////////////////////////////////////////////////////////////////////////////////

void ConvertRawY8GBRG(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR)
{
  UINT8 *pR,*pB,*pG0,*pG1;
  UINT32 i,j;
  struct pdst
  {
    UINT8 B;
    UINT8 G;
    UINT8 R;
  }*pDst;

  pDst=(struct pdst*)pBGR;

  for(i=0;i<YSize-1;i++)
  {
    if(i&1)
    {
      pR=pBuf+i*XSize;
      pG1=pR+1;
      pG0=pR+XSize;
      pB=pG0+1;
    }
    else
    {
      pG0=pBuf+i*XSize;
      pB=pG0+1;
      pR=pG0+XSize;
      pG1=pR+1;
    }

    // Go through all pixels
    for(j=0;j<XSize-1;j++)
    {
      pDst->B=*pB;
      pDst->G=(UINT8)((*pG0+*pG1)/2);
      pDst->R=*pR;

      pDst++;

      if(j&1)
      {
        pB+=2;
        pG1+=2;
      }
      else
      {
        pG0+=2;
        pR+=2;
      }
    }

    pDst->B=0;
    pDst->G=0;
    pDst->R=0;

    pDst++;
  }

  memset(pBGR+(XSize*(YSize-1))*3,0,XSize*3);
}

////////////////////////////////////////////////////////////////////////////////
// Convert raw mode with 8 bit values.
////////////////////////////////////////////////////////////////////////////////

void ConvertRawY8(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR,
                  UINT8 BayerPattern)
{
  switch(BayerPattern)
  {
    case 0: ConvertRawY8RGGB(XSize,YSize,pBuf,pBGR);
            break;

    case 1: ConvertRawY8GRBG(XSize,YSize,pBuf,pBGR);
            break;

    case 2: ConvertRawY8BGGR(XSize,YSize,pBuf,pBGR);
            break;

    case 3: ConvertRawY8GBRG(XSize,YSize,pBuf,pBGR);
            break;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Convert raw mode with 16 bit values pattern RG GB.
////////////////////////////////////////////////////////////////////////////////

void ConvertRawY16RGGB(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR)
{
  UINT16 *pR,*pB,*pG0,*pG1;
  UINT32 i,j;
  UINT16 *pMem;
  struct pdst
  {
    UINT8 B;
    UINT8 G;
    UINT8 R;
  }*pDst;

  pDst=(struct pdst*)pBGR;
  pMem=(UINT16*)pBuf;

  for(i=0;i<YSize-1;i++)
  {
    if(i&1)
    {
      pG1=pMem+i*XSize;
      pB=pG1+1;
      pR=pG1+XSize;
      pG0=pR+1;
    }
    else
    {
      pR=pMem+i*XSize;
      pG0=pR+1;
      pG1=pR+XSize;
      pB=pG1+1;
    }

    // Go through all pixels
    for(j=0;j<XSize-1;j++)
    {
      // We only use one byte
      pDst->B=(UINT8)(*(UINT8*)pB);
      pDst->G=(UINT8)(((*(UINT8*)pG0+*(UINT8*)pG1)/2));
      pDst->R=(UINT8)(*(UINT8*)pR);

      pDst++;

      if(j&1)
      {
        pB+=2;
        pG0+=2;
      }
      else
      {
        pR+=2;
        pG1+=2;
      }
    }
    pDst->B=0;
    pDst->G=0;
    pDst->R=0;

    pDst++;
  }

  memset(pBGR+(XSize*(YSize-1))*3,0,XSize*3);
}

////////////////////////////////////////////////////////////////////////////////
// Convert raw mode with 16 bit values pattern GR BG.
////////////////////////////////////////////////////////////////////////////////

void ConvertRawY16GRBG(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR)
{
  UINT16 *pR,*pB,*pG0,*pG1;
  UINT32 i,j;
  UINT16 *pMem;
  struct pdst
  {
    UINT8 B;
    UINT8 G;
    UINT8 R;
  }*pDst;

  pDst=(struct pdst*)pBGR;
  pMem=(UINT16*)pBuf;

  for(i=0;i<YSize-1;i++)
  {
    if(i&1)
    {
      pB=pMem+i*XSize;
      pG1=pB+1;
      pG0=pB+XSize;
      pR=pG0+1;
    }
    else
    {
      pG0=pMem+i*XSize;
      pR=pG0+1;
      pB=pG0+XSize;
      pG1=pB+1;
    }

    // Go through all pixels
    for(j=0;j<XSize-1;j++)
    {
      // We only use one MSB (it the lower one)
      pDst->B=(UINT8)(*(UINT8*)pB);
      pDst->G=(UINT8)(((*(UINT8*)pG0+*(UINT8*)pG1)/2));
      pDst->R=(UINT8)(*(UINT8*)pR);

      pDst++;

      if(j&1)
      {
        pR+=2;
        pG1+=2;
      }
      else
      {
        pG0+=2;
        pB+=2;
      }
    }

    pDst->B=0;
    pDst->G=0;
    pDst->R=0;

    pDst++;
  }

  memset(pBGR+(XSize*(YSize-1))*3,0,XSize*3);
}

////////////////////////////////////////////////////////////////////////////////
// Convert raw mode with 16 bit values pattern BG GR.
////////////////////////////////////////////////////////////////////////////////

void ConvertRawY16BGGR(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR)
{
  UINT16 *pR,*pB,*pG0,*pG1;
  UINT32 i,j;
  UINT16 *pMem;
  struct pdst
  {
    UINT8 B;
    UINT8 G;
    UINT8 R;
  }*pDst;

  pDst=(struct pdst*)pBGR;
  pMem=(UINT16*)pBuf;

  for(i=0;i<YSize-1;i++)
  {
    if(i&1)
    {
      pG1=pMem+i*XSize;
      pR=pG1+1;
      pB=pG1+XSize;
      pG0=pB+1;
    }
    else
    {
      pB=pMem+i*XSize;
      pG0=pB+1;
      pG1=pB+XSize;
      pR=pG1+1;
    }

    // Go through all pixels
    for(j=0;j<XSize-1;j++)
    {
      // We only use one MSB (it the lower one)
      pDst->B=(UINT8)(*(UINT8*)pB);
      pDst->G=(UINT8)(((*(UINT8*)pG0+*(UINT8*)pG1)/2));
      pDst->R=(UINT8)(*(UINT8*)pR);

      pDst++;

      if(j&1)
      {
        pG0+=2;
        pR+=2;
      }
      else
      {
        pB+=2;
        pG1+=2;
      }
    }
    pDst->B=0;
    pDst->G=0;
    pDst->R=0;

    pDst++;
  }

  memset(pBGR+(XSize*(YSize-1))*3,0,XSize*3);
}

////////////////////////////////////////////////////////////////////////////////
// Convert raw mode with 16 bit values pattern GB RG.
////////////////////////////////////////////////////////////////////////////////

void ConvertRawY16GBRG(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR)
{
  UINT16 *pR,*pB,*pG0,*pG1;
  UINT32 i,j;
  UINT16 *pMem;
  struct pdst
  {
    UINT8 B;
    UINT8 G;
    UINT8 R;
  }*pDst;

  pDst=(struct pdst*)pBGR;
  pMem=(UINT16*)pBuf;

  for(i=0;i<YSize-1;i++)
  {
    if(i&1)
    {
      pR=pMem+i*XSize;
      pG1=pR+1;
      pG0=pR+XSize;
      pB=pG0+1;
    }
    else
    {
      pG0=pMem+i*XSize;
      pB=pG0+1;
      pR=pG0+XSize;
      pG1=pR+1;
    }

    // Go through all pixels
    for(j=0;j<XSize-1;j++)
    {
      // We only use one MSB (it the lower one)
      pDst->B=(UINT8)(*(UINT8*)pB);
      pDst->G=(UINT8)(((*(UINT8*)pG0+*(UINT8*)pG1)/2));
      pDst->R=(UINT8)(*(UINT8*)pR);

      pDst++;

      if(j&1)
      {
        pB+=2;
        pG1+=2;
      }
      else
      {
        pG0+=2;
        pR+=2;
      }
    }

    pDst->B=0;
    pDst->G=0;
    pDst->R=0;

    pDst++;
  }

  memset(pBGR+(XSize*(YSize-1))*3,0,XSize*3);
}

////////////////////////////////////////////////////////////////////////////////
// Convert raw mode with 16 bit values.
////////////////////////////////////////////////////////////////////////////////

void ConvertRawY16(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR,
                   UINT8 BayerPattern)
{
  switch(BayerPattern)
  {
    case 0: ConvertRawY16RGGB(XSize,YSize,pBuf,pBGR);
            break;

    case 1: ConvertRawY16GRBG(XSize,YSize,pBuf,pBGR);
            break;

    case 2: ConvertRawY16BGGR(XSize,YSize,pBuf,pBGR);
            break;

    case 3: ConvertRawY16GBRG(XSize,YSize,pBuf,pBGR);
            break;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Convert raw mode with 16 bit invers values pattern RG GB.
////////////////////////////////////////////////////////////////////////////////

void ConvertRawY16aRGGB(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR)
{
  UINT16 *pR,*pB,*pG0,*pG1;
  UINT32 i,j;
  UINT16 *pMem;
  struct pdst
  {
    UINT8 B;
    UINT8 G;
    UINT8 R;
  }*pDst;

  pDst=(struct pdst*)pBGR;
  pMem=(UINT16*)pBuf;

  for(i=0;i<YSize-1;i++)
  {
    if(i&1)
    {
      pG1=pMem+i*XSize;
      pB=pG1+1;
      pR=pG1+XSize;
      pG0=pR+1;
    }
    else
    {
      pR=pMem+i*XSize;
      pG0=pR+1;
      pG1=pR+XSize;
      pB=pG1+1;
    }

    // Go through all pixels
    for(j=0;j<XSize-1;j++)
    {
      // We only use one byte
      pDst->B=(UINT8)(*((UINT8*)pB+1));
      pDst->G=(UINT8)(((*((UINT8*)pG0+1)+*((UINT8*)pG1+1))/2));
      pDst->R=(UINT8)(*((UINT8*)pR+1));

      pDst++;

      if(j&1)
      {
        pB+=2;
        pG0+=2;
      }
      else
      {
        pR+=2;
        pG1+=2;
      }
    }
    pDst->B=0;
    pDst->G=0;
    pDst->R=0;

    pDst++;
  }

  memset(pBGR+(XSize*(YSize-1))*3,0,XSize*3);
}

////////////////////////////////////////////////////////////////////////////////
// Convert raw mode with 16 bit invers values pattern GR BG.
////////////////////////////////////////////////////////////////////////////////

void ConvertRawY16aGRBG(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR)
{
  UINT16 *pR,*pB,*pG0,*pG1;
  UINT32 i,j;
  UINT16 *pMem;
  struct pdst
  {
    UINT8 B;
    UINT8 G;
    UINT8 R;
  }*pDst;

  pDst=(struct pdst*)pBGR;
  pMem=(UINT16*)pBuf;

  for(i=0;i<YSize-1;i++)
  {
    if(i&1)
    {
      pB=pMem+i*XSize;
      pG1=pB+1;
      pG0=pB+XSize;
      pR=pG0+1;
    }
    else
    {
      pG0=pMem+i*XSize;
      pR=pG0+1;
      pB=pG0+XSize;
      pG1=pB+1;
    }

    // Go through all pixels
    for(j=0;j<XSize-1;j++)
    {
      // We only use one byte
      pDst->B=(UINT8)(*((UINT8*)pB+1));
      pDst->G=(UINT8)(((*((UINT8*)pG0+1)+*((UINT8*)pG1+1))/2));
      pDst->R=(UINT8)(*((UINT8*)pR+1));

      pDst++;

      if(j&1)
      {
        pR+=2;
        pG1+=2;
      }
      else
      {
        pG0+=2;
        pB+=2;
      }
    }

    pDst->B=0;
    pDst->G=0;
    pDst->R=0;

    pDst++;
  }

  memset(pBGR+(XSize*(YSize-1))*3,0,XSize*3);
}

////////////////////////////////////////////////////////////////////////////////
// Convert raw mode with 16 bit invers values pattern BG GR.
////////////////////////////////////////////////////////////////////////////////

void ConvertRawY16aBGGR(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR)
{
  UINT16 *pR,*pB,*pG0,*pG1;
  UINT32 i,j;
  UINT16 *pMem;
  struct pdst
  {
    UINT8 B;
    UINT8 G;
    UINT8 R;
  }*pDst;

  pDst=(struct pdst*)pBGR;
  pMem=(UINT16*)pBuf;

  for(i=0;i<YSize-1;i++)
  {
    if(i&1)
    {
      pG1=pMem+i*XSize;
      pR=pG1+1;
      pB=pG1+XSize;
      pG0=pB+1;
    }
    else
    {
      pB=pMem+i*XSize;
      pG0=pB+1;
      pG1=pB+XSize;
      pR=pG1+1;
    }

    // Go through all pixels
    for(j=0;j<XSize-1;j++)
    {
      // We only use one byte
      pDst->B=(UINT8)(*((UINT8*)pB+1));
      pDst->G=(UINT8)(((*((UINT8*)pG0+1)+*((UINT8*)pG1+1))/2));
      pDst->R=(UINT8)(*((UINT8*)pR+1));

      pDst++;

      if(j&1)
      {
        pG0+=2;
        pR+=2;
      }
      else
      {
        pB+=2;
        pG1+=2;
      }
    }
    pDst->B=0;
    pDst->G=0;
    pDst->R=0;

    pDst++;
  }

  memset(pBGR+(XSize*(YSize-1))*3,0,XSize*3);
}

////////////////////////////////////////////////////////////////////////////////
// Convert raw mode with 16 bit invers values pattern GB RG.
////////////////////////////////////////////////////////////////////////////////

void ConvertRawY16aGBRG(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR)
{
  UINT16 *pR,*pB,*pG0,*pG1;
  UINT32 i,j;
  UINT16 *pMem;
  struct pdst
  {
    UINT8 B;
    UINT8 G;
    UINT8 R;
  }*pDst;

  pDst=(struct pdst*)pBGR;
  pMem=(UINT16*)pBuf;

  for(i=0;i<YSize-1;i++)
  {
    if(i&1)
    {
      pR=pMem+i*XSize;
      pG1=pR+1;
      pG0=pR+XSize;
      pB=pG0+1;
    }
    else
    {
      pG0=pMem+i*XSize;
      pB=pG0+1;
      pR=pG0+XSize;
      pG1=pR+1;
    }

    // Go through all pixels
    for(j=0;j<XSize-1;j++)
    {
      // We only use one byte
      pDst->B=(UINT8)(*((UINT8*)pB+1));
      pDst->G=(UINT8)(((*((UINT8*)pG0+1)+*((UINT8*)pG1+1))/2));
      pDst->R=(UINT8)(*((UINT8*)pR+1));

      pDst++;

      if(j&1)
      {
        pB+=2;
        pG1+=2;
      }
      else
      {
        pG0+=2;
        pR+=2;
      }
    }

    pDst->B=0;
    pDst->G=0;
    pDst->R=0;

    pDst++;
  }

  memset(pBGR+(XSize*(YSize-1))*3,0,XSize*3);
}

////////////////////////////////////////////////////////////////////////////////
// Convert raw mode with 16 bit inverted values.
////////////////////////////////////////////////////////////////////////////////

void ConvertRawY16a(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR,
                    UINT8 BayerPattern)
{
  switch(BayerPattern)
  {
    case 0: ConvertRawY16aRGGB(XSize,YSize,pBuf,pBGR);
            break;

    case 1: ConvertRawY16aGRBG(XSize,YSize,pBuf,pBGR);
            break;

    case 2: ConvertRawY16aBGGR(XSize,YSize,pBuf,pBGR);
            break;

    case 3: ConvertRawY16aGBRG(XSize,YSize,pBuf,pBGR);
            break;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Plane-Convert YUV to RGB in mode 4:4:4.
// Is: U Y V U Y V U Y V ...
////////////////////////////////////////////////////////////////////////////////

void XPlane444(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pR,UINT8 *pG,UINT8 *pB)
{
  UINT32    Cnt;
  TBLENTRY *pU,*pV;
  UINT16    Y;
  UINT16    Res;

  Cnt=(XSize*YSize);
  while(Cnt--)
  {
    pU=UTable+*pBuf++;
    Y=(UINT16)*pBuf++;
    pV=VTable+*pBuf++;

    // Blue
    if((Res=(UINT16)((SINT16)Y+pU->Ub))>255)
     Res=-(Res>>8);
    *pB++=(UINT8)Res;

    // Green
    if((Res=((UINT16)(SINT16)Y+pU->Ug+pV->Vg))>255)
     Res=-(Res>>8);
    *pG++=(UINT8)Res;

    // Red
    if((Res=((UINT16)(SINT16)Y+pV->Vr))>255)
     Res=-(Res>>8);
    *pR++=(UINT8)Res;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Plane Convert YUV to RGB in mode 4:2:2.
// Is: U Y V Y U Y V Y ...
// R* = Y            + 1.4022 V
// G* = Y - 0.3457 U - 0.7144 V
// B* = Y + 1.7710 U
////////////////////////////////////////////////////////////////////////////////

void XPlane422(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pR,UINT8 *pG,UINT8 *pB)
{
  UINT32    Cnt;
  TBLENTRY *pU,*pV;
  UINT16    Y1,Y2;
  UINT16    Res;

  Cnt=(XSize*YSize)/2;
  while(Cnt--)
  {
    pU=UTable+*pBuf++;
    Y1=(UINT16)*pBuf++;
    pV=VTable+*pBuf++;
    Y2=(UINT16)*pBuf++;

    // Blue0
    if((Res=(UINT16)((SINT16)Y1+pU->Ub))>255)
     Res=-(Res>>8);
    *pB++=(UINT8)Res;

    // Green0
    if((Res=((UINT16)(SINT16)Y1+pU->Ug+pV->Vg))>255)
     Res=-(Res>>8);
    *pG++=(UINT8)Res;

    // Red0
    if((Res=((UINT16)(SINT16)Y1+pV->Vr))>255)
     Res=-(Res>>8);
    *pR++=(UINT8)Res;

    // Blue1
    if((Res=(UINT16)((SINT16)Y2+pU->Ub))>255)
     Res=-(Res>>8);
    *pB++=(UINT8)Res;

    // Green1
    if((Res=((UINT16)(SINT16)Y2+pU->Ug+pV->Vg))>255)
     Res=-(Res>>8);
    *pG++=(UINT8)Res;

    // Red1
    if((Res=((UINT16)(SINT16)Y2+pV->Vr))>255)
     Res=-(Res>>8);
    *pR++=(UINT8)Res;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Plane Convert YUV to RGB in mode 4:1:1.
// Is: U Y Y V Y Y U Y Y V Y Y ...
////////////////////////////////////////////////////////////////////////////////

void XPlane411(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pR,UINT8 *pG,UINT8 *pB)
{
  UINT32    Cnt;
  TBLENTRY *pU,*pV;
  UINT16    Y1,Y2,Y3,Y4;
  UINT16    Res;

  Cnt=(XSize*YSize)/4;
  while(Cnt--)
  {
    pU=UTable+*pBuf++;
    Y1=(UINT16)*pBuf++;
    Y2=(UINT16)*pBuf++;
    pV=VTable+*pBuf++;
    Y3=(UINT16)*pBuf++;
    Y4=(UINT16)*pBuf++;

    // Blue0
    if((Res=(UINT16)((SINT16)Y1+pU->Ub))>255)
     Res=-(Res>>8);
    *pB++=(UINT8)Res;

    // Green0
    if((Res=((UINT16)(SINT16)Y1+pU->Ug+pV->Vg))>255)
     Res=-(Res>>8);
    *pG++=(UINT8)Res;

    // Red0
    if((Res=((UINT16)(SINT16)Y1+pV->Vr))>255)
     Res=-(Res>>8);
    *pR++=(UINT8)Res;

    // Blue1
    if((Res=(UINT16)((SINT16)Y2+pU->Ub))>255)
     Res=-(Res>>8);
    *pB++=(UINT8)Res;

    // Green1
    if((Res=((UINT16)(SINT16)Y2+pU->Ug+pV->Vg))>255)
     Res=-(Res>>8);
    *pG++=(UINT8)Res;

    // Red1
    if((Res=((UINT16)(SINT16)Y2+pV->Vr))>255)
     Res=-(Res>>8);
    *pR++=(UINT8)Res;

    // Blue2
    if((Res=(UINT16)((SINT16)Y3+pU->Ub))>255)
     Res=-(Res>>8);
    *pB++=(UINT8)Res;

    // Green2
    if((Res=((UINT16)(SINT16)Y3+pU->Ug+pV->Vg))>255)
     Res=-(Res>>8);
    *pG++=(UINT8)Res;

    // Red2
    if((Res=((UINT16)(SINT16)Y3+pV->Vr))>255)
     Res=-(Res>>8);
    *pR++=(UINT8)Res;

    // Blue3
    if((Res=(UINT16)((SINT16)Y4+pU->Ub))>255)
     Res=-(Res>>8);
    *pB++=(UINT8)Res;

    // Green3
    if((Res=((UINT16)(SINT16)Y4+pU->Ug+pV->Vg))>255)
     Res=-(Res>>8);
    *pG++=(UINT8)Res;

    // Red3
    if((Res=((UINT16)(SINT16)Y4+pV->Vr))>255)
     Res=-(Res>>8);
    *pR++=(UINT8)Res;
  }
}

