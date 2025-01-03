#include <iostream>
#include <fstream>
#include <vector>
#include <zlib.h>
#include <json/json.h>
#include <filesystem>
#include <cstdlib> 

namespace fs = std::filesystem;

std::string decompressToMemory(std::ifstream &input, uint64_t fileSize) {
    std::vector<char> compressedBuffer(fileSize);
    input.read(compressedBuffer.data(), fileSize);

    uLongf decompressedSize = fileSize * 4;
    std::vector<char> decompressedBuffer(decompressedSize);

    if (uncompress(reinterpret_cast<Bytef *>(decompressedBuffer.data()), &decompressedSize,
                   reinterpret_cast<const Bytef *>(compressedBuffer.data()), fileSize) != Z_OK) {
        throw std::runtime_error("Decompression failed");
    }

    return std::string(decompressedBuffer.data(), decompressedSize);
}


void executeEmbeddedCode(const std::string &sourceCode, const std::string &fileType) {
    if (fileType == "py") {

        std::string tempFile = "temp_script.py";
        std::ofstream scriptFile(tempFile);
        if (!scriptFile.is_open()) {
            throw std::runtime_error("Failed to create temporary Python script.");
        }
        scriptFile << sourceCode;
        scriptFile.close();

        int result = system(("python3 " + tempFile).c_str());
        fs::remove(tempFile);

        if (result != 0) {
            throw std::runtime_error("Execution failed for Python script.");
        }
    } else if (fileType == "rb") {

        std::string tempFile = "temp_script.rb";
        std::ofstream scriptFile(tempFile);
        if (!scriptFile.is_open()) {
            throw std::runtime_error("Failed to create temporary Ruby script.");
        }
        scriptFile << sourceCode;
        scriptFile.close();

        int result = system(("ruby " + tempFile).c_str());
        fs::remove(tempFile);

        if (result != 0) {
            throw std::runtime_error("Execution failed for Ruby script.");
        }
    } else if (fileType == "lisp") {

        std::string tempFile = "temp_script.lisp";
        std::ofstream scriptFile(tempFile);
        if (!scriptFile.is_open()) {
            throw std::runtime_error("Failed to create temporary Lisp script.");
        }
        scriptFile << sourceCode;
        scriptFile.close();

        int result = system(("sbcl --script " + tempFile).c_str());
        fs::remove(tempFile);

        if (result != 0) {
            throw std::runtime_error("Execution failed for Lisp script.");
        }
    } else {
        throw std::runtime_error("Unsupported file type for execution: " + fileType);
    }
}

void runCEX(const std::string &cexPath) {
    std::ifstream cexFile(cexPath, std::ios::binary);
    if (!cexFile.is_open()) {
        throw std::runtime_error("Failed to open .cex file: " + cexPath);
    }


    uint64_t sourceSize;
    cexFile.read(reinterpret_cast<char *>(&sourceSize), sizeof(sourceSize));

    std::vector<char> sourceData(sourceSize);
    cexFile.read(sourceData.data(), sourceSize);

    uint64_t metadataSize;
    cexFile.seekg(-static_cast<int>(sizeof(metadataSize)), std::ios::end);
    cexFile.read(reinterpret_cast<char *>(&metadataSize), sizeof(metadataSize));


    cexFile.seekg(-static_cast<int>(sizeof(metadataSize) + metadataSize), std::ios::end);
    std::vector<char> metadataBuffer(metadataSize);
    cexFile.read(metadataBuffer.data(), metadataSize);

    Json::Value metadata;
    Json::CharReaderBuilder reader;
    std::string errs;
    std::istringstream metadataStream(std::string(metadataBuffer.begin(), metadataBuffer.end()));
    if (!Json::parseFromStream(reader, metadataStream, &metadata, &errs)) {
        throw std::runtime_error("Failed to parse metadata");
    }


    std::string entryPoint = metadata["entry_point"].asString();
    std::string fileType = entryPoint.substr(entryPoint.find_last_of('.') + 1);

    executeEmbeddedCode(std::string(sourceData.begin(), sourceData.end()), fileType);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: ./cexr <file.cex>\n";
        return 1;
    }

    try {
        runCEX(argv[1]);
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
