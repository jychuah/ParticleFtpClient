// This #include statement was automatically added by the Particle IDE.
#include "ParticleFtpClient.h"

ParticleFtpClient ftp = ParticleFtpClient();

void patientStream() {
  // Stream data, allowing for a timeout of 3 seconds between
  // continuity of the stream
  int timeouts = 0;
  int count = 0;
  while(timeouts < 3) {
    delay(1000);
    if (ftp.data.connected() && ftp.data.available()) {
      timeouts = 0;
      while (ftp.data.available()) {
        char c = ftp.data.read();
        count++;
        Serial.print(c);

        // abort data stream after 1k of data
        if (count > 1000) {
          ftp.abor();
          return;
        }
      }
    } else {
      timeouts++;
    }
  }
}

void setup() {
    Serial.begin(9600);
    Serial.println("Hello world");

    // Connect to host and authenticate, or halt
    if (!ftp.open("192.168.1.102", 5)) {
      Serial.println("Couldn't connect to ftp host");
      while (1);
    }
    if (!ftp.user("jychuah")) {
      Serial.println("Bad username");
      while (1);
    }
    if (!ftp.pass("r6vegas")) {
      Serial.println("Bad password");
      while (1);
    }

    // Test LIST *
    if (!ftp.list("*")) {
      Serial.println("Unable to list files");
    } else {
      Serial.println("LIST *");
      patientStream();
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
      if (!ftp.stop()) {
        Serial.println("Couldn't stop file upload");
      }
    }

    // Test retrieving the file
    if (!ftp.retr("test.txt")) {
      patientStream();
    }

}
