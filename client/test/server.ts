// using socket to create a server
// first connect to a client, then get the data from the client

import * as net from "node:net";

const server = net.createServer((socket) => {
  socket.on("data", (data) => {
    const hex = data.toString("hex");
    if(hex == null) return;
    const bytes = hex.match(/.{1,2}/g);
    if(bytes == null) return;
    console.log(bytes.join(" "));
  });
});

server.listen(5377, () => {
  console.log("server is listening");
});
