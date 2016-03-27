// This #include statement was automatically added by the Particle IDE.
#include "ParticleFtpClient.h"
#include <time.h>

using namespace particleftpclient;

String hostname = "hostname";
String username = "username";
String password = "password";

int timeout = 5;

ParticleFtpClient ftp = ParticleFtpClient();

void readPasvStream() {
  int count = 0;
  while (ftp.data.connected()) {
    while (ftp.data.available()) {
      char c = ftp.data.read();
      count++;
      Serial.print(c);
    }
  }
}

void setup() {
    Serial.begin(9600);
    Serial.println("Hello world");

    // Connect to host and authenticate, or halt
    if (!ftp.open(hostname, timeout)) {
      Serial.println("Couldn't connect to ftp host");
      while (1);
    }
    if (!ftp.user(username)) {
      Serial.println("Bad username");
      while (1);
    }
    if (!ftp.pass(password)) {
      Serial.println("Bad password");
      while (1);
    }

    // Test LIST *
    if (!ftp.list("*")) {
      Serial.println("Unable to list files");
    } else {
      Serial.println("LIST *");
      readPasvStream();
    }

    // Test directory commands
    Serial.println("MKD ParticleFtpClient");
    if (!ftp.mkd("ParticleFtpClient")) {
      Serial.println("Couldn't make /ParticleFtpClient");
    } else if (!ftp.cwd("ParticleFtpClient")) {
      Serial.println("CWD ParticleFtpClient");
      Serial.println("Couldn't change directories to /ParticleFtpClient");
    } else {
      Serial.println("PWD (should be /ParticleFtpClient)");
      Serial.println(ftp.pwd());
      Serial.println("CDUP");
      if (!ftp.cdup()) {
        Serial.println("Couldnt' return to parent directory");
      } else {
        Serial.println("PWD (should be root)");
        Serial.println(ftp.pwd());
      }
    }

    // Test writing a file
    Serial.println("TYPE A (text file)");
    if (!ftp.type("A")) {
      Serial.println("Couldn't set file type");
    } else if (!ftp.stor("test.txt")) {
      Serial.println("Couldn't STOR test.txt");
    } else {
      Serial.println("STOR test.txt");
      for (int i = 0; i < 5; i++) {
        ftp.data.write('a' + i);
      }
      ftp.data.flush();
      if (!ftp.finish()) {
        Serial.println("Couldn't stop file upload");
      }
    }

    // Test retrieving the file
    Serial.println("RETR test.txt");
    if (!ftp.retr("test.txt")) {
      Serial.println("Couldn't retrieve test.txt");
    } else {
      readPasvStream();
      ftp.finish();
      // Because the file is so small, we've probably already a received
      // server response here, so this will timeout
    }

    // Test getting modified time of file
    Serial.println("MDTM test.txt");
    tm mdtm;
    if (!ftp.mdtm("test.txt", &mdtm)) {
      Serial.println("Couldn't retrieve modified date/time of test.txt");
    } else {
      Serial.print("Modified date/time: ");
      Serial.print(mdtm.tm_year);
      Serial.print("/");
      Serial.print(mdtm.tm_mon);
      Serial.print("/");
      Serial.print(mdtm.tm_mday);
      Serial.print(" ");
      Serial.print(mdtm.tm_hour);
      Serial.print(":");
      Serial.print(mdtm.tm_min);
      Serial.print(":");
      Serial.println(mdtm.tm_sec);
    }

    // Test deleting the file we made
    Serial.println("DELE test.txt");
    if (!ftp.dele("test.txt")) {
      Serial.println("Couldn't delete file");
    } else if (!ftp.list("test.txt")) {
      Serial.println("Couldn't retrieve directory listing of test.txt");
    } else {
      Serial.println("LIST test.txt (Should be empty)");
      readPasvStream();
    }

    // Test removing a directory
    Serial.println("RMD ParticleFtpClient");
    if (!ftp.rmd("ParticleFtpClient")) {
      Serial.println("Couldn't remove directory ParticleFtpClient");
    } else {
      Serial.println("Success!");
    }

    // Test a custom FTP command
    Serial.println("SYST");
    if (!ftp.simple_command("SYST", 200)) {
      Serial.println("That didn't work...");
    } else {
      Serial.println(ftp.get_response());
    }

    // Quit!
    Serial.println("QUIT");
    if (!ftp.quit()) {
      Serial.println("Couldn't quit FTP");
    } else {
      Serial.println("Goodbye!");
    }
}
