add_library(png STATIC IMPORTED)
find_library("${ROOT_PATH}/build/${libpng_binary_dir}" png)
set_target_properties(png PROPERTIES IMPORTED_LOCATION "${PNG_LIBRARY_PATH}")