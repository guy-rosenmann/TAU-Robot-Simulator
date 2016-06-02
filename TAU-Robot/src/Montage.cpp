#include "Montage.h"

#include <stdlib.h>
#include <string>

bool Montage::compose(const vector<string> &images, int cols, int rows, const string& composedImagePath)
{
	string montageCmd = "montage -geometry 60x60 -tile " + to_string(cols) + "x" + to_string(rows) + " ";
	for (auto &path : images)
	{
		montageCmd += "./avatars/" + path + " ";
	}
	montageCmd += composedImagePath + " > /dev/null 2>&1";
	int ret = system(montageCmd.c_str());
	if (ret == -1)
	{
		return false;
	}
	return true;
}

