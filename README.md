# ParticleFtpClient

A library for Particle.io platform devices to perform simple FTP commands.

### Usage

```
ParticleFtpClient ftp;
```

Most methods mirror FTP commands. To log into an FTP server, you must successively call `ftp.open("hostname", timeout)`, `ftp.user("username")` and `ftp.pass("password")`. Each should return true.

### Timeouts

FTP can be a slow protocol. Therefore, the `open` method has a `timeout` parameter in integer seconds. Certain methods will timeout. I've tried to make the client as robust as possible, but occassionally things will happen "out of sequence" and you may wish to retry your ftp commands.

### Passive Mode

This library implements a passive mode client only. _TLDR, The server needs to support passive mode._

In passive mode, any time the server needs to send or receive extensive data, the client will automatically send a PASV command. The server responds with a new port used for the data transfer. The sender of the data (either the client when uploading or server when downloading) stops the connection when the data transfer is finished and the port is discarded.

### Downloading Files

Certain methods (`list`, `retr`) requires the use of a passive connection. Therefore, ParticleFTPClient exposes a public `TCPClient` member `data`. For methods that retrieve passive data (`list` and `retr`), you may reasonably expect server sent data as long as `data` remaines `connected`. For example:

```
// Initiate a download
ftp.retr("download.txt");
// While the connection remains open, the server still intends to send data.
while (ftp.data.connected()) {
  // Read what data is in the current packet
  while (ftp.data.available()) {
    // get another byte
    char c = ftp.data.read();
  }
}
```

### Uploading Files

The `stor` method allows a file upload, using the `data` member. Call `finish` when you are done uploading. It will `stop` the `data` connection and flush any server responses. (There may be a timeout here.) For example:

```
// Set the file type to ASCII
ftp.type("A"); 
// Initiate an upload
ftp.stor("upload.txt");
for (int i = 0; i < strlen(data); i++) {
  ftp.data.write(data[i]);
}
// Call finish when you are done uploading. You may expect a timeout here.
ftp.finish();
```

### Debugging

If you want verbose serial output for server responses, uncomment `#define PARTICLE_FTP_DEBUG` in `ParticleFtpClient.cpp`