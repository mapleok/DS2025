#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>
using namespace std;

// 边界框结构体
struct BoundingBox {
    int x1, y1; // 左上角坐标（0~1000）
    int x2, y2; // 右下角坐标（宽高≥20）
    float score; // 置信度（0~1）
    // 打印边界框信息
    void print() const {
        cout << "位置(" << x1 << "," << y1 << ")-(" << x2 << "," << y2 
             << "), 置信度:" << score << endl;
    }
};

// 1. 随机分布边界框生成
vector<BoundingBox> generateRandomBoxes(int count) {
    vector<BoundingBox> boxes;
    srand(time(0)); // 随机种子
    for (int i = 0; i < count; i++) {
        BoundingBox box;
        // 随机生成位置，确保宽高≥20
        box.x1 = rand() % 900; // x1≤900，避免x2超出1000
        box.y1 = rand() % 900;
        box.x2 = box.x1 + 20 + rand() % 100; // 宽20~120
        box.y2 = box.y1 + 20 + rand() % 100; // 高20~120
        box.score = (float)rand() / RAND_MAX; // 置信度0~1均匀分布
        boxes.push_back(box);
    }
    return boxes;
}

// 正态分布随机数生成（Box-Muller算法）
float normalRandom(float mean, float std) {
    static float z0, z1;
    static bool generate = false;
    generate = !generate;
    if (!generate) return z1 * std + mean;
    float u1 = (float)rand() / RAND_MAX;
    float u2 = (float)rand() / RAND_MAX;
    z0 = sqrt(-2.0f * log(u1)) * cos(2.0f * 3.14159f * u2);
    z1 = sqrt(-2.0f * log(u1)) * sin(2.0f * 3.14159f * u2);
    return z0 * std + mean;
}

// 2. 聚集分布边界框生成
vector<BoundingBox> generateClusteredBoxes(int count) {
    vector<BoundingBox> boxes;
    srand(time(0));
    // 5个聚集区域的中心坐标（图像中心+四角）
    vector<pair<int, int>> centers = {{200,200}, {800,200}, {200,800}, {800,800}, {500,500}};
    
    for (int i = 0; i < count; i++) {
        BoundingBox box;
        float prob = (float)rand() / RAND_MAX;
        if (prob < 0.8) { // 80%在聚集区域
            int centerIdx = rand() % 5;
            int cx = centers[centerIdx].first;
            int cy = centers[centerIdx].second;
            // 位置在中心±50范围内，确保分散度
            box.x1 = cx - 50 + rand() % 100;
            box.y1 = cy - 50 + rand() % 100;
            box.x2 = box.x1 + 20 + rand() % 100;
            box.y2 = box.y1 + 20 + rand() % 100;
            // 置信度正态分布（0.7±0.1），截断到0~1
            box.score = max(0.0f, min(1.0f, normalRandom(0.7f, 0.1f)));
        } else { // 20%随机分布
            box.x1 = rand() % 900;
            box.y1 = rand() % 900;
            box.x2 = box.x1 + 20 + rand() % 100;
            box.y2 = box.y1 + 20 + rand() % 100;
            box.score = (float)rand() / RAND_MAX;
        }
        boxes.push_back(box);
    }
    return boxes;
}

// 测试函数
int main() {
    int testCount = 10; // 测试生成10个框
    cout << "随机分布边界框（" << testCount << "个）：" << endl;
    vector<BoundingBox> randomBoxes = generateRandomBoxes(testCount);
    for (auto& box : randomBoxes) box.print();

    cout << "\n聚集分布边界框（" << testCount << "个）：" << endl;
    vector<BoundingBox> clusteredBoxes = generateClusteredBoxes(testCount);
    for (auto& box : clusteredBoxes) box.print();

    return 0;
}