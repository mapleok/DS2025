#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <cmath>

using namespace std;

struct BoundingBox {
    int id;
    float x1, y1, x2, y2;
    float score;
    
    BoundingBox(int i, float sx1, float sy1, float sx2, float sy2, float s)
        : id(i), x1(sx1), y1(sy1), x2(sx2), y2(sy2), score(s) {}
    
    float area() const {
        return (x2 - x1) * (y2 - y1);
    }
};

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

// 统计信息
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
        cerr << "无法打开文件: " << filename << endl;
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
    cout << "数据已保存到: " << filename << endl;
}

// 测试数据生成器
void testDataGenerator() {
    cout << "========== 任务3：数据生成器实现 ==========\n" << endl;
    
    vector<int> test_sizes = {100, 500, 1000, 5000, 10000};
    
    for (int size : test_sizes) {
        cout << "\n=== 测试数据规模: " << size << " ===" << endl;
        
        // 生成随机分布数据
        auto random_boxes = generateRandomBoxes(size);
        analyzeDistribution(random_boxes, "随机分布");
        
        // 生成聚集分布数据
        auto clustered_boxes = generateClusteredBoxes(size);
        analyzeDistribution(clustered_boxes, "聚集分布");
        
        // 保存示例数据
        if (size == 1000) {
            saveToFile(random_boxes, "random_boxes_1000.csv");
            saveToFile(clustered_boxes, "clustered_boxes_1000.csv");
        }
    }

}

int main() {
    testDataGenerator();
    return 0;
}