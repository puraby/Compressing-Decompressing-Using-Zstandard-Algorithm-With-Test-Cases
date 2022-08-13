#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <holorZstd.h>




int main(int argc, char** argv) {
	if (argc > 3) {
		printf("wrong arguments\n");
		return 0;
	}
	int compressionLevel = atoi(argv[1]);
	int inputDataLength = atoi(argv[2]);
	size_t inputDataSize = sizeof(double) * inputDataLength;
	double* inputData = new double[inputDataSize];
	srand((int)time(0));
	for (int i = 0; i < inputDataLength; i++)
	{
		inputData[i] = (double)rand();
	}
	const char* fileName = "Compressed.zst";
	FILE* ofp = fopen(fileName, "wb");
	holorzCompressAndWriteToFile(ofp, inputDataSize, inputData,true);
	size_t compressedDataSize;
	FILE* ifp = fopen(fileName, "rb");
	fread(&compressedDataSize, sizeof(size_t), 1, ifp);
	double* decompressedData = new double[compressedDataSize + 1];
	size_t decompressedDataSize = holorzReadFromFileAndDecompress(ifp, compressedDataSize, decompressedData,true);
	bool flag = true;
	for (int i = 0; i < inputDataLength; i++) {
		if ((double)inputData[i] != (double)decompressedData[i]) {
			printf("Original data and Decompressed data are not same.\n");
			flag = false;
			break;
		}
	}
	if (flag)
		printf("Original data and Decompressed data are same.\n");
	if (inputDataSize == decompressedDataSize)
		printf("Original data size and Decompressed data size are same.\n");
	delete[] inputData;
	delete[] decompressedData;
}


