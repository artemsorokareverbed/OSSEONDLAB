#include <iostream>
#include <windows.h>
using namespace std;

int n;
int* arr;
volatile int minValue, maxValue;
volatile int minIndex, maxIndex;
volatile double average;

typedef void (*ReplaceFunc)(int*, int, int, double);

DWORD WINAPI min_max(LPVOID param)
{
    minValue = arr[0];
    maxValue = arr[0];
    minIndex = 0;
    maxIndex = 0;

    for (int i = 1; i < n; i++)
    {
        if (arr[i] < minValue){
            minValue = arr[i];
            minIndex = i;
        }
        Sleep(7);  

        if (arr[i] > maxValue)
        {
            maxValue = arr[i];
            maxIndex = i;
        }
        Sleep(7);  
    }

    cout << "min = " << minValue << endl;
    cout << "max = " << maxValue << endl;

    return 0;
}

DWORD WINAPI average_thread(LPVOID param)
{
    int sum = 0;

    for (int i = 0; i < n; i++)
    {
        sum = sum + arr[i];
        Sleep(12);
    }

    average = (double)sum / n;
    cout << "average = " << average << endl;

    return 0;
}

int main()
{
    HANDLE hMinMax;
    HANDLE hAverage;
    DWORD IDMinMax;
    DWORD IDAverage;

    
    cout << "Enter size of array: ";
    cin >> n;

    arr = new int[n];
    cout << "Enter elements: ";
    for (int i = 0; i < n; i++)
    {
        cin >> arr[i];
    }

    hMinMax = CreateThread(NULL, 0, min_max, NULL, 0, &IDMinMax);
    if (hMinMax == NULL)
        return GetLastError();

    hAverage = CreateThread(NULL, 0, average_thread, NULL, 0, &IDAverage);
    if (hAverage == NULL)
        return GetLastError();

    WaitForSingleObject(hMinMax, INFINITE);
    WaitForSingleObject(hAverage, INFINITE);
    CloseHandle(hMinMax);
    CloseHandle(hAverage);

    HMODULE hLib = LoadLibraryA("test.dll");
    if (hLib == NULL)
    {
        cout << "Cannot load test.dll" << endl;
        return GetLastError();
    }

    ReplaceFunc replace = (ReplaceFunc)GetProcAddress(hLib, "replace_min_max");
    if (replace == NULL)
    {
        cout << "Cannot find replace_min_max" << endl;
        return GetLastError();
    }
    replace(arr, minIndex, maxIndex, average);
    FreeLibrary(hLib);
    cout << "Result array: ";
    for (int i = 0; i < n; i++)
    {
        cout << arr[i] << " ";
    }
    cout << endl;
    delete[] arr;
    return 0;
}