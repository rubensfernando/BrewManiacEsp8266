#include <Arduino.h>
#include <pgmspace.h>

#if defined(ESP32)
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WebServer.h>
#include "ESP32HTTPUpdateServer.h"
#else
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266mDNS.h>
#endif

#include <FS.h>
#include "config.h"

static  char* _username;
static  char* _password;

#if SerialDebug == true
#define DBG_PRINT(...) DebugPort.print(__VA_ARGS__)
#define DBG_PRINTLN(...) DebugPort.println(__VA_ARGS__)
#define DBG_PRINTF(...) DebugPort.printf(__VA_ARGS__)
#else
#define DBG_PRINT(...)
#define DBG_PRINTLN(...)
#define DBG_PRINTF(...) 
#endif

#if ESP32
static WebServer server(UPDATE_SERVER_PORT);
static ESP32HTTPUpdateServer httpUpdater;
#else
static ESP8266WebServer server(UPDATE_SERVER_PORT);
static ESP8266HTTPUpdateServer httpUpdater;
#endif
//holds the current upload
static File fsUploadFile;

extern FS& FileSystem;
#include "data_edit_html_gz.h"

const String getContentType(const String& filename){
  if(server.hasArg("download")) return "application/octet-stream";
  else if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

static bool handleFileRead(String path){
  DBG_PRINTLN("handleFileRead: " + path);
  if(_username != NULL && _password != NULL && !server.authenticate(_username, _password)){

 		server.requestAuthentication();
 		return false;
	}

  if(path.endsWith("/")) path += "index.htm";
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if(FileSystem.exists(pathWithGz) || FileSystem.exists(path)){
    if(FileSystem.exists(pathWithGz))
      path += ".gz";
    File file = FileSystem.open(path, "r");
    /*size_t sent = */server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

static void handleFileUpload(void){
 	if(_username != NULL && _password != NULL && !server.authenticate(_username, _password))
 		return server.requestAuthentication();

  if(server.uri() != "/edit") return;
  HTTPUpload& upload = server.upload();
  if(upload.status == UPLOAD_FILE_START){
    String filename = upload.filename;
    if(!filename.startsWith("/")) filename = "/"+filename;
    DBG_PRINT("handleFileUpload Name: "); DBG_PRINTLN(filename);
    fsUploadFile = FileSystem.open(filename, "w");
    filename = String();
  } else if(upload.status == UPLOAD_FILE_WRITE){
    //DBG_PRINT("handleFileUpload Data: "); DBG_PRINTLN(upload.currentSize);
    if(fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize);
  } else if(upload.status == UPLOAD_FILE_END){
    if(fsUploadFile)
      fsUploadFile.close();
    DBG_PRINT("handleFileUpload Size: "); DBG_PRINTLN(upload.totalSize);
  }
}

static void handleFileDelete(void){
 	if(_username != NULL && _password != NULL && !server.authenticate(_username, _password))
 		return server.requestAuthentication();

  if(server.args() == 0) return server.send(500, "text/plain", "BAD ARGS");
  String path = server.arg(0);
  DBG_PRINTLN("handleFileDelete: " + path);
  if(path == "/")
    return server.send(500, "text/plain", "BAD PATH");
  if(!FileSystem.exists(path))
    return server.send(404, "text/plain", "FileNotFound");
  FileSystem.remove(path);
  server.send(200, "text/plain", "");
  path = String();
}

static void handleFileCreate(void){
	if(_username != NULL && _password != NULL && !server.authenticate(_username, _password))
 		return server.requestAuthentication();

  if(server.args() == 0)
    return server.send(500, "text/plain", "BAD ARGS");
  String path = server.arg(0);
  DBG_PRINTLN("handleFileCreate: " + path);
  if(path == "/")
    return server.send(500, "text/plain", "BAD PATH");
  if(FileSystem.exists(path))
    return server.send(500, "text/plain", "FILE EXISTS");
  File file = FileSystem.open(path, "w");
  if(file)
    file.close();
  else
    return server.send(500, "text/plain", "CREATE FAILED");
  server.send(200, "text/plain", "");
  path = String();
}
static void handleFileList(void) {
  if(!server.hasArg("dir")) {server.send(500, "text/plain", "BAD ARGS"); return;}

  String path = server.arg("dir");
  DBG_PRINTLN("handleFileList: " + path);
  // linked list(queue) is needed. 
  // avoid recursive call, which might open too many directories 
  #if ESP32
  File dir = FileSystem.open(path);  
  #else
  Dir dir = FileSystem.openDir(path);
  #endif
 
  DBG_PRINTF("open failed? %d\n",dir? 1:0);


  String output = "[";
  #if ESP32

  File entry = dir.openNextFile();
  while(entry){

  #else
  while(dir.next()){
    File entry = dir.openFile("r");
  #endif
    if (output != "[") output += ',';
    #if UseLittleFS
    bool isDir = entry.isDirectory();
    #else
    bool isDir = false;
    #endif

    DBG_PRINTF(" %s [%s]\n ", entry.name(),isDir? "DIR":"FILE");

    output += "{\"type\":\"";
    output += (isDir)? "dir":"file";
    output += "\",\"name\":\"";

    //DBG_PRINTF("%s, %s, cmp:%d, ", entry.name(),path.c_str(), strncmp(entry.name(),path.c_str(),path.length()));
    String name=(entry.name());
    if(name.startsWith(path)){
      
      int exlen =path.length();  
      if(path.charAt(exlen - 1) != '/') exlen ++;
      output += name.substring(exlen);
    }else{
       if(entry.name()[0] == '/'){
        output += name.substring(1);
       }else{
        output += name;
       }
    }
    output += "\"}";
    entry.close();  
  #if defined(ESP32)
    entry = dir.openNextFile();
  #endif

  }

  output += "]";
  DBG_PRINTF(" ret: %s \n",output.c_str());
  server.send(200, "text/json", output);
}

#if 0
static void handleFileList(void) {
 	if(_username != NULL && _password != NULL && !server.authenticate(_username, _password))
 		return server.requestAuthentication();

  if(!server.hasArg("dir")) {server.send(500, "text/plain", "BAD ARGS"); return;}

  String path = server.arg("dir");
  DBG_PRINTLN("handleFileList: " + path);
  // linked list(queue) is needed. 
  // avoid recursive call, which might open too many directories 
  #if ESP32
  File dir = FileSystem.open(path);
  #else
  Dir dir = FileSystem.openDir(path);
  #endif
  path = String();

  String output = "[";
  #if ESP32

  File entry = dir.openNextFile();
  while(entry){

  #else
  while(dir.next()){
    File entry = dir.openFile("r");
  #endif
    if (output != "[") output += ',';
    #if UseLittleFS
    bool isDir = dir.isDirectory();
    #else
    bool isDir = false;
    #endif
    output += "{\"type\":\"";
    output += (isDir)?"dir":"file";
    output += "\",\"name\":\"";
    #if UseLittleFS
    output += entry.name();
    #else
    output += String(entry.name()).substring(1);
    #endif
    output += "\"}";
    entry.close();  
  #if defined(ESP32)
    entry = dir.openNextFile();
  #endif

  }

  output += "]";
  server.send(200, "text/json", output);
}
#endif

void ESPUpdateServer_setup(const char* user, const char* pass){

  //SERVER INIT
  //list directory
  _username=(char*)user;
  _password=(char*)pass;
  server.on("/list", HTTP_GET, handleFileList);
  //load editor
  server.on(FILE_MANAGEMENT_PATH, HTTP_GET, [](){
      if(_username != NULL && _password != NULL && !server.authenticate(_username, _password))
 		  return server.requestAuthentication();
//      if(!handleFileRead("edit.htm")){
  	    server.sendHeader("Content-Encoding", "gzip");
	      server.send_P(200,"text/html",edit_htm_gz,edit_htm_gz_len);
//      }
  });
  //create file
  server.on("/edit", HTTP_PUT, handleFileCreate);
  //delete file
  server.on("/edit", HTTP_DELETE, handleFileDelete);
  //first callback is called after the request has ended with all parsed arguments
  //second callback handles file uploads at that location
  server.on("/edit", HTTP_POST, [](){ server.send(200, "text/plain", ""); }, handleFileUpload);

  //called when the url is not defined here
  //use it to load content from file system
  server.onNotFound([](){
    if(!handleFileRead(server.uri()))
      server.send(404, "text/plain", "FileNotFound");
  });


 // Flash update server
#if ESP32
  httpUpdater.setup(server,SYSTEM_UPDATE_PATH,user,pass);
#else
	httpUpdater.setup(&server,SYSTEM_UPDATE_PATH,user,pass);
#endif

  server.begin();
  DBG_PRINTLN("HTTP Update server started\n");

}


void ESPUpdateServer_loop(void){
  server.handleClient();
}
