#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <boost/filesystem.hpp>

std::vector<int> history;
std::vector<std::string> hist_s;
std::vector<std::string> local_var;
std::vector<std::string> global_var;
extern char **environ;


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
        if(d && eq){
        	local_var.push_back(el);
        }
    }
}

void find_var(const std::string var){
	for(auto el:local_var){
		std::vector<std::string> v;
		split(el, v, '=');
		if(v[0] == var){
			std::cout<<v[1]<<std::endl;
			history.push_back(0);
		}
	}
}


void mecho_func(std::vector<std::string> &command_v){
	bool d = false;
	for(auto el: command_v){
		for(auto i:el){
			if(i == '$'){
				find_var(el);
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


void run_prg(std::vector<std::string> &command_v){

	char *args[] = {0};
	char *env[command_v.size()+1];
	if(command_v.size()>1){
		for(int i=1;i<command_v.size();i++)
			env[i-1] = (char *) (command_v[i].c_str());
	}
	env[command_v.size()+1]=(char *)0;

	std::string prg_name = command_v[0];
	pid_t pid = fork();
	
	if(pid == -1){
		std::cerr << "Failed to run program" << std::endl;
		history.push_back(1);
	}
	else if (pid > 0){
		int status;
        waitpid(pid, &status, 0);
        std::cout << "Program stopped, exit code: " << status << std::endl;
	}

	else{
		execve(prg_name.c_str(), args, env);
	}
}

void mexport_func(const std::vector<std::string> &command_v){
	global_var.push_back(command_v[1]);
	history.push_back(0);
}

void mcd_func(const std::vector<std::string> &command_v){
    std::cout<<"PATH: "<<command_v[1]<<std::endl;
}

void mexit_func(const std::vector<std::string> &command_v){
    if(command_v.size()>2)
        exit(EXIT_FAILURE);
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
        boost::filesystem::path full_path(boost::filesystem::current_path());
        std::cout<<full_path.string()<<std::endl;
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
    if(command_v[0] == "mexit"){
        mexit_func(command_v);
    }
    if(command_v[0] == "mpwd"){
        mpwd_func(command_v);
    }
    if(command_v[0] == "merrno"){
        merrno_func(command_v);
    }
    if(command_v[0] == "mcd"){
        mcd_func(command_v);
    }
    if(command_v[0] == "mexport"){
    	mexport_func(command_v);
    }
    if(command_v[0] == "mecho"){
    	mecho_func(command_v);
    }
    else{
    	bool p = check_if_prog(command_v);
    	if(p){
    		run_prg(command_v);
    	}
    }
}

std::vector<std::string> parse_command_line(std::string &command){
    std::vector<std::string> command_v;
    split(command, command_v, ' ');
    return command_v;
}

void get_command(std::string &s){
    boost::filesystem::path full_path(boost::filesystem::current_path());
    std::cout << full_path.string() + " [myshell] $";
    getline(std::cin,s);
    hist_s.push_back(s);
}

int main() {
    std::string command;
    while(true){
        get_command(command);
        std::vector<std::string> command_v = parse_command_line(command);
        check_command(command_v);
        check_for_var(command_v);
    }
    return 0;
}