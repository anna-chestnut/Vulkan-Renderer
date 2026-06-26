#include "vr_pipeline.hpp"

//std
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace vr
{
    VrPipeline::VrPipeline(const std::string& vertFilePath, const std::string& fragFilePath){
        createGraphicsPipeline(vertFilePath, fragFilePath);
    }

    std::vector<char> VrPipeline::readFile(const std::string& filePath){

        //Open the file in binary mode and teleport the cursor to the end.
        std::ifstream file{filePath, std::ios::ate | std::ios::binary};

        if(!file.is_open()){
            throw std::runtime_error("failed to open file: " + filePath);
        }

        size_t fileSize = static_cast<size_t>(file.tellg());
        // Allocate a memory buffer exactly matching that size.
        std::vector<char> buffer(fileSize);

        //Teleport the cursor back to the beginning so we are ready to read.
        file.seekg(0);
        //Read the actual data from byte 0 all the way to the end, filling up our perfectly sized buffer.
        file.read(buffer.data(), fileSize);

        file.close();
        return buffer;
    }

        void VrPipeline::createGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath){

            auto vertCode = readFile(vertFilePath);
            auto fragCode = readFile(fragFilePath);

            std::cout << "Vertex Shader Code Size: " << vertCode.size() << '\n';
            std::cout << "Fragment Shader Code Size: " << fragCode.size() << '\n';
        }

} // namespace vr
