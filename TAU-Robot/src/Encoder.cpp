#include "Encoder.h"

#include <stdlib.h>
#include <string>

bool Encoder::encode(const string& imagesString, const string& videoOutput)
{
	string ffmpegCmd = "ffmpeg -y -i " + imagesString + " " + videoOutput + " > /dev/null 2>&1";
	int ret = system(ffmpegCmd.c_str());
	if (ret == -1)
	{
		return false;
	}
	return true;
}

