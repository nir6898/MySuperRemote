const express = require('express');
const { createProxyMiddleware } = require('http-proxy-middleware');

const app = express();
const ARDUINO_IP = '192.168.10.14'; // replace with your Arduino IP
const ARDUINO_PORT = 7000;

app.use('/', createProxyMiddleware({
  target: `http://${ARDUINO_IP}:${ARDUINO_PORT}`,
  changeOrigin: true
}));

app.listen(3330, () => console.log('Proxy running on http://localhost:3330'));
