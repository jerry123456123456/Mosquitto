#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mosquitto.h>
// broker： mosquitto -v
// 终端订阅：mosquitto_sub -t topic/on/unexpected/disconnect
// 然后执行此程序，过几秒钟再直接CTRL +C退出，订阅端将收到will message

static int run=-1;

int main(int argc,char *argv[]){ 
    int rc;
    struct mosquitto *mosq;

    int port=1883;
    if(argc>=2){
        port=atoi(argv[1]);
    }
    mosquitto_lib_init();

    //这里和client是否为NULL没有关系
    mosq=mosquitto_new("01-will-set",true,NULL);
    if(mosq==NULL){
        return 1;
    }
    mosquitto_will_set(mosq, "topic/on/unexpected/disconnect", strlen("will message"), 
		"will message", 1, true);
    rc=mosquitto_connect(mosq,"localhost",port,60);

    while(run == -1){
        mosquitto_loop(mosq,-1,1);

        mosquitto_destroy(mosq);
        mosquitto_lib_cleanup();
    }
    return 0;
}