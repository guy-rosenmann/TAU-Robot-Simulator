#include <map>
#include <string>
#include <algorithm>

using namespace std;

extern "C" int calc_score(const map<string, int>& score_params)
{
	try
	{
		bool isHouseClean = score_params.at("sum_dirt_in_house") == score_params.at("dirt_collected");
		bool isDocked = score_params.at("is_back_in_docking") == 1 ? true : false;
		bool isDone = isHouseClean && isDocked;

		int position_in_competition = isDone ? std::min(score_params.at("actual_position_in_competition"), 4) : 10;

		int points = 2000;
		points -= (position_in_competition - 1) * 50;
		points += (score_params.at("winner_num_steps") - score_params.at("this_num_steps")) * 10;
		points -= (score_params.at("sum_dirt_in_house") - score_params.at("dirt_collected")) * 3;
		points += isDocked ? 50 : -200;

		return std::max(0, points);
	}
	catch (const std::out_of_range&) // a parameter is missing
	{
		return -1;
	}
}
