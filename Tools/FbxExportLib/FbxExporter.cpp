#include "FbxExporter.h"

bool verboseOutput = false;
int Export(int argc, char *argv[])
{
	cxxopts::Options options(
		"FBX2Apollo",
		"FBX2Apollo 1.0: Generate a Apollo 1.0 representation of an FBX model.");

	std::string inputPath;
	std::string outputPath;
	std::string meshPath;
	std::string textureSearchPath;
	ConvertOptions convOpts;
	AnimExportOption animExpOpts;
	animExpOpts.PositionError = 1.5f;
	animExpOpts.RotationError = 1.5f;
	animExpOpts.ScaleError = 1.5f;
	animExpOpts.UseMatrixKeyFrame = false;
	options.positional_help("[<FBX File>]");


	//values::standard_value<float> value = std::make_shared<values::standard_value<float>>(animComOpts.PositionError);

	options.add_options()("i,input", "The FBX model to convert", cxxopts::value<std::string>(inputPath))
		("o,output", "Where to generate the output,without suffix", cxxopts::value<std::string>(outputPath))
		("t,texturepath", "Specify a texture path for texture searching", cxxopts::value<std::string>(textureSearchPath))
		("d,draco", "Apply Draco mesh compression to geometries", cxxopts::value<bool>(convOpts.useDarco))
		("c,corto", "Apply Corto mesh compression to geometries", cxxopts::value<bool>(convOpts.useCorto))
		("p,animposcom", "animation pos compress error", cxxopts::value<float>(animExpOpts.PositionError))
		("s,animsclcom", "animation scale compress error", cxxopts::value<float>(animExpOpts.ScaleError))
		("r,animrotcom", "animation rotation compress error(use cosin thelta)", cxxopts::value<float>(animExpOpts.RotationError))
		("m,animmat", "animation rotation compress error(use cosin thelta)", cxxopts::value<bool>(animExpOpts.UseMatrixKeyFrame))
		("h,help", "Show help info");

	try
	{
		options.parse_positional("input");
		options.parse(argc, argv);

	}
	catch (const cxxopts::OptionException &e)
	{
		printf(options.help().c_str());
		return 1;
	}

	if (options.count("help")) {
		printf(options.help().c_str());
		return 0;
	}

	if (!options.count("input")) {
		printf("You must supply a FBX file to convert.\n");
		printf(options.help().c_str());
		return 1;
	}

	if (options.count("output") == 0) {
		outputPath = "./" + StringUtils::GetFileBaseString(inputPath);
	}
	//else
	//{
	//	outputPath += "\\" + StringUtils::GetFileBaseString(inputPath);
	//}

	std::string outputFolder;

	meshPath = outputPath + "\\";


	if (!FileUtils::CreatePath(meshPath.c_str()))
	{
		fprintf(stderr, "ERROR: Failed to create folder:%s \n", meshPath.c_str());
		return 1;
	}

	RawModel  raw;
	fprintf(stdout, "Begin to import FBX: %s\n\n\n", inputPath.c_str());
	if (!LoadFBXFile(raw, inputPath.c_str(), textureSearchPath.c_str(), "png;jpg;jpeg;tga", animExpOpts)) {
		fprintf(stderr, "ERROR:: Failed to parse FBX: %s\n", inputPath.c_str());
		return 1;
	}

	std::ofstream outStream;
	const auto streamStart = outStream.tellp();

	if (!Raw2Apollo(meshPath, raw, convOpts))
	{
		fprintf(stderr, "ERROR: Failed to exprot raw model to binary file: %s\n", meshPath.c_str());
		return 1;
	}
	outStream.flush();
	outStream.close();

	return 0;
}
