const http = require('node:http');
const fs = require('fs');

const port = 80;
const server = http.createServer();

server.on('request', (req, res) => {
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

server.listen(port, function(error) {
    if (error) {
        console.log('Something went wrong: ' + error);
    }
    else {
        console.log('Server is listening on port ' + port);
    }
});