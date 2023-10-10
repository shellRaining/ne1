// using socket to create a server
// first connect to a client, then get the data from the client

import * as net from "node:net";

const server = net.createServer((socket) => {
  socket.on("data", (data) => {
    console.log(data.toString());
  });
});

server.listen(5377, () => {
  console.log("server is listening");
});
