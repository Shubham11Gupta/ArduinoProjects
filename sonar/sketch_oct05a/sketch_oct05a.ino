    #include <TinyGPS++.h>
    #include <GSM.h>//gsm library
    
    #include <MySQL_Connection.h>
    #include <MySQL_Cursor.h>
    
    #include <SoftwareSerial.h>
    #include <DNSServer.h>
    #include <WiFiManager.h>
    #include <DHT.h>
    
    #define DHTTYPE DHT11
    #define dht_apin D1 // Analog Pin sensor is connected to
    
    #define DHTTYPE DHT11 
    DHT dht(dht_apin, DHTTYPE);
    
    // Setting up variables
    int pm1_in, pm25_in, pm10_in, pm1_out, pm25_out, pm10_out;
    int time_delay;
    int sensor_sysid;
    int temperature, humidity,temp1,humi1;
    float lati ,lon;
    String lat_str, lng_str;
    
    TinyGPSPlus gps;
        
    SoftwareSerial ss(D3,4);
    SoftwareSerial pmsSerial1(D5, 3);
    SoftwareSerial pmsSerial2(D8, 3);
    
    //WiFiClient client;
    MySQL_Connection conn((Client *)&client);
    MySQL_Cursor* cursor;
    
    char INSERT_SQL_godaddy[] = "INSERT INTO cityfilter.cf_sensor_values(sensor_sysid, temperature, humidity, pm1_in, pm25_in, pm10_in, pm1_out, pm25_out, pm10_out, latitude, longitude) VALUES (%d, %d, %d, %d, %d, %d, %d, %d, %d, %.3f, %.3f)";
    
    char query[4000];
    
    //Primary Domain  cityfilters.site and cityfilters.com
    //Shared IP Address 166.62.28.116 for cityfilters.site godaddy
    //Public IP Address 35.247.138.105 for cityfilters.com gcp
    
    IPAddress server_addr_godaddy(148, 66, 137, 20);   // MySQL server IP for goDaddy
    
    char db_user[] = "singhrahulsp";                // MySQL user
    char db_password[] = "Gndc@123";       // MySQL password

    // PIN Number
    #define PINNUMBER ""
    
    // APN data
    #define GPRS_APN       "GPRS_APN" // replace your GPRS APN
    #define GPRS_LOGIN     ""    // GRRS default login
    #define GPRS_PASSWORD  "" // GPRS default password
    void setup()
    {
      Serial.begin(115200);
    
      dht.begin();
      pmsSerial1.begin(9600);
      pmsSerial2.begin(9600);
      ss.begin(9600);
    /*
      WiFiManager wifiManager;
      wifiManager.setTimeout(60);
    
      if(!wifiManager.autoConnect("CityFilter")) {
        Serial.println("failed to connect and hit timeout");
        delay(3000);
        //reset and try again, or maybe put it to deep sleep
        ESP.reset();
        delay(5000);
      }
    
    
      Serial.print("Connecting to ");
    
      while (WiFi.status() != WL_CONNECTED)
      {
        delay(500);
        Serial.print(".");
      }
      
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
    */
    
      sensor_sysid = ESP.getChipId();
    
    // connection to internet using GSM module
      Serial.println("Connecting to internet over gsm module");
      boolean notConnected = true;
      while (notConnected) {
        if ((gsmAccess.begin(PINNUMBER) == GSM_READY) &
            (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY)) {
          notConnected = false;
        } else {
          Serial.println("Not connected");
          delay(1000);
        }
      }
      
      Serial.println("Connected to internet over GSM module");
      Serial.println("Connecting to mysql server");
      
      while (conn.connect(server_addr_godaddy, 3306, db_user, db_password) != true) 
      {
        delay(200);
        Serial.print (".");
      }
    
      Serial.println("");
      Serial.println("Connected to mysql server!");
    }
    
    struct pms5003data {
      uint16_t framelen;
      uint16_t pm10_standard, pm25_standard, pm100_standard;
      uint16_t pm10_env, pm25_env, pm100_env;
      uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
      uint16_t unused;
      uint16_t checksum;
    };
     
    struct pms5003data data;
    
    void loop()
    {
    /*
     if (WiFi.status() != WL_CONNECTED)
     {
      
      WiFiManager wifiManager;
      wifiManager.setTimeout(60);
      
      if(!wifiManager.autoConnect("CityFilter"))
      {
        Serial.println("failed to connect and hit timeout");
        delay(3000);
        //reset and try again, or maybe put it to deep sleep
        ESP.reset();
        delay(5000);
      }
     }*/
    
           temp1 = dht.readTemperature();
           humi1 = dht.readHumidity();
    
     if (temp1 && humi1 <=100)
     {
     temperature=temp1;
     humidity=humi1;
     }
    
    if (readPMSdata1(&pmsSerial1)){
    
        pm1_in = data.pm10_env;
        pm25_in = data.pm25_env;
        pm10_in = data.pm100_env;
      }
      
    if (readPMSdata2(&pmsSerial2)){
    
        pm1_out = data.pm10_env;
        pm25_out = data.pm25_env;
        pm10_out = data.pm100_env;
      }
    
     if (ss.available()){
        gps.encode(ss.read());
        if (gps.location.isValid()){
    
            lati = gps.location.lat();
            lon = gps.location.lng();
        }
      }
      
      time_delay++;
    
      if (time_delay>=150000) {
      
      sprintf(query, INSERT_SQL_godaddy, sensor_sysid, temperature, humidity, pm1_in, pm25_in, pm10_in, pm1_out, pm25_out, pm10_out, lati, lon);
      Serial.println("Recording data for GoDaddy");
      
      Serial.println(query);
      
      MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
      cur_mem->execute(query);
      delete cur_mem;
      time_delay=0;
      }
    }
    
    boolean readPMSdata1(Stream *s) {
      if (! s->available()) {
        return false;
      }
      
      // Read a byte at a time until we get to the special '0x42' start-byte
      if (s->peek() != 0x42) {
        s->read();
        return false;
      }
     
      // Now read all 32 bytes
      if (s->available() < 32) {
        return false;
      }
        
      uint8_t buffer[64];    
      uint16_t sum = 0;
      s->readBytes(buffer, 64);
     
      // get checksum ready
      for (uint8_t i=0; i<30; i++) {
        sum += buffer[i];
      }
    
       // The data comes in endian'd, this solves it so it works on all platforms
      uint16_t buffer_u16[15];
      for (uint8_t i=0; i<15; i++) {
        buffer_u16[i] = buffer[2 + i*2 + 1];
        buffer_u16[i] += (buffer[2 + i*2] << 8);
      }
     
      // put it into a nice struct :)
      memcpy((void *)&data, (void *)buffer_u16, 30);
     
      if (sum != data.checksum) {
        Serial.println("Checksum failure");
        return false;
      }
      // success!
      return true;
    }
    
    boolean readPMSdata2(Stream *s) {
      if (! s->available()) {
        return false;
      }
      
      // Read a byte at a time until we get to the special '0x42' start-byte
      if (s->peek() != 0x42) {
        s->read();
        return false;
      }
     
      // Now read all 32 bytes
      if (s->available() < 32) {
        return false;
      }
        
      uint8_t buffer[64];    
      uint16_t sum2 = 0;
      s->readBytes(buffer, 64);
     
      // get checksum ready
      for (uint8_t i=0; i<30; i++) {
        sum2 += buffer[i];
      }
    
       // The data comes in endian'd, this solves it so it works on all platforms
      uint16_t buffer_u16[15];
      for (uint8_t i=0; i<15; i++) {
        buffer_u16[i] = buffer[2 + i*2 + 1];
        buffer_u16[i] += (buffer[2 + i*2] << 8);
      }
     
      // put it into a nice struct :)
      memcpy((void *)&data, (void *)buffer_u16, 30);
     
      if (sum2 != data.checksum) {
        Serial.println("Checksum failure");
        return false;
      }
      // success!
      return true;
    }
