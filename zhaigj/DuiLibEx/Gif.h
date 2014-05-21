#pragma once

#include <vector>
using namespace std;

//-----------start define
#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)

typedef unsigned char UCHAR;
typedef UCHAR *   LPUSTR;
typedef unsigned short USHORT;
class GIFINFO
{
public:
	USHORT FileType;//89a or 87a 
	USHORT ColorType;//1,2,4,8,16,32,64,128,256
	USHORT ColorMode;//1,2,3,4,5,6,7,8
	USHORT Width;
	USHORT Height;
	USHORT FrameCount;//共有幾幅圖	//UINT           DataOffset[100];//小於FrameCount有效 ,包括補充區及數據區
	UCHAR  InitPixelBits;//壓縮數據起始數據Bits位數
};

class MYRGB
{
public:
	BYTE bRed;
	BYTE bGreen;
	BYTE bBlue;
};
class BMPIMAGE
{
public:
	BMPIMAGE();
	virtual ~BMPIMAGE();
	short tColor;
	USHORT DelayTime;
	USHORT Left;
	USHORT Top;
	USHORT Width;
	USHORT Height;
	RGBQUAD		   Palette[256];	
    LPUSTR         pImageData;
	BOOL   SaveMode;//1 = normal; 0 = 交叉
};
//------------------------------------------------------				
class GIFHEADER//GIF 文件頭
{
public:
	UCHAR Signature[6];//圖像格式，版本
	USHORT ScreenWidth;//圖像寬度
	USHORT ScreenDepth;//圖像高度
	UCHAR GlobalFlagByte;//全局標記 a0 -- a7 有含意
	UCHAR BackGroundColor;//背景色
	UCHAR AspectRadio;//89a版的長寬比
};

class IMAGEDATAINFO//圖像數據區識別信息
{
public:
	UCHAR ImageLabel;// default 0x2c
	USHORT ImageLeft; 
	USHORT  ImageTop;
	USHORT  ImageWidth;
	USHORT  ImageHeight;
	UCHAR  LocalFlagByte; 
};

class LZWTABLE
{
public:
	USHORT Header;
	USHORT Tail;
	USHORT Code; 
};


//below only for 89a
class GRAPHCTRL
{
public:
	UCHAR ExtIntr;// 0x21	
	UCHAR Label;//0xF9
	UCHAR BlockSize;//0x04
	UCHAR PackedField;
	USHORT          DelayTime;
	UCHAR TranColorIndex;
	UCHAR blockTerminator;//0x00
};
class NOTEHCTRL
{
public:
	UCHAR ExtIntr;// 0x21	
	UCHAR Label;//0xFE
	UCHAR Data[256];//變長最大256
	UCHAR blockTerminator;//0x00
};
class TEXTCTRL
{
public:	
	UCHAR ExtIntr;// 0x21	
	UCHAR Label;//0x01
	UCHAR BlockSize;//0x0c
	USHORT          Left;
	USHORT          Top;
	USHORT          Width;
	USHORT          Height; 
	UCHAR ForeColorIndex;
	UCHAR BkColorIndex;
	char		  Data[256];//變長最大256
	UCHAR blockTerminator;//0x00
};
class APPCTRL
{
public:
	UCHAR ExtIntr;// 0x21	
	UCHAR Label;//0xFF
	UCHAR BlockSize;//0x0b
	UCHAR Identifier[8];
	UCHAR Authentication[3];
	UCHAR Data[256];//變長最大256
	UCHAR blockTerminator;//0x00
};
//-----------------------------

class CGIF  
{
public:
	TImageInfo *GetNextImage();

	UINT GetDelayTime();

	BOOL LoadGIF(LPCTSTR GifFileName);

	BOOL LoadGIF(CHAR *bBuff,DWORD dSize);
	CGIF();
	virtual ~CGIF();

private:
	BOOL ConvertToBmpImage(LPUSTR SrcData);
	BOOL SaveBmp(LPCTSTR BmpFileName,LPUSTR pData);
	UINT GetImage(LPUSTR pData);
	UINT GetNoteContent(LPUSTR pNote);
	UINT GetAppContent(LPUSTR pApp);
	UINT ShowText(LPUSTR pText);
	UINT GetGrphContent(LPUSTR pGrCtrl);
	UINT AnalizeFileHeader(LPUSTR pFileContent);
	//從字符串中任意字節的某個Bits開始取若干bits,並返回結果
	USHORT GetOneCode(LPUSTR CodeStr ,UINT CodeStrLen, UINT OffsetChar , UINT OffsetBits, UINT Length);
	//當某指針指向文件中壓縮數據前位置時,取得當前數據區中壓縮數據所佔字節數
	UINT GetCodeCountOnChar (LPUSTR CodeStr ,UINT &AllDataLen);
	//當某指針指向文件中壓縮數據前位置時,取得當前數據區中壓縮數據
	LPUSTR GetCodeDataOnChar (LPUSTR CodeStr);
	//把壓縮數據解於USHORT * 中,
	LPUSTR GetCodeDataOnBits (LPUSTR CodeDataStr,UINT InitLen ,UINT &CodeDataLen);
	//得到部份真實數據
	void  GetPartImageDataFromTable(LPUSTR &pImage,LZWTABLE * Table,UINT TableLen);
	//API end

	
	GIFINFO m_GifInfo;
	LPUSTR   m_pDataArea;
	LZWTABLE LZWTable[5200];
	
	vector<TImageInfo*> m_ImageList;

	RGBQUAD m_CurRgbQuad[256];//Bitmap pal;
	short   m_CurTColorIndex;
	USHORT  m_CurDelayTime;
	BOOL    m_CurSaveMode;
	USHORT  m_CurX,m_CurY,m_CurWidth,m_CurHeight;

	UINT	m_uCurIndex;
};


BOOL BitBltEx(
  HDC hdcDest, // handle to destination device context
  int nXDest,  // x-coordinate of destination rectangle's upper-left 
               // corner
  int nYDest,  // y-coordinate of destination rectangle's upper-left 
               // corner
  int nWidth,  // width of destination rectangle
  int nHeight, // height of destination rectangle
  HDC hdcSrc,  // handle to source device context
  int nXSrc,   // x-coordinate of source rectangle's upper-left 
               // corner
  int nYSrc,   // y-coordinate of source rectangle's upper-left 
  COLORREF cTransparentColor      // corner
   // raster operation code
);

