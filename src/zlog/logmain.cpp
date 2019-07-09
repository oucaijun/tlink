#include "logmain.h"
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <fstream>

LogMain ZL("10M", "10");

LogMain::LogMain(std::string LOGSIZEMAX="10M",
                 std::string LOGCNTMAX="10"):
    m_LOGSIZEMAX(LOGSIZEMAX),
    m_LOGCNTMAX(LOGCNTMAX)
{
    std::string proc_name;
    std::string proc_dname;
    get_executable_path(proc_dname, proc_name);
    if(proc_name.size ()<1) {
        proc_name="notdef";
    }
    if(proc_dname.size ()<1) {
        proc_dname="/notdef/";
    }

    m_conf_filename = proc_dname + proc_name + ".conf";
    m_log_dname = proc_dname + "logs/";
    m_log_filename = m_log_dname + proc_name; //实际的log文件名字是 m_log_filename.log

    InitZlogObj();
}

std::string LogMain::MakeZconfString(void)
{
    //simple = "%d(%Y%m%d-%T) [%p %f:%L:%U] %V %m%n"
    std::string zlogconf = "\
[formats] \n\
simple = \"%d(%Y%m%d-%T) [%p %f:%L:%U] %V %m%n\" \n\
[rules] \n\
cat.NOTICE  \"LOGFILENAME_notice.log\", LOGSIZEMAX*LOGCNTMAX ~  \"LOGFILENAME_notice.%d(%Y%m%d).#2s.log\" \; simple\n\
cat.DEBUG  \"LOGFILENAME_debug.log\", LOGSIZEMAX*LOGCNTMAX ~  \"LOGFILENAME_debug.%d(%Y%m%d).#2s.log\" \; simple\n\
";

    std::size_t pos = zlogconf.find("LOGFILENAME");
    while (pos!=zlogconf.npos)
    {
        zlogconf.replace(pos, strlen("LOGFILENAME"), m_log_filename);
        pos = zlogconf.find("LOGFILENAME", pos+1);
    }

    pos = zlogconf.find("LOGSIZEMAX");
    while (pos!=zlogconf.npos)
    {
        zlogconf.replace(pos, strlen("LOGSIZEMAX"), m_LOGSIZEMAX);
        pos = zlogconf.find("LOGSIZEMAX", pos+1);
    }

    pos = zlogconf.find("LOGCNTMAX");
    while (pos!=zlogconf.npos)
    {
        zlogconf.replace(pos, strlen("LOGCNTMAX"), m_LOGCNTMAX);
        pos = zlogconf.find("LOGCNTMAX", pos+1);
    }
    std::cout<< zlogconf <<std::endl;

    return zlogconf;
}

int LogMain::CheckConfFile(void)
{
    std::string cmd = "mkdir -p " + m_log_dname;
    system(cmd.c_str ());

    bool exist = true;
    std::ifstream fin(m_conf_filename.c_str());
    if (!fin)
    {
        exist = false;
    }

    if(!exist) {
        std::cout << "文件"<<m_conf_filename<<"不存在，即将自动生成\n";
        std::fstream s(m_conf_filename, s.binary | s.trunc | s.out );
        if (s.is_open())
        {
            std::string zlogconf = MakeZconfString();
            s << zlogconf;
        }
        s.close();
    }
    return 0;
}

int LogMain::InitZlogObj()
{
    int rc;
    CheckConfFile();

    rc = zlog_init(m_conf_filename.c_str());
    if (rc) {
            printf("init failed\n");
            return -1;
    }

    zcat = zlog_get_category("cat");
    if (!zcat) {
            printf("get cat fail\n");
            zlog_fini();
            return -2;
    }

    //zlog_notice(zcat, "hello mycat");
    //zlog_fini();
    return 0;
}


#include <string.h>
#include <string>
void get_executable_path(std::string& processd, std::string &processn)
{
    char processdir[1024];
    char processname[1024];
    memset(processdir, 0, 1024);
    memset(processname, 0, 1024);

    char* path_end;
    if(readlink("/proc/self/exe", processdir,1024) <=0)
        return ;
    path_end = strrchr(processdir,  '/');
    if(path_end == NULL)
        return ;
    ++path_end;
    strcpy(processname, path_end);

    processd = processdir;
    processd = processd.substr (0, path_end-processdir);
    processn = processname;
    return ;
}

