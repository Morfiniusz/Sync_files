//
// Created by bartl on 25.02.2023.
//

#include "FileCheck.h"

std::string FileCheck::getMD5(const std::filesystem::path& filePath)
{
    unsigned char result[MD5_DIGEST_LENGTH]; // 16 bytes = 128 bits. Length of the control sum
    MD5_CTX mdContext;  //Is a structure that is used for computing and storing MD5 message digest.
    std::ifstream fileStream(filePath, std::ios::binary); 
//    std::cout << "Debug1"<< std::endl;
    
    if (fileStream) {
        MD5_Init(&mdContext); //Initialize the MD5_CTX structure. Hashing starts with this function.
        char buffer[1024];
//        std::cout << "Debug2"<< std::endl;


        // Read file in chunks of 1024 bytes and update the control sum
        while (size_t s = fileStream.read(buffer, sizeof (buffer)).gcount()) {
            MD5_Update(&mdContext, buffer, s);
        }
//        std::cout << "Debug3"<< std::endl;

        // unsigned char digest[MD5_DIGEST_LENGTH];
        MD5_Final(result, &mdContext); 
        fileStream.close();

    } else {
        std::cerr << "unable to open: "<< filePath << std::endl;
        return 0;
    }
    char hexResult[2*MD5_DIGEST_LENGTH+1];
    int i;
    for (i=0; i<MD5_DIGEST_LENGTH; i++) {
        sprintf(&hexResult[2*i], "%02x", result[i]);
    }
    
    return std::string(hexResult);
}
