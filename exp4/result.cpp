#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <chrono>
#include <iomanip> 
#include <numeric>
using namespace std;
using namespace chrono;

// ============== 1. 边界框结构体定义（全局通用） ==============
struct BoundingBox {
    int x1, y1; // 左上角坐标（0~1000）
    int x2, y2; // 右下角坐标（宽高≥20）
    float score; // 置信度（0~1）
    // 计算边界框面积
    int area() const { return (x2 - x1) * (y2 - y1); }
};

// ============== 2. 四种排序算法实现 ==============
// （1）冒泡排序（优化版）
void bubbleSort(vector<float>& arr) {
    int n = arr.size();
    bool hasSwap;
    for (int i = 0; i < n - 1; i++) {
        hasSwap = false;
        for (int j = 0; j < n - 1 - i; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
                hasSwap = true;
            }
        }
        if (!hasSwap) break; // 无交换则有序，提前退出
    }
}

// （2）选择排序
void selectionSort(vector<float>& arr) {
    int n = arr.size();
    int minIdx;
    for (int i = 0; i < n - 1; i++) {
        minIdx = i;
        for (int j = i + 1; j < n; j++) {
            if (arr[j] < arr[minIdx]) minIdx = j;
        }
        if (minIdx != i) swap(arr[i], arr[minIdx]);
    }
}

// （3）二分插入排序
void binaryInsertionSort(vector<float>& arr) {
    int n = arr.size();
    for (int i = 1; i < n; i++) {
        float key = arr[i];
        int left = 0, right = i - 1;
        // 二分查找插入位置
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (arr[mid] > key) right = mid - 1;
            else left = mid + 1;
        }
        // 移动元素腾出位置
        for (int j = i; j > left; j--) {
            arr[j] = arr[j - 1];
        }
        arr[left] = key;
    }
}

// （4）快速排序（三数取中法优化）
int medianOfThree(vector<float>& arr, int low, int high) {
    int mid = low + (high - low) / 2;
    if (arr[low] > arr[mid]) swap(arr[low], arr[mid]);
    if (arr[low] > arr[high]) swap(arr[low], arr[high]);
    if (arr[mid] > arr[high]) swap(arr[mid], arr[high]);
    swap(arr[mid], arr[high - 1]); // 基准存于high-1
    return high - 1;
}

int partition(vector<float>& arr, int low, int high) {
    int pivotIdx = medianOfThree(arr, low, high);
    float pivot = arr[pivotIdx];
    int i = low, j = high - 2;
    while (true) {
        while (arr[++i] < pivot);
        while (arr[--j] > pivot);
        if (i < j) swap(arr[i], arr[j]);
        else break;
    }
    swap(arr[i], arr[pivotIdx]);
    return i;
}

void quickSortRecur(vector<float>& arr, int low, int high) {
    if (low + 10 < high) { // 小数组用插入排序优化
        int pivotIdx = partition(arr, low, high);
        quickSortRecur(arr, low, pivotIdx - 1);
        quickSortRecur(arr, pivotIdx + 1, high);
    } else {
        // 小数组插入排序
        for (int i = low + 1; i <= high; i++) {
            float key = arr[i];
            int j = i - 1;
            while (j >= low && arr[j] > key) {
                arr[j + 1] = arr[j];
                j--;
            }
            arr[j + 1] = key;
        }
    }
}

void quickSort(vector<float>& arr) {
    if (arr.size() <= 1) return;
    quickSortRecur(arr, 0, arr.size() - 1);
}

// ============== 3. 两种分布边界框生成 ==============
// （1）随机分布边界框生成
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

// （2）聚集分布边界框生成
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

// ============== 4. 基础NMS算法实现 ==============
// 计算IoU（交并比）
float calculateIoU(const BoundingBox& a, const BoundingBox& b) {
    int xMin = max(a.x1, b.x1);
    int yMin = max(a.y1, b.y1);
    int xMax = min(a.x2, b.x2);
    int yMax = min(a.y2, b.y2);
    if (xMax <= xMin || yMax <= yMin) return 0.0f; // 无交集
    int intersection = (xMax - xMin) * (yMax - yMin);
    int unionArea = a.area() + b.area() - intersection;
    return (float)intersection / unionArea;
}

// NMS算法（输入边界框+排序函数，返回筛选后结果）
vector<BoundingBox> NMS(vector<BoundingBox> boxes, void (*sortFunc)(vector<float>&), float iouThresh = 0.5) {
    if (boxes.empty()) return {};
    int n = boxes.size();
    vector<int> indices(n);
    iota(indices.begin(), indices.end(), 0);
    vector<float> scores;
    for (auto& box : boxes) scores.push_back(box.score);

    // 1. 用指定排序算法对置信度降序排序
    vector<float> sortedScores = scores;
    sortFunc(sortedScores);
    reverse(sortedScores.begin(), sortedScores.end()); // 转为降序

    // 2. 生成排序后的索引序列
    vector<bool> used(n, false);
    vector<int> sortedIndices;
    for (float s : sortedScores) {
        for (int i = 0; i < n; i++) {
            if (!used[i] && scores[i] == s) {
                sortedIndices.push_back(i);
                used[i] = true;
                break;
            }
        }
    }

    // 3. 迭代筛选非冗余框
    vector<bool> keep(n, true);
    for (int i = 0; i < sortedIndices.size(); i++) {
        int currIdx = sortedIndices[i];
        if (!keep[currIdx]) continue;
        for (int j = i + 1; j < sortedIndices.size(); j++) {
            int nextIdx = sortedIndices[j];
            if (calculateIoU(boxes[currIdx], boxes[nextIdx]) >= iouThresh) {
                keep[nextIdx] = false;
            }
        }
    }

    // 4. 收集结果
    vector<BoundingBox> result;
    for (int i = 0; i < n; i++) {
        if (keep[i]) result.push_back(boxes[i]);
    }
    return result;
}

// ============== 5. 性能测试工具函数 ==============
// 提取边界框的置信度数组（用于单独排序测试）
vector<float> extractScores(const vector<BoundingBox>& boxes) {
    vector<float> scores;
    for (auto& box : boxes) scores.push_back(box.score);
    return scores;
}

// 测试单个排序算法的性能（返回平均时间，单位：微秒）
long long testSortPerformance(void (*sortFunc)(vector<float>&), vector<float>& arr, int repeat = 5) {
    long long total = 0;
    for (int i = 0; i < repeat; i++) {
        vector<float> testArr = arr;
        auto start = high_resolution_clock::now();
        sortFunc(testArr);
        auto end = high_resolution_clock::now();
        total += duration_cast<microseconds>(end - start).count();
    }
    return total / repeat;
}

// 测试“排序+NMS”的整体性能（返回平均时间，单位：微秒）
long long testSortNMSPerformance(void (*sortFunc)(vector<float>&), vector<BoundingBox>& boxes, int repeat = 5) {
    long long total = 0;
    for (int i = 0; i < repeat; i++) {
        vector<BoundingBox> testBoxes = boxes;
        auto start = high_resolution_clock::now();
        NMS(testBoxes, sortFunc); // 执行排序+NMS
        auto end = high_resolution_clock::now();
        total += duration_cast<microseconds>(end - start).count();
    }
    return total / repeat;
}

// ============== 6. 主函数（测试入口） ==============
int main() {
    // 测试配置
    vector<int> sizes = {100, 500, 1000, 2000, 5000, 10000}; // 数据规模
    int repeat = 5; // 每个测试重复5次取平均
    cout << "==================== 排序算法与NMS性能测试 ====================" << endl;

    // -------------------------- （1）随机分布数据测试 --------------------------
    
    // 单独排序性能
    cout << setw(10) << "规模" << setw(15) << "冒泡排序(μs)" 
         << setw(15) << "选择排序(μs)" << setw(18) << "二分插入排序(μs)" 
         << setw(15) << "快速排序(μs)" << endl;
    for (int size : sizes) {
        vector<BoundingBox> boxes = generateRandomBoxes(size);
        vector<float> scores = extractScores(boxes);
        long long bubble = testSortPerformance(bubbleSort, scores, repeat);
        long long select = testSortPerformance(selectionSort, scores, repeat);
        long long insert = testSortPerformance(binaryInsertionSort, scores, repeat);
        long long quick = testSortPerformance(quickSort, scores, repeat);
        cout << setw(10) << size << setw(15) << bubble 
             << setw(15) << select << setw(18) << insert 
             << setw(15) << quick << endl;
    }

    // 排序+NMS性能
    cout << "\n【随机分布-排序+NMS】" << endl;
    cout << setw(10) << "规模" << setw(15) << "冒泡+NMS(μs)" 
         << setw(15) << "选择+NMS(μs)" << setw(18) << "二分插入+NMS(μs)" 
         << setw(15) << "快速+NMS(μs)" << endl;
    for (int size : sizes) {
        vector<BoundingBox> boxes = generateRandomBoxes(size);
        long long bubbleNms = testSortNMSPerformance(bubbleSort, boxes, repeat);
        long long selectNms = testSortNMSPerformance(selectionSort, boxes, repeat);
        long long insertNms = testSortNMSPerformance(binaryInsertionSort, boxes, repeat);
        long long quickNms = testSortNMSPerformance(quickSort, boxes, repeat);
        cout << setw(10) << size << setw(15) << bubbleNms 
             << setw(15) << selectNms << setw(18) << insertNms 
             << setw(15) << quickNms << endl;
    }

    // -------------------------- （2）聚集分布数据测试 --------------------------
    cout << "\n\n【聚集分布数据】" << endl;
    // 单独排序性能
    cout << setw(10) << "规模" << setw(15) << "冒泡排序(μs)" 
         << setw(15) << "选择排序(μs)" << setw(18) << "二分插入排序(μs)" 
         << setw(15) << "快速排序(μs)" << endl;
    for (int size : sizes) {
        vector<BoundingBox> boxes = generateClusteredBoxes(size);
        vector<float> scores = extractScores(boxes);
        long long bubble = testSortPerformance(bubbleSort, scores, repeat);
        long long select = testSortPerformance(selectionSort, scores, repeat);
        long long insert = testSortPerformance(binaryInsertionSort, scores, repeat);
        long long quick = testSortPerformance(quickSort, scores, repeat);
        cout << setw(10) << size << setw(15) << bubble 
             << setw(15) << select << setw(18) << insert 
             << setw(15) << quick << endl;
    }

    // 排序+NMS性能
    cout << "\n【聚集分布-排序+NMS】" << endl;
    cout << setw(10) << "规模" << setw(15) << "冒泡+NMS(μs)" 
         << setw(15) << "选择+NMS(μs)" << setw(18) << "二分插入+NMS(μs)" 
         << setw(15) << "快速+NMS(μs)" << endl;
    for (int size : sizes) {
        vector<BoundingBox> boxes = generateClusteredBoxes(size);
        long long bubbleNms = testSortNMSPerformance(bubbleSort, boxes, repeat);
        long long selectNms = testSortNMSPerformance(selectionSort, boxes, repeat);
        long long insertNms = testSortNMSPerformance(binaryInsertionSort, boxes, repeat);
        long long quickNms = testSortNMSPerformance(quickSort, boxes, repeat);
        cout << setw(10) << size << setw(15) << bubbleNms 
             << setw(15) << selectNms << setw(18) << insertNms 
             << setw(15) << quickNms << endl;
    }

    cout << "\n==================== 测试完成 ====================" << endl;
    return 0;
}