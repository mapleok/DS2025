#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

// 1. 冒泡排序（优化版）
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

// 2. 选择排序
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

// 3. 二分插入排序
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

// 4. 快速排序（三数取中法优化）
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

// 测试函数
int main() {
    vector<float> testArr = {3.2, 1.5, 4.8, 2.1, 0.7, 5.3};
    vector<float> arr1 = testArr, arr2 = testArr, arr3 = testArr, arr4 = testArr;

    bubbleSort(arr1);
    selectionSort(arr2);
    binaryInsertionSort(arr3);
    quickSort(arr4);

    cout << "冒泡排序结果：";
    for (float f : arr1) cout << f << " ";
    cout << "\n选择排序结果：";
    for (float f : arr2) cout << f << " ";
    cout << "\n二分插入排序结果：";
    for (float f : arr3) cout << f << " ";
    cout << "\n快速排序结果：";
    for (float f : arr4) cout << f << " ";

    return 0;
}