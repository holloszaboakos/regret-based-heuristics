//
// Created by LENOVO on 10/5/2024.
//

#ifndef REGRET_DATA_H
#define REGRET_DATA_H
#include <fstream>
#include <string>
#include <utility>
#include <filesystem>
#include <ranges>
#include <vector>
#include <coroutine>
#include <generator>

using namespace std;

struct regret_data {
    regret_data() : file(""),
                    distanceMatrix(vector<vector<double> >()),
                    expectedRegretMatrix(vector<vector<double> >()),
                    predictedRegretMatrix(vector<vector<double> >()),
                    optCost(0),
                    numberOfIterations(0),
                    initialCost(0),
                    bestCost(0) {
    }

    regret_data(
        string file,
        const vector<vector<double> > &distanceMatrix,
        const vector<vector<double> > &expectedRegretMatrix,
        const vector<vector<double> > &predictedRegretMatrix,
        const double optCost,
        const int numberOfIterations,
        const long initialCost,
        const long bestCost
    ) : file(move(file)),
        distanceMatrix(distanceMatrix),
        expectedRegretMatrix(expectedRegretMatrix),
        predictedRegretMatrix(predictedRegretMatrix),
        optCost(optCost),
        numberOfIterations(numberOfIterations),
        initialCost(initialCost),
        bestCost(bestCost) {
    }

    [[nodiscard]] string getFile() const { return file; }
    [[nodiscard]] vector<vector<double> > getDistanceMatrix() const { return distanceMatrix; }
    [[nodiscard]] vector<vector<double> > getExpectedRegretMatrix() const { return expectedRegretMatrix; }
    [[nodiscard]] vector<vector<double> > getPredictedRegretMatrix() const { return predictedRegretMatrix; }
    [[nodiscard]] double getOptCost() const { return optCost; }
    [[nodiscard]] int getNumberOfIterations() const { return numberOfIterations; }
    [[nodiscard]] long getInitialCost() const { return initialCost; }
    [[nodiscard]] long getBestCost() const { return bestCost; }

private:
    const string file;
    const vector<vector<double> > distanceMatrix;
    const vector<vector<double> > expectedRegretMatrix;
    const vector<vector<double> > predictedRegretMatrix;
    const double optCost;
    const int numberOfIterations;
    const long initialCost;
    const long bestCost;
};


generator<ifstream &> load_file_streams(const string source_folder) {
    auto file_iterator = filesystem::directory_iterator(source_folder);
    for (const auto &entry: file_iterator) {
        auto file_path = entry.path();
        ifstream is(file_path);
        co_yield is;
        is.close();
    }
    co_return;
}

generator<string> read_lines(ifstream &is) {
    string line;
    while (getline(is, line)) {
        co_yield line;
    }
    co_return;
}

generator<string> split(const string s, const string &delimiter) {
    size_t pos = 0;
    string token;
    string local_string = s;
    while ((pos = local_string.find(delimiter)) != string::npos) {
        token = local_string.substr(0, pos);
        co_yield token;
        local_string = local_string.substr(pos + delimiter.size(), local_string.size() - pos - delimiter.size());
    }
    co_yield local_string;
    co_return;
}

vector<regret_data> loadRegrets(const string &source_folder) {
    return load_file_streams(source_folder)
           | std::views::take(1) //TODO: remove
           | views::transform([](ifstream &is) {
               cout << "Loading regrets" << endl;

               const auto distanceMatrix =
                       read_lines(is)
                       | views::drop(1)
                       | views::take_while([](const string &line) { return !line.empty(); })
                       | views::transform([](const string &line) {
                           return split(line, " ")
                                  | views::transform([](const string &coordinate) { return stod(coordinate); })
                                  | ranges::to<vector<double> >();
                       })
                       | ranges::to<vector<vector<double> > >();

               const auto expectedRegretMatrix =
                       read_lines(is)
                       | views::drop_while([](string line) { return line.compare("regret:") != 0; })
                       | views::drop(1)
                       | views::take_while([](const string &line) { return !line.empty(); })
                       | views::transform([](string line) {
                           return split(line, " ")
                                  | views::transform([](string coordinate) {
                                      return stod(coordinate);
                                  })
                                  | ranges::to<vector<double> >();
                       })
                       | ranges::to<vector<vector<double> > >();
               const auto predictedRegretMatrix =
                       read_lines(is)
                       | views::drop_while([](string line) { return line.compare("regret_pred:") != 0; })
                       | views::drop(1)
                       | views::take_while([](const string &line) { return !line.empty(); })
                       | views::transform([](string line) {
                           return split(line, " ")
                                  | views::transform([](string coordinate) {
                                      return stod(coordinate);
                                  })
                                  | ranges::to<vector<double> >();
                       })
                       | ranges::to<vector<vector<double> > >();

               auto const optCost = *(
                   read_lines(is)
                   | views::filter([](const string &line) { return line.starts_with("opt_cost"); })
                   | views::transform([](const string &line) {
                       return stod(*(++split(line, " ").begin()));
                   })
                   | views::take(1)
               ).begin();

               constexpr auto numberOfIterations = 0;
               constexpr auto initialCost = 0L;
               constexpr auto bestCost = 0L;

               return regret_data(
                   "",
                   //          file.absolutePath,
                   distanceMatrix,
                   expectedRegretMatrix,
                   predictedRegretMatrix,
                   optCost,
                   numberOfIterations,
                   initialCost,
                   bestCost
               );
           })
           | ranges::to<vector<regret_data> >();
}

#endif //REGRET_DATA_H
