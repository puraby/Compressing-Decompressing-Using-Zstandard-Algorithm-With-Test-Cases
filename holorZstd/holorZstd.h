#pragma warning(disable:4996)
#pragma once
#ifndef HOLORZSTD_H_
#define HOLORZSTD_H_
#include <stdbool.h>
#include <time.h>
#include <iostream>
#include "common.h"


bool holorzCompressAndWriteToFile(FILE* ofp, size_t inputDataSize, void* inputData, bool debug = false, int compressLevel = 3);
size_t holorzReadFromFileAndDecompress(FILE* ifp, size_t outputDataSize, void* decompressedData, bool debug = false);

#endif


