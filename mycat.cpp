#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <functional>
#include <boost/program_options.hpp>
namespace po = boost::program_options;

std::pair<bool, std::vector<std::string>> read_option(int argc, const char *argv[]){
    std::pair<bool, std::vector<std::string>> res;
    std::vector<std::string> fnames;
    bool spec_sym = false;
    po::options_description desc("General options");
    std::string task_type;
    desc.add_options()
            ("help,h", "Show help")
            ("special-symbols,A", "Some special symbol");

    po::options_description hidden("Hidden options");
    hidden.add_options()
            ("filename", po::value<std::vector<std::string>>()->default_value(std::vector<std::string>{" "}, " "), "enter filenames");

    po::options_description options;
    options.add(desc).add(hidden);
    po::positional_options_description positional;
    positional.add("filename", -1);
    po::variables_map vm;

    po::store(po::command_line_parser(argc, argv).options(options).positional(positional).run(), vm);
    po::notify(vm);
    if(vm.count("help")){
        std::cout<<desc<<std::endl;
        exit(EXIT_FAILURE);
    }
    if (vm.count("special-symbols")){
        spec_sym = true;
    }
    if (vm.count("filename")){
        fnames = vm["filename"].as<std::vector<std::string>>();
    }
    return std::make_pair(spec_sym, fnames);
}

void write_from_buffer(const char msg[], ssize_t msg_size){
    ssize_t bytes_write = 0;
    while (bytes_write< msg_size) {
        ssize_t w = write(STDOUT_FILENO, msg + bytes_write, msg_size - bytes_write);
        if(w == -1){
            if (errno != EINTR){
                exit(EXIT_FAILURE);
            }
        }
        else{
            bytes_write += w;
        }
    }
}
std::string decToHexa(int n)
{
    std::string hexaDeciNum("    ");
    int i = 0;
    while(n!=0)
    {
        int temp  = 0;
        temp = n % 16;
        // check if temp < 10
        if(temp < 10){
            hexaDeciNum[i] = temp + 48;
            i++;
        } else {
            hexaDeciNum[i] = temp + 55;
            i++;
        }
        n = n/16;
    }
    hexaDeciNum[2] = 'x';
    hexaDeciNum[3] = '\\';
    return hexaDeciNum;
}


void open_file(const std::vector<std::string> filenames, std::vector<int> &fildes){
    for(auto &filename:filenames){
        ssize_t file = open(filename.c_str(), O_RDONLY);
        if (file == -1) {
            if (errno != EINTR) {
                exit(EXIT_FAILURE);
            }
        }else{
            fildes.emplace_back(file);
        }
    }
}

int in_buffer(char *buffer, const size_t &byte_r, const std::string &msg, const int &ind_i) {
    try {
        if (ind_i < byte_r){
            for (auto i = byte_r - 1; i > ind_i; i--)
                buffer[i + msg.size() - 1] = buffer[i];
        }
        for (size_t i = 0; i <= msg.size() - 1; i++){
            buffer[ind_i + i] = msg[msg.size() - 1 - i];
        }
    }catch (int err){
        std::cerr<<err<<std::endl;
    }
    return 0;
}

void spec_sym_func(char *buffer, ssize_t &byte_r, size_t &buffer_size){
    for(int i = 0 ; i<buffer_size;){
        if (!(isspace(buffer[i]) || isprint(buffer[i]))) {
            if(in_buffer(buffer, byte_r, decToHexa(buffer[i]), i) == 0){
                i += 4;
                byte_r += 3;
            }
        }
        else i++;
    }
}


void read_file_to_buffer(const std::vector<int> &fildes, const bool &spec_sym) {
    constexpr size_t buff_cap=444;
    char buffer[buff_cap];

    size_t buffer_size = sizeof(buffer);
    for(auto &f:fildes){
        ssize_t byte_r = 1;
        while(byte_r>0){
            byte_r = read(f, buffer, buffer_size);
            if(byte_r == -1){
                if (errno != EINTR){
                    exit(EXIT_FAILURE);
                }
            }else{
                if(spec_sym){spec_sym_func(buffer, byte_r, buffer_size);}
                write_from_buffer(buffer, byte_r);
            }
        }
    }
}

void close_file(std::vector<int> &fildes){
    for(auto &f:fildes){
        auto cl = close(f);
        if (cl==-1){
            if(errno != EINTR){
                break;
            }
        }
    }
}

int main(int argc, const char *argv[]) {
    std::vector<int> fildes;
    if(argc-1>0){
        std::pair<bool, std::vector<std::string>> options = read_option(argc, argv);
        bool spec_sym = options.first;
        std::vector<std::string> filenames = options.second;
        if(filenames[0]==" "){
            return 0;
        }
        open_file(filenames, fildes);
        read_file_to_buffer(fildes, spec_sym);
        close_file(fildes);
    }else{
        std::cerr<<"ERORR:not enought arguments"<<std::endl;
        return 1;
    }
    return 0;
}