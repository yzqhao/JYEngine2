#include "TextureLoader.h"
#include "Core/Interface/ILogSystem.h"
#include "Core/Interface/IFile.h"
#include "Core/Interface/IFileSystem.h"
#include "Engine/render/texture/TextureStream.h"
#include "Engine/render/texture/TextureFormat.h"

#include "png.h"
#include "pnginfo.h"
#include "pngstruct.h"
#include "jpeglib.h"
#include "libexif/exif-data.h"
#include "libyuv.h"

NS_JYE_BEGIN

bool TextureLoader::_IsJpgFile(const std::string& path)
{
	IFile* readfile = IFileSystem::Instance()->FileFactory(path);
	if (!readfile->OpenFile(IFile::AT_READ))
	{
		IFileSystem::Instance()->RecycleBin(readfile);
		return false;
	}
	uint8_t header[4];
	readfile->ReadFile(&header, sizeof(header));

	readfile->CloseFile();
	IFileSystem::Instance()->RecycleBin(readfile);

	// PNG Magic number：89 50 4e 47 0d 0a 1a 0a
	// JPG Magic number: FF D8 FF
	if (header[0] == 0xFF && header[1] == 0xD8 && header[2] == 0xFF)
	{
		JYWARNING("_IsJpgFile %s is jpeg with png postfix", path.c_str());
		return true;
	}
	else
	{
		return false;
	}
}

void* TextureLoader::_DoLoad(const String& path)
{
	if (!IFileSystem::Instance()->isFileExist(path))
	{
		JYERROR("unkown texture file %s", path.c_str());
	}
	else
	{
		const char* const cpath = path.c_str();
		const char* ext = strrchr(cpath, '.');
		if (ext)
		{
			ext++;
			if (!strcmp(ext, "tga") || !strcmp(ext, "TGA"))
			{
				return _LoadTga(path);
			}
			else if (!strcmp(ext, "png") || !strcmp(ext, "PNG"))
			{
				return _LoadPng(path);
			}
			else if (!strcmp(ext, "bmp") || !strcmp(ext, "BMP"))
			{
				return _LoadBmp(path);
			}
			else if (!strcmp(ext, "jpg") || !strcmp(ext, "JPG"))
			{
				return _LoadJpeg(path);
			}
		}
	}
	return _Error();
}

TextureStreams* TextureLoader::_LoadTga(std::string filename)
{
#define TGA_PUT_PIXEL(r, g, b, a) \
		int image_data_ofs = ((y*width) + x); \
		image_data[image_data_ofs * 4 + 0] = r; \
		image_data[image_data_ofs * 4 + 1] = g; \
		image_data[image_data_ofs * 4 + 2] = b; \
		image_data[image_data_ofs * 4 + 3] = a; \


	struct TARGA_HEADER
	{
		//字头的解释
		//http://blog.csdn.net/guanghua2009/article/details/6641665
		unsigned char   IDLength, ColormapType, ImageType;
		unsigned char   ColormapSpecification[5];
		unsigned short	XOrigin, YOrigin;
		unsigned short	ImageWidth, ImageHeight;
		unsigned char   PixelDepth;
		unsigned char   ImageDescriptor;
	};

	enum TARGA_ORIG_MODE   //图像起点
	{
		TGA_ORIGIN_BOTTOM_LEFT = 0x00,
		TGA_ORIGIN_BOTTOM_RIGHT = 0x01,
		TGA_ORIGIN_TOP_LEFT = 0x02,
		TGA_ORIGIN_TOP_RIGHT = 0x03,
		TGA_ORIGIN_SHIFT = 0x04,
		TGA_ORIGIN_MASK = 0x30
	};


	TARGA_HEADER header;
	TARGA_ORIG_MODE origMode;

	byte* pBits;
	uint nPixelSize;
	IFile* readfile;
	readfile = IFileSystem::Instance()->FileFactory(filename);
	if (!readfile->OpenFile(IFile::AT_READ))
	{
		return _Error();
	}

	readfile->ReadFile(&header, sizeof(header));
	nPixelSize = header.PixelDepth >> 3;


	pBits = _NEW byte[nPixelSize * header.ImageWidth * header.ImageHeight];//这里只分配，析构的时候会释放掉资源的

	if (2 == header.ImageType)//非压缩文理
	{
		readfile->ReadFile(pBits, nPixelSize * header.ImageWidth * header.ImageHeight);
	}
	else//读取压缩纹理
	{
		uint currentpixel = 0;												// Current pixel being read
		uint currentbyte = 0;												// Current byte 
		uint line = header.ImageHeight;
		byte* colorbuffer = _NEW byte[nPixelSize];//保存一个像素颜色的值
		do
		{
			byte chunkheader = 0; //chunk头
			readfile->ReadFile(&chunkheader, sizeof(byte));
			if (chunkheader < 128)												// If the ehader is < 128, it means the that is the number of RAW color packets minus 1
			{																	// that follow the header
				chunkheader++;													// add 1 to get number of following color values
				for (short counter = 0; counter < chunkheader; counter++)		// Read RAW color values
				{
					//if(fread(colorbuffer, 1, tga.bytesPerPixel, fTGA) != tga.bytesPerPixel) // Try to read 1 pixel
					readfile->ReadFile(colorbuffer, sizeof(byte) * nPixelSize);
					// write to memory
					pBits[currentbyte] = colorbuffer[2];				    // Flip R and B vcolor values around in the process 
					pBits[currentbyte + 1] = colorbuffer[1];
					pBits[currentbyte + 2] = colorbuffer[0];

					if (4 == nPixelSize)												// if its a 32 bpp image
					{
						pBits[currentbyte + 3] = colorbuffer[3];				// copy the 4th byte
					}

					currentbyte += nPixelSize;										// Increase thecurrent byte by the number of bytes per pixel
					currentpixel++;
					if (header.ImageWidth == currentpixel)
					{
						currentpixel = 0;
						line--;
						currentbyte = line * nPixelSize * header.ImageWidth;
					}
				}
			}
			else																			// chunkheader > 128 RLE data, next color reapeated chunkheader - 127 times
			{
				chunkheader -= 127;															// Subteact 127 to get rid of the ID bit
				//if(fread(colorbuffer, 1, tga.bytesPerPixel, fTGA) != tga.bytesPerPixel)		// Attempt to read following color values
				readfile->ReadFile(colorbuffer, sizeof(byte) * nPixelSize);
				for (short counter = 0; counter < chunkheader; counter++)					// copy the color into the image data as many times as dictated 
				{																			// by the header
					pBits[currentbyte] = colorbuffer[2];					// switch R and B bytes areound while copying
					pBits[currentbyte + 1] = colorbuffer[1];
					pBits[currentbyte + 2] = colorbuffer[0];

					if (4 == nPixelSize)												// If TGA images is 32 bpp
					{
						pBits[currentbyte + 3] = colorbuffer[3];				// Copy 4th byte
					}

					currentbyte += nPixelSize;										// Increase current byte by the number of bytes per pixel
					currentpixel++;															// Increase pixel count by 1
					if (header.ImageWidth == currentpixel)
					{
						currentpixel = 0;
						line--;
						currentbyte = line * nPixelSize * header.ImageWidth;
					}
				}
			}
		} while (0 < line);
		SAFE_DELETE_ARRAY(colorbuffer);
	}

	readfile->CloseFile();
	IFileSystem::Instance()->RecycleBin(readfile);

	//convert image
	unsigned int x_start;
	int x_step;
	unsigned int x_end;
	unsigned int y_start;
	int y_step;
	unsigned int y_end;
	unsigned int width = header.ImageWidth;
	unsigned int height = header.ImageHeight;
	origMode = static_cast<TARGA_ORIG_MODE>((header.ImageDescriptor & TGA_ORIGIN_MASK) >> TGA_ORIGIN_SHIFT); //起点模式
	if (origMode == TGA_ORIGIN_TOP_LEFT || origMode == TGA_ORIGIN_TOP_RIGHT) {
		y_start = 0;
		y_step = 1;
		y_end = height;
	}
	else {
		y_start = height - 1;
		y_step = -1;
		y_end = -1;
	}
	if (origMode == TGA_ORIGIN_TOP_LEFT || origMode == TGA_ORIGIN_BOTTOM_LEFT) {
		x_start = 0;
		x_step = 1;
		x_end = width;
	}
	else {
		x_start = width - 1;
		x_step = -1;
		x_end = -1;
	}

	TextureStreams* res = _NEW TextureStreams();
	TextureStream* ts = _NEW TextureStream();
	res->push_back(ts);
	ts->SetStreamType(Math::IntVec2(header.ImageWidth, header.ImageHeight), RHIDefine::PixelFormat::PF_R8G8B8A8);
	unsigned char* image_data = ts->GetBuffer();//这里只分配，析构的时候会释放掉资源的 and 全部转换为RGBA

	size_t i = 0;
	unsigned int x = x_start;
	unsigned int y = y_start;

	//（NOTE:单色的不处理)
	if (header.PixelDepth == 24) {
		while (y != y_end) {
			while (x != x_end) {
				uint8_t r = pBits[i + 2];
				uint8_t g = pBits[i + 1];
				uint8_t b = pBits[i + 0];
				TGA_PUT_PIXEL(r, g, b, 0xff)
					x += x_step;
				i += 3;
			}
			x = x_start;
			y += y_step;
		}
	}
	else if (header.PixelDepth == 32) {
		while (y != y_end) {
			while (x != x_end) {
				uint8_t a = pBits[i + 3];
				uint8_t r = pBits[i + 2];
				uint8_t g = pBits[i + 1];
				uint8_t b = pBits[i + 0];
				TGA_PUT_PIXEL(r, g, b, a)
					x += x_step;
				i += 4;
			}
			x = x_start;
			y += y_step;
		}
	}
	SAFE_DELETE_ARRAY(pBits);
	return res;
}

static void png_rw(png_structp png_ptr, png_bytep data, png_size_t length)
{
	IFile* file = static_cast<IFile*>(png_ptr->io_ptr);
	file->ReadFile(data, length);
}

static void user_error_fn(png_structp png_ptr, png_const_charp error_msg)
{
	JYERROR("_LoadPng %s ", error_msg);
}

static void user_warning_fn(png_structp png_ptr, png_const_charp warning_msg)
{
	JYWARNING("_LoadPng %s ", warning_msg);
}

TextureStreams* TextureLoader::_LoadPng(std::string filename)
{
	png_structp PngPtr = nullptr;
	png_infop InfoPtr = nullptr;
	IFile* readfile = nullptr;

	readfile = IFileSystem::Instance()->FileFactory(filename);
	if (!readfile->OpenFile(IFile::AT_READ))
	{
		JYERROR("png open file fail");
		return _Error();
	}

	/* initialise structures for reading a png file */
	PngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, user_error_fn, user_warning_fn);
	InfoPtr = png_create_info_struct(PngPtr);
	png_set_sig_bytes(PngPtr, 0);
	png_set_chunk_malloc_max(PngPtr, 0);

	if (setjmp(png_jmpbuf(PngPtr)))
	{
		JYERROR("png file parse fail");
		png_destroy_read_struct(&PngPtr, &InfoPtr, 0);
		IFileSystem::Instance()->RecycleBin(readfile);

		return _Error();
	}

	//相同代码
	png_set_read_fn(PngPtr, readfile, png_rw);//调用回调函数读取数据

	//png_read_png(PngPtr, InfoPtr, PNG_TRANSFORM_EXPAND, 0);
	png_read_info(PngPtr, InfoPtr); // 先不解码 获取图片信息

	int width = png_get_image_width(PngPtr, InfoPtr);
	int height = png_get_image_height(PngPtr, InfoPtr);
	int color_type = png_get_color_type(PngPtr, InfoPtr);
	int channel = png_get_channels(PngPtr, InfoPtr);
	int bytedepth = png_get_bit_depth(PngPtr, InfoPtr);

	// libpng example.c
	// void read_png(FILE *fp, unsigned int sig_read) 
	if (bytedepth == 16)
	{
		png_set_strip_16(PngPtr);   
	}

	if (color_type == PNG_COLOR_TYPE_PALETTE)
	{
		png_set_palette_to_rgb(PngPtr);
	}

	if (color_type == PNG_COLOR_TYPE_GRAY && bytedepth < 8)
	{
		png_set_expand_gray_1_2_4_to_8(PngPtr);
	}

	// expand any tRNS chunk data into a full alpha channel
	if (png_get_valid(PngPtr, InfoPtr, PNG_INFO_tRNS))
	{
		png_set_tRNS_to_alpha(PngPtr);
	}

	if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
	{
		png_set_gray_to_rgb(PngPtr);
	}

	boolean needScale = false;
	if (width > TEXTURE_LIMIT::LIMIT_SIZE || height > TEXTURE_LIMIT::LIMIT_SIZE)
	{
		png_set_add_alpha(PngPtr, 0xFFFF, PNG_FILLER_AFTER);
		JYWARNING("_LoadPng png_set_add_alpha width %d height %d color_type %d channel %d depth %d %s",
			width, height, color_type, channel, bytedepth, filename.c_str());
		needScale = true;
	}

	png_read_update_info(PngPtr, InfoPtr);

	// update info 
	width = png_get_image_width(PngPtr, InfoPtr);
	height = png_get_image_height(PngPtr, InfoPtr);
	color_type = png_get_color_type(PngPtr, InfoPtr);
	channel = png_get_channels(PngPtr, InfoPtr);
	bytedepth = png_get_bit_depth(PngPtr, InfoPtr);

	//分辨是三通道还是四通道读取
	TextureStream* ts = _NEW TextureStream();
	ts->SetStreamType(Math::IntVec2(width, height), TextureFormat::FormatMapping(channel));
	byte* data = ts->GetBuffer();
	//png_bytep* row_pointers = png_get_rows(PngPtr, InfoPtr);
	//memcpy( data, row_pointers, size );//内存不一定是连续的,需要用下面那种方法
	int lineSize = png_get_rowbytes(PngPtr, InfoPtr);
	if (lineSize != width * channel)
	{
		JYERROR("png error lineSize=%d, w=%d, h=%d, channel=%d, color_type=%d",
			lineSize, width, height, channel, color_type);
		JY_ASSERT(lineSize == width * channel); // 8bit depth 
	}

	std::vector<png_bytep> row_pointers{ (uint32_t)height };
	for (int row = 0; row < height; row++)
	{
		row_pointers[row] = data + (lineSize * row);
	}

	png_read_image(PngPtr, row_pointers.data());


	png_destroy_read_struct(&PngPtr, &InfoPtr, 0);

	readfile->CloseFile();
	IFileSystem::Instance()->RecycleBin(readfile);

	if (needScale)
	{
		int targetWidth = TEXTURE_LIMIT::LIMIT_SIZE;
		int targetHeight = TEXTURE_LIMIT::LIMIT_SIZE;
		if (width > height)
		{
			targetHeight = targetWidth * height / width;
		}
		else
		{
			targetWidth = targetHeight * width / height;
		}

		JYWARNING("_LoadPng scale (%d,%d) -> (%d,%d) color_type %d channel %d depth %d",
			width, height,
			targetWidth, targetHeight,
			color_type, channel, bytedepth);


		TextureStream* ts3 = _NEW TextureStream();
		ts3->SetStreamType(Math::IntVec2(targetWidth, targetHeight), TextureFormat::FormatMapping(channel));
		byte* pDstData = ts3->GetBuffer();
		byte* pSrtData = ts->GetBuffer();

		width = targetWidth;
		height = targetHeight;

		SAFE_DELETE(ts);
		ts = ts3;
	}

	TextureStreams* res = _NEW TextureStreams();
	res->push_back(ts);

	return res;
}

TextureStreams* TextureLoader::_LoadBmp(std::string filename)
{
#pragma pack(1)
	struct BmpFileHeader //文件头
	{
		unsigned short bfType;		//标识该文件为bmp文件,判断文件是否为bmp文件，即用该值与"0x4d42"比较是否相等即可，0x4d42 = 19778
		unsigned int  bfSize;		//文件大小
		unsigned short bfReserved1;	//预保留位
		unsigned short bfReserved2;	//预保留位
		unsigned int  bfOffBits;	//图像数据区的起始位置
	};//14字节
	struct BmpInfoHeader //信息头
	{
		unsigned int   	biSize;	//图像数据大小
		unsigned int   biWidth;	//宽度
		unsigned int   biHeight;	//高度
		unsigned short 	biPlanes;//为1
		unsigned short 	biBitCount; //像素位数，8-灰度图；24-真彩色
		unsigned int   longbiCompression;//压缩方式
		unsigned int   longbiSizeImage;  //图像区数据大小
		unsigned int   biXPelsPerMeter;  //水平分辨率，像素每米
		unsigned int   biYPelsPerMeter;
		unsigned int   	biClrUsed;   //位图实际用到的颜色数
		unsigned int   	biClrImportant;//位图显示过程，重要的颜色数；0--所有都重要
	};//40字节
	struct RGBPallete //调色板
	{
		unsigned char b;
		unsigned char g;
		unsigned char r;
		unsigned char alpha; //预保留位
	};
#pragma pack()
	BmpFileHeader pFileHeader;		// bmp文件头
	BmpInfoHeader pInfoHeader;		// bmp头信息
	RGBPallete rq[256];			// 调色板
	byte* data;
	byte* pData;
	// 打开图像文件 
	IFile* readfile;
	readfile = IFileSystem::Instance()->FileFactory(filename);
	if (!readfile->OpenFile(IFile::AT_READ))
	{
		return _Error();
	}
	// 读取文件头
	readfile->ReadFile(&pFileHeader.bfType, sizeof(pFileHeader.bfType));
	readfile->ReadFile(&pFileHeader.bfSize, sizeof(pFileHeader.bfSize));
	readfile->ReadFile(&pFileHeader.bfReserved1, sizeof(pFileHeader.bfReserved1));
	readfile->ReadFile(&pFileHeader.bfReserved2, sizeof(pFileHeader.bfReserved2));
	readfile->ReadFile(&pFileHeader.bfOffBits, sizeof(pFileHeader.bfOffBits));
	// 读取图像信息
	readfile->ReadFile(&pInfoHeader.biSize, sizeof(pInfoHeader.biSize));
	readfile->ReadFile(&pInfoHeader.biWidth, sizeof(pInfoHeader.biWidth));
	readfile->ReadFile(&pInfoHeader.biHeight, sizeof(pInfoHeader.biHeight));
	readfile->ReadFile(&pInfoHeader.biPlanes, sizeof(pInfoHeader.biPlanes));
	readfile->ReadFile(&pInfoHeader.biBitCount, sizeof(pInfoHeader.biBitCount));
	readfile->ReadFile(&pInfoHeader.longbiCompression, sizeof(pInfoHeader.longbiCompression));
	readfile->ReadFile(&pInfoHeader.longbiSizeImage, sizeof(pInfoHeader.longbiSizeImage));
	readfile->ReadFile(&pInfoHeader.biXPelsPerMeter, sizeof(pInfoHeader.biXPelsPerMeter));
	readfile->ReadFile(&pInfoHeader.biYPelsPerMeter, sizeof(pInfoHeader.biYPelsPerMeter));
	readfile->ReadFile(&pInfoHeader.biClrUsed, sizeof(pInfoHeader.biClrUsed));
	readfile->ReadFile(&pInfoHeader.biClrImportant, sizeof(pInfoHeader.biClrImportant));

	int Height = pInfoHeader.biHeight;
	int Width = pInfoHeader.biWidth;
	bool reverse = false;
	if (Height < 0)
	{
		Height = -1 * Height;
		reverse = true;//左下角和左上角为起点的区别，此处暂未处理
	}

	RHIDefine::PixelFormat t_pf = RHIDefine::PixelFormat::PF_AUTO;

	switch (pInfoHeader.biBitCount)
	{
	case 8:
		if (pFileHeader.bfOffBits - 1024 < 54)
		{
			return _Error();
		}
		data = new byte[Width * Height];
		pData = new byte[Width * Height * 3];
		// 定位调色板，并读取调色板
		readfile->Seek(pFileHeader.bfOffBits - 1024, SEEK_SET);
		readfile->ReadFile(rq, sizeof(RGBPallete) * 256);
		// 读取位图
		readfile->ReadFile(data, Width * Height);

		for (int i = 0; i < Width * Height; i++)
		{
			pData[i * 3] = rq[data[i]].r;
			pData[i * 3 + 1] = rq[data[i]].g;
			pData[i * 3 + 2] = rq[data[i]].b;
		}
		SAFE_DELETE_ARRAY(data);
		t_pf = RHIDefine::PixelFormat::PF_R8G8B8;
		break;
	case 24:
	{
		data = new byte[Height * Width * 3];
		pData = new byte[Height * Width * 3];

		readfile->ReadFile(data, Height * Width * 3);

		for (int i = 0; i < Height * Width; i++)
		{
            std::swap(pData[i * 3], pData[i * 3 + 2]);
		}

		SAFE_DELETE_ARRAY(data);
		t_pf = RHIDefine::PixelFormat::PF_R8G8B8;
		break;
	}
	case 32:
	{
		pData = new byte[Height * Width * 4];
		readfile->ReadFile(pData, Height * Width * 4);
		t_pf = RHIDefine::PixelFormat::PF_R8G8B8A8;
		break;
	}
	default:
		return _Error();
	}

	TextureStreams* res = _NEW TextureStreams();
	TextureStream* ts = _NEW TextureStream();
	res->push_back(ts);
	ts->SetStreamType(Math::IntVec2(Width, Height), t_pf);
	byte* thedata = ts->GetBuffer();
	memcpy(thedata, pData, Height * Width * 4);

	SAFE_DELETE_ARRAY(pData);
	readfile->CloseFile();
	IFileSystem::Instance()->RecycleBin(readfile);

	return res;
}

struct my_error_mgr {
	struct jpeg_error_mgr pub;

	jmp_buf setjmp_buffer;
};
typedef struct my_error_mgr* my_error_ptr;
METHODDEF(void) my_error_exit(j_common_ptr cinfo)
{
	my_error_ptr myerr = (my_error_ptr)cinfo->err;

	char jpegLastErrorMsg[JMSG_LENGTH_MAX];
	(*(cinfo->err->format_message)) (cinfo, jpegLastErrorMsg);
	jpegLastErrorMsg[JMSG_LENGTH_MAX - 1] = '\0';

	JYERROR("_LoadJpeg fail %s ", jpegLastErrorMsg);

	longjmp(myerr->setjmp_buffer, 1);
}

enum TextureLoader::JPEGOrientation TextureLoader::_GetOrientation(const std::string& filename)
{
	/*
		libexif 引入和编译说明

		http://sourceforge.net/projects/libexif/files/libexif/0.6.21/

		使用msys.bat和vs2015 native tools command prompt

		config.h auto generate by
		autoconf
		./configure --host=i586-mingw32msvc --disable-nls --enable-ship-binaries
		不用构建 直接拷贝 config.h 和其他libexif目录下的源文件  到third/libexif工程目录下

		config.h 需要格外增加 如下

		#if defined(_MSC_VER)
		#include <BaseTsd.h>
		typedef SSIZE_T ssize_t;
		#endif



	*/
	// 处理jpeg图片旋转问题 
	enum JPEGOrientation orientation = JPEGOrientation::DEFAULT;
	ExifData* ed = exif_data_new_from_file(IFileSystem::Instance()->PathAssembly(filename).c_str());
	if (ed)
	{
		ExifByteOrder byteOrder = exif_data_get_byte_order(ed);
		ExifEntry* exifEntry = exif_data_get_entry(ed, EXIF_TAG_ORIENTATION);
		if (exifEntry)
		{
			int ori = exif_get_short(exifEntry->data, byteOrder);
			orientation = (enum JPEGOrientation)ori;
		}

		/*
			0th Row      0th Column
			1   top          left side
			2   top          right side   左右镜像
			3   bottom       right side
			4   bottom       left side
			5   left side    top
			6   right side   top
			7   right side   bottom
			8   left side    bottom    90度CW  顺时针旋转了90度
		*/
		exif_data_unref(ed);
	}
	return orientation;
}

TextureStreams* TextureLoader::_LoadJpeg(std::string filename)
{
	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;
	JSAMPARRAY buffer;
	int row_stride;

	IFile* readfile;
	byte* pBits;

	enum JPEGOrientation orientation = _GetOrientation(filename);

	readfile = IFileSystem::Instance()->FileFactory(filename);
	if (!readfile->OpenFile(IFile::AT_READ))
	{
		return _Error();
	}

	uint fileSize = readfile->GetSize();
	pBits = _NEW byte[fileSize];
	readfile->ReadFile(pBits, fileSize);

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	if (setjmp(jerr.setjmp_buffer))
	{
		jpeg_destroy_decompress(&cinfo);
		readfile->CloseFile();
		IFileSystem::Instance()->RecycleBin(readfile);
		return 0;
	}
	jpeg_create_decompress(&cinfo);
	jpeg_mem_src(&cinfo, pBits, fileSize);


	(void)jpeg_read_header(&cinfo, TRUE);
	if (orientation != JPEGOrientation::DEFAULT)
	{
		cinfo.out_color_space = JCS_EXT_RGBX;
		// 修改后 alpha通道会覆盖为0xFF
		// cinfo.output_components 原来是3会返回4 
		// libyuv没有找到旋转rgb的 所以解码成为rgba
	}

	boolean needScale = false;
	jpeg_calc_output_dimensions(&cinfo);

	if (cinfo.output_width > TEXTURE_LIMIT::LIMIT_SIZE || cinfo.output_height > TEXTURE_LIMIT::LIMIT_SIZE)
	{
		cinfo.out_color_space = JCS_EXT_RGBX;
		needScale = true;
	}

	(void)jpeg_start_decompress(&cinfo);

	int width = cinfo.output_width;
	int height = cinfo.output_height;
	int channel = cinfo.output_components;

	TextureStream* ts = _NEW TextureStream();
	ts->SetStreamType(Math::IntVec2(width, height), TextureFormat::FormatMapping(channel));
	byte* pData = ts->GetBuffer();

	row_stride = cinfo.output_width * cinfo.output_components;
	buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);

	while (cinfo.output_scanline < cinfo.output_height) {

		(void)jpeg_read_scanlines(&cinfo, buffer, 1);
		memcpy(pData, buffer[0], row_stride);
		pData += row_stride;

	}

	(void)jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	SAFE_DELETE_ARRAY(pBits);

	readfile->CloseFile();
	IFileSystem::Instance()->RecycleBin(readfile);

	if (needScale)
	{
		int targetWidth = TEXTURE_LIMIT::LIMIT_SIZE;
		int targetHeight = TEXTURE_LIMIT::LIMIT_SIZE;
		if (width > height)
		{
			targetHeight = targetWidth * height / width;
		}
		else
		{
			targetWidth = targetHeight * width / height;
		}

		JYERROR("_LoadJpeg scale (%d,%d) -> (%d,%d) color_space %d channel %d",
			width, height,
			targetWidth, targetHeight,
			cinfo.out_color_space, channel);

		TextureStream* ts3 = _NEW TextureStream();
		ts3->SetStreamType(Math::IntVec2(targetWidth, targetHeight), TextureFormat::FormatMapping(channel));
		byte* pDstData = ts3->GetBuffer();
		byte* pSrtData = ts->GetBuffer();

		libyuv::ARGBScale(
			pSrtData, width * channel, width, height,
			pDstData, targetWidth * channel, targetWidth, targetHeight,
			libyuv::FilterMode::kFilterBilinear);

		width = targetWidth;
		height = targetHeight;

		SAFE_DELETE(ts);
		ts = ts3;
	}

	// 处理jpeg图片旋转问题 
	if (orientation != JPEGOrientation::DEFAULT)
	{
		JYERROR("JPEG Decode orientation = %d width = %d height = %d channel = %d",
			orientation, width, height, channel);
		TextureStream* ts2 = NULL;

		switch (orientation)
		{
		case JPEGOrientation::LEFT_BOTTOM: // 前摄像头  顺时针转270就是正图
		{
			ts2 = _NEW TextureStream();
			ts2->SetStreamType(Math::IntVec2(height, width), TextureFormat::FormatMapping(channel));// 宽高转置
			byte* pDstData = ts2->GetBuffer();
			byte* pSrtData = ts->GetBuffer();
			// libyuv::RotatePlane  Plane平面 要求是连续buffer 
			libyuv::ARGBRotate(pSrtData, width * channel, pDstData, height * channel, width, height, libyuv::RotationMode::kRotate270);
			break;
		}
		case JPEGOrientation::RIGHT_TOP: // 后摄像头  顺时针旋转90就是正图 
		{
			ts2 = _NEW TextureStream();
			ts2->SetStreamType(Math::IntVec2(height, width), TextureFormat::FormatMapping(channel)); // 宽高转置
			byte* pDstData = ts2->GetBuffer();
			byte* pSrtData = ts->GetBuffer();
			libyuv::ARGBRotate(pSrtData, width * channel, pDstData, height * channel, width, height, libyuv::RotationMode::kRotate90);
			break;
		}
		case JPEGOrientation::BOTTOM_RIGHT: // 顺时针转180就是正图
		{
			ts2 = _NEW TextureStream();
			ts2->SetStreamType(Math::IntVec2(width, height), TextureFormat::FormatMapping(channel));
			byte* pDstData = ts2->GetBuffer();
			byte* pSrtData = ts->GetBuffer();
			libyuv::ARGBRotate(pSrtData, width * channel, pDstData, width * channel, width, height, libyuv::RotationMode::kRotate180);
			break;
		}
		case JPEGOrientation::TOP_RIGHT:  // 0度 左右镜像 
		{
			ts2 = _NEW TextureStream();
			ts2->SetStreamType(Math::IntVec2(width, height), TextureFormat::FormatMapping(channel));
			byte* pDstData = ts2->GetBuffer();
			byte* pSrtData = ts->GetBuffer();
			// 只是左右镜像  -height
			libyuv::ARGBRotate(pSrtData, width * channel, pDstData, width * channel, width, -height, libyuv::RotationMode::kRotate0);
			break;
		}
		case JPEGOrientation::BOTTOM_LEFT: // 180 + 镜像 
		{
			ts2 = _NEW TextureStream();
			ts2->SetStreamType(Math::IntVec2(width, height), TextureFormat::FormatMapping(channel));
			byte* pDstData = ts2->GetBuffer();
			byte* pSrtData = ts->GetBuffer();
			libyuv::ARGBRotate(pSrtData, width * channel, pDstData, width * channel, width, -height, libyuv::RotationMode::kRotate180);
			break;
		}
		case JPEGOrientation::LEFT_TOP: // 
		{
			ts2 = _NEW TextureStream();
			ts2->SetStreamType(Math::IntVec2(width, height), TextureFormat::FormatMapping(channel));
			byte* pDstData = ts2->GetBuffer();
			byte* pSrtData = ts->GetBuffer();
			libyuv::ARGBRotate(pSrtData, width * channel, pDstData, height * channel, width, -height, libyuv::RotationMode::kRotate90);
			break;
		}
		case JPEGOrientation::RIGHT_BOTTOM:
		{
			ts2 = _NEW TextureStream();
			ts2->SetStreamType(Math::IntVec2(width, height), TextureFormat::FormatMapping(channel));
			byte* pDstData = ts2->GetBuffer();
			byte* pSrtData = ts->GetBuffer();
			libyuv::ARGBRotate(pSrtData, width * channel, pDstData, height * channel, width, -height, libyuv::RotationMode::kRotate270);
			break;
		}

		}

		if (ts2 != NULL)
		{
			SAFE_DELETE(ts);
			ts = ts2;
		}
	}

	TextureStreams* res = _NEW TextureStreams();
	res->push_back(ts);
	return res;
}

TextureStreams* TextureLoader::_Error()
{
	byte buffer[2 * 2 * 3] = { 255,0,255,255,0,255 ,255,0,255 ,255,0,255 };
	TextureStreams* res = _NEW TextureStreams();
	TextureStream* ts = _NEW TextureStream();
	ts->SetStreamType(Math::IntVec2(2, 2), RHIDefine::PF_R8G8B8);
	ts->SetBuffer(buffer);
	res->push_back(ts);
	return res;
}

void TextureLoader::Release(void* data)
{
	if (data)
	{
		TextureStreams* todelete = static_cast<TextureStreams*>(data);
		for (TextureStream* ts : *todelete)
		{
			SAFE_DELETE(ts);
		}
		SAFE_DELETE(todelete);
	}
}

ILoader* TextureLoader::CreateCacheLoader() const
{
	return NULL;
}


NS_JYE_END