#ifndef JSON_H
#define JSON_H

#include "jsonlist.h"
#include <fstream>

class JSON
{
private:
    std::string _json_file_path;
    std::ifstream _json_file;

public:
    JSON();
    JSON(std::string json_file_name);
    ~JSON();
    void open(std::string json_file_name = "");
    void close();
    JSONList read();
};

#endif // JSON_H
