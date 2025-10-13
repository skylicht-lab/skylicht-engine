// server.js
const express = require('express');
const cors = require('cors');

const app = express();
const PORT = process.env.PORT || 8080;

// middlewares
app.use(cors());
app.use(express.json());
app.use(express.urlencoded({ extended: true }));

// routes
app.all(['/','/api/hello'], (req, res) => {
  res.json({
    ok: true,
    message: 'hello world',
    method: req.method,
    path: req.path,
    query: req.query,
    headers: req.headers,
    body: req.body ?? null
  });
});

// 404
app.use((req, res) => {
  res.status(404).json({ ok: false, error: 'Not Found', path: req.path });
});

// start
app.listen(PORT, () => {
  console.log(`Express server listening on http://localhost:${PORT}`);
});
