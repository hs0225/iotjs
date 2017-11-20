var fs = require('fs');
var http = require('http');
var config = require('./config');
var device = require('./device');
var utils = require('./utils');

var server = http.createServer(function(req, res) {
  var data = '', path = utils.filter(req);

  switch (path[0]) {
    case '/':
      utils.status(res, JSON.stringify(device));
      break;

    case '/repl':
      utils.recvDelegator(req, function(body) {
        utils.status(res, data, 200);
        close_application(body);
      });
      break;

    case '/tweet':
      utils.recvDelegator(req, function(body) {
        console.log(body);
        utils.status(res, data, 200);
      });
    break;

    default:
      utils.status(res, data, 404, req);
      break;
  }
}).listen(config.port);

function close_application(data) {
  fs.writeFileSync(device.config.new_filepath, new Buffer(data));
  server.close();
  console.log('TERMINATED');
  process.exit(0);
}
