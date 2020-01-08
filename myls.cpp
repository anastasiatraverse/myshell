#include <iostream>
#include <vector>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>
#include <filesystem>
#include <algorithm>
#include <fstream>
#include <time.h>
#include <typeinfo>

namespace fs = std::filesystem;
namespace opt = boost::program_options;

std::string path;
std::string sort;
std::vector<std::string> fvec;
bool r = false;
bool F = false;
bool l = false;
bool R = false;

void parse_opt(int argc, char *argv[]){
    opt::options_description desc("General options");
    desc.add_options()
        ("help,h","Display help")
        ("recursive,R","recursive directory traversal")
        ("large,l","display detailed information about files")
        ("file-type,F","displays file type")
        ("reverse,r","displays files in reverse order")
        ("sort,S",opt::value<std::string>(),"sets the arguments to the sort function");


    opt::options_description hidden("Hidden options");
    hidden.add_options()
            ("files", opt::value<std::vector<std::string>>()->default_value(std::vector<std::string>{"."}, "."), "path to directory or files");
    
    opt::options_description options;
    options.add(desc).add(hidden);

    opt::positional_options_description positional;
    positional.add("files", -1);
    opt::variables_map vm;

    auto parsed = opt::command_line_parser(argc, argv).options(options).positional(positional).run();
    opt::store(parsed, vm);
    //must be called after parsing and storing command line
    opt::notify(vm);
    

    if (vm.count("help")){
        std::cout<<desc<<std::endl;
        exit(EXIT_SUCCESS);
    }
    if (vm.count("recursive")){
        r = true;
    }
    if(vm.count("large")){
        l = true;
    }
    if(vm.count("recursive")){
        R = true;
    }
    if(vm.count("file-type")) {
        F = true;
    }
    if(vm.count("sort")){
        sort = vm["sort"].as<std::string>();
        if(sort.length()>2){
            std::cerr<<"Error: Wrong number of arguments"<<std::endl;
            exit(EXIT_FAILURE);
        }
    }
    fvec = vm["files"].as<std::vector<std::string>>();
}

// COMPARE FUNCTION
bool compareFunctionName (std::string &a, std::string &b) {return a<b;}

bool compareFunctionSize (std::string &a, std::string &b) {
    if (!boost::filesystem::is_directory(a) && !boost::filesystem::is_directory(b))
        return boost::filesystem::file_size(a)<boost::filesystem::file_size(b);
    return false;
}

bool compareFunctionExt(std::string &a, std::string &b){
    boost::filesystem::path a_path(a);
    boost::filesystem::path b_path(b);
    if(a_path.has_extension() || b_path.has_extension()){
        return a_path.extension().string() < b_path.extension().string();
    }
    return false;
}

bool compareFunctionTime(std::string &a, std::string &b){
    boost::filesystem::path a_path(a);
    boost::filesystem::path b_path(b);
    if (!(boost::filesystem::is_directory(a_path)) || !(boost::filesystem::is_directory(b_path))){
        std::time_t result_a = boost::filesystem::last_write_time(a_path);
        std::time_t result_b = boost::filesystem::last_write_time(b_path);
        return difftime(result_a,result_b) > 0.0;
    }
    return false;
}

void check_sort(std::vector<std::string> &files){
    std::vector<char> v1 = {'U', 'S', 't', 'X', 'N'};
    std::vector<char> v2 = {'D', 's'};

    if(sort.length()>0) {
        for (auto &v:sort) {
            if (v == 'U') {
                break;
            }
            else if (v == 'S') {
                std::sort(files.begin(),files.end(),compareFunctionSize);
            }
            else if (v == 't') {
                std::sort(files.begin(),files.end(),compareFunctionTime);
            }
            else if (v == 'X') {
                std::sort(files.begin(),files.end(),compareFunctionExt);
            }
            else if (v == 'N') {
                std::sort(files.begin(),files.end(),compareFunctionName);
            }

            else if(v == 's'){
                std::sort(files.begin(),files.end(),compareFunctionExt);
            }
            else{
                std::cerr<<"Error: wrong arguments for sort function"<<std::endl;
            }
        }
    }
    else{
        std::sort(files.begin(),files.end(),compareFunctionName);
    }
}

void find_all_files(const std::string &p, std::vector<boost::filesystem::path> &files){
    for(auto& s : boost::make_iterator_range(boost::filesystem::directory_iterator(p), {})){
        boost::filesystem::path f_path(s);
        auto f_name = f_path.filename();
        files.push_back(f_name);
    }
}

void find_all_dir(std::string inputPath, std::vector<boost::filesystem::path> &files){
    for (boost::filesystem::directory_entry& entry : boost::filesystem::directory_iterator(inputPath)){
        if(boost::filesystem::is_directory(entry.path())){
            files.push_back(entry.path());
            find_all_dir(entry.path().string(), files);
        }else{
            files.push_back(entry.path());
        }
    }
}

void collect_file(std::vector<boost::filesystem::path> &files){
    std::string p;
    if (path.length()>0){
        p = path;
    }else{
        p = ".";
    }
    if(R){
        find_all_dir(p, files);
    }else{
        find_all_files(p, files);
    }
}

std::vector<std::string> update_res(std::vector<boost::filesystem::path> files){
    std::vector<std::string> res;
    for(auto & el:files){
        if(boost::filesystem::is_directory(el)){
            res.push_back("./"+el.string());
            files.erase(std::remove(files.begin(), files.end(), el), files.end());
        }
    }
    if(F){
        for(auto &el:files){
            if(boost::filesystem::is_symlink(el)){
                res.push_back("@"+el.filename().string());
            }else if(boost::filesystem::extension(el)==".exe" || boost::filesystem::extension(el)==".out" || boost::filesystem::extension(el)==".o"
            || !el.has_extension()){
                res.push_back("*"+el.filename().string());
            }else if(el.parent_path().filename() == "pipe"){
                res.push_back("|"+el.filename().string());
            }
            else{
                res.push_back("?"+el.filename().string());
            }
        }
    }
    else{
        for(auto &el:files)
            res.push_back(el.filename().string());
    }
    return res;
}

void print_long_info(std::string el){
    boost::filesystem::path path(el);
    std::time_t result = boost::filesystem::last_write_time(path);
    std::string res = std::asctime(std::localtime(&result));
    if (boost::filesystem::is_regular_file(path)) {
        std::cout << el << "   " << boost::filesystem::file_size(path) << "   " << res << std::endl;
    } else {
        std::cout << el << "   " << 0 << "  " << res << std::endl;
    }
}

int main(int argc, char *argv[]) {
    std::vector<boost::filesystem::path> files;
    if(argc-1>0)
        parse_opt(argc, argv);
    if (fvec.size()>0){
        for (auto &el:fvec){
            
            boost::filesystem::path path(el);
            if(!boost::filesystem::is_directory(el)){
                print_long_info(el);
            }else{
                collect_file(files);
            }
        }
    }else{
        collect_file(files);
    }
    std::vector<std::string> f = update_res(files);
    check_sort(f);
    if(r){
        std::reverse(f.begin(),f.end());
    }
    if(l) {
        for (auto &el:f) {
            print_long_info(el);
        }
    }else{
        for(auto &el:f){
            std::cout<<el<<" ";
        }
    }

    return 0;
}