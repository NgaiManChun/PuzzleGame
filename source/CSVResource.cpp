// =======================================================
// CSVResource.cpp
// 
// CSVファイルを読み込み関数
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/08/06
// =======================================================
#include "CSVResource.h"
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include "windows.h"

using std::string;
using std::map;
using std::vector;

void ReadCSVDataLine(string& line, D_TABLE& result);

void ReadCSVFromResource(int resourceId, D_TABLE& result) {
    // リソースハンドルの取得
    HMODULE hModule = GetModuleHandle(NULL);
    HRSRC hRes = FindResourceW(hModule, MAKEINTRESOURCEW(resourceId), L"CSV");
    if (hRes == NULL) {
        return;
    }

    // リソースのロード
    HGLOBAL hData = LoadResource(hModule, hRes);
    if (hData == NULL) {
        return;
    }

    // リソースデータのロック
    LPVOID pData = LockResource(hData);
    if (pData == NULL) {
        return;
    }

    // リソースサイズの取得
    DWORD dataSize = SizeofResource(hModule, hRes);
    if (dataSize == 0) {
        return;
    }

    // リソースデータを文字列に変換
    string csvData(static_cast<char*>(pData), dataSize);

    ReadCSVData(csvData, result);
}

void ReadCSVFromPath(string path, D_TABLE& result) {
    std::ifstream file(path);
    std::string line;
    if (file.is_open()) {
        while (getline(file, line)) {
            ReadCSVDataLine(line, result);
        }
    }
}

void ReadCSVData(string &csvData, D_TABLE &result) {
    std::stringstream ss(csvData);
    string line;
    while (getline(ss, line)) {
        ReadCSVDataLine(line, result);
    }
}

void ReadCSVDataLine(string &line, D_TABLE& result) {
    std::stringstream lineStream(line);
    string cell;
    D_ROW row;
    while (getline(lineStream, cell, ',')) {
        row.push_back(cell);
    }
    result.push_back(row);
}

void TableToKeyValuePair(const string key, D_TABLE &data_table, D_KVTABLE &result) {
    D_ROW header = data_table[0];
    int keyIndex = 0;
    for (int i = 0; i < header.size(); i++) {
        if (header[i] == key) {
            keyIndex = i;
            break;
        }
    }
    for (int y = 1; y < data_table.size(); y++) {
        D_KVPAIR pair;
        for (int x = 0; x < data_table[y].size(); x++) {
            pair[header[x]] = data_table[y][x];
        }
        result[data_table[y][keyIndex]] = pair;
    }
}