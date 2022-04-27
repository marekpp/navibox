/*
 * This arduino code maps physical buttons from a keypad to bluetooth keyboard commands
 * Intended to make using your phone for navigation on a motorcycle easier
 * More info at https://jaxeadv.com/gear-gadgets/navigation-setup/handlebar-buttons/
 *
 * - The basic idea is that the keys are in a 'matrix keypad' numbered 1..9.' These
 *   are mapped to actual keys
 * - You can have multiple keymaps and switch with the top rightbutton (currently removed, because I did not use it)
 * - keys can have an option for long press
 * 
 * 
 * 
 * How to build for Wemos C3 mini:
 * - Install https://github.com/T-vK/ESP32-BLE-Keyboard 0.3.1-beta or higher. Don't use NimBLE support since this did not work for me
 * - Install keypad library
 * - Update ESP32 board library to at least 2.0.3rc1 to support C3 - add https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_dev_index.json to additional board url in preferences
 * - Choose LOLIN C3 mini board in IDE
 * - Update partition scheme in Userdirectory: AppData\Local\Arduino15\packages\esp32\hardware\esp32\2.0.3-RC1\boards.txt to add an option for minimal spiffs to accomodate larger images by adding these lines:
 *     lolin_c3_mini.menu.PartitionScheme.min_spiffs=Minimal SPIFFS (1.9MB APP with OTA/190KB SPIFFS) 
 *     lolin_c3_mini.menu.PartitionScheme.min_spiffs.build.partitions=min_spiffs
 *     lolin_c3_mini.menu.PartitionScheme.min_spiffs.upload.maximum_size=1966080
 * - Set partition scheme to 'Minimal SPIFFS' (after restart of IDE)
 */


 
#include <Keypad.h>

// For ESP32 Bluetooth keyboard HID https://github.com/T-vK/ESP32-BLE-Keyboard
#include <BleKeyboard.h> 

// Set up the bleKeyboard instance
BleKeyboard bleKeyboard("NaviControl", "Joost Bijl", 100);

// Debugging flag. Mostly controls if serial output is enabled. 
// On the C3, the serial output blocks the device unless the serial monitor is attached
const int DEBUG = 0;

// Keypad library set up
////////////////////////

// This needs to be aligned with how you wired the buttons exactly. 
// see https://www.circuitbasics.com/how-to-set-up-a-keypad-on-an-arduino/ for an intro into the keypad library
const byte ROWS = 3; 
const byte COLS = 3; 

// for this project we'll number the keys, and use their value to 'press' the correct button
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'}
};


// pin assignments (https://randomnerdtutorials.com/esp32-pinout-reference-gpios/)
// IMPORTANT: choose pins that are actually broken out on your board, otherwise the ESP won't boot
//////////////////

// Signal led for status on handlebar, and flash on keymap change
const int LED_PIN = 6;  

// pins for keypad library, these need to be aligned with how you wired the buttons exactly. 
byte rowPins[COLS] = {2, 1, 0};  // top to bottom
byte colPins[ROWS] = {3, 4, 5};  // left to right


// For OTA updates
#include <WiFi.h>
#include <Update.h>
const char* SSID = "navi-box";
const char* PSWD = "navi-box";
String host = "jaxeadv.com"; 
int port = 80; 
String bin = "/navibox-firmware/latest.bin"; // bin file name with a slash in front.

// Initialization
/////////////////

// Initialize keypad library
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

int last_keypad_state = IDLE; // Used to distinguish between button release from HOLD or PRESSED.

// Initialize variable that determines how long a key is pressed to the current time in milliseconds
unsigned long hold_time = millis();

// define time for a long press (ms)
const int long_press_time = 440;
const int long_press_repeat_interval = 160; 

int app_status = 0;       // holds the current status 
int led_delays[4][2] = {  // holds the on/off pattern/times for the different status'es
  { 500,  500  },  // Not connected to BT
  { 5000, 100  },  // Connected
  { 500,  1500 },  // Connected & gopro is online    (not implemented yet)
  { 10,   3000 },  // Connected & gopro is recording (not implemented yet)
};

int led_state = 0;       // holds the state of the led (0=LOW, 1=HIGH)
int led_state_time = 0;  // holds the time we've switched to the current led_state


// These keys will be repeated (and also sent on initial press of the button). 
// Other keys will have an alternate function on long press
char repeating_keys[10] = { '1', '4', '2', '5', '6', '8' }; 

/*
 * The 2 functions below determine what key gets sent. 
 * 
 * See here for special keys:
 * - https://github.com/T-vK/ESP32-BLE-Keyboard/blob/master/BleKeyboard.h
 * - https://www.arduino.cc/en/Reference/KeyboardModifiers
 *
 * Candidates are:
 * NUM_PLUS / NUM_MINUS    
 * KEY_MEDIA_VOLUME_UP / KEY_MEDIA_VOLUME_DOWN 
 * KEY_UP_ARROW / KEY_DOWN_ARROW / KEY_LEFT_ARROW / KEY_RIGHT_ARROW
 * KEY_MEDIA_PLAY_PAUSE
 */


// Routine to send the keystrokes on a short press of the keypad
void send_short_press(KeypadEvent key) {
  
  if(DEBUG) {
    Serial.print("Sending short press key ");
    Serial.println(key);
  }
  
  switch(key) {
    case '7': bleKeyboard.write('c'); flash_led(); break;
    case '3': bleKeyboard.write('n'); flash_led(); break;
    
    case '1': bleKeyboard.write(KEY_MEDIA_VOLUME_UP); flash_led(); break;
    case '4': bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN); flash_led(); break;   
    
    case '2': bleKeyboard.write(KEY_UP_ARROW); flash_led(); break;
    case '8': bleKeyboard.write(KEY_DOWN_ARROW); flash_led(); break;
    case '6': bleKeyboard.write(KEY_RIGHT_ARROW); flash_led(); break;
    case '5': bleKeyboard.write(KEY_LEFT_ARROW); flash_led(); break;
    
  }
  
}

// Routine to send the keystrokes on a long press of the keypad
void send_long_press(KeypadEvent key) {
  
  if(DEBUG) {
    Serial.print("Sending long press key for button ");
    Serial.println(key);
  }
  
  switch(key) {
    case '7': bleKeyboard.write('d'); flash_led(); break;
    case '3': bleKeyboard.write('x'); update_navibox_firmware(); flash_led(); break; 
    
    case '1': send_repeating_key(KEY_MEDIA_VOLUME_UP); break;
    case '4': send_repeating_key(KEY_MEDIA_VOLUME_DOWN); break;
    
    case '2': send_repeating_key(KEY_UP_ARROW); break;
    case '8': send_repeating_key(KEY_DOWN_ARROW); break;
    case '6': send_repeating_key(KEY_RIGHT_ARROW); break;
    case '5': send_repeating_key(KEY_LEFT_ARROW); break;
    
  }
}

// Routine that sends a key repeatedly (for single char)
void send_repeating_key(uint8_t key) {
  
  digitalWrite(LED_PIN, HIGH);
  while(keypad.getState() == HOLD) {  
    //Serial.print('.');
    bleKeyboard.write(key);
    delay(long_press_repeat_interval); // pause between presses
    keypad.getKey(); // update keypad event handler
  }
  digitalWrite(LED_PIN, LOW);
  //Serial.println(' ');
}

// Routine that sends a key repeatedly (for double char 'MediaKeyReport')
void send_repeating_key(const MediaKeyReport key) {

  digitalWrite(LED_PIN, HIGH);
  while(keypad.getState() == HOLD) {  
    //Serial.print('.');
    bleKeyboard.write(key);
    delay(long_press_repeat_interval); // pause between presses
    keypad.getKey(); // update keypad event handler
  }
  digitalWrite(LED_PIN, LOW);
  //Serial.println(' ');
}


// This function handles events from the keypad.h library. Each event is only called once
void keypad_handler(KeypadEvent key){

  // State changes for the buttons
  switch(keypad.getState()) {

    case PRESSED: // Directly on the press of a button
      if(DEBUG) {
        Serial.println("keypad.getState = PRESSED");
      }
      
      if (is_key_repeating(key)){
          send_short_press(key); 
      }
      break;
      
    case HOLD: // When a button is held beyond the long_press_time value
      if(DEBUG) { Serial.println("keypad.getState = HOLD"); }
      send_long_press(key);
      break;

    case RELEASED: // When a button is released 
      if(DEBUG) {
        Serial.println("keypad.getState = RELEASED");
        Serial.print("Previous state: ");
        Serial.println(last_keypad_state);
      }

      // 
      if (last_keypad_state == PRESSED) {
        if (!is_key_repeating(key)){
          send_short_press(key); 
        }
      }

      
      break;
      
  case IDLE: // not sure why this generates a callback
      if(DEBUG) { Serial.println("keypad.getState = IDLE"); }
      break;
  }

  last_keypad_state = keypad.getState();
}


// Arduino built-in setup loop
void setup(){
  if(DEBUG) { Serial.begin(9600); } 
  
  // Handle all keypad events through this listener 
  keypad.addEventListener(keypad_handler); // Add an event listener for this keypad

  // set HoldTime  
  keypad.setHoldTime(long_press_time);
  
  // Start bluetooth keyboard module
  bleKeyboard.begin();

  // Enable the led to indicate we're switched on
  pinMode(LED_PIN, OUTPUT);

  // gopro temp
  //gp.enableDebug(&Serial);


  // End of setup()
  if(DEBUG) { Serial.println("Good to go!"); }
}

void loop(){

  // Need to call this function constantly to make the keypad library work 
  keypad.getKey();

  // influence led state based on BT connectivity
  if(app_status == 0 && bleKeyboard.isConnected())  { app_status = 1; }
  if(app_status == 1 && !bleKeyboard.isConnected()) { app_status = 0; }

  // toggle between on/off for the led
  if ((millis() - led_state_time) > led_delays[app_status][led_state]) {
    led_state = 1 - led_state; // Toggle between 0 and 1
    digitalWrite(LED_PIN, led_state); // Update the led
    led_state_time = millis(); // update last switch
  }
  
  delay(10);
}

// Quick flash of the led
void flash_led(){
  digitalWrite(LED_PIN, HIGH);
  delay(150);
  digitalWrite(LED_PIN, LOW);
  
}

// Helper function to determine if a key is supposed to be repeating, or special
int is_key_repeating(char key_pressed) {

  for( int i=0; i< sizeof (repeating_keys); i++) {
    if ( repeating_keys[i] == key_pressed) {
      return true;
    }
  }
  return false;
}


void update_navibox_firmware() {

  WiFiClient client;

  // Variables to validate
  // response from S3
  long contentLength = 0;
  bool isValidContentType = false;
  
  if(DEBUG) { Serial.println("Connecting to " + String(SSID)); }

  // Connect to provided SSID and PSWD
  WiFi.begin(SSID, PSWD);

  // Wait for connection to establish
  while (WiFi.status() != WL_CONNECTED) {
    if(DEBUG) { Serial.print("."); } // Keep the serial monitor lit! 
    digitalWrite(LED_PIN, HIGH);
    delay(250);
    digitalWrite(LED_PIN, LOW);
    delay(250);
  }
  digitalWrite(LED_PIN, HIGH);

  // Connection Succeed
  if(DEBUG) {
    Serial.println("");
    Serial.println("Connected to " + String(SSID));
  }

  // Execute OTA Update
  if(DEBUG) { Serial.println("Connecting to: " + String(host)); }
  
  if (client.connect(host.c_str(), port)) {
    // Connection Succeed.
    // Fecthing the bin
    if(DEBUG) { Serial.println("Fetching Bin: " + String(bin)); }

    // Get the contents of the bin file
    client.print(String("GET ") + bin + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Cache-Control: no-cache\r\n" +
                 "Connection: close\r\n\r\n");

    // Check what is being sent
    //    Serial.print(String("GET ") + bin + " HTTP/1.1\r\n" +
    //                 "Host: " + host + "\r\n" +
    //                 "Cache-Control: no-cache\r\n" +
    //                 "Connection: close\r\n\r\n");

    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        if(DEBUG) { Serial.println("Client Timeout !"); }
        client.stop();
        return;
      }
    }
    // Once the response is available,
    // check stuff

    /*
       Response Structure
        HTTP/1.1 200 OK
        x-amz-id-2: NVKxnU1aIQMmpGKhSwpCBh8y2JPbak18QLIfE+OiUDOos+7UftZKjtCFqrwsGOZRN5Zee0jpTd0=
        x-amz-request-id: 2D56B47560B764EC
        Date: Wed, 14 Jun 2017 03:33:59 GMT
        Last-Modified: Fri, 02 Jun 2017 14:50:11 GMT
        ETag: "d2afebbaaebc38cd669ce36727152af9"
        Accept-Ranges: bytes
        Content-Type: application/octet-stream
        Content-Length: 357280
        Server: AmazonS3
                                   
        {{BIN FILE CONTENTS}}

    */
    
    while (client.available()) {
      // read line till /n
      String line = client.readStringUntil('\n');
      // remove space, to check if the line is end of headers
      line.trim();

      // if the the line is empty,
      // this is end of headers
      // break the while and feed the
      // remaining `client` to the
      // Update.writeStream();
      if (!line.length()) {
        //headers ended
        break; // and get the OTA started
      }

      // Check if the HTTP Response is 200
      // else break and Exit Update
      if (line.startsWith("HTTP/1.1")) {
        if (line.indexOf("200") < 0) {
          if(DEBUG) { Serial.println("Got a non 200 status code from server. Exiting OTA Update."); }
          break;
        }
      }

      // extract headers here
      // Start with content length
      if (line.startsWith("Content-Length: ")) {
        contentLength = atol((getHeaderValue(line, "Content-Length: ")).c_str());
        if(DEBUG) { Serial.println("Got " + String(contentLength) + " bytes from server"); }
      }

      // Next, the content type
      if (line.startsWith("Content-Type: ")) {
        String contentType = getHeaderValue(line, "Content-Type: ");
        if(DEBUG) { Serial.println("Got " + contentType + " payload."); }
        if (contentType == "application/octet-stream") {
          isValidContentType = true;
        }
      }
    }
  } else {
    // Connect to S3 failed
    // May be try?
    // Probably a choppy network?
    if(DEBUG) { Serial.println("Connection to " + String(host) + " failed. Please check your setup"); }
    // retry??
    // execOTA();
  }

  // Check what is the contentLength and if content type is `application/octet-stream`
  if(DEBUG) { Serial.println("contentLength : " + String(contentLength) + ", isValidContentType : " + String(isValidContentType)); } 

  // check contentLength and content type
  if (contentLength && isValidContentType) {
    // Check if there is enough to OTA Update
    bool canBegin = Update.begin(contentLength);

    // If yes, begin
    if (canBegin) {
      if(DEBUG) { Serial.println("Begin OTA. This may take 2 - 5 mins to complete. Things might be quite for a while.. Patience!"); } 
      // No activity would appear on the Serial monitor
      // So be patient. This may take 2 - 5mins to complete
      size_t written = Update.writeStream(client);

      if (written == contentLength) {
        if(DEBUG) { Serial.println("Written : " + String(written) + " successfully"); }
      } else {
        if(DEBUG) { Serial.println("Written only : " + String(written) + "/" + String(contentLength) + ". Retry?" ); }
        // retry??
        // execOTA();
      }

      if (Update.end()) {
        if(DEBUG) { Serial.println("OTA done!"); }
        if (Update.isFinished()) {
          if(DEBUG) { Serial.println("Update successfully completed. Rebooting."); }
          ESP.restart();
        } else {
          if(DEBUG) { Serial.println("Update not finished? Something went wrong!"); }
        }
      } else {
        if(DEBUG) { Serial.println("Error Occurred. Error #: " + String(Update.getError())); }
      }
    } else {
      // not enough space to begin OTA
      // Understand the partitions and
      // space availability
      if(DEBUG) { Serial.println("Not enough space to begin OTA"); }
      client.flush();
    }
  } else {
    if(DEBUG) { Serial.println("There was no content in the response"); }
    client.flush();
  } 
}

// Utility to extract header value from headers
String getHeaderValue(String header, String headerName) {
  return header.substring(strlen(headerName.c_str()));
}
