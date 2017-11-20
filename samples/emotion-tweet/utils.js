var http = require('http');
var count = 0;

// utils
function status(res, data, code, req, isjson) {
  var headers;
  if (!code) code = 200;
  if (req && req.method == 'OPTIONS') {
    code = 200;
    headers = {
      'Access-Control-Allow-Origin': '*',
      'Access-Control-Allow-Headers': 'Origin, X-Requested-With, X-HTTP-Method-Override, Content-Type, Accept',
      'Access-Control-Allow-Methods': 'GET,PUT,POST,OPTIONS',
    };
  } else {
    headers = {
      'Access-Control-Allow-Origin': '*',
      'Access-Control-Allow-Headers': 'Origin, X-Requested-With, Content-Type, Accept',
      'Content-Type': isjson ? 'application/json' : 'text/plain',
    };
  }
  res.writeHead(code, headers);
  return res.end(data);
}

function filter(req) {
  var path, match;

  if (req.method == 'OPTIONS') return [''];
  console.log(count++, req.method, req.url);

  // TODO: use querystring if memory is enough.
  path = req.url.split('?');
  //match = /(\/device)(\/*(.*)\/((\w+)\_(\w{1,2}))\?*(.*))*/g.exec(path[0]);// (/device)[(/....)/(XXX)_(dd)?(....)]
  match && match.shift();
  return match != null ? match : path;
}

function recvDelegator(req, doneCallback) {
  var body = '';

  req.on('data', function(chunk) {
    body += chunk;
  });

  req.on('end', function() {
    doneCallback ? doneCallback(body) : '';
  });
}

function request(url) {
  console.log(url);
  // TODO: protocol parse

  http.get(url, function (resp) {
    var data = '';
  
    resp.on('data', function (chunk) {
      data += chunk;
    });
  
    resp.on('end', function () {
    });

  }).on('error', function (err) {
    console.log('Error: ' + err.message);
  });
}

/*
e.g) How to use Task
var task = new Task;
task.addTask(function() {
  var inputOpenCallback = this.createConditionCallback(INPUT_PINS.length, 
                          this.commonCallback.bind(null, 'input pins opened', true));
  INPUT_PINS.forEach(function(pin) {
    PINS[pin] = gpio.open({ pin: pin, direction: gpio.DIRECTION.IN }, inputOpenCallback);
  });
})
*/

function Task() {
  this.ASYNC_QUEUE = [];
  this.ASYNC_QUEUE_CURRENT = -1;
}

Task.prototype.addTask = function addTask(func) {
  this.ASYNC_QUEUE.push(func);
};

Task.prototype.createConditionCallback = function createConditionCallback(total, callback) {
  var current = 0, errors = [], results = [];
  return function(err, result) {
    results[current] = result;
    errors[current++] = err;
    if (current === total)
      callback(errors, results);
  };
};

Task.prototype.commonCallback = function commonCallback(msg, isCallNext, errors) {
  if (this.checkErrorList(errors)) {
    console.error(errors);
  } else {
    console.log(msg);
    if (isCallNext) this.callNextTask();
  }
};

Task.prototype.callNextTask = function callNextTask() {
  if (++this.ASYNC_QUEUE_CURRENT >= this.ASYNC_QUEUE.length) {
    console.log('task empty');
  } else {
    this.ASYNC_QUEUE[this.ASYNC_QUEUE_CURRENT]();
  }
};

Task.prototype.checkErrorList = function checkErrorList(errors) {
  if (!errors) return false;
  return !!errors.filter(function(err) {
    return err;
  }).length;
};


module.exports = {
  filter: filter,
  status: status,
  recvDelegator: recvDelegator,
  Task: Task,
  request: request,
};

/*
// manupulate device requests
function manupulate(req, res, path) {
  if (req.method == 'GET') {
    utils.status(res, JSON.stringify(device));

  } else if (req.method == 'POST' || req.method == 'PUT') {
    utils.recvDelegator(req, function(body) {
      var raw = JSON.parse(body);
      deviceIO.handleRequest('write', path[4], path[5], raw.value, raw);
      utils.status(res);
    });
  }
}
*/
