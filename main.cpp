#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <boost/filesystem.hpp>

std::vector<int> history;
std::vector<std::string> hist_s;
std::vector<std::string> local_var;
std::vector<std::string> global_var;
extern char **environ;
boost::filesystem::path mainpath;

void stdout_to_file(std::vector<std::string> command_v){
    std::cout<<"PRIVET VANYA"<<std::endl;
    std::string file_name=command_v[2];
}

void stderr_to_file(std::vector<std::string> command_v){
    std::string file_name;
}

std::string command_comments(std::string command_v){
   std::string out = "";
   bool comment = false;
   for(auto el:command_v){
        if(el=='#')comment=true;
        if (!comment)
        {
            /* code */
            out+=el;
        }
   }
   return out;
}

void split(std::string str, std::vector<std::string> &v, char split_char){
    std::string word = "";
    for (auto x : str){
        if (x == split_char){
            v.push_back(word);
            word = "";
        }else
            word = word + x;
    }
    v.push_back(word);
}

void check_for_var(std::vector<std::string> &command_v){
	bool d = false;
	bool eq = false;
    for(auto el:command_v){
        for (auto x : el){
            if(x == '=')
                eq = true;
            if(x == '$')
            	d = true;
        }
        if(d || eq){
            local_var.push_back(el);
        }
    }
}

void find_var(const std::string var){
	for(auto el:global_var){
        std::vector<std::string> temp_var;
        split(var, temp_var, '$');
        std::string upd_var = temp_var[1];
		std::vector<std::string> v;
		split(el, v, '=');
		if(v[0] == upd_var){
			std::cout<<v[1]<<std::endl;
			history.push_back(0);
		}
	}
}


void mecho_func(std::vector<std::string> &command_v){
	bool d = false;
	for(auto el: command_v){
		for(auto i:el){
			if(i=='$'){
                d=true;
				find_var(el);
                break;
			}
		}
	}
	if(!d){
		for(int i = 1; i<command_v.size(); i++)
			std::cout<<command_v[i]<< " ";
		std::cout<<"\n";
		history.push_back(0);
	}
}

bool check_if_prog(std::vector<std::string> &command_v){
	for(auto el:command_v){
		for(int i=0;i<el.length()-1;i++){
			if(el[i]=='.' && el[i+1]=='/'){
				return true;
			}
		}
	}
	return false;
}


int run_prg2(const std::vector<std::string> args){
    pid_t parent = getpid();
    pid_t pid = fork();

    
    std::string prg_name = args[0];

    if (pid == -1)
    {
        std::cerr << "Failed to fork()" << std::endl;
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        // We are parent process
        int status;
        waitpid(pid, &status, 0);
        std::cout << "Parent: child stopped, exit code: " << status << std::endl;
    }
    else
    {
        auto path_ptr = getenv("PATH");
        std::string path_var;
        if(path_ptr != nullptr)
            path_var = path_ptr;
        path_var += ":.";
        setenv("PATH", path_var.c_str(), 1);
        //! Environment is ready
        //! Prepare args array in form suitable for execvp
        std::vector<const char*> arg_for_c;
        for(int i=0;i<args.size();i++){
            arg_for_c.push_back(args[i].c_str());
        }
        for(auto s:arg_for_c)
            std::cout<<s<<std::endl;


        arg_for_c.push_back(nullptr);

        execvp(prg_name.c_str(), const_cast<char* const*>(arg_for_c.data()));
        std::cerr << "Parent: Failed to execute \n\tCode: " << errno << std::endl;
        exit(EXIT_FAILURE);   
    }
    return 0;
}


void mexport_func(const std::vector<std::string> &command_v){
    bool eq = false;
    bool d = false;

    for(int i = 0; i<command_v[1].size();i++){
        if(command_v[1][i]=='=')
            eq=true;
    }   
    if(eq){
        global_var.push_back(command_v[1]);
    }
    if(!eq){
        std::string var = command_v[1];
        for(auto el:local_var){
            std::vector<std::string> temp;
            split(el, temp, '=');
            if(temp[0]==var){
                global_var.push_back(el);
            }
        }
        std::cout<<"Myshell: Variable is exported"<<std::endl;
    }
	history.push_back(0);
}

bool check_dir(const std::string obj){
    boost::filesystem::path fpath(obj);
    if (boost::filesystem::exists(fpath) && boost::filesystem::is_directory(fpath))
        return true;
    return false;
}

void mcd_func(const std::vector<std::string> &command_v){
    if(check_dir(command_v[1])){
        mainpath = mainpath.string()+'/'+command_v[1];
        history.push_back(0);
    }
    else{
        history.push_back(-1);
        std::cerr<<"ERORR: directory not found"<<std::endl;
    }
}

void mexit_func(const std::vector<std::string> &command_v){
    if(command_v.size()>2){
        history.push_back(-1);
        exit(EXIT_FAILURE);}
    else if(command_v.size()>1){
        int ex = stoi(command_v[1]);
        exit(ex);
    }
    else
        exit(EXIT_SUCCESS);
}

void mpwd_func(const std::vector<std::string> &command_v){
    if(command_v.size()==1){
        history.push_back(0);
        std::cout<<mainpath.string()<<std::endl;
    }else{
        history.push_back(1);
        std::cerr<<"ERORR: not correct number of arguments"<<std::endl;
    }
}

void merrno_func(const std::vector<std::string> &command_v){
    if(command_v.size()>1)
        std::cerr<<"ERORR: not correct number of arguments"<<std::endl;
    else{
        if(history.size()>=1)
            std::cout<<history[history.size()-1]<<std::endl;
        else
            std::cout<<0<<std::endl;
    }
}

void check_command(std::vector<std::string> &command_v){
    // stdout_to_file(command_v);
    if(command_v[0] == "mexit")mexit_func(command_v);
    if(command_v[0] == "mpwd")mpwd_func(command_v);
    if(command_v[0] == "merrno")merrno_func(command_v);
    if(command_v[0] == "mcd")mcd_func(command_v);
    if(command_v[0] == "mexport")mexport_func(command_v);
    if(command_v[0] == "mecho")mecho_func(command_v);
    if(command_v[0] == "myls")run_prg2(command_v);
    if(command_v[0] == "mycat")run_prg2(command_v);
    if(command_v[0] == "mycp")
        std::cout<<"should be run mycp programm"<<std::endl;
    if(command_v[0] == "mymv")
        std::cout<<"should be run mymv programm"<<std::endl;
    if(command_v[0] == "myrm") run_prg2(command_v);
    if(command_v[0] == "mymkdir")run_prg2(command_v);
    if(command_v[0] == "mygrep")
        std::cout<<"should be run mygrep programm"<<std::endl;
    
    bool p = check_if_prog(command_v);
    if(p){
    	run_prg2(command_v);
    }
}

std::vector<std::string> parse_command_line(std::string &command){
    std::vector<std::string> command_v;
    split(command, command_v, ' ');
    return command_v;
}

void get_command(std::string &s){
    std::cout << mainpath.string() + " [myshell] $";
    getline(std::cin,s);
    hist_s.push_back(s);
}

int main() {
    std::string command;
    boost::filesystem::path full_path(boost::filesystem::current_path());
    mainpath = full_path;
    while(true){
        get_command(command);
        std::vector<std::string> command_v = parse_command_line(command);
        check_command(command_v);
        check_for_var(command_v);
    }
    return 0;
}