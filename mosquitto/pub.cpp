#include <cstdlib>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<iostream>
#include<sstream>
#include<memory>
#include<unistd.h>  
#include<mosquitto.h>
#include<jsoncpp/json/json.h>
#define HOST "localhost"
#define PORT 1883
#define KEEP_ALIVE 60
#define QOS 1
#define USERNAME "jerry"
#define PASSWORD "123456"
#define TOPIC_NAME "mtopic"
#define CLIENT_ID "jerry123456"
static bool clean_session = false;

//生成随机数
static float get_random_float(float min,float max){
    float random = ((float)rand())/(float)RAND_MAX;
    return min + (random * (max - min));
}

void my_log_callback(struct mosquitto *mosq,void *userdata,int level,const char *str){
    printf("log_callback-> %s\n", str);
}

int main(){
    int ret=0;
    struct mosquitto *mosq=NULL;
    // 使用mosquitto库函数前，要先初始化，使用之后就要清除。清除函数；int mosquitto_lib_cleanup()
    mosquitto_lib_init();
    // 创建一个新的mosquitto客户端实例，新建客户端。
    // id ：用作客户端ID的字符串。如果为NULL，将生成一个随机客户端ID。如果id为NULL，clean_session必须为true。
    // clean_session：设置为true以指示代理在断开连接时清除所有消息和订阅，设置为false以指示其保留它们，
    //                客户端将永远不会在断开连接时丢弃自己的传出消息。调用mosquitto_connect或mosquitto_reconnect
    //                将导致重新发送消息。使mosquitto_reinitialise将客户端重置为其原始状态。如果id参数为NULL，则必须将其设置为true。
    //                简言之：就是断开后是否保留订阅信息true/false
    // obj： 用户指针，将作为参数传递给指定的任何回调，（回调参数）
    mosq=mosquitto_new(CLIENT_ID,clean_session,NULL);
    if(!mosq){
        printf("create client failed..\n");
        mosquitto_lib_cleanup();
        return 1;
    }
    //设置密码
    if(mosquitto_username_pw_set(mosq, USERNAME,PASSWORD)!=MOSQ_ERR_SUCCESS){
        mosquitto_lib_cleanup();
        return 1;
    }
    // 设置日志回调函数，需要时可使用
    // PUBLISH (d0, q0, r0, m0, 'topic_pose ', ... (33 bytes))
    // 可以自定义日志回调函数
    mosquitto_log_callback_set(mosq,my_log_callback);
    // 连接到MQTT代理/服务器（主题订阅要在连接服务器之后进行）
    // mosq： 有效的mosquitto实例，mosquitto_new（）返回的mosq.
    // host: 服务器ip地址
    // port：服务器的端口号
    // keepalive：保持连接的时间间隔， 单位秒。如果在这段时间内没有其他消息交换，则代理应该将PING消息发送到客户端的秒数。
    if(mosquitto_connect(mosq,HOST,PORT,KEEP_ALIVE)!=MOSQ_ERR_SUCCESS){
        fprintf(stderr, "Unable to connect.\n");
         mosquitto_lib_cleanup();
        return 1;
    }
     // 网络事件循环处理函数，通过创建新的线程不断调用mosquitto_loop() 函数处理网络事件，不阻塞
    int loop=mosquitto_loop_start(mosq);
    if(loop!=MOSQ_ERR_SUCCESS){
        printf("mosquitto loop error\n");
        mosquitto_lib_cleanup();
        return 1;
    }

    while(1){
        //发布消息
        /*发布消息  坐标位置、偏航信息 */
        Json::Value root;
        root["x"] = get_random_float(0, 90);
        root["y"] = get_random_float(0, 90);
        root["yaw"] = get_random_float(0, 30);
        Json::StreamWriterBuilder writerBuilder;
        std::ostringstream os;
        std::unique_ptr<Json::StreamWriter> json_writer(writerBuilder.newStreamWriter());
        json_writer->write(root, &os);
        std::string str_json = os.str();
        // 主题发布函数
        // mosq：有效的mosquitto实例，客户端
        // mid：指向int的指针。如果不为NULL，则函数会将其设置为该特定消息的消息ID。然后可以将其与发布回调一起使用，
        //      以确定何时发送消息。请注意，尽管MQTT协议不对QoS = 0的消息使用消息ID，但libmosquitto为其分配了消息ID，
        //      以便可以使用此参数对其进行跟踪。
        // topic：要发布的主题，以null结尾的字符串
        // payloadlen：有效负载的大小（字节），有效值在0到268，435，455之间；主题消息的内容长度
        // payload： 主题消息的内容，指向要发送的数据的指针，如果payloadlen >0，则它必须时有效的存储位置。
        // qos：整数值0、1、2指示要用于消息的服务质量。
        // retain：设置为true以保留消息, 可以修改该值做测试, 比如先pub然后停止，然后调用sub看看是否可以收到消息
        ret = mosquitto_publish(mosq, NULL, TOPIC_NAME, str_json.size() + 1, str_json.c_str(),
             QOS, 1);
        if(ret != MOSQ_ERR_SUCCESS) {
            printf("mosquitto_publish failed\n");
        } else {
            printf("pub %s\n", str_json.c_str());
        }
        sleep(1);
    }
    //释放客户端
    mosquitto_destroy(mosq);
    //使用完后要做清除工作
    mosquitto_lib_cleanup();
    return 0;
}