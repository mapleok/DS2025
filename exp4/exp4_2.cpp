#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <iomanip>

using namespace std;

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

// 基础NMS算法
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

// 生成测试数据
vector<BoundingBox> generateTestBoxes(int n) {
    vector<BoundingBox> boxes;
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<float> pos_dist(0, 800);
    uniform_real_distribution<float> size_dist(20, 100);
    uniform_real_distribution<float> score_dist(0.1, 0.99);
    
    for (int i = 0; i < n; i++) {
        float x = pos_dist(gen);
        float y = pos_dist(gen);
        float w = size_dist(gen);
        float h = size_dist(gen);
        float score = score_dist(gen);
        
        boxes.emplace_back(i, x, y, x + w, y + h, score);
    }
    return boxes;
}

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

// 测试NMS算法
void testNMSAlgorithm() {
    cout << "========== 任务2：基础NMS算法实现 ==========\n" << endl;
    
    // 生成测试数据
    vector<BoundingBox> boxes = generateTestBoxes(10);
    printBoxes(boxes, "原始边界框");
    
    // 应用NMS
    float iou_threshold = 0.5;
    vector<BoundingBox> result = nmsBasic(boxes, iou_threshold);
    printBoxes(result, "NMS处理后边界框");
    
    // 计算抑制率
    float suppression_rate = 1.0 - (float)result.size() / boxes.size();
    cout << "\n统计信息:" << endl;
    cout << "原始边界框数量: " << boxes.size() << endl;
    cout << "处理后边界框数量: " << result.size() << endl;
    cout << "抑制率: " << fixed << setprecision(2) << (suppression_rate * 100) << "%" << endl;

}

int main() {
    testNMSAlgorithm();
    return 0;
}