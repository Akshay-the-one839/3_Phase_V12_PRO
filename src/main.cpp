#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <Preferences.h>
#include <HTTPClient.h>
//#include <Ticker.h>

#define TRIGGER_PIN 7

#define LED_PIN 4

// Function prototypes
void saveConfigCallback();
void loadParameters();
void saveParameters();
void clearParameters();
void checkButton();
void checkprevent();
void send_cyclic_time();
void readservercyclic();
void readserversample();
void readserver();
void senddata_pic();
void updatetoserver1();
void updatetoserver2();
void updatetoserver3();
void updatetimer();
void readack();

WiFiManager wm;
Preferences preferences;
//Ticker serialCheck;


//char customerID[40] = "";
char deviceID[40] = "";
char tokenID[40] = "";
bool shouldSaveConfig = false;
String state,drys,drya,powon,lr,ly,lb;

String statep,dryp,dryap,powonp;
const String host = "http://newqbitronics.org.in";
const char customhead[]= "<style>h1 {text-align:center;font-size:30px;}</style>";//<h1>New Qbitronics</h1>
String rec;
String level="";;
int sflag=0;
     int flagf2=0;
String state1;
String stateack;
String payload,payload1,drya1,drya2,drya3,drya11,vr,vy,vb;
 String low_vr,low_vy,low_vb,high_vr,high_vy,high_vb,over_rc,over_yc,over_bc,timer_st,phase;
 String tlow_vr,tlow_vy,tlow_vb,thigh_vr,thigh_vy,thigh_vb,tover_rc,tover_yc,tover_bc,ttimer_st,tphase;
 String cyclic_state,cyclic_1on,cyclic_1off,cyclic_2on,cyclic_2off,cyclic_3on,cyclic_3off,cyclic_4on,cyclic_4off,cyclic_5on,cyclic_5off,on_delay,off_delay,dry_rst,pw_ondly;
 String cyclic_state1,cyclic_1on1,cyclic_1off1,cyclic_2on1,cyclic_2off1,cyclic_3on1,cyclic_3off1,cyclic_4on1,cyclic_4off1,cyclic_5on1,cyclic_5off1,on_delay1,off_delay1,dry_rst1,pw_ondly1;
 int cy_time_flag=0;
 int preventflag;
 String t1="",t2="",vflag="",vflag1="",t11="",t12="",t13="",t14="";
WiFiClient wifiClient; // Create a WiFiClient object

 String rv="";
   String yv="";
   String bv="";
    String rr="";
   String yr="";
   String br="";

   String ssid="";
   String pass="";
 //  String vflag="";
 //   String vflag1="";
String fing="";
int brflg=0;
int setflag=0;
int jflg=0;
int stv;
String vflaga="";

int tflag=0;

int powerfg=0;
unsigned long previousMillis = 0;
unsigned long interval = 30000;

void saveConfigCallback() {
  Serial.println("Config should be saved");
  shouldSaveConfig = true;
}

void loadParameters() {
  preferences.begin("wifi-config", true);
  //strcpy(customerID, preferences.getString("customerID", "").c_str());
  strcpy(deviceID, preferences.getString("deviceID", "").c_str());
  strcpy(tokenID, preferences.getString("tokenID", "").c_str());
  preferences.end();
}

void saveParameters() {
  preferences.begin("wifi-config", false);
  //preferences.putString("customerID", customerID);
  preferences.putString("deviceID", deviceID);
  preferences.putString("tokenID", tokenID);
  preferences.end();
}


void clearParameters() {
  preferences.begin("wifi-config", false);
  preferences.clear();
  preferences.end();
}


void checkButton() {
  
      
  if (digitalRead(TRIGGER_PIN) == LOW) {
     digitalWrite(LED_PIN,HIGH);
      delay(300);
      digitalWrite(LED_PIN,LOW);
       delay(300);
      digitalWrite(LED_PIN,HIGH);
      delay(300);
      digitalWrite(LED_PIN,LOW);
       delay(300);
      digitalWrite(LED_PIN,HIGH);
      delay(300);
      digitalWrite(LED_PIN,LOW);
       delay(300);
      digitalWrite(LED_PIN,HIGH);
      delay(300);
      digitalWrite(LED_PIN,LOW);
      delay(2000);
       
    if (digitalRead(TRIGGER_PIN) == LOW) {
      unsigned long startTime = millis();
      while (digitalRead(TRIGGER_PIN) == LOW) {
        if (millis() - startTime > 3000) {
      digitalWrite(LED_PIN,HIGH);
      delay(300);
      digitalWrite(LED_PIN,LOW);
       delay(300);
      digitalWrite(LED_PIN,HIGH);
      delay(300);
      digitalWrite(LED_PIN,LOW);
       delay(300);
      digitalWrite(LED_PIN,HIGH);
      delay(300);
      digitalWrite(LED_PIN,LOW);
       delay(300);
      digitalWrite(LED_PIN,HIGH);
      delay(300);
      digitalWrite(LED_PIN,LOW);
          Serial.println("Button held for 3 seconds. Clearing settings and restarting...");
          wm.resetSettings();
          clearParameters();
          ESP.restart();
        }
        delay(10);
        yield();
      }

      Serial.println("Button pressed. Starting Config Portal...");
      
      WiFiManager tempWM;
      tempWM.setSaveConfigCallback(saveConfigCallback);

      //WiFiManagerParameter custom_customerID("customerID", "Customer ID", customerID, 40);
      WiFiManagerParameter custom_deviceID("deviceID", "Device ID", deviceID, 40);
      WiFiManagerParameter custom_tokenID("tokenID", "Token ID", tokenID, 40);

      //tempWM.addParameter(&custom_customerID);
      tempWM.addParameter(&custom_deviceID);
      tempWM.addParameter(&custom_tokenID);

      tempWM.setShowInfoUpdate(false);
      tempWM.setShowInfoErase(false);
      std::vector<const char *> wm_menu = {"wifi", "exit"};
      tempWM.setMenu(wm_menu);
      tempWM.setCustomHeadElement("<script>document.title = 'New Qbitronics';</script>");

      if (!tempWM.startConfigPortal("Q SMART CONFIG", "password")) {
        Serial.println("Failed to connect or hit timeout");
      }

      if (shouldSaveConfig) {
        //strcpy(customerID, custom_customerID.getValue());
        strcpy(deviceID, custom_deviceID.getValue());
        strcpy(tokenID, custom_tokenID.getValue());
        saveParameters();
         digitalWrite(LED_PIN,HIGH);
      delay(1000);
      digitalWrite(LED_PIN,LOW);
       delay(1000);
      
      }

      //Serial.print("SSID: ");
      //Serial.println(customerID);
      Serial.print("Device ID: ");
      Serial.println(deviceID);
      Serial.print("Token ID: ");
      Serial.println(tokenID);
    }
  }
}

void checkprevent(){
// low_vr,low_vy,low_vb,high_vr,high_vy,high_vb,over_rc,over_yc,over_bc;
//tlow_vr,tlow_vy,tlow_vb,thigh_vr,thigh_vy,thigh_vb,tover_rc,tover_yc,tover_bc;
Serial.println("$*"+low_vr +"*" +low_vy +"*" + low_vb +"*" + high_vr +"*" + high_vy + "*" +high_vb + "*" +over_rc + "*" +over_yc + "*" + over_bc);

// Serial.flush();
delay(500);
    readack(); 

  
tlow_vr = low_vr;
tlow_vy = low_vy;
tlow_vb = low_vb;

thigh_vr = high_vr;
thigh_vy = high_vy;
thigh_vb = high_vb;

tover_rc = over_rc;
tover_yc = over_yc;
tover_bc = over_bc;

}

void send_cyclic_time(){

if (cy_time_flag == 1){

cyclic_state1 = cyclic_state ; 
  cyclic_1on1 = cyclic_1on ; cyclic_1off1 = cyclic_1off ; 
  cyclic_2on1 = cyclic_2on ; cyclic_2off1 = cyclic_2off ;
  cyclic_3on1 = cyclic_3on ; cyclic_3off1 = cyclic_3off ;
  cyclic_4on1 = cyclic_4on ; cyclic_4off1 = cyclic_4off ;
  cyclic_5on1 = cyclic_5on ; cyclic_5off1 = cyclic_5off;

}

if(cyclic_state1 != cyclic_state && 
  cyclic_1on1 != cyclic_1on && cyclic_1off1 != cyclic_1off && 
  cyclic_2on1 != cyclic_2on && cyclic_2off1 != cyclic_2off &&
  cyclic_3on1 != cyclic_3on && cyclic_3off1 != cyclic_3off &&
  cyclic_4on1 != cyclic_4on && cyclic_4off1 != cyclic_4off &&
  cyclic_5on1 != cyclic_5on && cyclic_5off1 != cyclic_5off){

Serial.println("~*"+ cyclic_state + "*" + cyclic_1on+"*"+cyclic_1off+"*"+cyclic_2on+"*"+cyclic_2off+"*"+cyclic_3on+"*"+cyclic_3off+"*"+cyclic_4on+"*"+cyclic_4off+"*"+cyclic_5on+"*"+cyclic_5off+"+");
delay(2000);
cy_time_flag =0;
  }


}




void readservercyclic() {
  if (WiFi.status() == WL_CONNECTED) {
    String url = "/motorpro/motorvoltvpro/readCTR.php?";    
    url += "deviceid=";
    url += deviceID;
    url += "&tokenid=";
    url += tokenID;
   // url += "&customerid=";
   // url += customerID;

    String server_url = host + url;

    HTTPClient http;
   // Serial.println(server_url);
    http.begin(wifiClient, server_url);  // Updated API call
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      payload1 = http.getString();
      if (httpResponseCode == HTTP_CODE_OK) {
        //Serial.println("HTTP Response code: " + String(httpResponseCode));
       // Serial.println("Response: ");
       // Serial.println(payload);
      } else {
        Serial.println("HTTP request failed");
      }
    } else {
      Serial.println("Failed to connect to server " + String(http.errorToString(httpResponseCode)));
    }
    http.end();
  } else {
    Serial.println("WiFi not connected");
  }
  int a=0; 
  a=payload1.indexOf('@');
 //Serial.print("a="); 
 //Serial.println(payload1); 

  if(a==16)
  {

char buf[150];
payload1.toCharArray(buf, sizeof(buf));

// 3. Tokenize by "*"
String tokens[20];  // store up to 20 fields
int idx = 0;

char *c = strtok(buf, "*");
while (c != NULL && idx < 20) {
  tokens[idx++] = String(c);
  c = strtok(NULL, "*");
}
cyclic_state =   tokens[2];       
cyclic_1on =  tokens[3];
cyclic_1off =  tokens[4];
cyclic_2on = tokens[5];
cyclic_2off =  tokens[6];
cyclic_3on =  tokens[7];
cyclic_3off =  tokens[8];
cyclic_4on =  tokens[9];
cyclic_4off =  tokens[10];
cyclic_5on =  tokens[11];
cyclic_5off =  tokens[12];
/*
// 4. Now you can use them ,cyclic_1on,cyclic_1off
Serial.println("state2 = " + tokens[1]);       // "0"
Serial.println("cyclic_state = " + tokens[2]); // "1",cyclic_1on,cyclic_1off
Serial.println("cyclic_1on = " + tokens[3]);      // "14:07:00"
Serial.println("cyclic_1off = " + tokens[4]);      // "16:07:00"
Serial.println("cyclic_2on = " + tokens[5]);      // "14:07:00"
Serial.println("cyclic_2off = " + tokens[6]);      // "16:07:00"
Serial.println("cyclic_3on = " + tokens[7]);      // "14:07:00"
Serial.println("cyclic_3off = " + tokens[8]);      // "16:07:00"
Serial.println("cyclic_4on = " + tokens[9]);      // "14:07:00"
Serial.println("cyclic_4off = " + tokens[10]);      // "16:07:00"
Serial.println("cyclic_5on = " + tokens[11]);      // "14:07:00"
Serial.println("cyclic_5off = " + tokens[12]);      // "16:07:00" */
 
 }


}
//////////////////////////////////////////



//////////////////////////////////////////
void readserversample() {
  if (WiFi.status() == WL_CONNECTED) {
    String url = "/motorpro/motorvoltvpro/sample.php?";    
    url += "deviceid=";
    url += deviceID;
    url += "&tokenid=";
    url += tokenID;
   // url += "&customerid=";
   // url += customerID;

    String server_url = host + url;

    HTTPClient http;
   // Serial.println(server_url);
    http.begin(wifiClient, server_url);  // Updated API call
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      payload = http.getString();
      if (httpResponseCode == HTTP_CODE_OK) {
        //Serial.println("HTTP Response code: " + String(httpResponseCode));
       // Serial.println("Response: ");
       // Serial.println(payload);
        //   Serial.println(payload);
      } else {
        Serial.println("HTTP request failed");
      }
    } else {
      Serial.println("Failed to connect to server " + String(http.errorToString(httpResponseCode)));
    }
    http.end();

  } else {
    Serial.println("WiFi not connected");
  }

}

//////////////////////////////////////////
void readserver() {
  if (WiFi.status() == WL_CONNECTED) {
    String url = "/motorpro/motorvoltvpro/read.php?";    
    url += "deviceid=";
    url += deviceID;
    url += "&tokenid=";
    url += tokenID;
   // url += "&customerid=";
   // url += customerID;

    String server_url = host + url;

    HTTPClient http;
   // Serial.println(server_url);
    http.begin(wifiClient, server_url);  // Updated API call
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      payload = http.getString();
      if (httpResponseCode == HTTP_CODE_OK) {
        //Serial.println("HTTP Response code: " + String(httpResponseCode));
       // Serial.println("Response: ");
       // Serial.println(payload);
        //   Serial.println(payload);
      } else {
        Serial.println("HTTP request failed");
      }
    } else {
      Serial.println("Failed to connect to server " + String(http.errorToString(httpResponseCode)));
    }
    http.end();

  } else {
    Serial.println("WiFi not connected");
  }
  int a=0; 
  a=payload.indexOf('#');
 //Serial.print("a="); 
 //Serial.println(a); 

  if(a==16)
  {

char buf[85];
payload.toCharArray(buf, 85); 
//Serial.println(buf);
char *p = strtok(buf, "*"); // first token
String state0 = String(p);

p = strtok(NULL, "*");      // second token
state = String(p);

p = strtok(NULL, "*"); 
 drys  = String(p);

p = strtok(NULL, "*"); 
 drya  = String(p);

p = strtok(NULL, "*"); 
 powon  = String(p);

 p = strtok(NULL, "*"); 
 timer_st  = String(p);

p = strtok(NULL, "*"); 
 low_vr  = String(p);

p = strtok(NULL, "*"); 
 low_vy  = String(p);

p = strtok(NULL, "*"); 
 low_vb  = String(p);

p = strtok(NULL, "*"); 
 high_vr  = String(p);

p = strtok(NULL, "*"); 
 high_vy  = String(p);

p = strtok(NULL, "*"); 
 high_vb  = String(p);

p = strtok(NULL, "*"); 
 over_rc  = String(p);

p = strtok(NULL, "*"); 
 over_yc  = String(p);

p = strtok(NULL, "*"); 
 over_bc  = String(p);

 p = strtok(NULL, "*");
if (p != NULL) {
    char buf[4];                   
    sprintf(buf, "%02d", atoi(p));  
    on_delay = String(buf);         
}


 p = strtok(NULL, "*");
if (p != NULL) {
    char buf[4];                   
    sprintf(buf, "%02d", atoi(p));  
    off_delay = String(buf);         
}

p = strtok(NULL, "*"); 
 String phase3  = String(p);

p = strtok(NULL, "*"); 
 String phase4  = String(p);
 
 p = strtok(NULL, "*"); 
  phase  = String(p);

   p = strtok(NULL, "*");
if (p != NULL) {
    char buf[4];                    // 3 digits + null terminator
    sprintf(buf, "%02d", atoi(p));  // format with leading zeros
    pw_ondly = String(buf);          // if you still need Arduino String
}

 p = strtok(NULL, "*");
if (p != NULL) {
    char buf[4];                    // 3 digits + null terminator
    sprintf(buf, "%03d", atoi(p));  // format with leading zeros
    dry_rst = String(buf);          // if you still need Arduino String
}
 
 }

  if( state!=statep || drys !=dryp || drya !=dryap || powon!=powonp || timer_st != ttimer_st || phase != tphase || on_delay != on_delay1 || off_delay != off_delay1 || dry_rst != dry_rst1 || pw_ondly != pw_ondly1)
   {
     
     setflag=1;

      
     Serial.print('#'); 
    if(state=="")
    {
       Serial.print("0");
    }
    else
     Serial.print(state);   
     
     ///////////////////////////////////////////// 
      Serial.print('*'); 
      int myInt = drys.toInt(); 
     if(myInt < 10)
     {
        Serial.print("0");   
       Serial.print(drys);   
       
     }
     else
     Serial.print(drys); 
     ///////////////////////////////////////////////////////////  
     Serial.print('*'); 
    int myInta = drya.toInt(); 
     if(myInta < 10)
     {
        Serial.print("0");   
       Serial.print(drya);   
       
     }
     else
     Serial.print(drya);  
     ///////////////////////////////////////////////////// 
     Serial.print('*');      
      
     Serial.print(powon);
      Serial.print('*');   
      
    
      
     Serial.print(timer_st);
      Serial.print('*');
        
      
     Serial.print(phase);
      Serial.print('*');
       Serial.print(on_delay);
      Serial.print('*');
       Serial.print(off_delay);
      Serial.print('*');
        Serial.print(pw_ondly);
      Serial.print('*');
       Serial.print(dry_rst);
      Serial.print('*');
    
     delay(1000); 
    statep=state;
    dryp=drys; 
    dryap=drya;
    powonp=powon;
    ttimer_st = timer_st;
    tphase = phase;
    on_delay1 = on_delay;
    off_delay1 = off_delay;
    dry_rst1 = dry_rst;
    pw_ondly1 = pw_ondly;
   }

if(low_vr != tlow_vr || low_vy != tlow_vy || low_vb != tlow_vb || high_vr != thigh_vr || high_vy != thigh_vy || high_vb != thigh_vb  || over_rc  != tover_rc || over_yc  != tover_yc || over_bc  != tover_bc ){
setflag=2;
}

}

void senddata_pic()
{
   setflag=1;

      
     Serial.print('#'); 
    if(state=="")
    {
       Serial.print("0");
    }
    else
     Serial.print(state);   
     
     ///////////////////////////////////////////// 
      Serial.print('*'); 
      int myInt = drys.toInt(); 
     if(myInt < 10)
     {
        Serial.print("0");   
       Serial.print(drys);   
       
     }
     else
     Serial.print(drys); 
     ///////////////////////////////////////////////////////////  
     Serial.print('*'); 
    int myInta = drya.toInt(); 
     if(myInta < 10)
     {
        Serial.print("0");   
       Serial.print(drya);   
       
     }
     else
     Serial.print(drya);  
     ///////////////////////////////////////////////////// 
     Serial.print('*');      
      
     Serial.print(powon);
      Serial.print('*');   
       

     
      
     delay(1000); 
    statep=state;
    dryp=drys; 
    dryap=drya;
    powonp=powon;
    
}
  
void updatetoserver1() {
 // Serial.println("Updating to server...");
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "/motorpro/motorvoltvpro/update.php?"; 
    url += "deviceid=";
    url += deviceID;
    url += "&tokenid=";
    url += tokenID;
    //url += "&customerid=";
    //url += customerID;
    url += "&state=";
    url += state1;
    url += "&state_ack=";
    url += stateack;
    url += "&current_r=";
    url += drya1;
     url += "&current_y=";
    url += drya2;
     url += "&current_b=";
    url += drya3;
    url += "&dryrun_ack=";
    url += drya11;
    url += "&voltage_r=";
    url += rv;
    url += "&voltage_y=";
    url += yv;
    url += "&voltage_b=";
    url += bv;    
     url+="&pch_r=";
     url+=rr;
      url+="&pch_y=";
     url+=yr;
      url+="&pch_b=";
     url+=br;
    url += "&wlevel=";
    url +=level;
    
    String serverurl = host + url;
    //Serial.println(serverurl);
    http.begin(wifiClient, serverurl);  // Updated API call
    http.addHeader("Content-Type", "text/plain");

    int httpResponseCode = http.POST("");
    String response = http.getString();

  //  Serial.println("HTTP Response code: update  " + String(httpResponseCode));
   // Serial.println("Response: up  " + response);

    http.end();
    for(int k=0;k<5;k++)
    {
    delay(1000);
    }
    
  } else {
    Serial.println("\nNot connected to Wi-Fi");
  }
}
void updatetoserver2() {
  //Serial.println("Updating to server...");
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "/motorpro/motorvoltvpro/update.php?";
    url += "deviceid=";
    url += deviceID;
    url += "&tokenid=";
    url += tokenID;
    //url += "&customerid=";
    //url += customerID;
    url += "&current_r=";
    url += drya1;
     url += "&current_y=";
    url += drya2;
     url += "&current_b=";
    url += drya3;
    url += "&voltage_r=";
    url += rv;
    url += "&voltage_y=";
    url += yv;
    url += "&voltage_b=";
    url += bv;  
    url += "&wlevel=";
    url +=level;
    String serverurl = host + url;
  //  Serial.println(serverurl);
    http.begin(wifiClient, serverurl);  // Updated API call
    http.addHeader("Content-Type", "text/plain");

    int httpResponseCode = http.POST("");
    String response = http.getString();

   // Serial.println("HTTP Response code: update  " + String(httpResponseCode));
   // Serial.println("Response: up  " + response);

    http.end();
    delay(5000);
  } else {
    Serial.println("\nNot connected to Wi-Fi");
  }
}

void updatetoserver3() {
  fing="1";
  //Serial.println("Updating to server...");
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "/motorpro/motorvoltvpro/update.php?";
    url += "deviceid=";
    url += deviceID;
    url += "&tokenid=";
    url += tokenID;
    url+="&SET_DATA=";
     url+=fing;
    String serverurl = host + url;
  //  Serial.println(serverurl);
    http.begin(wifiClient, serverurl);  // Updated API call
    http.addHeader("Content-Type", "text/plain");

    int httpResponseCode = http.POST("");
    String response = http.getString();

   // Serial.println("HTTP Response code: update  " + String(httpResponseCode));
   // Serial.println("Response: up  " + response);

    http.end();
    delay(5000);
  } else {
    Serial.println("\nNot connected to Wi-Fi");
  }
}

void updatetimer() {
 // Serial.println("Updating to server...");
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "/motorpro/motorvoltvpro/compare.php?";
    url += "deviceid=";
    url += deviceID;
    url += "&tokenid=";
    url += tokenID;
    
    String serverurl3 = host + url;
  //  Serial.println(serverurl);
    http.begin(wifiClient, serverurl3);  // Updated API call
    http.addHeader("Content-Type", "text/plain");

    int httpResponseCode = http.POST("");
    String response = http.getString();

   // Serial.println("HTTP Response code: update  " + String(httpResponseCode));
   // Serial.println("Response: up  " + response);

    http.end();
    
    delay(5000);
  } else {
    Serial.println("\nNot connected to Wi-Fi");
  }
}

void readack(){
 // Serial.flush();
   rec=Serial.readString();
   
  //Serial.println("rec");
    String  ta=rec.substring(0,1); 
   
    if(ta=="$")
 {
   
   jflg=1;
   t1=rec.substring(1,2); 
   t2=rec.substring(3,4);
   drya1=rec.substring(5,9);
   drya2=rec.substring(9,13);
   drya3=rec.substring(13,17);

   rv=rec.substring(17,20); 
   yv=rec.substring(20,23);
   bv=rec.substring(23,26);  
   vflag=rec.substring(26,27);

   vflaga=rec.substring(27,28);
    rr=rec.substring(28,29);
    yr=rec.substring(29,30);
    br=rec.substring(30,31);

   // Serial.println(level );
   //Serial.println(" seq "); 
   //Serial.println(vflag1);
   //Serial.println(rr); 
   

    if(t1=="a")
  {
   state1="0";
   stateack="0";
   state="0";
   drya11="0";
     flagf2=1;
  }
   
 if(t1=="A")
 {
  state="1";
   state1="1";
 stateack="1";
drya11="0";
 }

 if(t2=="1")
 {
  state1="0";
 stateack="0";
 state="0";
 drya11="1";
 //Serial.println("k4"); 
 }

 if(t2=="2")
 {
  state1="0";
 stateack="0";
 state="0";
 drya11="2";
 //Serial.println("k4"); 
 }

 if(vflaga=="1")
{
  vflag1="0";
  state1="0";
 stateack="0";
 state="0";
 drya11="0";
  
  Serial.println("k5"); 
}
 
 updatetoserver1();
 brflg=1;
 digitalWrite(LED_PIN,LOW);
               delay(200);
              digitalWrite(LED_PIN,HIGH);
              delay(200);
              digitalWrite(LED_PIN,LOW);
              delay(200);
              digitalWrite(LED_PIN,HIGH);
              delay(200);
              digitalWrite(LED_PIN,LOW);
               delay(200);
              digitalWrite(LED_PIN,HIGH);
              setflag=0;
 }
  if(ta=="&")
 {
    jflg=1;
   drya1=rec.substring(1,5);
   drya2=rec.substring(5,9);
   drya3=rec.substring(9,13);  
   
   rv=rec.substring(13,16);
   yv=rec.substring(16,19);
   bv=rec.substring(19,22);
  
   updatetoserver2();
   brflg=1;
   digitalWrite(LED_PIN,LOW);
               delay(500);
              digitalWrite(LED_PIN,HIGH);
             // setflag=0;
}
 if(ta=="#")
 {
setflag = 1;
  Serial.println("%"); 
 }

  if(ta=="^")
 {
cy_time_flag = 1;
setflag = 1;
  Serial.println("^"); 
 }
}
/*
void checkSerial() {
  if (Serial.available()) {
    readack();     
  }
}
*/
 


void setup() {
  Serial.begin(9600);
  // Serial.setPins(44, 43); // RX, TX
 // serialCheck.attach_ms(1000, checkSerial); // Call every 50ms

  pinMode(TRIGGER_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN,HIGH);
  delay(1000);
  digitalWrite(LED_PIN,LOW);
     
  
   checkButton();

  loadParameters();
  
  //Serial.print("ESP.getFreeHeap(): ");
 // Serial.println(ESP.getFreeHeap());

  wm.setSaveConfigCallback(saveConfigCallback);

  //WiFiManagerParameter custom_customerID("customerID", "Customer ID", customerID, 40);
  WiFiManagerParameter custom_deviceID("deviceID", "Device ID", deviceID, 40);
  WiFiManagerParameter custom_tokenID("tokenID", "Token ID", tokenID, 40);

  //wm.addParameter(&custom_customerID);
  wm.addParameter(&custom_deviceID);
  wm.addParameter(&custom_tokenID);
  //wm.setConnectTimeout(300); // how long try to connect to the wifi
  wm.setConfigPortalTimeout(300);
  wm.setShowInfoUpdate(false);
  wm.setShowInfoErase(false);
  std::vector<const char *> wm_menu = {"wifi", "exit"};
  wm.setMenu(wm_menu);
  wm.setTitle("NEW QBITRONICS");
  wm.setCustomHeadElement(customhead);
 
    
  if (WiFi.status() != WL_CONNECTED ) {
         
      digitalWrite(LED_PIN,HIGH);  
       delay(500);
       digitalWrite(LED_PIN,LOW);      
      delay(500);
      digitalWrite(LED_PIN,HIGH);  
       delay(500);
       digitalWrite(LED_PIN,LOW);      
      delay(500);
      digitalWrite(LED_PIN,HIGH);  
       delay(500);
       digitalWrite(LED_PIN,LOW);      
      delay(500);
      
      checkButton();
      //wm.autoConnect();
      wm.setConfigPortalTimeout(180);
      if (!wm.autoConnect("SMART CONFIG", "password")) {           
            ESP.restart();
      }
      digitalWrite(LED_PIN,HIGH); 
     
    }
   

  if (shouldSaveConfig) {
    //strcpy(customerID, custom_customerID.getValue());
    strcpy(deviceID, custom_deviceID.getValue());
    strcpy(tokenID, custom_tokenID.getValue());
    saveParameters();
    ssid = WiFi.SSID();
    Serial.println(ssid);
     pass = WiFi.psk();
     Serial.println(pass);

  }

  //Serial.print("Customer ID: ");
  //Serial.println(customerID);
  Serial.print("Device ID: ");
  Serial.println(deviceID);
  Serial.print("Token ID: ");
  Serial.println(tokenID);
  drya11="0";
  
  readserver();  // Initial read from the server after connecting

   tflag=1;
   Serial.print("@");
    digitalWrite(LED_PIN,LOW); 
        delay(500);         
        digitalWrite(LED_PIN,HIGH);  
        delay(500); 
         Serial.print("@");
}

 

void loop() {

  //////////////Wifi Connect//////////////////////////////////////
   if (WiFi.status() != WL_CONNECTED ) {
    wm.setConfigPortalTimeout(30);
    
   digitalWrite(LED_PIN,LOW); 
   if(!wm.autoConnect()) {           
   ESP.restart();
    }
    digitalWrite(LED_PIN,HIGH);  
    delay(500);
 
    }
   //////////////////CK Button Wifi Confihg//////////////////////////
       
  //checkButton();

  //////////////Get Server and Send Data to PIC////////////////////////////////////
//readservercyclic();
   
   readserversample();

    if(setflag==0)
  {   

   Serial.print("Q");
    delay(500);
    if (Serial.available()) {
    readack();     
  }


   
  }
  
  readserver();

  if(setflag==1)
  {   
 
    Serial.print("@");
    while (Serial.available()) {   
        readack();   
        delay(2000);
        delay(2000);
        delay(2000);
        
  }


  } else if(setflag==2){
    checkprevent();
  }



  if(timer_st == "1" ){
    
readservercyclic();
    if(cyclic_state =="1"){
send_cyclic_time();
    }


  }
  
/////////////////////////////////////////////////////////////////////////
  
   
     updatetimer();
     updatetoserver3();
     delay(100);
    // Serial.flush();
  
}
