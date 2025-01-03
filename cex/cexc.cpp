#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <zlib.h>
#include <json/json.h>
#include <toml.hpp>

namespace fs = std::filesystem;

void embedBinaryIntoCEX(const std::string &sourcePath, const std::string &outputCEX, const Json::Value &metadata) {
    std::ifstream sourceFile(sourcePath, std::ios::binary);
    if (!sourceFile.is_open()) {
        throw std::runtime_error("Failed to open source file: " + sourcePath);
    }

    std::vector<char> sourceData((std::istreambuf_iterator<char>(sourceFile)), std::istreambuf_iterator<char>());
    sourceFile.close();

    std::ofstream cexFile(outputCEX, std::ios::binary);
    if (!cexFile.is_open()) {
        throw std::runtime_error("Failed to create .cex file: " + outputCEX);
    }

    uint64_t sourceSize = sourceData.size();
    cexFile.write(reinterpret_cast<const char *>(&sourceSize), sizeof(sourceSize));
    cexFile.write(sourceData.data(), sourceSize);

    std::string metadataString = metadata.toStyledString();
    uint64_t metadataSize = metadataString.size();
    cexFile.write(metadataString.c_str(), metadataSize);
    cexFile.write(reinterpret_cast<const char *>(&metadataSize), sizeof(metadataSize));

    cexFile.close();
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: ./cexc <folder_name> <output.cex>\n";
        return 1;
    }

    try {
        std::string folderPath = argv[1];
        std::string outputCEX = argv[2];

        if (!fs::is_directory(folderPath)) {
            throw std::runtime_error("The specified folder does not exist: " + folderPath);
        }

        std::string configPath = folderPath + "/config.toml";
        if (!fs::exists(configPath)) {
            throw std::runtime_error("Missing config.toml in the folder: " + folderPath);
        }

        auto config = toml::parse(configPath);
        std::string entryPoint = toml::find<std::string>(config, "general", "entry_point");

        std::string sourcePath = folderPath + "/" + entryPoint;
        std::string fileExtension = sourcePath.substr(sourcePath.find_last_of('.') + 1);

        if (fileExtension != "py" && fileExtension != "rb" && fileExtension != "lisp" &&
            fileExtension != "cpp" && fileExtension != "c" && fileExtension != "f90" && fileExtension != "f95") {
            throw std::runtime_error("Unsupported source file type: " + sourcePath);
        }

        Json::Value metadata;
        metadata["entry_point"] = entryPoint;

        if (fileExtension == "py" || fileExtension == "rb" || fileExtension == "lisp") {
            embedBinaryIntoCEX(sourcePath, outputCEX, metadata);
        } else {
            std::string binaryPath = "embedded_binary";

            std::string compiler = (fileExtension == "cpp") ? "g++" : (fileExtension == "c") ? "gcc" : "gfortran";
            int compileResult = system((compiler + " \"" + sourcePath + "\" -o \"" + binaryPath + "\"").c_str());
            if (compileResult != 0) {
                throw std::runtime_error("Failed to compile source file: " + sourcePath);
            }

            embedBinaryIntoCEX(binaryPath, outputCEX, metadata);
            fs::remove(binaryPath);
        }

        std::cout << "Successfully created .cex file: " << outputCEX << "\n";
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
