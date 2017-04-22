////////////////////////////////////////////////////////////////////////////////
// Headerfile for camera stuff.
// intek, C. Kuehnel, 14.2.2001
////////////////////////////////////////////////////////////////////////////////

#ifndef CAMERA_H
#define CAMERA_H

#include <fgcamera.h>
#include "isodma.h"

// Partial scan video format is 7
#define PSFORMAT                        7

// Some offsets for our Configuration-ROM
#define HIGHOFFSET                      0xFFFF
#define SYSBASE                         0xF0000000
#define INITOFFSET                      0x00F00000
#define RESETSTARTOFFSET                0x0C
#define VENDORIDOFFSET                  0x40C
#define MODULEIDOFFSET                  0x410
#define ROOTOFFSET                      0x414
#define UNITOFFSET                      0x424
#define UNITDEPOFFSET                   0x0C
#define CMDBASEOFFSET                   0x04
#define VENDORLEAFOFFSET                0x08
#define MODELLEAFOFFSET                 0x0C
#define FORMATINQUIREOFFSET             0x100
#define FORMAT0MODEINQUIREOFFSET        0x180
#define FRAMERATEINQUIREOFFSET          0x200

// Offsets for CSR register
#define CSRMAXIMAGEINQ                  0x00
#define CSRUNITSIZEINQ                  0x04
#define CSRIMAGEPOSITION                0x08
#define CSRIMAGESIZE                    0x0C
#define CSRCOLORCODING                  0x10
#define CSRCOLORCODINGINQ               0x14
#define CSRBYTEPERFRAME                 0x3C
#define CSRPACKETINQ                    0x40
#define CSRBYTEPERPACKET                0x44
#define CSRPACKETPERFRAME               0x48
#define CSRUNITPOSINQ                   0x4C
#define CSRVALUESETTING                 0x7C

// Offsets for command register
#define CCR_FRAMERATE                   0x600
#define CCR_VMODE                       0x604
#define CCR_VFORMAT                     0x608
#define CCR_CHNSPEED                    0x60C
#define CCR_ISOENABLE                   0x614
#define CCR_ONESHOT                     0x61C
#define CCR_TRGMODE                     0x830

// Offsets for feature registers
#define BASIC_INQ                       0x400
#define FEATUREHI_INQ                   0x404
#define FEATURELO_INQ                   0x408

#define BRIGHTNESS_INQ                  0x500
#define AUTOEXPOSURE_INQ                0x504
#define SHARPNESS_INQ                   0x508
#define WHITEBAL_INQ                    0x50C
#define HUE_INQ                         0x510
#define SATURATION_INQ                  0x514
#define GAMMA_INQ                       0x518
#define SHUTTER_INQ                     0x51C
#define GAIN_INQ                        0x520
#define IRIS_INQ                        0x524
#define FOCUS_INQ                       0x528
#define TEMPERATURE_INQ                 0x52C
#define TRIGGER_INQ                     0x530
#define TRIGGERDLY_INQ                  0x534
#define WHITESHD_INQ                    0x538
#define FRAMERATE_INQ                   0x53C

#define ZOOM_INQ                        0x580
#define PAN_INQ                         0x584
#define TILT_INQ                        0x588
#define OPTICALFILTER_INQ               0x58C
#define CAPTURESIZE_INQ                 0x5C0
#define CAPTUREQUALITY_INQ              0x5C4

#define INQTOCTRLOFFSET                 0x300
#define INQTOABSOFFSET                  0x200

// Feature types
enum
{
  CFT_BRIGHTNESS,
  CFT_AUTOEXPOSURE,
  CFT_SHARPNESS,
  CFT_WHITEBAL,
  CFT_HUE,
  CFT_SATURATION,
  CFT_GAMMA,
  CFT_SHUTTER,
  CFT_GAIN,
  CFT_IRIS,
  CFT_FOCUS,
  CFT_TEMPERATURE,
  CFT_TRIGGER,
  CFT_TRIGGERDLY,
  CFT_WHITESHD,
  CFT_FRAMERATE,
  CFT_ZOOM,
  CFT_PAN,
  CFT_TILT,
  CFT_OPTICALFILTER,
  CFT_CAPTURESIZE,
  CFT_CAPTUREQUALITY,
  CFT_LAST
};

enum
{
  RE_EXIT=0,
  RE_WAITLONG,
  RE_WAITSHORT
};

////////////////////////////////////////////////////////////////////////////////
// Type for feature property on the bus.
////////////////////////////////////////////////////////////////////////////////

typedef union
{
  struct
  {
    UINT32              MaxValue : 12;          // Maximum Value
    UINT32              MinValue : 12;          // Minimum Value
    UINT32              Manual   : 1;           // Is manual controllable
    UINT32              Auto     : 1;           // Automode available
    UINT32              OnOff    : 1;           // Can be switched on/off
    UINT32              ReadOut  : 1;           // Can be read
    UINT32              OneShot  : 1;           // One shot auto set possible
    UINT32              Reserved : 1;
    UINT32              Absolute : 1;           // Absolute controllable
    UINT32              Present  : 1;           // Is present
  }AsBits;

  UINT32                AsUINT32;

}DCAMFEATINQ;

////////////////////////////////////////////////////////////////////////////////
// Type for feature property on the bus.
////////////////////////////////////////////////////////////////////////////////

typedef union
{
  struct
  {
    UINT32              ValueLow : 12;          // First/main value
    UINT32              ValueHigh: 12;          // Second Value

    UINT32              Auto     : 1;           // Automode 
    UINT32              OnOff    : 1;           // OnOff bit
    UINT32              OneShot  : 1;           // One shot
    UINT32              Reserved : 3;
    UINT32              Absolute : 1;           // Absolute controllable
    UINT32              Present  : 1;           // Is present
  }AsBits;

  UINT32                AsUINT32;

}DCAMFEATCSR;

////////////////////////////////////////////////////////////////////////////////
// Single feature for a camera.
////////////////////////////////////////////////////////////////////////////////

typedef struct
{
  UINT32                Offset;                 // Address offset
  DCAMFEATINQ           Inquire;                // Inquiry register
  DCAMFEATCSR           Control;                // Control register
  UINT32                AbsoluteOffset;         // Absolute offset
  UINT32                AbsoluteValue;          // Absolute value
}CAMFEATURE;

////////////////////////////////////////////////////////////////////////////////
// Type for format 7 descriptor.
////////////////////////////////////////////////////////////////////////////////

typedef struct
{
  UINT32                Offset;                 // Register offset
  UINT32                MaxImageSize;           // Max image size
  UINT32                UnitSize;               // Unit of image size
  UINT32                UnitPos;                // Unit of image position
  UINT32                ColorCoding;            // Standard color coding
  UINT32                VendorColorCoding[4];   // Vendor unknown color coding
}FORMAT7DSC;

////////////////////////////////////////////////////////////////////////////////
// Properties for a camera (camera specific properties).
////////////////////////////////////////////////////////////////////////////////

typedef struct
{
  UINT32HL              Guid;                   // Guid of this camera
  char                  VendorName[128];        // 0 terminated Vendorname
  char                  ModelName[128];         // 0 terminated Modelname
  UINT32                CmdRegBase;             // Address of command reg base
  UINT32                BasicInq;               // Basic function inquiry
  UINT32                UnitDirOffset;          // Unit directory offset
  UINT32                UnitDepDirOffset;       // Unit dependant directory offset
  UINT32                VendorLeafOffset;       // Vendor leaf offset
  UINT32                ModelLeafOffset;        // Model leaf offset
  UINT8                 FormatMask;             // Mask for videoformats
  UINT8                 ModeMask[8];            // Mask for videomodes
  UINT8                 FrameRateMask[8][8];    // Mask for framerate
  FORMAT7DSC            Format7Dsc[8];          // Format 7 descriptors
  UINT32                FirstImageFormat;       // First image format we support
  UINT32                FirstDcamFormat;        // First DCAM format
}CAMPROPS;

////////////////////////////////////////////////////////////////////////////////
// Parameters for camera (actual working parameters).
////////////////////////////////////////////////////////////////////////////////

typedef struct
{
  UINT32                ImageFormat;            // Actual image format
  UINT8                 Synchronized;           // Is equal to camera?
  
  UINT8                 VideoFormat;            // Actual videoformat
  UINT8                 VideoMode;              // Actual videomode
  UINT8                 FrameRate;              // Actual framerate
  UINT8                 ColorFormat;            // Actual ColorFormat
  UINT32                XPos;                   // X-Offset
  UINT32                YPos;                   // Y-Offset
  UINT32                XSize;                  // X-Dimension
  UINT32                YSize;                  // Y-Dimension
  UINT16                PktSize;                // Payload per packet
  UINT32                PktCnt;                 // Packets per frame

  UINT8                 IsoChn;                 // Real ISO channel in camera
  UINT8                 IsoSpeed;               // ISO speed
  UINT8                 AsySpeed;               // Asy speed
  UINT32                BufferCnt;              // Number of frames to allocate
  UINT32                RealBufferCnt;          // Really allocated frames
  UINT8                 DmaChn;                 // Dma channel used
  UINT16                DmaMaxPktSize;          // DMA max. packet size (resize!)
  UINT32                DmaMaxPktCnt;           // DMA max. packet count (resize!)
  UINT8                 Y16InversOrder;         // Inversmode for Y16 (obsolete)
  UINT8                 YIsRawMode;             // Raw mode flag (obsolete)
  UINT8                 BayerPattern;           // Used bayer pattern (obsolete)
  UINT16                PktOffset;              // Offset for each packet
  
  UINT8                 DmaMode;                // Dma mode
  UINT32                BurstCount;             // How many frames to produce
}CAMPARMS;

////////////////////////////////////////////////////////////////////////////////
// Resize parameters.
////////////////////////////////////////////////////////////////////////////////

typedef struct
{
  UINT32                XPos;                   // X-Offset
  UINT32                YPos;                   // Y-Offset
  UINT32                XSize;                  // X-Dimension
  UINT32                YSize;                  // Y-Dimension
  UINT16                PktSize;                // Payload per packet
}RESIZEPARMS;

////////////////////////////////////////////////////////////////////////////////
// User parameters.
////////////////////////////////////////////////////////////////////////////////

typedef struct
{
  UINT8                 AllocateBandwidth;      // Allocate ISO bandwidth
  UINT8                 AdjustParameters;       // Adjust parameters
  UINT8                 StartImmediately;       // Start burst immediately
  UINT8                 AllocateChannel;        // Allocate iso channel or not
  UINT8                 AcceptDelay;            // Accept delay after ISO parms
  UINT8                 AccessDelay;            // Access delay in ms
  UINT8                 DmaFlags;               // Flags for ISO DMA
}USERPARMS;

////////////////////////////////////////////////////////////////////////////////
// Declare node manager class.
////////////////////////////////////////////////////////////////////////////////

class CFGNodeMgr;
class CFGIsoDma;

////////////////////////////////////////////////////////////////////////////////
// Class for camera.
////////////////////////////////////////////////////////////////////////////////

class CSPEC CCamera
{
private:
  CIsoDma               m_IsoDma;               // We got one DMA
  CFGIsoDma*            m_Unused;

  CFGNodeMgr           *m_pNodeMgr;             // Our node manager
  void                 *m_pNode;                // Pointer to our system node
  UINT32                m_PktMinMax;            // Packet Min/Max (valid for PSFORMAT)
  UINT8                 m_DeviceSpeed;          // Speed of device (not route)
  UINT8                 m_MaxSpeed;             // Max. speed from SelfId

  CAMPROPS              m_Props;                // Camera properties (fix)
  CAMPARMS              m_Parms;                // Camera parameters (changeable)
  CAMFEATURE            m_Feature[CFT_LAST];    // Features
  USERPARMS             m_UserParms;            // User parameters
  RESIZEPARMS           m_ResizeParms;          // Resize parameters
  UINT8                 m_ResizeActive;         // Resize is active
  
  UINT32                m_LastResult;           // Last error on register access
  UINT8                 m_LastAckCode;          // Last AckCode on error
  UINT8                 m_LastResCode;          // Last ResCode on error
  
  UINT8                 m_Running;              // Camera is running
  
  UINT32                m_ImageFormatEnum;      // Actual enumerated image format
  UINT32                m_DcamFormatEnum;       // DCAM enumeration
  
  UINT8                 m_EnumIsDcam;           // Enumeration is DCAM
  CRITICAL_SECTION      m_AsyCS;                // For asynchronous access
  
  UINT8                 m_Card;                 // Our card number
  UINT32                m_BytesPerFrame;        // Information from camera
  UINT8                 m_IsoAllocated;         // ISO allocated or not
  void*                 m_NotifyContext;        // Notiyf context

public:
  virtual UINT32        LastResult();           // Last result (asy. access)
  virtual UINT8         LastAckCode();          // Last Ack code (asy. access)
  virtual UINT8         LastResCode();          // Last Res code (asy. access)
  
  virtual UINT8         GetCard();              // Get our card number
  virtual UINT8         GetIsoSpeed();          // Get real iso speed
  
  virtual UINT8         FindHighestBit(UINT8 Val);   // Find highest set bit
  virtual UINT8         FindHighestBit(UINT32 Val);  // Find highest set bit
  virtual UINT8         FindLowestBit(UINT8 Val);    // Find lowest set bit
  virtual UINT8         FindLowestBit(UINT32 Val);   // Find lowest set bit

  virtual CAMPARMS*     GetPtrParms();
  virtual CAMPROPS*     GetPtrProps();
  virtual void*         GetPtrNode();
  
  virtual UINT32        HandleError(UINT8 AckCode,UINT8 ResCode); // Handle error
  
  virtual UINT32        CalcPacketCnt();        // Calculate number of packets

  virtual UINT8         ReadLeaf(UINT32 Offset,UINT32 MaxSize,char *pStr); // Read leaf
  virtual UINT8         GetBasics();            // Get basic address stuff
  virtual UINT8         GetFormats();           // Get available formats
  virtual UINT8         GetModes();             // Get available modes
  virtual UINT8         GetFrameRates();        // Get available frame rates
  virtual UINT8         GetFormat7();           // Get format 7 properties
  virtual UINT32        FindDirKey(UINT32 StartAddress,UINT32 Cnt,UINT8 Key,UINT32 *pVal,
                                   UINT32 *pOffset);

  virtual UINT32        ReadFeature(UINT8 Which);
  virtual UINT8         ReadFeatures();         // Read all features
  virtual UINT32        WriteFeatureControl(UINT8 Which); // Write feature control
  virtual UINT32        ReadFeatureControl(UINT8 Which);  // Read feature control

  virtual UINT8         GetProperties();        // Get all above properties
  virtual void          FindLowestImageFormat();// Find lowest format
  virtual void          FindLowestDcamFormat(); // Find lowest DCAM format
  virtual void          AdjustParameters();     // Adjust parameters
  virtual UINT8         ReadParameters();       // Read all parameters

  virtual UINT32        GetFormat7PktMinMax();  // Acquire packet min max
  virtual UINT32        SelectFormat7();        // Select partial scan

  virtual UINT16        GetIsoPayload();        // Get iso payload
  virtual UINT32        StdFrameSize(UINT32 XSize,UINT32 YSize,UINT8 ColorFormat);
  virtual UINT32        GetFrameMemorySize(UINT32 *pSize);

  virtual UINT8         GetFormatMode(UINT8 Res,UINT8 Col,UINT8 Rate,UINT8 *pFormat,UINT8 *pMode);
  virtual UINT8         CheckFormatMode(UINT8 Format,UINT8 Mode,UINT8 Col,UINT8 Rate);
  virtual UINT8         GetResFromFormatMode(UINT8 Format,UINT8 Mode,UINT8 *pRes);
  virtual UINT32        GetBestImageFormat();
  virtual UINT32        AdjustImageFormat(UINT32 Val);
  
  virtual UINT32        SetImageFormat(UINT32 Val);
  virtual UINT32        GetImageFormat(UINT32 *pVal);

  virtual UINT32        SetEnumImageFormat(UINT32 Val);
  virtual UINT32        GetEnumImageFormat(UINT32 *pVal);
  
  virtual UINT32        SetFeature(UINT16 Nr,UINT32 Val);
  virtual UINT32        GetFeature(UINT16 Nr,UINT32 *pVal);
  virtual UINT32        GetFeatureInfo(UINT16 Nr,FGPINFO *pInfo);
  
  virtual UINT32        SetTrigger(UINT32 Val);
  virtual UINT32        GetTrigger(UINT32 *pVal);
  virtual UINT32        GetTriggerInfo(FGPINFO *pInfo);

  virtual UINT32        SetXSize(UINT32 Val);
  virtual UINT32        GetXSize(UINT32 *pVal);
  virtual UINT32        GetXSizeInfo(FGPINFO *pInfo);

  virtual UINT32        SetYSize(UINT32 Val);
  virtual UINT32        GetYSize(UINT32 *pVal);
  virtual UINT32        GetYSizeInfo(FGPINFO *pInfo);

  virtual UINT32        SetXPosition(UINT32 Val);
  virtual UINT32        GetXPosition(UINT32 *pVal);
  virtual UINT32        GetXPositionInfo(FGPINFO *pInfo);

  virtual UINT32        SetYPosition(UINT32 Val);
  virtual UINT32        GetYPosition(UINT32 *pVal);
  virtual UINT32        GetYPositionInfo(FGPINFO *pInfo);

  virtual UINT32        SetPacketSize(UINT32 Val);
  virtual UINT32        GetPacketSize(UINT32 *pVal);
  virtual UINT32        GetPacketSizeInfo(FGPINFO *pInfo);
  virtual UINT32        GetPacketCount(UINT32 *pVal);

  virtual UINT32        SetBurstCount(UINT32 Val);
  virtual UINT32        GetBurstCount(UINT32 *pVal);
  virtual UINT32        GetBurstCountInfo(FGPINFO *pInfo);
  
  virtual UINT32        SetFrameBufferCount(UINT32 Value);
  virtual UINT32        GetFrameBufferCount(UINT32 *pValue);
  virtual UINT32        GetFrameBufferCountInfo(FGPINFO *pInfo);
  
  virtual UINT32        SetPhySpeed(UINT32 Value);
  virtual UINT32        GetPhySpeed(UINT32 *pValue);
  virtual UINT32        GetPhySpeedInfo(FGPINFO *pInfo);

  virtual UINT32        SetWhiteBal(UINT8 Hi,UINT32 Value);
  virtual UINT32        GetWhiteBal(UINT8 Hi,UINT32 *pValue);
  virtual UINT32        GetWhiteBalInfo(UINT8 Hi,FGPINFO *pInfo);
  
  virtual UINT32        GetIrmBandwidth(UINT32 *pVal);
  virtual UINT32        GetCycleTime(UINT32 *pTime);
  
  virtual UINT32        GetR0CInfo(FGPINFO *pInfo);
  virtual UINT32        GetBusAddress(UINT32 *pValue);
  virtual UINT32        GetLicenseInfo(UINT32 *pValue);
  
  virtual UINT32        SetTimeout(UINT32 TimeoutInMs);
  virtual UINT32        GetTimeout(UINT32 *pTimeoutInMs);

                        CCamera(UINT32HL *pGuid,CFGNodeMgr *pNodeMgr);
  virtual              ~CCamera();

  virtual UINT32        OnStartup(UINT8 Speed,void* NotifyContext=NULL,UINT8 Card=0xFF);
  virtual void          OnShutdown();

  virtual UINT32        DoFastTrigger();
  virtual UINT32        DoBusTrigger();
  virtual UINT32        Reset();
  
  virtual UINT32        ReadRegister(UINT16 High,UINT32 Address,UINT32 *pValue);
  virtual UINT32        WriteRegister(UINT16 High,UINT32 Address,UINT32 Value);
  virtual UINT32        FastWriteRegister(UINT16 High,UINT32 Address,UINT32 Value);
  virtual UINT32        BusWriteRegister(UINT16 High,UINT32 Address,UINT32 Value);

  virtual UINT32        ReadBlock(UINT16 High,UINT32 Address,UINT8 *pBuf,UINT32 Size);
  virtual UINT32        WriteBlock(UINT16 High,UINT32 Address,UINT8 *pBuf,UINT32 Size);

  virtual UINT32        SetParameter(UINT16 Which,UINT32 Value);
  virtual UINT32        GetParameter(UINT16 Which,UINT32 *pValue);
  virtual UINT32        GetParameterInfo(UINT16 Which,FGPINFO *pInfo);

  virtual UINT32        DoResize();
  virtual UINT32        OnResize(UINT8 Enable);

  virtual UINT32        StartCamera();
  virtual UINT32        StopCamera();

  virtual UINT32        SetDmaMode(UINT32 Val);
  virtual UINT32        GetDmaMode(UINT32 *pVal);
  virtual UINT32        GetDmaModeInfo(FGPINFO *pInfo);

  virtual UINT32        StartIso();
  virtual UINT32        StopIso();
  
  virtual UINT32        AllocateIsoResources();
  virtual UINT32        FreeIsoResources();

  virtual UINT32        AssignUserBuffers(UINT32 Cnt,UINT32 Size,void* *ppMemArray);
  virtual UINT32        GetFrame(FGFRAME *pFrame,UINT32 TimeoutInMs);
  virtual UINT32        PutFrame(FGFRAME *pFrame);
  virtual UINT32        DiscardFrames();

  virtual void          RemoveNodeMgr();
  
  virtual UINT32        GetLicenseRequest(char *pStr,UINT32 MaxLen);
  
  virtual UINT8         GetRunningState();
  virtual void          SetRunningState(UINT8 val);
  virtual void          SetBytesPerFrame(UINT32 BytesPerFrame);
  virtual void          SetPacketMinMax(UINT32 MinMax);

  virtual UINT32        DoCardTrigger(UINT32 Mask);
  virtual UINT32        CardWriteRegister(UINT16 High,UINT32 Address,UINT32 Value,UINT32 Mask);
  
  virtual UINT32        DoCardReset();
};

#endif

