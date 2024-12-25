#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <limits>
#include <stdexcept>

using namespace std;

vector<double> calculateRanks(const vector<double>& data) {
    vector<pair<double, int>> sortedData;
    for (int i = 0; i < data.size(); ++i) {
        sortedData.emplace_back(data[i], i);
    }

    sort(sortedData.begin(), sortedData.end());

    vector<double> ranks(data.size());
    int i = 0;
    while (i < sortedData.size()) {
        int j = i;
        while (j < sortedData.size() && sortedData[j].first == sortedData[i].first) {
            ++j;
        }

        double rank = (i + j - 1) / 2.0 + 1;
        for (int k = i; k < j; ++k) {
            ranks[sortedData[k].second] = rank;
        }

        i = j;
    }

    return ranks;
}
double calculatePValue(double Z) {
    return erfc(fabs(Z) / sqrt(2.0));
}

pair<double, double> mannWhitneyU(const vector<double>& group1, const vector<double>& group2) {
    vector<double> combined = group1;
    combined.insert(combined.end(), group2.begin(), group2.end());

    vector<double> ranks = calculateRanks(combined);

    double rankSum1 = accumulate(ranks.begin(), ranks.begin() + group1.size(), 0.0);
    double rankSum2 = accumulate(ranks.begin() + group1.size(), ranks.end(), 0.0);

    int n1 = group1.size();
    int n2 = group2.size();

    if (n1 == 0 || n2 == 0) {
        throw runtime_error("Ошибка: одна из выборок пуста.");
    }

    double U1 = rankSum1 - (n1 * (n1 + 1)) / 2.0;
    double U2 = rankSum2 - (n2 * (n2 + 1)) / 2.0;

    double U = min(U1, U2);

    double meanU = (n1 * n2) / 2.0;
    double stdU = sqrt((double)n1 * n2 * (n1 + n2 + 1) / 12.0);

    if (stdU == 0) {
        throw runtime_error("Ошибка: стандартное отклонение равно нулю.");
    }

    double Z = (U - meanU) / stdU;
    return {U, Z};
}

void printUsage() {
    cout << "Usage: ./test file1 file2" << endl;
    cout << "  file1: Path to the first file containing the data for group 1." << endl;
    cout << "  file2: Path to the second file containing the data for group 2." << endl;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printUsage();
        return 1;
    }

    string file1Path = argv[1];
    string file2Path = argv[2];

    ifstream file1(file1Path);
    if (!file1.is_open()) {
        cerr << "Ошибка: не удалось открыть первый файл." << endl;
        return 1;
    }

    vector<double> group1;
    double number;
    while (file1 >> number) {
        group1.push_back(number);
    }
    file1.close();

    ifstream file2(file2Path);
    if (!file2.is_open()) {
        cerr << "Ошибка: не удалось открыть второй файл." << endl;
        return 1;
    }

    vector<double> group2;
    while (file2 >> number) {
        group2.push_back(number);
    }
    file2.close();

    if (group1.empty() || group2.empty()) {
        cerr << "Ошибка: одна из групп данных пуста." << endl;
        return 1;
    }

    try {
        auto [U, Z] = mannWhitneyU(group1, group2);
        double pValue = calculatePValue(Z);

        pValue = max(0.0, min(1.0, pValue));

        cout << "Статистика U: " << U << endl;
        cout << "Z-значение: " << Z << endl;
        cout << "p-value: " << pValue << endl;

        if (pValue < 0.05) {
            cout << "Результат: Есть основания отвергнуть нулевую гипотезу (p < 0.05)." << endl;
        } else {
            cout << "Результат: Нет оснований отвергать нулевую гипотезу (p >= 0.05)." << endl;
        }
    } catch (const runtime_error& e) {
        cerr << e.what() << endl;
        return 1;
    }

    return 0;
}
