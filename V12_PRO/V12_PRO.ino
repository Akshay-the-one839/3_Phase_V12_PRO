#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiManager.h>
#include <Preferences.h>

#define TRIGGER_PIN 7
#define LED_PIN 4

WiFiManager wm;
Preferences preferences;

char deviceID[40] = "";
char tokenID[40] = "";
bool shouldSaveConfig = false;
String state, drys, drya, powon, lr, ly, lb;

String statep, dryp, dryap, powonp;
const String host = "http://newqbitronics.org.in";
const char customhead[] = "<style>h1 {text-align:center;font-size:30px;}</style>";
String rec;
String level = "";
int sflag = 0;
int flagf2 = 0;
String state1;
String stateack;
String payload, payload1, drya1, drya2, drya3, drya11, vr, vy, vb;
String low_vr, low_vy, low_vb, high_vr, high_vy, high_vb, over_rc, over_yc, over_bc, timer_st, phase;
String tlow_vr, tlow_vy, tlow_vb, thigh_vr, thigh_vy, thigh_vb, tover_rc, tover_yc, tover_bc, ttimer_st, tphase;
String cyclic_state, cyclic_1on, cyclic_1off, cyclic_2on, cyclic_2off, cyclic_3on, cyclic_3off, cyclic_4on, cyclic_4off, cyclic_5on, cyclic_5off, on_delay, off_delay, dry_rst, pw_ondly;
String cyclic_state1, cyclic_1on1, cyclic_1off1, cyclic_2on1, cyclic_2off1, cyclic_3on1, cyclic_3off1, cyclic_4on1, cyclic_4off1, cyclic_5on1, cyclic_5off1, on_delay1, off_delay1, dry_rst1, pw_ondly1;
int cy_time_flag = 0;
int preventflag;
String t1 = "", t2 = "", vflag = "", vflag1 = "", t11 = "", t12 = "", t13 = "", t14 = "";
WiFiClient wifiClient;

String rv = "";
String yv = "";
String bv = "";
String rr = "";
String yr = "";
String br = "";

String ssid = "";
String pass = "";
String fing = "";
int brflg = 0;
int setflag = 0;
int jflg = 0;
int stv;
String vflaga = "";

int tflag = 0;
int powerfg = 0;
unsigned long previousMillis = 0;
unsigned long interval = 30000;

// ─── UART0 on pins 43(TX) / 44(RX) for PIC ──────────────────────────────────
#define PIC_RX_PIN  44   // GPIO44 = RX  ← PIC TX
#define PIC_TX_PIN  43   // GPIO43 = TX  → PIC RX
HardwareSerial PicSerial(0);  // UART0
// ─────────────────────────────────────────────────────────────────────────────

void saveConfigCallback() {
  // Serial debug removed — pins 43/44 used for PIC
  shouldSaveConfig = true;
}

void loadParameters() {
  preferences.begin("wifi-config", true);
  strcpy(deviceID, preferences.getString("deviceID", "").c_str());
  strcpy(tokenID,  preferences.getString("tokenID",  "").c_str());
  preferences.end();
}

void saveParameters() {
  preferences.begin("wifi-config", false);
  preferences.putString("deviceID", deviceID);
  preferences.putString("tokenID",  tokenID);
  preferences.end();
}

void clearParameters() {
  preferences.begin("wifi-config", false);
  preferences.clear();
  preferences.end();
}

void checkButton() {
  if (digitalRead(TRIGGER_PIN) == LOW) {
    digitalWrite(LED_PIN, HIGH); delay(300);
    digitalWrite(LED_PIN, LOW);  delay(300);
    digitalWrite(LED_PIN, HIGH); delay(300);
    digitalWrite(LED_PIN, LOW);  delay(300);
    digitalWrite(LED_PIN, HIGH); delay(300);
    digitalWrite(LED_PIN, LOW);  delay(300);
    digitalWrite(LED_PIN, HIGH); delay(300);
    digitalWrite(LED_PIN, LOW);
    delay(2000);

    if (digitalRead(TRIGGER_PIN) == LOW) {
      unsigned long startTime = millis();
      while (digitalRead(TRIGGER_PIN) == LOW) {
        if (millis() - startTime > 3000) {
          digitalWrite(LED_PIN, HIGH); delay(300);
          digitalWrite(LED_PIN, LOW);  delay(300);
          digitalWrite(LED_PIN, HIGH); delay(300);
          digitalWrite(LED_PIN, LOW);  delay(300);
          digitalWrite(LED_PIN, HIGH); delay(300);
          digitalWrite(LED_PIN, LOW);  delay(300);
          digitalWrite(LED_PIN, HIGH); delay(300);
          digitalWrite(LED_PIN, LOW);
          // Serial debug removed
          wm.resetSettings();
          clearParameters();
          ESP.restart();
        }
        delay(10);
        yield();
      }

      // Serial debug removed
      WiFiManager tempWM;
      tempWM.setSaveConfigCallback(saveConfigCallback);

      WiFiManagerParameter custom_deviceID("deviceID", "Device ID", deviceID, 40);
      WiFiManagerParameter custom_tokenID("tokenID",   "Token ID",  tokenID,  40);

      tempWM.addParameter(&custom_deviceID);
      tempWM.addParameter(&custom_tokenID);

      tempWM.setShowInfoUpdate(false);
      tempWM.setShowInfoErase(false);
      std::vector<const char *> wm_menu = {"wifi", "exit"};
      tempWM.setMenu(wm_menu);
      tempWM.setCustomHeadElement("<script>document.title = 'New Qbitronics';</script>");

      if (!tempWM.startConfigPortal("Q SMART CONFIG", "password")) {
        // Serial debug removed
      }

      if (shouldSaveConfig) {
        strcpy(deviceID, custom_deviceID.getValue());
        strcpy(tokenID,  custom_tokenID.getValue());
        saveParameters();
        digitalWrite(LED_PIN, HIGH); delay(1000);
        digitalWrite(LED_PIN, LOW);  delay(1000);
      }
      // Serial debug removed
    }
  }
}

void checkprevent() {
  PicSerial.println("$*" + low_vr + "*" + low_vy + "*" + low_vb + "*" +
                    high_vr + "*" + high_vy + "*" + high_vb + "*" +
                    over_rc + "*" + over_yc + "*" + over_bc);
  delay(500);
  readack();

  tlow_vr  = low_vr;  tlow_vy  = low_vy;  tlow_vb  = low_vb;
  thigh_vr = high_vr; thigh_vy = high_vy; thigh_vb = high_vb;
  tover_rc = over_rc; tover_yc = over_yc; tover_bc = over_bc;
}

void send_cyclic_time() {
  if (cy_time_flag == 1) {
    cyclic_state1 = cyclic_state;
    cyclic_1on1  = cyclic_1on;  cyclic_1off1 = cyclic_1off;
    cyclic_2on1  = cyclic_2on;  cyclic_2off1 = cyclic_2off;
    cyclic_3on1  = cyclic_3on;  cyclic_3off1 = cyclic_3off;
    cyclic_4on1  = cyclic_4on;  cyclic_4off1 = cyclic_4off;
    cyclic_5on1  = cyclic_5on;  cyclic_5off1 = cyclic_5off;
  }

  if (cyclic_state1 != cyclic_state &&
      cyclic_1on1  != cyclic_1on  && cyclic_1off1 != cyclic_1off &&
      cyclic_2on1  != cyclic_2on  && cyclic_2off1 != cyclic_2off &&
      cyclic_3on1  != cyclic_3on  && cyclic_3off1 != cyclic_3off &&
      cyclic_4on1  != cyclic_4on  && cyclic_4off1 != cyclic_4off &&
      cyclic_5on1  != cyclic_5on  && cyclic_5off1 != cyclic_5off) {

    PicSerial.println("~*" + cyclic_state +
                      "*" + cyclic_1on + "*" + cyclic_1off +
                      "*" + cyclic_2on + "*" + cyclic_2off +
                      "*" + cyclic_3on + "*" + cyclic_3off +
                      "*" + cyclic_4on + "*" + cyclic_4off +
                      "*" + cyclic_5on + "*" + cyclic_5off + "+");
    delay(2000);
    cy_time_flag = 0;
  }
}

void readservercyclic() {
  if (WiFi.status() == WL_CONNECTED) {
    String url = "/motorpro/motorvoltvpro/readCTR.php?";
    url += "deviceid="; url += deviceID;
    url += "&tokenid="; url += tokenID;

    HTTPClient http;
    http.begin(wifiClient, host + url);
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      payload1 = http.getString();
    } else {
      // Serial debug removed
    }
    http.end();
  }

  int a = payload1.indexOf('@');
  if (a == 16) {
    char buf[150];
    payload1.toCharArray(buf, sizeof(buf));

    String tokens[20];
    int idx = 0;
    char *c = strtok(buf, "*");
    while (c != NULL && idx < 20) {
      tokens[idx++] = String(c);
      c = strtok(NULL, "*");
    }

    cyclic_state = tokens[2];
    cyclic_1on   = tokens[3];  cyclic_1off = tokens[4];
    cyclic_2on   = tokens[5];  cyclic_2off = tokens[6];
    cyclic_3on   = tokens[7];  cyclic_3off = tokens[8];
    cyclic_4on   = tokens[9];  cyclic_4off = tokens[10];
    cyclic_5on   = tokens[11]; cyclic_5off = tokens[12];
  }
}

void readserversample() {
  if (WiFi.status() == WL_CONNECTED) {
    String url = "/motorpro/motorvoltvpro/sample.php?";
    url += "deviceid="; url += deviceID;
    url += "&tokenid="; url += tokenID;

    HTTPClient http;
    http.begin(wifiClient, host + url);
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      payload = http.getString();
    } else {
      // Serial debug removed
    }
    http.end();
  }
}

void readserver() {
  if (WiFi.status() == WL_CONNECTED) {
    String url = "/motorpro/motorvoltvpro/read.php?";
    url += "deviceid="; url += deviceID;
    url += "&tokenid="; url += tokenID;

    HTTPClient http;
    http.begin(wifiClient, host + url);
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      payload = http.getString();
    } else {
      // Serial debug removed
    }
    http.end();
  }

  int a = payload.indexOf('#');
  if (a == 16) {
    char buf[85];
    payload.toCharArray(buf, 85);

    char *p = strtok(buf, "*");
    String state0 = String(p);

    p = strtok(NULL, "*"); state    = String(p);
    p = strtok(NULL, "*"); drys     = String(p);
    p = strtok(NULL, "*"); drya     = String(p);
    p = strtok(NULL, "*"); powon    = String(p);
    p = strtok(NULL, "*"); timer_st = String(p);
    p = strtok(NULL, "*"); low_vr   = String(p);
    p = strtok(NULL, "*"); low_vy   = String(p);
    p = strtok(NULL, "*"); low_vb   = String(p);
    p = strtok(NULL, "*"); high_vr  = String(p);
    p = strtok(NULL, "*"); high_vy  = String(p);
    p = strtok(NULL, "*"); high_vb  = String(p);
    p = strtok(NULL, "*"); over_rc  = String(p);
    p = strtok(NULL, "*"); over_yc  = String(p);
    p = strtok(NULL, "*"); over_bc  = String(p);

    p = strtok(NULL, "*");
    if (p != NULL) {
      char tmp[4]; sprintf(tmp, "%02d", atoi(p)); on_delay = String(tmp);
    }
    p = strtok(NULL, "*");
    if (p != NULL) {
      char tmp[4]; sprintf(tmp, "%02d", atoi(p)); off_delay = String(tmp);
    }

    p = strtok(NULL, "*"); String phase3 = String(p);
    p = strtok(NULL, "*"); String phase4 = String(p);
    p = strtok(NULL, "*"); phase = String(p);

    p = strtok(NULL, "*");
    if (p != NULL) {
      char tmp[4]; sprintf(tmp, "%02d", atoi(p)); pw_ondly = String(tmp);
    }
    p = strtok(NULL, "*");
    if (p != NULL) {
      char tmp[4]; sprintf(tmp, "%03d", atoi(p)); dry_rst = String(tmp);
    }
  }

  if (state != statep || drys != dryp || drya != dryap || powon != powonp ||
      timer_st != ttimer_st || phase != tphase ||
      on_delay != on_delay1 || off_delay != off_delay1 ||
      dry_rst != dry_rst1 || pw_ondly != pw_ondly1) {

    setflag = 1;

    PicSerial.print('#');
    if (state == "") PicSerial.print("0");
    else             PicSerial.print(state);

    PicSerial.print('*');
    if (drys.toInt() < 10) { PicSerial.print("0"); PicSerial.print(drys); }
    else                      PicSerial.print(drys);

    PicSerial.print('*');
    if (drya.toInt() < 10) { PicSerial.print("0"); PicSerial.print(drya); }
    else                      PicSerial.print(drya);

    PicSerial.print('*');
    PicSerial.print(powon);
    PicSerial.print('*');
    PicSerial.print(timer_st);
    PicSerial.print('*');
    PicSerial.print(phase);    PicSerial.print('*');
    PicSerial.print(on_delay); PicSerial.print('*');
    PicSerial.print(off_delay);PicSerial.print('*');
    PicSerial.print(pw_ondly); PicSerial.print('*');
    PicSerial.print(dry_rst);  PicSerial.print('*');

    delay(1000);
    statep     = state;
    dryp       = drys;
    dryap      = drya;
    powonp     = powon;
    ttimer_st  = timer_st;
    tphase     = phase;
    on_delay1  = on_delay;
    off_delay1 = off_delay;
    dry_rst1   = dry_rst;
    pw_ondly1  = pw_ondly;
  }

  if (low_vr != tlow_vr || low_vy != tlow_vy || low_vb != tlow_vb ||
      high_vr != thigh_vr || high_vy != thigh_vy || high_vb != thigh_vb ||
      over_rc != tover_rc || over_yc != tover_yc || over_bc != tover_bc) {
    setflag = 2;
  }
}

void senddata_pic() {
  setflag = 1;

  PicSerial.print('#');
  if (state == "") PicSerial.print("0");
  else             PicSerial.print(state);

  PicSerial.print('*');
  if (drys.toInt() < 10) { PicSerial.print("0"); PicSerial.print(drys); }
  else                      PicSerial.print(drys);

  PicSerial.print('*');
  if (drya.toInt() < 10) { PicSerial.print("0"); PicSerial.print(drya); }
  else                      PicSerial.print(drya);

  PicSerial.print('*');
  PicSerial.print(powon);
  PicSerial.print('*');

  delay(1000);
  statep = state;
  dryp   = drys;
  dryap  = drya;
  powonp = powon;
}

void updatetoserver1() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "/motorpro/motorvoltvpro/update.php?";
    url += "deviceid=";    url += deviceID;
    url += "&tokenid=";    url += tokenID;
    url += "&state=";      url += state1;
    url += "&state_ack=";  url += stateack;
    url += "&current_r=";  url += drya1;
    url += "&current_y=";  url += drya2;
    url += "&current_b=";  url += drya3;
    url += "&dryrun_ack="; url += drya11;
    url += "&voltage_r=";  url += rv;
    url += "&voltage_y=";  url += yv;
    url += "&voltage_b=";  url += bv;
    url += "&pch_r=";      url += rr;
    url += "&pch_y=";      url += yr;
    url += "&pch_b=";      url += br;
    url += "&wlevel=";     url += level;

    http.begin(wifiClient, host + url);
    http.addHeader("Content-Type", "text/plain");
    http.POST("");
    http.end();
    for (int k = 0; k < 5; k++) delay(1000);
  }
}

void updatetoserver2() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "/motorpro/motorvoltvpro/update.php?";
    url += "deviceid=";   url += deviceID;
    url += "&tokenid=";   url += tokenID;
    url += "&current_r="; url += drya1;
    url += "&current_y="; url += drya2;
    url += "&current_b="; url += drya3;
    url += "&voltage_r="; url += rv;
    url += "&voltage_y="; url += yv;
    url += "&voltage_b="; url += bv;
    url += "&wlevel=";    url += level;

    http.begin(wifiClient, host + url);
    http.addHeader("Content-Type", "text/plain");
    http.POST("");
    http.end();
    delay(5000);
  }
}

void updatetoserver3() {
  fing = "1";
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "/motorpro/motorvoltvpro/update.php?";
    url += "deviceid=";  url += deviceID;
    url += "&tokenid=";  url += tokenID;
    url += "&SET_DATA="; url += fing;

    http.begin(wifiClient, host + url);
    http.addHeader("Content-Type", "text/plain");
    http.POST("");
    http.end();
    delay(5000);
  }
}

void updatetimer() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "/motorpro/motorvoltvpro/compare.php?";
    url += "deviceid="; url += deviceID;
    url += "&tokenid="; url += tokenID;

    http.begin(wifiClient, host + url);
    http.addHeader("Content-Type", "text/plain");
    http.POST("");
    http.end();
    delay(5000);
  }
}

void readack() {
  rec = PicSerial.readString();

  String ta = rec.substring(0, 1);

  if (ta == "$") {
    jflg   = 1;
    t1     = rec.substring(1, 2);
    t2     = rec.substring(3, 4);
    drya1  = rec.substring(5, 9);
    drya2  = rec.substring(9, 13);
    drya3  = rec.substring(13, 17);
    rv     = rec.substring(17, 20);
    yv     = rec.substring(20, 23);
    bv     = rec.substring(23, 26);
    vflag  = rec.substring(26, 27);
    vflaga = rec.substring(27, 28);
    rr     = rec.substring(28, 29);
    yr     = rec.substring(29, 30);
    br     = rec.substring(30, 31);

    if (t1 == "a") {
      state1 = "0"; stateack = "0"; state = "0"; drya11 = "0"; flagf2 = 1;
    }
    if (t1 == "A") {
      state = "1"; state1 = "1"; stateack = "1"; drya11 = "0";
    }
    if (t2 == "1") {
      state1 = "0"; stateack = "0"; state = "0"; drya11 = "1";
    }
    if (t2 == "2") {
      state1 = "0"; stateack = "0"; state = "0"; drya11 = "2";
    }
    if (vflaga == "1") {
      vflag1 = "0"; state1 = "0"; stateack = "0"; state = "0"; drya11 = "0";
      PicSerial.println("k5");
    }

    updatetoserver1();
    brflg = 1;
    digitalWrite(LED_PIN, LOW);  delay(200);
    digitalWrite(LED_PIN, HIGH); delay(200);
    digitalWrite(LED_PIN, LOW);  delay(200);
    digitalWrite(LED_PIN, HIGH); delay(200);
    digitalWrite(LED_PIN, LOW);  delay(200);
    digitalWrite(LED_PIN, HIGH);
    setflag = 0;
  }

  if (ta == "&") {
    jflg  = 1;
    drya1 = rec.substring(1, 5);
    drya2 = rec.substring(5, 9);
    drya3 = rec.substring(9, 13);
    rv    = rec.substring(13, 16);
    yv    = rec.substring(16, 19);
    bv    = rec.substring(19, 22);

    updatetoserver2();
    brflg = 1;
    digitalWrite(LED_PIN, LOW);  delay(500);
    digitalWrite(LED_PIN, HIGH);
  }

  if (ta == "#") {
    setflag = 1;
    PicSerial.println("%");
  }

  if (ta == "^") {
    cy_time_flag = 1;
    setflag = 1;
    PicSerial.println("^");
  }
}

void setup() {
  // ── PIC UART on pins 43(TX) / 44(RX) — no USB Serial debug ─────────────────
  PicSerial.begin(9600, SERIAL_8N1, PIC_RX_PIN, PIC_TX_PIN);

  pinMode(TRIGGER_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH); delay(1000);
  digitalWrite(LED_PIN, LOW);

  checkButton();
  loadParameters();

  wm.setSaveConfigCallback(saveConfigCallback);

  WiFiManagerParameter custom_deviceID("deviceID", "Device ID", deviceID, 40);
  WiFiManagerParameter custom_tokenID("tokenID",   "Token ID",  tokenID,  40);

  wm.addParameter(&custom_deviceID);
  wm.addParameter(&custom_tokenID);
  wm.setConfigPortalTimeout(300);
  wm.setShowInfoUpdate(false);
  wm.setShowInfoErase(false);
  std::vector<const char *> wm_menu = {"wifi", "exit"};
  wm.setMenu(wm_menu);
  wm.setTitle("NEW QBITRONICS");
  wm.setCustomHeadElement(customhead);

  if (WiFi.status() != WL_CONNECTED) {
    for (int i = 0; i < 4; i++) {
      digitalWrite(LED_PIN, HIGH); delay(500);
      digitalWrite(LED_PIN, LOW);  delay(500);
    }
    checkButton();
    wm.setConfigPortalTimeout(180);
    if (!wm.autoConnect("SMART CONFIG", "password")) {
      ESP.restart();
    }
    digitalWrite(LED_PIN, HIGH);
  }

  if (shouldSaveConfig) {
    strcpy(deviceID, custom_deviceID.getValue());
    strcpy(tokenID,  custom_tokenID.getValue());
    saveParameters();
    ssid = WiFi.SSID();
    pass = WiFi.psk();
    PicSerial.println(ssid);
    PicSerial.println(pass);
  }

  drya11 = "0";
  readserver();  // initial read from server

  tflag = 1;
  PicSerial.print("@");
  digitalWrite(LED_PIN, LOW);  delay(500);
  digitalWrite(LED_PIN, HIGH); delay(500);
  PicSerial.print("@");
}

void loop() {
  // ── WiFi watchdog ──────────────────────────────────────────────────────────
  if (WiFi.status() != WL_CONNECTED) {
    wm.setConfigPortalTimeout(30);
    digitalWrite(LED_PIN, LOW);
    if (!wm.autoConnect()) ESP.restart();
    digitalWrite(LED_PIN, HIGH); delay(500);
  }

  // ── Poll server & relay to PIC ─────────────────────────────────────────────
  readserversample();

  if (setflag == 0) {
    PicSerial.print("Q");
    delay(500);
    if (PicSerial.available()) readack();
  }

  readserver();

  if (setflag == 1) {
    PicSerial.print("@");
    while (PicSerial.available()) {
      readack();
      delay(2000);
      delay(2000);
      delay(2000);
    }
  } else if (setflag == 2) {
    checkprevent();
  }

  // ── Cyclic timer ───────────────────────────────────────────────────────────
  if (timer_st == "1") {
    readservercyclic();
    if (cyclic_state == "1") send_cyclic_time();
  }

  // ── Heartbeat / keep-alive ────────────────────────────────────────────────
  updatetimer();
  updatetoserver3();
  delay(100);
}
