#include <stdio.h>
#include <stdlib.h>
#include <mosquitto.h>
#include <string.h>
#include "cjson/cJSON.h"  // 这里保留cjson，目的是让大家既学会jsoncpp也学会cjson
#include <unistd.h>
#define HOST "localhost"
#define PORT  1883
#define KEEP_ALIVE 60
#define MSG_MAX_SIZE  512
#define TOPIC_NUM 3

#define QOS   1
#define CLIENT_ID "king456123"
#define USERNAME "0voice_mqtt"
#define PASSWORD "1234567"
#define TOPIC_NAME  "mtopic" //"mtopic"


// 订阅端的问题
static bool clean_session = false;

void print_mqtt_message(const struct mosquitto_message *message)
{
    // struct mosquitto_message{
    //     int mid;
    //     char *topic;
    //     void *payload;
    //     int payloadlen;
    //     int qos;
    //     bool retain;
    // };
    if(message) {
        printf("msg ->  topic:%s mid:%d, qos:%d, retain:%d payload:%s \n", \
            message->topic, message->mid, message->qos, message->retain, (char *)message->payload);
    }
}

void my_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
    print_mqtt_message(message);
    if(message->payloadlen){
        //parse json here
        // char *data=(char *)message->payload;
        // cJSON *pose=cJSON_Parse(data);
        // cJSON *item_x = cJSON_GetObjectItem(pose,"x"); //获取这个对象成员
        // double pose_x=item_x->valuedouble;

        // cJSON *item_y = cJSON_GetObjectItem(pose,"y"); //获取这个对象成员
        // double pose_y = item_y->valuedouble;

        // cJSON *item_yaw = cJSON_GetObjectItem(pose,"yaw"); //获取这个对象成员
        // double pose_yaw = item_yaw->valuedouble;
        // printf("pose_x :%lf, pose_y :%lf, pose_yaw :%lf\n\n", pose_x, pose_y, pose_yaw);

    }else{
        printf("%s (null)\n", message->topic);
    }
    fflush(stdout);
}

void my_connect_callback(struct mosquitto *mosq, void *userdata, int result)
{
    if(!result){
        printf("Connect ok, start sub %s, qos:%d\n", TOPIC_NAME, QOS);
        // 订阅主题函数
        // ①mosq：有效的mosquitto实例，客户端
        // ②mid： 指向int的指针。如果不为NULL，则函数会将其设置为该特定消息的消息ID。
        //        然后可以将其与订阅回调一起使用，以确定何时发送消；主题的消息ID
        // ③sub： 主题名称，订阅模式。
        // ④qos : 此订阅请求的服务质量。
        if(mosquitto_subscribe(mosq, NULL, TOPIC_NAME, QOS) != MOSQ_ERR_SUCCESS) {
            printf("mosquitto_subscribe %s failed\n", TOPIC_NAME);
        } 
         if(mosquitto_subscribe(mosq, NULL, "mqttx_4299c767/home/PM2_5", QOS) != MOSQ_ERR_SUCCESS) {
            printf("mosquitto_subscribe %s failed\n", TOPIC_NAME);
        } 
        if(mosquitto_subscribe(mosq, NULL, "mqttx_4299c767/home/+", QOS) != MOSQ_ERR_SUCCESS) {
            printf("mosquitto_subscribe %s failed\n", TOPIC_NAME);
        } 

    }else{
        printf( "Connect failed, result:%d\n", result);
    }
}

void my_subscribe_callback(struct mosquitto *mosq, void *userdata, int mid, int qos_count, const int *granted_qos)
{
    int i;
    printf("Subscribed (mid: %d): %d", mid, granted_qos[0]);
    for(i = 1; i < qos_count; i++){
        printf(", %d", granted_qos[i]);
    }
    printf("\n");
}
/*
og_callback-> Client null received PUBLISH (d0, q2, r0, m6, 'topic_pose', ... (86 bytes))
log_callback-> Client null sending PUBREC (m6, rc0)
log_callback-> Client null received PUBREL (Mid: 6)
log_callback-> Client null sending PUBCOMP (m6)
PUBREC: 是MQTT（Message Queuing Telemetry Transport）协议中的一个消息类型。
    在MQTT中，当客户端发布(PUBLISH)一条消息到服务器时，服务器会给予一个确认回复(PUBACK)，
    而当客户端收到PUBACK后，会发送一个确认接收的回复(PUBREC)给服务器。这个过程用于确保消息的可靠传递。

PUBREL: 是MQTT（Message Queuing Telemetry Transport）协议中的一种消息类型。
    它是用于在发布者和订阅者之间进行可靠消息传输的确认机制之一。
    当一个客户端发送PUBREC消息作为对收到的QoS 2级别的PUBLISH消息的确认时，
    服务器将会响应一个PUBREL消息。这个PUBREL消息表示服务器已经成功接收到客户端的确认，并且可以释放相关资源。
    通过使用PUBREL消息，MQTT确保了在QoS 2级别下消息的可靠性和顺序性。

PUBCOMP: 是MQTT协议中的一种消息类型，用于确认收到的QoS 2级别的发布消息。当客户端收到QoS 2级别的发布消息后，
    会发送PUBCOMP消息作为确认，以通知服务器已成功接收和处理该消息。
*/
void my_log_callback(struct mosquitto *mosq, void *userdata, int level, const char *str)
{
    /* Pring all log messages regardless of level. */
    // printf("log_callback-> %s\n", str);
}

int main()
{
    struct mosquitto *mosq = NULL;
    //libmosquitto 库初始化
    mosquitto_lib_init();
    mosquitto_int_option(mosq, MOSQ_OPT_PROTOCOL_VERSION, MQTT_PROTOCOL_V5);

    //创建mosquitto客户端
    mosq = mosquitto_new(CLIENT_ID, clean_session, NULL);
    if(!mosq){
        printf("create client failed..\n");
        mosquitto_lib_cleanup();
        return 1;
    }
    // 设置日志回调函数，需要时可使用
    // PUBLISH (d0, q0, r0, m0, 'topic_pose ', ... (33 bytes))
    // 可以自定义日志回调函数
    mosquitto_log_callback_set(mosq, my_log_callback);

    // 连接确认回调函数，当代理发送CONNACK消息以响应连接时，将调用此方法
    // obj：mosquitto_new中提供的用户数据
    // rc 连接响应的返回码，其中有：
    //      0-成功
    //      1-连接被拒绝（协议版本不可接受）
    //      2-连接被拒绝（标识符被拒绝）
    //      3-连接被拒绝（经纪人不可用）
    //      4-255-保留供将来使用
    mosquitto_connect_callback_set(mosq, my_connect_callback);
    // 消息回调函数，收到订阅的消息后调用
    // mosq： 有效的mosquitto实例，客户端。
    // on_message 回调函数，格式如下:void callback（struct mosquitto *mosq，void *obj，const struct mosquitto_message *message）
    //     回调的参数：
    //     ①mosq：进行回调的mosquitto实例
    //     ②obj： mosquitto_new中提供的用户数据
    //     ③message: 消息数据，回调完成后，库将释放此变量和关联的内存，客户应复制其所需要的任何数据。
    //         struct mosquitto_message{
    //         int mid; //消息序号ID
    //         char *topic; //主题
    //         void *payload; //主题内容 ，MQTT 中有效载荷
    //         int payloadlen; //消息的长度，单位是字节
    //         int qos;     //服务质量
    //         bool retain; //是否保留消息
    //         };
    mosquitto_message_callback_set(mosq, my_message_callback);
    // 设置订阅回调。当代理响应订阅请求时，将调用此函数
    // mosq：结构体mosquitto的指针
    // n_subscribe：一个回调函数
    //     void (*on_subscribe)(struct mosquitto *mosq, void *obj, int mid, int qos_count, const int *granted_qos)
    //         ①mosq：结构体mosquitto的指针
    //         ②obj：创建客户端的回调参数，是mosquitto_new中提供的用户数据
    //         ③mid：订阅消息的消息 id
    //         ④qos_count：授予的订阅数（granted_qos的大小）
    //         ⑤granted_qos：一个整数数组，指示为每个订阅授予的 QoS
    mosquitto_subscribe_callback_set(mosq, my_subscribe_callback);  
     //设置密码,  如果不设置密码，将收不到主题数据
    if(mosquitto_username_pw_set(mosq, USERNAME, PASSWORD) != MOSQ_ERR_SUCCESS) {
        printf("client username_pw_set failed..\n");
        mosquitto_lib_cleanup();
        return 1;
    }

    //连接服务器
    if(mosquitto_connect(mosq, HOST, PORT, KEEP_ALIVE)){
        printf("Unable to connect.\n");
        return 1;
    }
    //开启一个线程，在线程里不停的调用 mosquitto_loop() 来处理网络信息
    int loop = mosquitto_loop_start(mosq);
    if(loop != MOSQ_ERR_SUCCESS)
    {
        printf("mosquitto loop error\n");
        return 1;
    }

    while(1)
    {
       sleep(1);
    }

    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    return 0;
}

