#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/resource.h>
#include <iostream>
#include <string.h>
#include <signal.h>
#include <libgen.h>
#include <fstream>
#include <vector>
#include <map>
#include "../file/file.h"

using namespace fileio;


void sig_quit(int signo){
    std::cout << "Caught signal " << signo << std::endl;
    exit(0);
}

void block_signal(int signo) {
    sigset_t newmask, oldmask;
    sigemptyset(&newmask);
    sigaddset(&newmask, signo);
    if(sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0){
        std::cout << "sigprocmask error!" << std::endl;
    }
}

void child_run() {
    std::cout << "Child process started!" << std::endl;
    struct rlimit climit;
    climit.rlim_cur = RLIM_INFINITY;
    climit.rlim_max = RLIM_INFINITY;
    if (setrlimit(RLIMIT_CORE, &climit) < 0) {
        std::cout << "setrlimit: " << strerror(errno);
    }

    /*while (true) {
        sleep(10);
    }*/

    // 4 test cases
    //exit(7); // -> normal termination, exitstatus = 7
    //abort(); // -> abnormal termination, signalstatus = 6 (SIGABRT)
    //int i = 1 / 0; // -> abnormal termination, signalstatus = 8 (SIGFPE)
    char *p = NULL; *p = 'a'; // -> abnormal termination, signalstatus = 11 (SIGSEGV)
}

void coredump_handler(const std::string & core_pattern, unsigned int reserve_num=3) {
    //make sure you have root privilege to modify the pattern_file content to the core file path.
    std::string pattern_file = "/proc/sys/kernel/core_pattern";
    char buffer[200];
    std::string dir = "";
    std::ifstream ifs(pattern_file);
    if(ifs.is_open()) {
        ifs.getline(buffer, 200);
    }
    dir = dirname(buffer);
    if (!fexists(dir)) {
        std::cout << "Core pattern file does not exist!" << std::endl;
        return;
    }
    std::vector<std::string> files;
    int ret = ls(dir, "core", files);
    if (ret != 0) {
        std::cout << "No core files found in " << dir;
        return;
    }
    std::vector<std::string> out_files;
    ret = grep(core_pattern, files, out_files);
    if (ret != 0) {
        std::cout << "No core files match the pattern :" << core_pattern << std::endl;
        return;
    }

    if (out_files.empty()) {
        std::cout << "No core files match the pattern :" << core_pattern << std::endl;
        return;
    }
    std::map<time_t, std::string, std::greater<time_t> > file_pairs;
    for(const auto & filename : out_files) {
        if (fexists(filename)) {
            time_t time = flastmodtime(filename);
            if (time == 0) {
                return;
            }
            file_pairs[time] = filename;
        }
    }

    std::vector<std::string> remove_vec;
    unsigned int index = 0;
    for(const auto & pair : file_pairs) {
        if (index < reserve_num) {
            index++;
            continue;
        }
        remove_vec.push_back(pair.second);
    }

    if (!remove_vec.empty()) {
        for(const auto & file : remove_vec) {
            if (fremove(file) == 0) {
                std::cout << "Core file : " << file << " removed!" << std::endl;
            } else {
                std::cout << "Core file : " << file << " remove failed!" << std::endl;
            }

        }
    }

}

int main(void)
{
    pid_t pid;
    int status;

    printf("before fork\n");
    fflush(stdout);

    int watch_time = 10;

    while (watch_time > 0) {
        pid = fork();
        if (pid < 0) {
            std::cout << "Fork error!" << std::endl;
            exit(-1);
        } else if (pid == 0){
            signal(SIGQUIT, sig_quit);
            child_run();
            break;
        } else {
            block_signal(SIGQUIT);
            wait(&status);
            if (WIFEXITED(status)) {
                std::cout << "Child terminated normally, exit status: " << WEXITSTATUS(status) << std::endl;
                exit(0);
            } else if (WIFSIGNALED(status)) {
                std::cout << "Child terminated abnormally, signal status: " << WTERMSIG(status) << std::endl;
                if (WCOREDUMP(status)) {
                    std::cout << "Child Core file dumped!" << std::endl;
                    coredump_handler("watchdog_demo");
                }
            }
            sleep(2);
            watch_time--;
        }
    }

    return 0;
}