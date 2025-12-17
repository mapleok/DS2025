#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <iomanip>
#include <chrono>
#include <functional>
#include <fstream>
#include <cmath>
#include <string>

using namespace std;
using namespace chrono;

// ==================== 数据结构定义 ====================
struct BoundingBox {
    int id;
    float x1, y1, x2, y2;
    float score;
    bool suppressed;
    
    BoundingBox(int i, float sx1, float sy1, float sx2, float sy2, float s)
        : id(i), x1(sx1), y1(sy1), x2(sx2), y2(sy2), score(s), suppressed(false) {}
    
    // 计算面积
    float area() const {
        return (x2 - x1) * (y2 - y1);
    }
    
    // 计算IoU
    float iou(const BoundingBox& other) const {
        float inter_x1 = max(x1, other.x1);
        float inter_y1 = max(y1, other.y1);
        float inter_x2 = min(x2, other.x2);
        float inter_y2 = min(y2, other.y2);
        
        if (inter_x2 <= inter_x1 || inter_y2 <= inter_y1) {
            return 0.0f;
        }
        
        float inter_area = (inter_x2 - inter_x1) * (inter_y2 - inter_y1);
        float union_area = area() + other.area() - inter_area;
        
        if (union_area <= 0) return 0.0f;
        return inter_area / union_area;
    }
};

// ==================== 四种排序算法实现 ====================
// 1. 冒泡排序（优化版）
void bubbleSort(vector<BoundingBox>& arr) {
    int n = arr.size();
    bool hasSwap;
    for (int i = 0; i < n - 1; i++) {
        hasSwap = false;
        for (int j = 0; j < n - 1 - i; j++) {
            if (arr[j].score < arr[j + 1].score) { // 降序排序
                swap(arr[j], arr[j + 1]);
                hasSwap = true;
            }
        }
        if (!hasSwap) break; // 无交换则有序，提前退出
    }
}

// 2. 选择排序
void selectionSort(vector<BoundingBox>& arr) {
    int n = arr.size();
    int maxIdx;
    for (int i = 0; i < n - 1; i++) {
        maxIdx = i;
        for (int j = i + 1; j < n; j++) {
            if (arr[j].score > arr[maxIdx].score) maxIdx = j; // 降序排序
        }
        if (maxIdx != i) swap(arr[i], arr[maxIdx]);
    }
}

// 3. 二分插入排序
void binaryInsertionSort(vector<BoundingBox>& arr) {
    int n = arr.size();
    for (int i = 1; i < n; i++) {
        BoundingBox key = arr[i];
        int left = 0, right = i - 1;
        // 二分查找插入位置（降序）
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (arr[mid].score < key.score) right = mid - 1;
            else left = mid + 1;
        }
        // 移动元素腾出位置
        for (int j = i; j > left; j--) {
            arr[j] = arr[j - 1];
        }
        arr[left] = key;
    }
}

// 4. 快速排序（三数取中法优化）
int medianOfThree(vector<BoundingBox>& arr, int low, int high) {
    int mid = low + (high - low) / 2;
    if (arr[low].score < arr[mid].score) swap(arr[low], arr[mid]); // 降序排序
    if (arr[low].score < arr[high].score) swap(arr[low], arr[high]);
    if (arr[mid].score < arr[high].score) swap(arr[mid], arr[high]);
    swap(arr[mid], arr[high - 1]); // 基准存于high-1
    return high - 1;
}

int partition(vector<BoundingBox>& arr, int low, int high) {
    int pivotIdx = medianOfThree(arr, low, high);
    float pivot = arr[pivotIdx].score;
    int i = low, j = high - 2;
    while (true) {
        while (arr[++i].score > pivot); // 降序排序
        while (arr[--j].score < pivot);
        if (i < j) swap(arr[i], arr[j]);
        else break;
    }
    swap(arr[i], arr[pivotIdx]);
    return i;
}

void quickSortRecur(vector<BoundingBox>& arr, int low, int high) {
    if (low + 10 < high) { // 小数组用插入排序优化
        int pivotIdx = partition(arr, low, high);
        quickSortRecur(arr, low, pivotIdx - 1);
        quickSortRecur(arr, pivotIdx + 1, high);
    } else {
        // 小数组插入排序
        for (int i = low + 1; i <= high; i++) {
            BoundingBox key = arr[i];
            int j = i - 1;
            while (j >= low && arr[j].score < key.score) { // 降序排序
                arr[j + 1] = arr[j];
                j--;
            }
            arr[j + 1] = key;
        }
    }
}

void quickSort(vector<BoundingBox>& arr) {
    if (arr.size() <= 1) return;
    quickSortRecur(arr, 0, arr.size() - 1);
}

// ==================== 基础NMS算法 ====================
vector<BoundingBox> nmsBasic(vector<BoundingBox> boxes, float iou_threshold = 0.5) {
    if (boxes.empty()) return {};
    
    // 1. 按置信度降序排序
    sort(boxes.begin(), boxes.end(), [](const BoundingBox& a, const BoundingBox& b) {
        return a.score > b.score;
    });
    
    vector<BoundingBox> result;
    
    // 2. 遍历所有边界框
    for (size_t i = 0; i < boxes.size(); i++) {
        if (boxes[i].suppressed) continue;
        
        // 3. 保留当前边界框
        result.push_back(boxes[i]);
        
        // 4. 抑制与当前框IoU大于阈值的框
        for (size_t j = i + 1; j < boxes.size(); j++) {
            if (!boxes[j].suppressed) {
                float iou = boxes[i].iou(boxes[j]);
                if (iou > iou_threshold) {
                    boxes[j].suppressed = true;
                }
            }
        }
    }
    
    return result;
}

// ==================== 数据生成器 ====================
// 随机分布生成器
vector<BoundingBox> generateRandomBoxes(int num_boxes, float image_width = 1000.0f, 
                                        float image_height = 1000.0f) {
    vector<BoundingBox> boxes;
    boxes.reserve(num_boxes);
    
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<float> pos_dist(0.0f, image_width - 200.0f);
    uniform_real_distribution<float> size_dist(20.0f, 200.0f);
    uniform_real_distribution<float> score_dist(0.1f, 0.99f);
    
    for (int i = 0; i < num_boxes; i++) {
        float x = pos_dist(gen);
        float y = pos_dist(gen);
        float width = size_dist(gen);
        float height = size_dist(gen);
        
        // 确保边界框在图像范围内
        float x2 = min(x + width, image_width);
        float y2 = min(y + height, image_height);
        
        boxes.emplace_back(i, x, y, x2, y2, score_dist(gen));
    }
    
    return boxes;
}

// 聚集分布生成器
vector<BoundingBox> generateClusteredBoxes(int num_boxes, int num_clusters = 5,
                                           float image_width = 1000.0f,
                                           float image_height = 1000.0f) {
    vector<BoundingBox> boxes;
    boxes.reserve(num_boxes);
    
    random_device rd;
    mt19937 gen(rd());
    
    // 生成聚类中心
    vector<pair<float, float>> cluster_centers(num_clusters);
    uniform_real_distribution<float> center_dist(100.0f, image_width - 100.0f);
    
    for (int i = 0; i < num_clusters; i++) {
        cluster_centers[i] = {center_dist(gen), center_dist(gen)};
    }
    
    // 生成每个聚类的边界框
    uniform_real_distribution<float> offset_dist(-50.0f, 50.0f);
    uniform_real_distribution<float> size_dist(20.0f, 100.0f);
    uniform_real_distribution<float> score_dist(0.1f, 0.99f);
    
    int boxes_per_cluster = num_boxes / num_clusters;
    
    for (int cluster_idx = 0; cluster_idx < num_clusters; cluster_idx++) {
        float center_x = cluster_centers[cluster_idx].first;
        float center_y = cluster_centers[cluster_idx].second;
        
        for (int i = 0; i < boxes_per_cluster; i++) {
            float x = center_x + offset_dist(gen);
            float y = center_y + offset_dist(gen);
            float width = size_dist(gen);
            float height = size_dist(gen);
            
            // 确保边界框在图像范围内
            x = max(0.0f, min(x, image_width - width));
            y = max(0.0f, min(y, image_height - height));
            
            boxes.emplace_back(
                cluster_idx * boxes_per_cluster + i,
                x, y,
                x + width, y + height,
                score_dist(gen)
            );
        }
    }
    
    // 添加剩余的边界框（随机分布）
    int remaining = num_boxes - boxes.size();
    for (int i = 0; i < remaining; i++) {
        uniform_real_distribution<float> pos_dist(0.0f, image_width - 100.0f);
        float x = pos_dist(gen);
        float y = pos_dist(gen);
        float width = size_dist(gen);
        float height = size_dist(gen);
        
        boxes.emplace_back(
            boxes.size(),
            x, y,
            x + width, y + height,
            score_dist(gen)
        );
    }
    
    return boxes;
}

// ==================== NMS算法（带计时） ====================
template<typename SortFunc>
pair<vector<BoundingBox>, long long> nmsWithTiming(vector<BoundingBox> boxes, 
                                                   float iou_threshold,
                                                   SortFunc sortFunc,
                                                   const string& sortName) {
    // 重置所有边界框的状态
    for (auto& box : boxes) {
        box.suppressed = false;
    }
    
    auto total_start = high_resolution_clock::now();
    
    // 排序阶段
    auto sort_start = high_resolution_clock::now();
    sortFunc(boxes);
    auto sort_end = high_resolution_clock::now();
    auto sort_time = duration_cast<microseconds>(sort_end - sort_start).count();
    
    // NMS阶段
    vector<BoundingBox> result;
    auto nms_start = high_resolution_clock::now();
    
    for (size_t i = 0; i < boxes.size(); i++) {
        if (boxes[i].suppressed) continue;
        
        result.push_back(boxes[i]);
        
        for (size_t j = i + 1; j < boxes.size(); j++) {
            if (!boxes[j].suppressed) {
                float iou = boxes[i].iou(boxes[j]);
                if (iou > iou_threshold) {
                    boxes[j].suppressed = true;
                }
            }
        }
    }
    
    auto nms_end = high_resolution_clock::now();
    auto nms_time = duration_cast<microseconds>(nms_end - nms_start).count();
    
    auto total_end = high_resolution_clock::now();
    auto total_time = duration_cast<microseconds>(total_end - total_start).count();
    
    // 输出结果
    cout << setw(12) << sortName << " | "
         << setw(10) << sort_time << " μs | "
         << setw(10) << nms_time << " μs | "
         << setw(10) << total_time << " μs | "
         << setw(8) << result.size() << endl;
    
    return {result, total_time};
}

// ==================== 辅助函数 ====================
// 打印边界框信息
void printBoxes(const vector<BoundingBox>& boxes, const string& title) {
    cout << "\n" << title << " (" << boxes.size() << "个):" << endl;
    cout << "ID\tScore\t坐标(x1,y1,x2,y2)\t面积" << endl;
    cout << "------------------------------------------------" << endl;
    
    for (const auto& box : boxes) {
        cout << box.id << "\t" << fixed << setprecision(3) << box.score << "\t"
             << "(" << box.x1 << "," << box.y1 << "," << box.x2 << "," << box.y2 << ")\t"
             << box.area() << endl;
    }
}

// 统计信息分析
void analyzeDistribution(const vector<BoundingBox>& boxes, const string& title) {
    cout << "\n" << title << " 统计信息:" << endl;
    cout << "边界框数量: " << boxes.size() << endl;
    
    // 计算平均面积
    float total_area = 0;
    float min_area = INFINITY, max_area = 0;
    float total_score = 0;
    
    for (const auto& box : boxes) {
        float area = box.area();
        total_area += area;
        total_score += box.score;
        min_area = min(min_area, area);
        max_area = max(max_area, area);
    }
    
    cout << "平均面积: " << fixed << setprecision(2) << (total_area / boxes.size()) << endl;
    cout << "最小面积: " << min_area << endl;
    cout << "最大面积: " << max_area << endl;
    cout << "平均置信度: " << (total_score / boxes.size()) << endl;
    
    // 位置分布分析
    float avg_x = 0, avg_y = 0;
    for (const auto& box : boxes) {
        avg_x += (box.x1 + box.x2) / 2;
        avg_y += (box.y1 + box.y2) / 2;
    }
    avg_x /= boxes.size();
    avg_y /= boxes.size();
    cout << "中心位置: (" << avg_x << ", " << avg_y << ")" << endl;
}

// 保存数据到文件
void saveToFile(const vector<BoundingBox>& boxes, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Cannot open file: " << filename << endl;
        return;
    }
    
    file << "id,x1,y1,x2,y2,score" << endl;
    for (const auto& box : boxes) {
        file << box.id << ","
             << box.x1 << "," << box.y1 << ","
             << box.x2 << "," << box.y2 << ","
             << box.score << endl;
    }
    
    file.close();
    cout << "Data saved to: " << filename << endl;
}

// ==================== 主测试函数 ====================
void testNMSAlgorithm() {
    cout << "========== Task 2: Basic NMS Algorithm Test ==========\n" << endl;
    
    // 生成测试数据
    auto boxes = generateRandomBoxes(10);
    printBoxes(boxes, "Original Bounding Boxes");
    
    // 应用NMS
    float iou_threshold = 0.5;
    vector<BoundingBox> result = nmsBasic(boxes, iou_threshold);
    printBoxes(result, "After NMS Processing");
    
    // 计算抑制率
    float suppression_rate = 1.0 - (float)result.size() / boxes.size();
    cout << "\nStatistics:" << endl;
    cout << "Original boxes count: " << boxes.size() << endl;
    cout << "After NMS boxes count: " << result.size() << endl;
    cout << "Suppression rate: " << fixed << setprecision(2) << (suppression_rate * 100) << "%" << endl;
}

void testDataGenerator() {
    cout << "\n========== Task 3: Data Generator Test ==========\n" << endl;
    
    vector<int> test_sizes = {100, 500, 1000, 5000, 10000};
    
    for (int size : test_sizes) {
        cout << "\n=== Test Data Size: " << size << " ===" << endl;
        
        // 生成随机分布数据
        auto random_boxes = generateRandomBoxes(size);
        analyzeDistribution(random_boxes, "Random Distribution");
        
        // 生成聚集分布数据
        auto clustered_boxes = generateClusteredBoxes(size);
        analyzeDistribution(clustered_boxes, "Clustered Distribution");
        
        // 保存示例数据
        if (size == 1000) {
            saveToFile(random_boxes, "random_boxes_1000.csv");
            saveToFile(clustered_boxes, "clustered_boxes_1000.csv");
        }
    }
    
    // 数据分布分析
    cout << "\n========== Data Distribution Analysis ==========" << endl;
    cout << "1. Random Distribution Features:" << endl;
    cout << "   - Bounding boxes are uniformly distributed across the image" << endl;
    cout << "   - Random sizes and confidence scores" << endl;
    cout << "   - Simulates general object detection scenarios" << endl;
    
    cout << "\n2. Clustered Distribution Features:" << endl;
    cout << "   - Bounding boxes cluster around several center points" << endl;
    cout << "   - Simulates dense object scenes (e.g., crowd detection)" << endl;
    cout << "   - Higher probability of IoU overlap" << endl;
    
    cout << "\n3. Impact on NMS Algorithm:" << endl;
    cout << "   - Random distribution: relatively less IoU calculations" << endl;
    cout << "   - Clustered distribution: significantly more IoU calculations" << endl;
    cout << "   - Clustered distribution may lead to higher suppression rate" << endl;
}

void performanceTest() {
    cout << "\n========== Task 4: Comprehensive Performance Test ==========\n" << endl;
    
    vector<int> sizes = {100, 500, 1000, 5000, 10000};
    float iou_threshold = 0.5;
    
    // 定义排序算法（四种）- 使用 push_back 而不是初始化列表
    vector<pair<function<void(vector<BoundingBox>&)>, string>> algorithms;
    algorithms.push_back({bubbleSort, "BubbleSort"});
    algorithms.push_back({selectionSort, "SelectionSort"});
    algorithms.push_back({binaryInsertionSort, "BinaryInsertion"});
    algorithms.push_back({quickSort, "QuickSort"});
    
    // 测试不同数据分布 - 使用 push_back 而不是初始化列表
    vector<pair<string, function<vector<BoundingBox>(int)>>> distributions;
    distributions.push_back({"Random Distribution", generateRandomBoxes});
    distributions.push_back({"Clustered Distribution", generateClusteredBoxes});
    
    for (const auto& distribution : distributions) {
        cout << "\n=== " << distribution.first << " ===" << endl;
        cout << "Algorithm        Sort Time    NMS Time     Total Time   Boxes Kept" << endl;
        cout << "---------------------------------------------------------------" << endl;
        
        for (int size : sizes) {
            cout << "\nData Size: " << size << endl;
            
            auto boxes = distribution.second(size);
            
            for (const auto& [sortFunc, name] : algorithms) {
                auto [result, time] = nmsWithTiming(boxes, iou_threshold, sortFunc, name);
            }
        }
    }
    
    // 性能分析总结
    cout << "\n========== Performance Test Summary ==========" << endl;
    cout << "1. Sorting Algorithm Performance Comparison:" << endl;
    cout << "   - QuickSort: Usually the fastest, suitable for large-scale data" << endl;
    cout << "   - BinaryInsertionSort: Efficient for small datasets" << endl;
    cout << "   - BubbleSort/SelectionSort: Suitable for teaching demonstrations, low efficiency in practice" << endl;
    
    cout << "\n2. Impact of Data Distribution:" << endl;
    cout << "   - Clustered distribution: Longer NMS time (more IoU calculations)" << endl;
    cout << "   - Random distribution: Relatively shorter NMS time" << endl;
    
    cout << "\n3. Impact of Data Size:" << endl;
    cout << "   - Small data (≤1000): Little difference between algorithms" << endl;
    cout << "   - Large data (≥5000): QuickSort shows significant advantage" << endl;
}

// ==================== 主函数 ====================
int main() {
    cout << fixed << setprecision(2);
    
    cout << "=====================================================" << endl;
    cout << "          NMS Algorithm Performance Test System       " << endl;
    cout << "=====================================================\n" << endl;
    
    // 测试基础NMS算法
    testNMSAlgorithm();
    
    // 测试数据生成器
    testDataGenerator();
    
    // 运行性能测试
    performanceTest();
    
    cout << "\n========== All Tests Completed ==========" << endl;
    cout << "Generated CSV files can be used for further analysis:" << endl;
    cout << "- random_boxes_1000.csv" << endl;
    cout << "- clustered_boxes_1000.csv" << endl;
    
    return 0;
}