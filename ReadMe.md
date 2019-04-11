```
tlink
"usage: ./tlink-a 192.168.15.25 -p 6305 -i eth0 -t 1"
首先尝试获取本地ip，如果没有ip那么说明没有联网的条件，返回erro。
然后尝试连接指定的服务器端口，连接失败或者超时（3秒）则返回erro。

typedef enum {
    IF_CONNECT_OK,
    IF_PARA_ERROR,
    IF_NOT_FIND,
    IF_IP_NOT_FIND,
    IF_CONNECT_FAIL,
    IF_CONNECT_TIMEOUT,
}iferr_t;

```