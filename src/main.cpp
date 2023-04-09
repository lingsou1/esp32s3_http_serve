/*
接线说明:该程序应该烧录进合宙的板子中去

程序说明:通过建立一个webServe服务器接受来自客户端的HTTP信息,通过对客户端的信息进行解析得到客户端的某引脚状态,
         根据引脚状态来控制服务器的灯的亮灭状态,实现通过客户端来控制服务器
         当按下客户端的 BOOT 按键时服务器的两颗LED灯关闭,否则灯常亮

注意事项:当该程序只运行一点时,有可能不是他自己的问题,可能是客户端出现问题了,导致服务器无数据可处理,不运行

函数示例:无

作者:灵首

时间:2023_3_20

*/
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WebServer.h>

WebServer esp32s3_webServe(80);//实例化一个网页服务的对象
WiFiMulti wifi_multi;  //建立WiFiMulti 的对象,对象名称是 wifi_multi

//通过 ping ipconfig 方法实现找到以下设置参数
IPAddress local_IP(192, 168, 0, 123); // 设置ESP32s3-NodeMCU联网后的IP
IPAddress gateway(192, 168, 0, 1);    // 设置网关IP（通常网关IP是WiFI路由IP）
IPAddress subnet(255, 255, 255, 0);   // 设置子网掩码
IPAddress dns(192,168,0,1);           // 设置局域网DNS的IP（通常局域网DNS的IP是WiFI路由IP）
 
#define LED_A 10
#define LED_B 11



/*
# brief 连接WiFi的函数
# param 无
# retval 无
*/
void wifi_multi_con(void){
  int i=0;
  while(wifi_multi.run() != WL_CONNECTED){
    delay(1000);
    i++;
    Serial.print(i);
  }

}


/*
# brief 写入自己要连接的WiFi名称及密码,之后会自动连接信号最强的WiFi
# param 无
# retval  无
*/
void wifi_multi_init(void){
  wifi_multi.addAP("LINGSOU123","12345678");
  wifi_multi.addAP("LINGSOU12","12345678");
  wifi_multi.addAP("LINGSOU1","12345678");
  wifi_multi.addAP("LINGSOU234","12345678");   //通过 wifi_multi.addAP() 添加了多个WiFi的信息,当连接时会在这些WiFi中自动搜索最强信号的WiFi连接
}



/*
# brief  处理来自客户端的信息,通过客户端返回的信息找到服务器灯的亮灭信息
# param 无
# retval 无
*/ 
void handleUpdate(){
  //获取客户端发送的HTTP请求信息中的浮点数,整数以及按键值
  float floatValue = esp32s3_webServe.arg("float").toFloat();
  int intValue = esp32s3_webServe.arg("int").toInt();
  int buttonValue = esp32s3_webServe.arg("button").toInt();

  //发送HTTP响应
  esp32s3_webServe.send(200,"text/plain","Recieved");

  //通过客户端中获取的buttonValue值来设置开发板的LED灯的亮灭
  if(buttonValue == 0){
    digitalWrite(LED_A,0);
    digitalWrite(LED_B,0);
  }
  else{
    digitalWrite(LED_A,1);
    digitalWrite(LED_B,1);
  }

  // 通过串口监视器输出获取到的变量数值
  Serial.print("floatValue = ");  Serial.println(floatValue);  
  Serial.print("intValue = ");  Serial.println(intValue); 
  Serial.print("buttonValue = ");  Serial.println(buttonValue);   
  Serial.print("\n");

}



void esp32s3_webServe_init(){
  esp32s3_webServe.begin();
  esp32s3_webServe.on("/update",handleUpdate);
  Serial.print("HTTP Serve begin successfully!!!\n");
}

void setup() {
  // 启动串口通讯
  Serial.begin(9600);          
  Serial.println("");
  
  //设置LED灯的状态
  pinMode(LED_A,OUTPUT);
  pinMode(LED_B,OUTPUT);
  digitalWrite(LED_A,1);
  digitalWrite(LED_B,0);

  //设置开发板的网络环境
  if(!WiFi.config(local_IP,gateway,subnet)){
    Serial.print("Failed to config the esp32s3 ip!!!");
  }

  //WiFi连接设置
  wifi_multi_init();//储存多个WiFi
  wifi_multi_con();//自动连接WiFi

  //输出连接信息(连接的WIFI名称及开发板的IP地址)
  Serial.print("\nconnect wifi:");
  Serial.print(WiFi.SSID());
  Serial.print("\n");
  Serial.print("\nIP address:");
  Serial.print(WiFi.localIP());
  Serial.print("\n");

  //处理服务器更新函数
  esp32s3_webServe_init();
}

void loop() {
  esp32s3_webServe.handleClient();    //保证服务器一直在工作能接收到来自客户端的信息
}