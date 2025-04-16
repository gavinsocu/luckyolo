#include <fstream>
#include <string>
#include <stdio.h>

#include "settxt.h"


using namespace std;


void writeLineToFile(const string& content) {
    ofstream outfile(TXT_FILE_NAME, ios::app);

    if (outfile.is_open()) {
        outfile << content << endl;  // 换行
        outfile.close();
    } else {
        printf("无法打开文件：%s", &TXT_FILE_NAME);
    }
}
