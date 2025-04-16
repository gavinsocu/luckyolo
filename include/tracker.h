#ifndef TRACKER_H
#define TRACKER_H

#include <vector>
#include <map>

using namespace std;

#define ALLOW_GAP_FRAMES_NUM 8
#define TRACKER_PIXEL_DISTANCE 50

class CustomTracker {
public:
    CustomTracker();

    vector<vector<int>> custom_update(const vector<vector<int>>& rects);

private:
    map<int, pair<int, int>> custom_center_points;
    map<int, int> disappear_counter;
    int custom_id_count;
};

#endif // TRACKER_H
