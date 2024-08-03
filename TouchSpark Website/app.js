const http = require('node:http');
const fs = require('fs');

const port = 8000;
const server = http.createServer();

server.on('request', (req, res) => {
    fs.readFile('public/index.html', 'utf8', function(error, data) {
        if(error){
            res.writeHeader(404);
            res.write('Error: File Not Found!');
        }
        else {
            res.writeHead(200, { 'Content-Type' : 'text/html' });
            res.write(data);
        }
        res.end();
    });
});

server.listen(port, function(error) {
    if (error) {
        console.log('Something went wrong: ' + error);
    }
    else {
        console.log('Server is listening on port ' + port);
    }
});