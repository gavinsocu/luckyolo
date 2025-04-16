#include <iostream>
#include <cmath>
#include <set>
#include "tracker.h"
#include "settxt.h"
#include <time.h>

CustomTracker::CustomTracker() {
    custom_id_count = 0;
}

vector<vector<int>> CustomTracker::custom_update(const vector<vector<int>>& rects) {
    vector<vector<int>> objects_bbs_ids;
    set<int> current_ids; 

    char text[100];
    time_t now = time(0);
    char* dt = ctime(&now);
    dt[24] = '\0'; 

    for (const auto& rect : rects) {
        int sx = rect[0];
        int sy = rect[1];
        int ex = rect[2];
        int ey = rect[3];
        int cx = (sx + ex) / 2;
        int cy = (sy + ey) / 2;

        bool same_object_detected = false;

        for (auto& [id, pt] : custom_center_points) {
            double dist = hypot(cx - pt.first, cy - pt.second);
            if (dist < TRACKER_PIXEL_DISTANCE) {
                custom_center_points[id] = {cx, cy};
                objects_bbs_ids.push_back({sx, sy, ex, ey, id});
                disappear_counter[id] = 0;
                current_ids.insert(id);
                same_object_detected = true;
                break;
            }
        } 

        if (!same_object_detected) {
            custom_center_points[custom_id_count] = {cx, cy};
            disappear_counter[custom_id_count] = 0;
            objects_bbs_ids.push_back({sx, sy, ex, ey, custom_id_count});
            current_ids.insert(custom_id_count);
            custom_id_count++;
        }
    }
        // 更新没检测到的目标的丢失次数
    for (auto& [id, _] : custom_center_points) {
        if (current_ids.find(id) == current_ids.end()) {
            disappear_counter[id]++;
        }
    }

    map<int, pair<int, int>> new_center_points;  //删掉不使用的
    map<int, int> updated_disappear_counter;
    for (auto& [id, pt] : custom_center_points) {
        if (disappear_counter[id] < ALLOW_GAP_FRAMES_NUM) {
            new_center_points[id] = pt;
            updated_disappear_counter[id] = disappear_counter[id];
        }else{
            if(pt.first > 320){
                sprintf(text,"时间:%s personId<%d> 去往阳台方向",dt,id);
                writeLineToFile(text);
            }else{
                sprintf(text,"时间:%s personId<%d> 去往门口方向",dt,id);
                writeLineToFile(text);
            }
        }
    }

    custom_center_points = new_center_points;
    disappear_counter = updated_disappear_counter;

    return objects_bbs_ids;
}
