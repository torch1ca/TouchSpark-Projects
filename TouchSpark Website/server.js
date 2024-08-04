const http = require('node:http');
const ws = require('ws');
const fs = require('fs');
const { EventEmitter } = require('node:events');

const httpPort = 80;
const httpServer = http.createServer();

const wsPort = 81;
const wsServer = new ws.Server({ port: wsPort});
const wsEvent = new EventEmitter();

//WEB APP
httpServer.on('request', (req, res) => {
    //webpage request
    if (req.url.split('/').length === 2) {
        let filePath = req.url;
        if (filePath === '/') filePath = '/index';
        filePath = 'views' + filePath + '.html';
        fs.readFile(filePath, 'utf8', function(err, data) {
            if(err){
                res.writeHeader(404);
                res.write('Error: File Not Found!');
                console.log(err);
            }
            else {
                res.writeHead(200, { 'Content-Type' : 'text/html' });
                res.write(data);
            }
            res.end();
        });
    }
    //url request
    else {
        let file = fs.readFileSync('public' + req.url);
        res.end(file);
    }
});

httpServer.listen(httpPort, function(error) {
    if (error) {
        console.log('Something went wrong: ' + error);
    }
    else {
        console.log('Server is listening on port ' + httpPort);
    }
});

//WEB SOCKET
wsServer.on('connection', socket => {
    console.log('New websocket connected');

    socket.on('message', message => {
        let obj = JSON.parse(message.toString());

        if (obj.userInfo !== undefined) {
            if (obj.userInfo === 'webApp') {
                console.log('New websocket is public web app');
            }
            else if (obj.userInfo === 'esp32') {
                console.log('New websocket is ESP32');
                wsEvent.on('buttonPress', (arg) => {
                    socket.send(arg);
                });
            }
            else {
                console.log('Unkown user: ' + obj.userInfo);
            }
        }

        if (obj.buttonPressed !== undefined) {
            console.log(obj.buttonPressed);
            wsEvent.emit('buttonPress', obj.buttonPressed);
        }
    })
});
