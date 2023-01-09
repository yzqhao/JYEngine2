#pragma once
#include <vector>
#include <unordered_map>
#include <map>
#include <iostream>
#include <fstream>

#if defined( __unix__ ) || defined( __APPLE__ )
#include <sys/stat.h>
#define _stricmp strcasecmp
#endif

#include "String_Utils.h"
#include "File_Utils.h"
#include "cxxopts.hpp"
#include "RawModel.h"
#include "Fbx2Raw.h"
#include "Raw2Apollo.h"

int Export(int argc, char *argv[]);

