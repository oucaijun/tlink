#ifndef LOGMAIN_H
#define LOGMAIN_H

#include "zlog.h"
#include <string>
#include <unistd.h>

void get_executable_path(std::string& processd, std::string &processn);

class LogMain
{
public:
    LogMain(std::string LOGSIZEMAX,
             std::string LOGCNTMAX);

    zlog_category_t *ZC(void)
    {
        return zcat;
    }

private:
    std::string m_LOGSIZEMAX;
    std::string m_LOGCNTMAX;
    std::string m_conf_filename;
    std::string m_log_filename; //实际的log文件名字是 m_log_filename.log
    std::string m_log_dname;

    zlog_category_t* zcat;
    std::string MakeZconfString(void);
    int CheckConfFile(void);
    int InitZlogObj();
};

extern LogMain ZL;
#define zError(format, args...) \
    zlog(ZL.ZC(), __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
    ZLOG_LEVEL_ERROR, format, ##args)
#define zNotice(format, args...) \
    zlog(ZL.ZC(), __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
    ZLOG_LEVEL_NOTICE, format, ##args)
#define zInfo(format, args...) \
    zlog(ZL.ZC(), __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
    ZLOG_LEVEL_INFO, format, ##args)
#define zDebug(format, args...) \
    zlog(ZL.ZC(), __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
    ZLOG_LEVEL_DEBUG, format, ##args)
#endif // LOGMAIN_H
