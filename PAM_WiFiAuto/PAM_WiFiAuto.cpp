//
// Include the PAM_WiFiAuto header
//
// created by PI and more, piandmore@j-tools.net
//
#include <PAM_WiFiAuto.h>
//
// Include the PAM_Tools library for the file system functions
//
#include <PAM_Tools.h>
//
// Include the PAM_WiFi library for the wifi functions
//
#include <PAM_WiFi.h>
//
// Include the ESP8266WiFi library for the ESP wifi functions
//
#include <ESP8266WiFi.h>

//
// wifiConnectGetPW allows you to connect to a wifi by providing just the
// ssid. The password is retrieved from the file system.
//
void wifiConnectGetPW (String ssid) {
  //
  // The password is located in the folder wifi
  //
  String password = getStringKey("wifi",ssid);
  if (password!=NOKEYFOUND) {
    wifiConnect(ssid,password);
  } else {
    WiFi.disconnect();
  }
}

//
// The wifiAutoConnect function allows you to have your device connect to an
// available wifi point that it knows the password to. The way it does that is
// by having all the passwords of your available wifi ssid's stored in the file system,
// more specifically in the folder called wifi. Each file in the folder wifi, will have the
// ssid name as it's file name followed by the extension .txt. The content of the file will be
// one string containing the password for that ssid.
// wifi/myssid1.txt will contain for example mypassword1
// wifi.myssid2.txt will contain for example mypassword2
//
// What wifiAutoConnect will do is get a list of all available wifi ssid's and cycle through
// each one to see if there is an associated password know. If there is, it connects to
// that network. If there isn't, it moves on to the next.
//
// There are two special cases. If you have a preffered network, your home one for example,
// you can put the name of the ssid in the file called wifi/1.txt. The next preferred 
// network will be in 2.txt etc. wifiAutoConnect will try to connect to those first.
// If no network is found matching any of the credentials, it will connect to the network
// in wifi/0.txt which could be a hidden ssid.
//
// Please note that wifiAutoConnect will only see broadcasted ssid's and only as a final
// resort connect to the ssid in 0.txt.
//
void wifiAutoConnect () {
  byte j = 1;
  String found;
  //
  // First check if there are preferred networks. Please note that it assumes a consecutive
  // list of files, so 1.txt, 2.txt, 3.txt etc. If 2.txt for example is missing, the loop
  // stops and will not reach 3.txt
  //
  do {
    found = getStringKey("wifi",String(j));
    if (found!=NOKEYFOUND) {
      int n = WiFi.scanNetworks();
      if (n>0) {
        for (int i=0;i<n;i++) {
          if (WiFi.SSID(i)==found) {
            //
            // One of the preferred networks is found. Connect to it by getting the password
            // and exit the function.
            //
            wifiConnectGetPW(WiFi.SSID(i));
            return;
          }
        }
      }
      j++;
    }
  } while (found!=NOKEYFOUND);
  int n = WiFi.scanNetworks();
  if (n>0) {
    for (int i=0;i<n;i++) {
      String pw = getStringKey("wifi",WiFi.SSID(i));
      if (pw!=NOKEYFOUND) {
        //
        // A wifi network has been found for which we have a password. Connect to it and
        // exit the function.
        wifiConnect(WiFi.SSID(i),pw);
        return;
      }
    }
  }
  found = getStringKey("wifi",String(0));
  if (found!=NOKEYFOUND) {
    //
    // Not connected to any network that has a visible ssid. Now try to connect to a
    // predefined, possibly hidden, ssid
    //
    wifiConnectGetPW(found);
  }
}
