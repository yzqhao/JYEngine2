#include <string>
#include <iostream>
#include <direct.h>
#include <stdio.h>
#include <io.h>
#include <fstream>
#include <sys/stat.h>
#include <errno.h>
#include <assert.h>
#include "../../third/zipper/zipper/CDirEntry.h"
#include <Windows.h>
#include "lz4frame.h"
#include "lz4hc.h"
#include "zip.h"
using namespace std;

//压缩文件相关
static const char* shaderCodeSuffix = ".code";
static const char* archiveSuffix = ".pkg";
static const char* compressionSuffix = ".lz4";
static const char* dictCompressionSuffix = ".lzd";
static const int compSuffixLen = 4;
static const int AIresource = -1000;

#define IN_CHUNK_SIZE  (16*1024)
#define RETURN_SUCCESS 0
#define RETURN_FAILURE 1
#define DICTIONARY_BYTES (1024 * 60)

std::vector<unsigned char>         decompressBuffer;
unsigned                  decompressBufferSize = 0;
unsigned                  decompressPos = 0;

static const char dictRelaFile[] = "../Tools/Lz4ZipCompression/dictionary_89.dic";
string dictPath;
static const int dictID = 89;

void printHelp()
{
	printf("-h/--help : print help \n");
	printf("-p/--pack : path of the destination package file \n");
	printf("-u/--unpack : path of the source package file \n");
	printf("-s/--source : path of the source directory to be compressed \n");
	printf("-d/--destination : path of the destination directory to be decompressed to \n");
	printf("-D/--dictionary : compress or decompress with dictionary \n");
}

typedef struct {
	int error;
	unsigned size_in;
	unsigned size_out;
} compressResult_t;

//---------------------------------------------------------------------------------------------------------
static compressResult_t
compress_file_internal(FILE* inFp, vector<char>& fDst,
	LZ4F_compressionContext_t ctx, const LZ4F_preferences_t& kPrefs,
	vector<char>& inBuff,
	vector<char>& outBuff)
{
	compressResult_t result = { 1, 0, 0 };  /* result for an error */
	unsigned count_in = 0, count_out;
	unsigned outCapacity = outBuff.size();

	assert(inFp != NULL);
	assert(ctx != NULL);
	assert(outCapacity >= LZ4F_HEADER_SIZE_MAX);


	/* write frame header */
	{
		size_t const headerSize = LZ4F_compressBegin(ctx, outBuff.data(), outCapacity, &kPrefs);
		if (LZ4F_isError(headerSize))
		{
			printf("Failed to start compression: error %u \n", (unsigned)headerSize);
			return result;
		}
		count_out = headerSize;
		fDst.resize(headerSize);
		memcpy(fDst.data(), outBuff.data(), headerSize);
	}

	/* stream file */
	for (;;)
	{
		size_t readSize = fread( inBuff.data(), 1, IN_CHUNK_SIZE, inFp);
		if (readSize == 0)
		{
			break; /* nothing left to read from input file */
		}
		count_in += readSize;

		size_t const compressedSize = LZ4F_compressUpdate(ctx,
			outBuff.data(), outCapacity,
			inBuff.data(), readSize,
			NULL);
		if (LZ4F_isError(compressedSize))
		{
			printf("Compression failed: error %u \n", (unsigned)compressedSize);
			return result;
		}

		//printf("Writing %u bytes\n", (unsigned)compressedSize);
		fDst.resize(count_out + compressedSize);
		memcpy(fDst.data() + count_out, outBuff.data(), compressedSize);
		count_out += compressedSize;
	}

	/* flush whatever remains within internal buffers */
	{
		size_t const compressedSize = LZ4F_compressEnd(ctx,
			outBuff.data(), outCapacity,
			NULL);
		if (LZ4F_isError(compressedSize))
		{
			printf("Failed to end compression: error %u \n", (unsigned)compressedSize);
			return result;
		}

		//printf("Writing %u bytes \n", (unsigned)compressedSize);
		fDst.resize(count_out + compressedSize);
		memcpy(fDst.data() + count_out, outBuff.data(), compressedSize);
		count_out += compressedSize;
	}

	result.size_in = count_in;
	result.size_out = count_out;
	result.error = 0;
	return result;
}
//---------------------------------------------------------------------------------------------------------
static compressResult_t
compress_file_internal_dict(FILE* inFp, LZ4F_CDict* dict, vector<char>& fDst,
							LZ4F_compressionContext_t ctx, const LZ4F_preferences_t& kPrefs,
							vector<char>& inBuff,
							vector<char>& outBuff)
{
	compressResult_t result = { 1, 0, 0 };  /* result for an error */
	unsigned count_in = 0, count_out = 0;
	unsigned outCapacity = outBuff.size();

	assert(inFp != NULL);
	assert(ctx != NULL);
	assert(outCapacity >= LZ4F_HEADER_SIZE_MAX);

	size_t readSize = fread(inBuff.data(), 1, kPrefs.frameInfo.contentSize, inFp);
	if (readSize == 0)
	{
		return result; /* nothing left to read from input file */
	}
	count_in += readSize;
	size_t const compressedSize = LZ4F_compressFrame_usingCDict(ctx,
						outBuff.data(), outCapacity,
						inBuff.data(), readSize,
						dict,
						&kPrefs);
	if (LZ4F_isError(compressedSize))
	{
		printf("Compression failed: error %u \n", (unsigned)compressedSize);
		return result;
	}

	//printf("Writing %u bytes\n", (unsigned)compressedSize);
	fDst.resize(count_out + compressedSize);
	memcpy(fDst.data() + count_out, outBuff.data(), compressedSize);
	count_out += compressedSize;


	result.size_in = count_in;
	result.size_out = count_out;
	result.error = 0;
	return result;
}
//---------------------------------------------------------------------------------------------------------
static compressResult_t compress_file(FILE* inFp, vector<char>& fDst)
{
	/* ressource allocation */
	LZ4F_compressionContext_t ctx;
	size_t const ctxCreation = LZ4F_createCompressionContext(&ctx, LZ4F_VERSION);

	auto currentPos = ftell(inFp);
	fseek(inFp, 0, SEEK_END);
	unsigned inFileSize = ftell(inFp);
	fseek(inFp, 0, currentPos);

	const LZ4F_preferences_t kPrefs = {
			{ LZ4F_max256KB, LZ4F_blockLinked, LZ4F_noContentChecksum, LZ4F_frame,
			  inFileSize /* known content size */, 0 /* no dictID */ , LZ4F_noBlockChecksum },
			 0 /*LZ4HC_CLEVEL_MIN*/ /*LZ4HC_CLEVEL_MAX*/,   /* compression level; 0 == default */
			  0,   /* autoflush */
			  /*1*/ 0,   /* favor decompression speed */
			  { 0, 0, 0 },  /* reserved, must be set to 0 */
	};

	try
	{
		vector<char> srcBuff(IN_CHUNK_SIZE);
		size_t const outbufCapacity = LZ4F_compressBound(IN_CHUNK_SIZE, &kPrefs);   /* large enough for any input <= IN_CHUNK_SIZE */
		vector<char> outBuff(outbufCapacity);
		compressResult_t result = { 1, 0, 0 };  /* == error (default) */
		if (!LZ4F_isError(ctxCreation))
		{
			result = compress_file_internal(inFp, fDst,
				ctx, kPrefs,
				srcBuff, outBuff);
		}
		else
		{
			printf("ressource allocation failed \n");
		}

		LZ4F_freeCompressionContext(ctx);   /* supports free on NULL */
		return result;
	}
	catch (bad_alloc)
	{
		printf("Allocation Failed!");
	}
}
//---------------------------------------------------------------------------------------------------------
static compressResult_t compress_file_dict(FILE* inFp, LZ4F_CDict* dict, vector<char>& fDst)
{
	/* ressource allocation */
	LZ4F_compressionContext_t ctx;
	size_t const ctxCreation = LZ4F_createCompressionContext(&ctx, LZ4F_VERSION);

	auto currentPos = ftell(inFp);
	fseek(inFp, 0, SEEK_END);
	unsigned inFileSize = ftell(inFp);
	fseek(inFp, 0, currentPos);

	const LZ4F_preferences_t kPrefs = {
			{ 
			LZ4F_max256KB, LZ4F_blockLinked, LZ4F_noContentChecksum, LZ4F_frame,
			  inFileSize /* known content size */, dictID /* no dictID */ , LZ4F_noBlockChecksum
			},
			  /*0*/ /*LZ4HC_CLEVEL_MIN*/ LZ4HC_CLEVEL_DEFAULT,   /* compression level; 0 == default */
			   0,   /* autoflush */
			   1 /*0*/,   /* favor decompression speed */
			   { 0, 0, 0 },  /* reserved, must be set to 0 */
	};

	try
	{
		vector<char> srcBuff(inFileSize);
		size_t const outbufCapacity = LZ4F_compressBound(inFileSize, &kPrefs);   /* large enough for any input <= IN_CHUNK_SIZE */
		vector<char> outBuff(outbufCapacity);
		compressResult_t result = { 1, 0, 0 };  /* == error (default) */
		if (!LZ4F_isError(ctxCreation))
		{
			result = compress_file_internal_dict(inFp, dict, fDst,
				ctx, kPrefs,
				srcBuff, outBuff);
		}
		else
		{
			printf("ressource allocation failed \n");
		}

		LZ4F_freeCompressionContext(ctx);   /* supports free on NULL */
		return result;
	}
	catch (bad_alloc)
	{
		printf("Allocation Failed!");
	}
}
//---------------------------------------------------------------------------------------------------------
void CompressLZ4(const string & zipName, const string& sourceDir, const vector<string>& pathsinzip, void* dictBuffer, const unsigned dictSize)
{
	vector<vector<char>*> toBeReleased;
	int errorp;
	zip_t *zipper = zip_open((zipName).c_str(), ZIP_CREATE | ZIP_TRUNCATE, &errorp);
	unsigned index = 0;

	LZ4F_CDict* dict = NULL;
	if (dictBuffer != NULL)
	{
		dict = LZ4F_createCDict(dictBuffer, dictSize);
	}

	for (int i = 0; i < pathsinzip.size(); ++i)
	{
		//printf("path %s", paths[i].c_str());
		FILE* inFp = fopen((sourceDir + "/" + pathsinzip[i]).c_str(), "rb");
		if (!inFp)
			continue;

		vector<char>* dst = new vector<char>();
		toBeReleased.push_back(dst);
		compressResult_t result;
		if (dict)
		{
			result = compress_file_dict(inFp, dict, *dst);
		}
		else
		{
			result = compress_file(inFp, *dst);
		}
		if (result.error == 0 && result.size_out > 0)
		{
			zip_source_t* source = zip_source_buffer(zipper, (*dst).data(), result.size_out, 0);
			string entry = dict != NULL ? pathsinzip[i] + dictCompressionSuffix : pathsinzip[i] + compressionSuffix;
			zip_file_add(zipper, entry.c_str(), source, ZIP_FL_ENC_UTF_8);
			zip_set_file_compression(zipper, index, ZIP_CM_STORE, 0);
		}

		index++;
		fclose(inFp);
	}
	zip_close(zipper);
	for (unsigned i = 0; i < toBeReleased.size(); ++i)
	{
		delete (toBeReleased[i]);
	}

	if (dict)
	{
		LZ4F_freeCDict(dict);
	}

	return;

}
//-------------------------------------------------------------------------------------------------------------------------------
static size_t get_block_size(const LZ4F_frameInfo_t* info) {
	switch (info->blockSizeID) {
	case LZ4F_default:
	case LZ4F_max64KB:  return 1 << 16;
	case LZ4F_max256KB: return 1 << 18;
	case LZ4F_max1MB:   return 1 << 20;
	case LZ4F_max4MB:   return 1 << 22;
	default:
		cerr << "Impossible with expected frame specification (<=v1.6.1)" << endl;;
	}
}
//-------------------------------------------------------------------------------------------------------------------------------
int	_Decompress_Blocks(
	zip_file_t* file,
	LZ4F_dctx* dctx,
	void* dict, size_t dictSize,
	void* srcBuffer, const size_t srcCapacity, const size_t filled, size_t alreadyConsumed,
	void* dstBuffer, const size_t dstCapacity)
{
	int firstChunk = 1;
	size_t ret = 1;

	assert(alreadyConsumed <= filled);

	/* Decompression */
	while (ret != 0)
	{
		/* Load more input */
		size_t readSize = firstChunk ? filled : zip_fread(file, srcBuffer, srcCapacity);
		firstChunk = 0;
		const void* srcPtr = (const char*)srcBuffer + alreadyConsumed;
		alreadyConsumed = 0;
		const void* const srcEnd = (const char*)srcPtr + readSize;
		if (readSize == 0)
		{
			printf("Decompress: not enough data in blocks\n");
			return RETURN_FAILURE;
		}

		/* Decompress:
		 * Continue while there is more input to read (srcPtr != srcEnd)
		 * and the frame isn't over (ret != 0)
		 */
		while (srcPtr < srcEnd && ret != 0)
		{
			/* Any data within dstBuffer has been saved at this stage */
			size_t dstSize = dstCapacity;
			size_t srcSize = (const char*)srcEnd - (const char*)srcPtr;
			//ret = LZ4F_decompress(dctx, dstBuffer, &dstSize, srcPtr, &srcSize, /* LZ4F_decompressOptions_t */ NULL);
			ret = LZ4F_decompress_usingDict(dctx, dstBuffer, &dstSize, srcPtr, &srcSize, /* LZ4F_decompressOptions_t */ dict, dictSize, NULL);
			if (LZ4F_isError(ret))
			{
				printf("Decompression error: %s\n", LZ4F_getErrorName(ret));
				return RETURN_FAILURE;
			}
			/* Save output */
			if (dstSize != 0)
			{
				decompressBuffer.resize(decompressBufferSize + dstSize);
				memcpy(decompressBuffer.data() + decompressBufferSize, dstBuffer, dstSize);
				decompressBufferSize += dstSize;
			}
			/* Update input */
			srcPtr = (const char*)srcPtr + srcSize;
		}

		assert(srcPtr <= srcEnd);

		/* Ensure all input data has been consumed.
		 * It is valid to have multiple frames in the same file,
		 * but this example only supports one frame.
		 */
		if (srcPtr < srcEnd)
		{
			printf("Decompress: Trailing data left in file after frame\n");
			return RETURN_FAILURE;
		}
	}

	/* Check that there isn't trailing data in the file after the frame.
	 * It is valid to have multiple frames in the same file,
	 * but this example only supports one frame.
	 */
	 //{   size_t const readSize = fread(src, 1, 1, f_in);
	 //if (readSize != 0 || !feof(f_in)) {
	 //	printf("Decompress: Trailing data left in file after frame\n");
	 //	return 1;
	 //}   }

	return RETURN_SUCCESS;
}

//-------------------------------------------------------------------------------------------------------------------------------
int _Decompress_File(zip_file_t* file, void* dict, size_t dictSize)
{
	/* Ressource allocation */
	/* ensure LZ4F_getFrameInfo() can read enough data */
	size_t srcCapacity = IN_CHUNK_SIZE;
	void* const srcBuffer = malloc(srcCapacity);

	if (!srcBuffer)
	{
		printf("decompress_file(srcBuffer = NULL)");
		return RETURN_FAILURE;
	}

	LZ4F_dctx* dctx;
	{
		size_t const dctxStatus = LZ4F_createDecompressionContext(&dctx, LZ4F_VERSION);
		if (LZ4F_isError(dctxStatus))
		{
			printf("LZ4F_dctx creation error: %s\n", LZ4F_getErrorName(dctxStatus));
			free(srcBuffer);
			return RETURN_FAILURE;
		}
	}
	assert(dctx != NULL);

	/* Read Frame header */
	size_t const readSize = zip_fread(file, srcBuffer, srcCapacity);
	if (readSize == 0)
	{
		printf("Decompress: not enough input or error reading file\n");
		free(srcBuffer);
		LZ4F_freeDecompressionContext(dctx);   /* note : free works on NULL */
		return RETURN_FAILURE;
	}

	LZ4F_frameInfo_t info;
	size_t consumedSize = readSize;
	{
		size_t const fires = LZ4F_getFrameInfo(dctx, &info, srcBuffer, &consumedSize);
		if (LZ4F_isError(fires))
		{
			printf("LZ4F_getFrameInfo error: %s\n", LZ4F_getErrorName(fires));
			free(srcBuffer);
			LZ4F_freeDecompressionContext(dctx);   /* note : free works on NULL */
			return RETURN_FAILURE;
		}
	}

	/* Allocating enough space for an entire block isn't necessary for
	 * correctness, but it allows some memcpy's to be elided.
	 */
	size_t const dstCapacity = get_block_size(&info);
	void* const dstBuffer = malloc(dstCapacity);
	if (!dstBuffer)
	{
		printf("decompress_file(dst)");
		free(srcBuffer);
		LZ4F_freeDecompressionContext(dctx);   /* note : free works on NULL */
		return RETURN_FAILURE;
	}

	int const decompressionResult = _Decompress_Blocks(file,dctx,
		dict, dictSize,
		srcBuffer, srcCapacity,
		readSize - consumedSize, consumedSize,
		dstBuffer, dstCapacity);

	free(srcBuffer);
	free(dstBuffer);
	LZ4F_freeDecompressionContext(dctx);   /* note : free works on NULL */
	return decompressionResult;
}
//---------------------------------------------------------------------------------------------------------
void DeCompressAll(const string & zipName, const string & dstPath)
{
	int errorp = ZIP_ER_OK;
	zip_t *zipFp = zip_open(zipName.c_str(), ZIP_RDONLY, &errorp);
	if (errorp != ZIP_ER_OK)
	{
		printf("Can not Open Zip File %s! Error Code = %d \n", zipName.c_str(), errorp);
	}
	std::vector<char> dict;

	int numEntries = zip_get_num_entries(zipFp, ZIP_FL_UNCHANGED);
	for (int i = 0; i < numEntries; ++i)
	{
		decompressBufferSize = 0;
		decompressBuffer.clear();
		decompressPos = 0;

		string name = zip_get_name(zipFp, i, ZIP_FL_UNCHANGED);
		zip_file_t* file = zip_fopen_index(zipFp, i, 0);
		if (file == NULL)
		{
			printf("Failed to Open Entry %s! \n", name.c_str());
			continue;
		}
		bool compressed = (name.length() > 4 && (name.find(compressionSuffix) == name.length() - 4)
			|| name.find(dictCompressionSuffix) == name.length() - 4);
		if (!compressed)
		{
			struct zip_stat st;
			zip_stat_init(&st);
			zip_stat_index(zipFp, i, 0, &st);
			assert(st.comp_method == ZIP_CM_STORE);
			assert(st.size > LZ4F_HEADER_SIZE_MIN);
			decompressBuffer.resize(st.size);
			decompressBufferSize = st.size;
			zip_fread(file, decompressBuffer.data(), st.size);
		}
		else
		{
			struct zip_stat st;
			zip_stat_init(&st);
			zip_stat_index(zipFp, i, 0, &st);
			assert(st.comp_method == ZIP_CM_STORE);
			assert(st.size > LZ4F_HEADER_SIZE_MIN);
			if (name.compare(name.length() - 4, 4, dictCompressionSuffix) == 0)
			{
				FILE* dictFile = fopen(dictPath.c_str(), "rb");
				dict.resize(DICTIONARY_BYTES);
				size_t dictSize = fread(dict.data(), 1, 1024 * 60, dictFile);
				dict.resize(dictSize);
				fclose(dictFile);
			}
			if (_Decompress_File(file,dict.data() ,dict.size()) != RETURN_SUCCESS)
			{
				printf("Decompression Fails %s!", name.c_str());
				zip_fclose(file);
				continue;
			}
			printf("Decompression File %s!", name.c_str());
		}
		zip_fclose(file);

		if (compressed)
			name = name.substr(0, name.length() - 4);
		string filePath = dstPath + "/" + name;
		zipper::CDirEntry::createDir(zipper::CDirEntry::dirName(filePath));
		FILE* outFp = fopen(filePath.c_str(), "wb");
		fwrite(decompressBuffer.data(), 1, decompressBufferSize, outFp);
		fclose(outFp);
	}

	zip_close(zipFp);
}

int GetAllFilesInDir(const string& path, const string& relaPath, vector<string>& files, const string& wildCard)
{
	string fullpath = path + "/" + wildCard;
	struct _finddata_t c_file;
	intptr_t   hfile;

	hfile = _findfirst(fullpath.c_str(), &c_file);
	if (-1 == hfile)
	{
		printf("_findfirst error with pattern %s\n", fullpath.c_str());
		return 0;
	}
	do {
		//判断是否有子目录  
		if (c_file.attrib & _A_SUBDIR)
		{
			//判断是否为"."当前目录，".."上一层目录
			if ((strcmp(c_file.name, ".") != 0) && (strcmp(c_file.name, "..") != 0))
			{
				string newPath = path + "/" + c_file.name;
				GetAllFilesInDir(newPath, relaPath + c_file.name + "/", files, wildCard);
			}
		}
		else
		{
			files.push_back(relaPath + c_file.name);
		}
	} while (_findnext(hfile, &c_file) == 0);

	_findclose(hfile);
	return files.size();
}

void main(int argc, char *argv[])
{
	if (argc < 5)
	{
		cerr<<"Invalid Arguments."<<endl;
		printHelp();
		return;
	}
	vector <string> sources;
	string pkgPath;
	string srcDirPath;
	string dstDirPath;
	int mode = -1;
	bool hasDictionary = false;
	for (int i = 1; i < argc; ++i) {
		string arg = argv[i];
		if ((arg == "-h") || (arg == "--help")) {
			printHelp();
			return;
		}
		else if ((arg == "-p") || (arg == "--pack")) 
		{
			if (i + 1 < argc) { // Make sure we aren't at the end of argv!
				pkgPath = argv[++i];
			}
			else 
			{
				cerr << "-p/--pack option requires one argument as destination path." << endl;
				return;
			}
			mode = 0;
		}
		else if ((arg == "-u") || (arg == "--unpack"))
		{
			if (i + 1 < argc) { // Make sure we aren't at the end of argv!
				pkgPath = argv[++i];
			}
			else
			{
				cerr << "-u/--unpack option requires one argument as source package path." << endl;
				return;
			}
			mode = 1;
		}
		else if ((arg == "-s") || (arg == "--source"))
		{
			if (i + 1 < argc) { // Make sure we aren't at the end of argv!
				srcDirPath = argv[++i];
			}
			else
			{
				cerr << "-s/--source option requires one argument as source directory path." << endl;
				return;
			}
		}
		else if ((arg == "-d") || (arg == "--destination"))
		{
			if (i + 1 < argc) { // Make sure we aren't at the end of argv!
				dstDirPath = argv[++i];
			}
			else
			{
				cerr << "-d/--destination option requires one argument as destination directory path." << endl;
				return;
			}
		}
		else if ((arg == "-D") || (arg == "--dictionary"))
		{
			hasDictionary = true;
		}
		else {
			cerr << "--Invalid arguments." << endl;
			printHelp();
			return;
		}
	}

	vector<char> dict;
	const char* fullPath = _fullpath(NULL, argv[0], 200);
	dictPath = zipper::CDirEntry::dirName(string(fullPath)) + "/" + dictRelaFile;

	//compression
	if (mode == 0)
	{
		if (srcDirPath == "")
		{
			cerr << "--Invalid arguments. Source directory is needed!" << endl;
			return;
		}
		vector<string> files;
		GetAllFilesInDir(srcDirPath, "", files, "*.*");
		if (hasDictionary)
		{
			FILE* dictFile = fopen(dictPath.c_str(), "rb");
			if (dictFile)
			{
				dict.resize(DICTIONARY_BYTES);
				size_t dictSize = fread(dict.data(), 1, DICTIONARY_BYTES, dictFile);
				dict.resize(dictSize);
				fclose(dictFile);
			}
			else
			{
				cerr << "--dictPath not find!" << endl;
			}
		}
		if (pkgPath.length() < 5 || pkgPath.substr(pkgPath.length() - 4) != string(archiveSuffix))
		{
			//cerr << "--Invalid package path!" << endl;
			//return;
			pkgPath = pkgPath + string(archiveSuffix);
		}
		CompressLZ4(pkgPath, srcDirPath, files, dict.data(), dict.size());
	}
	else if (mode == 1)
	{
		if (dstDirPath == "")
		{
			cerr << "--Invalid arguments. Destination directory is needed!" << endl;
			return;
		}
		if (pkgPath.length() < 5 || pkgPath.substr(pkgPath.length() - 4) != string(archiveSuffix))
		{
			cerr << "--Invalid package path!" << endl;
			return;
		}

		DeCompressAll(pkgPath, dstDirPath);
	}
	else
	{
		cerr << "--Invalid arguments. You must input -z or -u to triger compression or decompression" << endl;
		return;
	}

	return;
}