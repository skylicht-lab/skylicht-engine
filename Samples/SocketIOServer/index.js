const express = require('express');
const app = express();
const http = require('http');
const server = http.createServer(app);
const { Server } = require("socket.io");
const io = new Server(server);

io.on('connection', (socket) => {
  console.log(`User connected ${socket.id}`);

  socket.on('disconnect', () => {
    console.log(`User ${socket.id} disconnect`);
  });

  socket.on('hello', (callback) => {
    // receive hello from client
    console.log(`${socket.id} emit 'hello'`);
    callback({ response: "ok" });

    // make an event to client
    socket.emit("message", { id: socket.id });
  })

  socket.on('helloParam', (param, callback) => {
    // receive helloParam from client
    console.log(`${socket.id} emit 'helloParam'`);
    console.log(param);
    callback({ response: "ok" });
  })
});

server.listen(8080, () => {
  console.log('listening on *:8080');
});