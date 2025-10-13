const express = require("express");
const app = express();
const cors = require('cors');
app.use(cors());
const http = require("http");
const server = http.createServer(app);
const { Server } = require("socket.io");
const io = new Server(server, 
    {
        cors: {
            origin: ['http://localhost:3000', 'http://localhost:8080', 'http://localhost'],
            methods: ['GET','POST'],
            credentials: true
        }
    }
);

io.on("connection", (socket) => {
  console.log(`User connected ${socket.id}`);

  // notify to global
  io.to("global").emit("join", { user: socket.id });

  // join this socket to room global
  socket.join("global");

  // DISCONNECTED
  socket.on("disconnect", () => {
    console.log(`User ${socket.id} disconnect`);

    io.to("global").emit("left", { user: socket.id });
  });

  // CHAT
  socket.on("chat", ({ message }) => {
    io.to("global").emit("message", { user: socket.id, message: message });
  });
});

server.listen(8080, () => {
  console.log("listening on *:8080");
});
