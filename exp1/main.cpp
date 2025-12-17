#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>
#include <cmath>
#include <chrono>
#include <iomanip>

using namespace std;

// ============== 1. 复数类定义 ==============
class Complex {
private:
    double real;
    double imag;
public:
    Complex(double r = 0, double i = 0) : real(r), imag(i) {}
    
    // 获取模
    double modulus() const {
        return sqrt(real * real + imag * imag);
    }
    
    // 重载运算符
    bool operator==(const Complex& other) const {
        return fabs(real - other.real) < 1e-9 && fabs(imag - other.imag) < 1e-9;
    }
    
    bool operator<(const Complex& other) const {
        double mod1 = modulus();
        double mod2 = other.modulus();
        if (fabs(mod1 - mod2) < 1e-9) {
            return real < other.real;
        }
        return mod1 < mod2;
    }
    
    friend ostream& operator<<(ostream& os, const Complex& c) {
        os << "(" << c.real << (c.imag >= 0 ? "+" : "") << c.imag << "i)";
        return os;
    }
};

// 置乱函数
template<typename T>
void shuffleVector(vector<T>& vec) {
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    shuffle(vec.begin(), vec.end(), default_random_engine(seed));
}

// 查找函数（返回索引，未找到返回-1）
template<typename T>
int findVector(const vector<T>& vec, const T& value) {
    for (int i = 0; i < vec.size(); i++) {
        if (vec[i] == value) return i;
    }
    return -1;
}

// 唯一化函数
template<typename T>
int deduplicate(vector<T>& vec) {
    int oldSize = vec.size();
    int i = 1;
    while (i < vec.size()) {
        if (findVector(vec, vec[i]) < i) {
            vec.erase(vec.begin() + i);
        } else {
            i++;
        }
    }
    return oldSize - vec.size();
}

// 冒泡排序
template<typename T>
void bubbleSort(vector<T>& vec) {
    int n = vec.size();
    for (int i = 0; i < n - 1; i++) {
        bool swapped = false;
        for (int j = 0; j < n - i - 1; j++) {
            if (!(vec[j] < vec[j+1])) {
                swap(vec[j], vec[j+1]);
                swapped = true;
            }
        }
        if (!swapped) break;
    }
}

// 归并排序
template<typename T>
void merge(vector<T>& vec, int left, int mid, int right) {
    vector<T> temp(right - left + 1);
    int i = left, j = mid + 1, k = 0;
    
    while (i <= mid && j <= right) {
        if (vec[i] < vec[j]) {
            temp[k++] = vec[i++];
        } else {
            temp[k++] = vec[j++];
        }
    }
    
    while (i <= mid) temp[k++] = vec[i++];
    while (j <= right) temp[k++] = vec[j++];
    
    for (int p = 0; p < k; p++) {
        vec[left + p] = temp[p];
    }
}

template<typename T>
void mergeSort(vector<T>& vec, int left, int right) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;
    mergeSort(vec, left, mid);
    mergeSort(vec, mid + 1, right);
    merge(vec, left, mid, right);
}

template<typename T>
void mergeSort(vector<T>& vec) {
    if (vec.empty()) return;
    mergeSort(vec, 0, vec.size() - 1);
}

// 区间查找函数
vector<Complex> rangeSearch(const vector<Complex>& vec, double m1, double m2) {
    vector<Complex> result;
    for (const auto& c : vec) {
        double mod = c.modulus();
        if (mod >= m1 && mod <= m2) {
            result.push_back(c);
        }
    }
    return result;
}

// ============== 2. 栈计算器实现 ==============
class Stack {
private:
    vector<double> data;
public:
    bool empty() const { return data.empty(); }
    int size() const { return data.size(); }
    void push(double val) { data.push_back(val); }
    double pop() {
        double val = data.back();
        data.pop_back();
        return val;
    }
    double top() const { return data.back(); }
};

// 操作符优先级
int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}

// 执行运算
double applyOp(double a, double b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': 
            if (fabs(b) < 1e-9) throw runtime_error("除零错误");
            return a / b;
    }
    return 0;
}

// 表达式求值
double evaluateExpression(const string& expression) {
    Stack values;
    Stack ops;
    
    for (int i = 0; i < expression.length(); i++) {
        if (expression[i] == ' ') continue;
        
        if (isdigit(expression[i]) || expression[i] == '.') {
            double val = 0;
            int decimal = 0;
            bool hasDecimal = false;
            
            while (i < expression.length() && 
                   (isdigit(expression[i]) || expression[i] == '.')) {
                if (expression[i] == '.') {
                    hasDecimal = true;
                    decimal = 1;
                } else {
                    if (hasDecimal) {
                        val = val + (expression[i] - '0') / pow(10, decimal);
                        decimal++;
                    } else {
                        val = val * 10 + (expression[i] - '0');
                    }
                }
                i++;
            }
            i--;
            values.push(val);
        }
        else if (expression[i] == '(') {
            ops.push('(');
        }
        else if (expression[i] == ')') {
            while (!ops.empty() && ops.top() != '(') {
                double val2 = values.pop();
                double val1 = values.pop();
                char op = ops.pop();
                values.push(applyOp(val1, val2, op));
            }
            if (!ops.empty()) ops.pop();
        }
        else if (expression[i] == '+' || expression[i] == '-' ||
                 expression[i] == '*' || expression[i] == '/') {
            while (!ops.empty() && precedence(ops.top()) >= precedence(expression[i])) {
                double val2 = values.pop();
                double val1 = values.pop();
                char op = ops.pop();
                values.push(applyOp(val1, val2, op));
            }
            ops.push(expression[i]);
        }
    }
    
    while (!ops.empty()) {
        double val2 = values.pop();
        double val1 = values.pop();
        char op = ops.pop();
        values.push(applyOp(val1, val2, op));
    }
    
    return values.pop();
}

// ============== 3. 最大矩形面积 ==============
int largestRectangleArea(const vector<int>& heights) {
    int n = heights.size();
    vector<int> left(n), right(n, n);
    vector<int> stack;
    
    // 计算左边界
    for (int i = 0; i < n; i++) {
        while (!stack.empty() && heights[stack.back()] >= heights[i]) {
            stack.pop_back();
        }
        left[i] = stack.empty() ? -1 : stack.back();
        stack.push_back(i);
    }
    
    stack.clear();
    
    // 计算右边界
    for (int i = n - 1; i >= 0; i--) {
        while (!stack.empty() && heights[stack.back()] >= heights[i]) {
            stack.pop_back();
        }
        right[i] = stack.empty() ? n : stack.back();
        stack.push_back(i);
    }
    
    // 计算最大面积
    int maxArea = 0;
    for (int i = 0; i < n; i++) {
        maxArea = max(maxArea, heights[i] * (right[i] - left[i] - 1));
    }
    return maxArea;
}

// 随机生成测试数据
vector<int> generateRandomHeights(int n) {
    vector<int> heights(n);
    srand(time(nullptr));
    for (int i = 0; i < n; i++) {
        heights[i] = rand() % 10001; // 0~10000
    }
    return heights;
}

// ============== 主函数测试 ==============
int main() {
    cout << "============= 任务1：复数向量操作 =============\n";
    
    // 生成随机复数向量
    vector<Complex> complexVec;
    srand(time(nullptr));
    for (int i = 0; i < 20; i++) {
        complexVec.push_back(Complex(rand() % 100 - 50, rand() % 100 - 50));
    }
    
    cout << "原始向量: ";
    for (const auto& c : complexVec) cout << c << " ";
    cout << "\n大小: " << complexVec.size() << endl;
    
    // 1.1 置乱
    shuffleVector(complexVec);
    cout << "\n置乱后: ";
    for (const auto& c : complexVec) cout << c << " ";
    
    // 1.2 查找测试
    Complex target = complexVec[5];
    int idx = findVector(complexVec, target);
    cout << "\n查找 " << target << " 的位置: " << idx << endl;
    
    // 1.3 插入和删除
    complexVec.insert(complexVec.begin() + 3, Complex(100, 100));
    cout << "插入后大小: " << complexVec.size() << endl;
    
    complexVec.erase(complexVec.begin() + 3);
    cout << "删除后大小: " << complexVec.size() << endl;
    
    // 1.4 唯一化
    int removed = deduplicate(complexVec);
    cout << "唯一化移除元素: " << removed << endl;
    cout << "唯一化后大小: " << complexVec.size() << endl;
    
    // 1.5 排序效率比较
    vector<Complex> vec1 = complexVec;
    vector<Complex> vec2 = complexVec;
    vector<Complex> vec3 = complexVec;
    
    // 顺序（已排序）
    mergeSort(vec1);
    
    // 逆序
    reverse(vec2.begin(), vec2.end());
    
    cout << "\n排序效率比较（元素数: " << complexVec.size() << "）:" << endl;
    cout << left << setw(15) << "状态" << setw(20) << "冒泡排序(ms)" << setw(20) << "归并排序(ms)" << endl;
    
    // 测试三种状态
    vector<vector<Complex>> testVecs = {vec1, vec2, vec3};
    string states[] = {"顺序", "逆序", "乱序"};
    
    for (int i = 0; i < 3; i++) {
        vector<Complex> bubbleCopy = testVecs[i];
        vector<Complex> mergeCopy = testVecs[i];
        
        auto start = chrono::high_resolution_clock::now();
        bubbleSort(bubbleCopy);
        auto end = chrono::high_resolution_clock::now();
        auto bubbleTime = chrono::duration_cast<chrono::microseconds>(end - start).count();
        
        start = chrono::high_resolution_clock::now();
        mergeSort(mergeCopy);
        end = chrono::high_resolution_clock::now();
        auto mergeTime = chrono::duration_cast<chrono::microseconds>(end - start).count();
        
        cout << left << setw(15) << states[i] 
             << setw(20) << bubbleTime 
             << setw(20) << mergeTime << endl;
    }
    
    // 1.6 区间查找
    mergeSort(complexVec);
    cout << "\n排序后向量: ";
    for (const auto& c : complexVec) cout << c << " ";
    
    double m1 = 20, m2 = 50;
    vector<Complex> result = rangeSearch(complexVec, m1, m2);
    cout << "\n模在[" << m1 << "," << m2 << "]之间的元素(" << result.size() << "个): ";
    for (const auto& c : result) cout << c << " ";
    
    cout << "\n\n============= 任务2：栈计算器 =============\n";
    
    vector<string> expressions = {
        "3 + 5 * 2",
        "(3 + 5) * 2",
        "10 / 2 - 3",
        "2.5 * 3 + 1.5",
        "1 + 2 * (3 + 4) / 2"
    };
    
    for (const auto& expr : expressions) {
        try {
            double result = evaluateExpression(expr);
            cout << expr << " = " << result << endl;
        } catch (const exception& e) {
            cout << expr << " -> 错误: " << e.what() << endl;
        }
    }
    
    cout << "\n============= 任务3：最大矩形面积 =============\n";
    
    // 示例测试
    vector<vector<int>> testCases = {
        {2, 1, 5, 6, 2, 3},
        {2, 4},
        {1, 2, 3, 4, 5},
        {5, 4, 3, 2, 1},
        {1}
    };
    
    for (int i = 0; i < testCases.size(); i++) {
        int area = largestRectangleArea(testCases[i]);
        cout << "测试用例 " << i+1 << ": [";
        for (int j = 0; j < testCases[i].size(); j++) {
            cout << testCases[i][j];
            if (j < testCases[i].size() - 1) cout << ", ";
        }
        cout << "] -> 最大面积: " << area << endl;
    }
    
    // 随机生成10组数据测试
    cout << "\n随机生成10组数据测试:" << endl;
    srand(time(nullptr));
    for (int i = 1; i <= 10; i++) {
        int n = rand() % 100 + 1; // 1~100个元素
        vector<int> heights = generateRandomHeights(n);
        int area = largestRectangleArea(heights);
        cout << "第" << setw(2) << i << "组: n=" << setw(4) << n 
             << ", 最大面积=" << setw(8) << area << endl;
    }
    
    return 0;
}