
var util = require('util');
var EventEmitter = require('events').EventEmitter;
var SmartThingsNative = native;
var SmartThingsResponse = require('smartthings_response');

// function SmartThingsResponse() {
//   this._data = {};
// }

// SmartThingsResponse.prototype.set = function(key, value) {
//   this._data[key] = value;
// };

// SmartThingsResponse.prototype.get = function(key) {
//   return this._data[key];
// };


function SmartThingsGetMessage(url, pKey, query) {
  this.resourceUri = url;
  this._queryRaw = query; // key1=value1;key2=value2;
  this._propertyKey = null;

  if (pKey) {
    this._propertyKey = pKey.split(';');
  }

  console.log('url', url);
  console.log('pKey', pKey);
  console.log('query', query);
}

SmartThingsGetMessage.prototype.setResourceUri = function(uri) {
  this.resourceUri = uri;
};

SmartThingsGetMessage.prototype.hasPropertyKey = function(key) {
  if (!this._propertyKey) {
    return false;
  }

  if (this._propertyKey.indexOf(key) > 0) {
    return false;
  }

  return true;
};


function SmartThingsSetMessage(url, query, _req) {
  this.resourceUri = url;
  this._queryRaw = query; // key1=value1?key2=value2?
  this.req = new SmartThingsResponse(_req);
  
  console.log('smartThings', url);
}


function SmartThings(config, cb) {
  EventEmitter.call(this);

  var callback = cb || function() {};

  this._st = new SmartThingsNative(this, config, callback);

  process.on('exit', this.onExit.bind(this));
}

util.inherits(SmartThings, EventEmitter);

SmartThings.prototype.emitGetRequest = function(_url, _pKey, _query, _req) {
  var msg = new SmartThingsGetMessage(_url, _pKey, _query);

  var req = new SmartThingsResponse(_req);

  this.emit('getRequest', msg, req);

  return;
};


SmartThings.prototype.emitSetRequest = function(_url, _query, _req1, _req2) {
  var msg = new SmartThingsSetMessage(_url, _query, _req1);

  var req = new SmartThingsResponse(_req2);

  this.emit('setRequest', msg, req);

  return;
};

SmartThings.prototype.notifyObservers = function(res) {
  this._st.notifyObservers(res);
};

SmartThings.prototype.test = function() {
  this._st.test();
};

SmartThings.prototype.onExit = function() { 
  if (this._st) {
    this._st.close();
    this._st = null;
  }
};


module.exports = {
  create: function(config, cb) {
    return new SmartThings(config, cb);
  },
};
