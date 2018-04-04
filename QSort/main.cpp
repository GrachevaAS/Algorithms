#include "input_output.hpp"

inline int FindMedian(int* arr, int left, int right) {
    int mid = (left + right) / 2;
    if (arr[left] > arr[mid]) {
        std::swap(arr[left], arr[mid]);
    }
    if (arr[mid] > arr[right]) {
        std::swap(arr[mid], arr[right]);
    }
    if (arr[left] > arr[mid]) {
        std::swap(arr[left], arr[mid]);
    }
    return mid;
}

inline int Partition(int* arr, int left, int right) {
    int pivotPos = FindMedian(arr, left, right);
    std::swap(arr[pivotPos], arr[right]);
    int pivot = arr[right];
    int i = left;
    int j = left;
    for (int j = left; j < right; ++j) {
        if (arr[j] < pivot) {
            std::swap(arr[i], arr[j]);
            ++i;
        }
    }
    std::swap(arr[i], arr[right]);
    return i;
}

inline void InsertionSort(int* arr, int first, int last) {
    int temp;
	for (int i = first + 1; i <= last; ++i) {
		temp = arr[i];
		int j = i - 1;
		for (; j >= first && arr[j] > temp; j--) {
            arr[j+1] = arr[j];
		}
		arr[j+1] = temp;
    }
}
void QuickSort (int*arr, int first, int last) {
const int border = 70;
    int* Lranges = new int[100];
    int* Rranges = new int[100];
    Lranges[0] = first;
    Rranges[0] = last;
    int rSize = 1;
    int left, right = 0;
    while (rSize > 0) {
        left = Lranges[rSize-1];
        right = Rranges[rSize-1];
        int mid = Partition (arr, left, right);
        --rSize;
        if (left < mid - 1) {
            if (left < mid - border) {
                Lranges[rSize] = left;
                Rranges[rSize] = mid - 1;
                rSize++;
            } else {
                InsertionSort(arr, left, mid - 1);
            }
        }
        if (mid + 1 < right) {
            if (mid + border < right) {
                Lranges[rSize] = mid + 1;
                Rranges[rSize] = right;
                rSize++;
            } else {
                InsertionSort(arr, mid + 1, right);
            }
        }
    }
    delete [] Lranges;
    delete [] Rranges;
}

int main () {
freopen("input.txt", "r", stdin);
const int maxSize = 250000000;
int n = 0;
int* arr = new int[maxSize];
while (readInt(arr[n])) {
    ++n;
}
QuickSort(arr, 0, n - 1);
int write_buf_pos = 0;
for (int i = 9; i < n; i += 10) {
    writeInt(arr[i], write_buf_pos);
}
writeChar('\n', write_buf_pos);
delete [] arr;
}

