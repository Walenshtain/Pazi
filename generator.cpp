#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <string>

using namespace std;


vector<double> generateUniform(int count, double min, double max) {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(min, max);

    vector<double> numbers(count);
    for (int i = 0; i < count; ++i) {
        numbers[i] = dis(gen);
    }
    return numbers;
}


vector<double> generateNormal(int count, double mean, double stddev) {
    random_device rd;
    mt19937 gen(rd());
    normal_distribution<> dis(mean, stddev);

    vector<double> numbers(count);
    for (int i = 0; i < count; ++i) {
        numbers[i] = dis(gen);
    }
    return numbers;
}


void writeToFile(const string& filename, const vector<double>& numbers) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Ошибка: не удалось открыть файл для записи." << endl;
        return;
    }

    for (const auto& num : numbers) {
        file << num << "\n";
    }

    file.close();
    cout << "Числа успешно записаны в файл " << filename << endl;
}

void printUsage() {
    cout << "Usage: ./generator distribution count output_file [params]\n";
    cout << "  distribution: uniform or normal\n";
    cout << "  count: number of random numbers to generate\n";
    cout << "  output_file: name of the output file\n";
    cout << "  params for uniform: min max\n";
    cout << "  params for normal: mean stddev\n";
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        printUsage();
        return 1;
    }

    string distribution = argv[1];
    int count = stoi(argv[2]);
    string outputFile = argv[3];

    vector<double> numbers;

    if (distribution == "uniform") {
        if (argc != 6) {
            cerr << "Ошибка: для равномерного распределения нужно указать min и max." << endl;
            printUsage();
            return 1;
        }

        double min = stod(argv[4]);
        double max = stod(argv[5]);
        numbers = generateUniform(count, min, max);

    } else if (distribution == "normal") {
        if (argc != 6) {
            cerr << "Ошибка: для нормального распределения нужно указать mean и stddev." << endl;
            printUsage();
            return 1;
        }

        double mean = stod(argv[4]);
        double stddev = stod(argv[5]);
        numbers = generateNormal(count, mean, stddev);

    } else {
        cerr << "Ошибка: неизвестное распределение \"" << distribution << "\"." << endl;
        printUsage();
        return 1;
    }

    writeToFile(outputFile, numbers);

    return 0;
}
