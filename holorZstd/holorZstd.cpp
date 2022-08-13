#pragma warning(disable:4996)

#include "holorZstd.h"
#include<array>



/**
 * holorzCompressAndWriteToFile takes file pointer, data size and data as input. Compresses and writes the compressed data in a file.
 *
 * @param[in] ofp File pointer, compressed data would be written in the file.
 * @param[in] inputDataSize Input data size in bytes.
 * @param[in] inputData Input data.
 * @param[in] debug Optional parameter. Default value is false. If true, debug information will be printed such as elapsed time and compression ratio.
 * @param[in] compressLevel Compression level. Optional parameter. Default value is 3.
 * @retval true if Successful or
 * @retval false if Unsuccessful.
 */

bool holorzCompressAndWriteToFile(FILE* ofp, size_t inputDataSize, void* inputData, bool debug, int compressLevel)
{
	bool compressionSuccess = false;
	size_t compressedSize = 0;
	clock_t compressionTime;
	if (ofp == nullptr)
	{
		fprintf(stderr, "Error No : %d\n", errno);
		perror("File cannot be opened");
		exit(-1);
	}
	if ((inputDataSize <= 0) || (inputData == NULL) /*|| (compressLevel<0 || compressLevel>22)*/)
	{
		fprintf(stderr, "Invalid input. \n");
		exit(-1);
	}
	if (compressLevel < 0 || compressLevel>22)
	{
		fprintf(stderr, "Compress level should be 1 to 22. \n");
		exit(-1);
	}
	size_t const buffOutSize = ZSTD_CStreamOutSize();
	void* buffout = malloc_orDie(buffOutSize);
	ZSTD_inBuffer input = { inputData, inputDataSize, 0 };
	ZSTD_CCtx* const cctx = ZSTD_createCCtx();
	CHECK(cctx != NULL, "ZSTD_createCCtx() failed!");
	CHECK_ZSTD(ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, compressLevel));
	CHECK_ZSTD(ZSTD_CCtx_setParameter(cctx, ZSTD_c_checksumFlag, 1));
	ZSTD_EndDirective const mode = ZSTD_e_end;
	fwrite_orDie(&inputDataSize, sizeof(size_t), ofp);
	if (debug)
		compressionTime = clock();
	for (;;)
	{
		ZSTD_outBuffer output = { (void*)buffout, buffOutSize, 0 };
		size_t const remaining = ZSTD_compressStream2(cctx, &output, &input, mode);
		CHECK_ZSTD(remaining);
		fwrite_orDie(buffout, output.pos, ofp);
		compressedSize += output.pos;
		if (remaining == 0)
			break;
	}
	if (debug)
	{
		compressionTime = clock() - compressionTime;
		printf("Size of oroginal data : %d bytes\n", inputDataSize);
		printf("Size of Compressed Data: %zu bytes\n", compressedSize);
		printf("Compression Ratio: %.2f percent\n", ((double)compressedSize / inputDataSize) * 100);
		printf("Time to Compress: %g s\n", ((double)compressionTime) / CLOCKS_PER_SEC);
	}
	free(buffout);
	ZSTD_freeCCtx(cctx);
	fclose_orDie(ofp);
	compressionSuccess = true;
	return compressionSuccess;
}

/**
 * holorzReadFromFileAndDecompress takes file pointer and compressed data size as input, decompresses and writes the decompresed data in memory.

 * @param[in] ifp File pointer of Compressed file.
 * @param[in] compressedDataSize Size of Compressed file.
 * @param[in] debug Optional parameter. Default value is false. If true, debug information will be printed such as elapsed time.
 * @param[out] decompressedData Decompressed data  would be written on it.
 * @retval decompressedSize if Successful or.
 * @retval 0 if Unsuccessful.
 */
size_t holorzReadFromFileAndDecompress(FILE* ifp, size_t compressedDataSize, void* decompressedData, bool debug)
{
	size_t decompressedSize = 0;
	clock_t decompressionTime;
	if (ifp == nullptr)
	{
		fprintf(stderr, "Error No : %d\n", errno);
		perror("File cannot be opened");
		exit(-1);
	}
	if (compressedDataSize <= 0)
	{
		fprintf(stderr, "Invalid Compressed File : File size should not be zero.  \n");
		exit(-1);
	}
	size_t const buffInSize = ZSTD_DStreamInSize();
	void* buffIn = malloc_orDie(buffInSize);
	ZSTD_DCtx* const dctx = ZSTD_createDCtx();
	CHECK(dctx != NULL, "ZSTD_createDCtx() failed!");
	size_t const toRead = buffInSize;
	size_t read;
	ZSTD_outBuffer output = { decompressedData, compressedDataSize, 0 };
	if (debug)
		decompressionTime = clock();
	while ((read = fread_orDie(buffIn, toRead, ifp))) {
		ZSTD_inBuffer input = { buffIn, read, 0 };
		while (input.pos < input.size) {
			size_t const ret = ZSTD_decompressStream(dctx, &output, &input);
		}
	}
	decompressedSize = output.pos;
	if (debug) {
		decompressionTime = clock() - decompressionTime;
		printf("Time to decompress: %g s\n", ((double)decompressionTime) / CLOCKS_PER_SEC);
		printf("Size of decompressed Data: %zu bytes\n", decompressedSize);
	}
	free(buffIn);
	ZSTD_freeDCtx(dctx);
	fclose_orDie(ifp);
	return decompressedSize;

}




