var fs = require('fs');
var http = require('http');
var querystring = require('querystring');
var url = require('url');
var Gpio = require('gpio');

var config = require('./config');
var device = require('./device');
var utils = require('./utils');

var count = 0;
var RED_PIN = 0,
    GREEN_PIN = 1,
    BLUE_PIN = 2;

var LED_ON = true,
    LED_OFF = false;
    
var ledPin = [31, 30, 32];
var ledGpio = [];

var gpio = new Gpio();

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

createLedGpio();

console.log('START');

function change_mood(score) {
  // TODO: change mood based on emotion score (-5 ~ 5)
  if (ledGpio.length < ledPin.length) {
    return;
  }

  console.log(count++, 'TODO: change mood light based on emotion score:', score);
  if (score < 0) {  // turn on blue led
    ledGpio[BLUE_PIN].writeSync(LED_ON);
    ledGpio[GREEN_PIN].writeSync(LED_OFF);
    ledGpio[RED_PIN].writeSync(LED_OFF);
  } else if (score === 0) { // turn on green led
    ledGpio[GREEN_PIN].writeSync(LED_ON);
    ledGpio[BLUE_PIN].writeSync(LED_OFF);
    ledGpio[RED_PIN].writeSync(LED_OFF);
  } else { // socre > 0, turn on red led
    ledGpio[RED_PIN].writeSync(LED_ON);
    ledGpio[GREEN_PIN].writeSync(LED_OFF);
    ledGpio[BLUE_PIN].writeSync(LED_OFF);
  }
}

function createLedGpio() {
  for (var i = 0; i < ledPin.length; i++) {
    ledGpio[i] = gpio.open({
      pin: ledPin[i],
      direction: gpio.DIRECTION.OUT,
    });
  }
}

function close_application(data) {
  fs.writeFileSync(device.config.new_filepath, new Buffer(data));
  server.close();
  console.log('TERMINATED');
  // process.exit(0);
}
