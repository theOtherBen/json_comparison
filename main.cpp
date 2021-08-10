#include <iostream>
#include <fstream>
#include "picojson.h"
#include <chrono>
#include "json.h"
#include <unistd.h>
#include <sstream>
#include <iostream>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/error/en.h"

using namespace std::chrono;
using time_point = steady_clock::time_point;
using namespace std;

std::string get_file_contents(const char *filename) {
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (in) {
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return (contents);
    }
    throw (errno);
}

void report_time_diff(std::string parser_name, std::chrono::steady_clock::time_point begin,
                      std::chrono::steady_clock::time_point end) {
    std::cout << parser_name << " time = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()
              << "[ms]" << std::endl;
}

void print_divider(string parser_name) {
    std::cout << "-------------------  " << parser_name << "  -------------------" << std::endl;
}

//-------------------------------- picojson ----------------------------------

void parse_from_file_picojson(string &json, picojson::value &v) {
    auto begin = steady_clock::now();
    std::string err = picojson::parse(v, json);
    if (!err.empty()) {
        std::cerr << err << std::endl;
    }
    auto end = steady_clock::now();
    report_time_diff("PicoJson parse", begin, end);
}

void export_raw_picojson(picojson::value &v, string output_path) {
    auto begin = steady_clock::now();
    std::ofstream o(output_path + "raw.json");
    o << v.serialize(false) << std::endl;
    auto end = steady_clock::now();
    report_time_diff("PicoJson export raw", begin, end);
}

void export_pretty_picojson(picojson::value &v, string output_path) {
    auto begin = steady_clock::now();
    std::ofstream o(output_path + "pretty.json");
    o << v.serialize(true) << std::endl;
    auto end = steady_clock::now();
    report_time_diff("PicoJson export pretty", begin, end);
}

void export_single_line_picojson(picojson::value &v, string output_path) {
    auto begin = steady_clock::now();
    std::ofstream o(output_path + "single_line.json");
    auto all_data = v.get<picojson::array>();
    for (auto x : all_data) {
        o << x << endl;
    }
    auto end = steady_clock::now();
    report_time_diff("PicoJson export single line", begin, end);
}

void benchmark_picojson(string &json) {
    print_divider("picoJson");
    string output_path = "outputs/picojson/";
    picojson::value picojson_v;

    parse_from_file_picojson(json, picojson_v);
    export_raw_picojson(picojson_v, output_path);
    export_pretty_picojson(picojson_v, output_path);
    export_single_line_picojson(picojson_v, output_path);
}

//-------------------------------- nlohmann ----------------------------------

void parse_from_file_nlohmann(string &json, nlohmann::json &v) {
    auto begin = steady_clock::now();
    v = nlohmann::json::parse(json);
    auto end = steady_clock::now();
    report_time_diff("Nlohmann parse", begin, end);
}

void export_raw_nlohmann(nlohmann::json &v, string &output_path) {
    auto begin = steady_clock::now();
    std::ofstream o(output_path + "raw.json");
    o << v << std::endl;
    auto end = steady_clock::now();
    report_time_diff("Nlohmann export raw", begin, end);
}

void export_pretty_nlohmann(nlohmann::json &v, string &output_path) {
    auto begin = steady_clock::now();
    std::ofstream o(output_path + "pretty.json");
    o << v.dump(4) << std::endl;
    auto end = steady_clock::now();
    report_time_diff("Nlohmann export pretty", begin, end);
}

void export_single_line_nlohmann(nlohmann::json &v, string &output_path) {
    auto begin = steady_clock::now();
    std::ofstream o(output_path + "one_line.json");
    for (auto &el : v.items()) {
        o << el.value() << std::endl;
    }
    auto end = steady_clock::now();
    report_time_diff("Nlohmann export single line time", begin, end);
}

void benchmark_nlohmann(string json) {
    print_divider("nlohmann");
    string output_path = "outputs/nlohmann/";
    nlohmann::json nlohmann_v;

    parse_from_file_nlohmann(json, nlohmann_v);
    export_raw_nlohmann(nlohmann_v, output_path);
    export_pretty_nlohmann(nlohmann_v, output_path);
    export_single_line_nlohmann(nlohmann_v, output_path);

}
//-------------------------------- rapidjson ----------------------------------

void parse_from_file_rapidjson(const char *json, rapidjson::Document &d) {
    auto begin = std::chrono::steady_clock::now();
    d.Parse(json);
    auto end = std::chrono::steady_clock::now();
    report_time_diff("rapidJson parse", begin, end);
}

void export_raw_rapidjson(rapidjson::Document &d, string &output_path) {
    auto begin = std::chrono::steady_clock::now();
    std::ofstream o(output_path + "raw.json");
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    d.Accept(writer);
    o << buffer.GetString() << std::endl;
    auto end = std::chrono::steady_clock::now();
    report_time_diff("PicoJson export raw", begin, end);

}

void export_pretty_rapidjson(rapidjson::Document &d, string &output_path) {
    char writeBuffer[65536];
    FILE *pFile;
    string full_output_path = output_path + "pretty.json";

    auto begin = std::chrono::steady_clock::now();
    std::ofstream o(output_path + "pretty.json");
    pFile = fopen(full_output_path.c_str(), "w");
    if (pFile == NULL) {
        perror("Error opening file");
    }
    rapidjson::FileWriteStream os(pFile, writeBuffer, sizeof(writeBuffer));
    rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);
    d.Accept(writer);
    auto end = std::chrono::steady_clock::now();
    report_time_diff("PicoJson export pretty", begin, end);
    fclose(pFile);
}

void benchmark_rapidjson(const char *json_c_str) {
    print_divider("rapidjson");
    string output_path = "outputs/rapidjson/";
    rapidjson::Document d;
    parse_from_file_rapidjson(json_c_str, d);
    export_raw_rapidjson(d, output_path);
    export_pretty_rapidjson(d, output_path);
}

int main() {
    std::string json = get_file_contents("dummy_data.json");
    auto json_c_str = json.c_str();

    benchmark_picojson(json);
    benchmark_nlohmann(json);
    benchmark_rapidjson(json_c_str);

    return 0;
}
