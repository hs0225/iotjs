var fs = require('fs');
var http = require('http');
var querystring = require('querystring');
var url = require('url');

var config = require('./config');
var device = require('./device');
var utils = require('./utils');

var count = 0;

var server = http.createServer(function(req, res) {
  var data = '';
  var parsed = url.parse(req.url);
  
  console.log(count++, req.method, parsed.pathname);

  switch (parsed.pathname) {
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
      var query = querystring.parse(parsed.query);
      // {"date":"","text":"","emotion":{"score":3,"comparative":1}}
      change_mood(query.score);
      utils.status(res, '', 200);
      break;

    default:
      utils.status(res, data, 404, req);
      break;
  }
}).listen(config.port);

function change_mood(score) {
  // TODO: change mood based on emotion score (-5 ~ 5)
  console.log(count++, 'TODO: change mood light based on emotion score:', score);
}

function close_application(data) {
  fs.writeFileSync(device.config.new_filepath, new Buffer(data));
  server.close();
  console.log('TERMINATED');
  process.exit(0);
}
