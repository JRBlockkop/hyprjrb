const {WebSocketServer} = require('ws');
const dgram = require("dgram");
const http = require('http');
const fs = require('fs');

const server = dgram.createSocket("udp4");

const UDP_to_WS = {
    ws: null,
    rinfo: null,
    send(data,rinfo){
        try{
        UDP_to_WS.ws.send(data, { binary: false });
        if(Buffer.from(data).toString() == "Delay"){
            UDP_to_WS.rinfo = rinfo;
        }
        }catch(e){}
    }
}

server.on("message", (msg, rinfo) => {
    UDP_to_WS.send(msg,rinfo);
}).bind(53745);

http.createServer((_,res)=>{
    res.end(fs.readFileSync('./index.html'));
}).listen(53746);

const wss = new WebSocketServer({ port: 53747 });

wss.on('connection', function connection(ws) {
    UDP_to_WS.ws = ws;
    ws.on('message', function message(data) {
        server.send(data,UDP_to_WS.rinfo.port,UDP_to_WS.rinfo.address)
    });
});