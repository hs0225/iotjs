
var util = require('util');
var EventEmitter = require('events').EventEmitter;
var SmartThingsNative = native;
var STRepresentation = require('strepresentation');


function SmartThingsGetMessage(url, pKey, query) {
  this.resourceUri = url;
  this._queryRaw = query; // key1=value1;key2=value2;
  this._propertyKey = null;

  if (pKey) {
    this._propertyKey = pKey.split(';');
  }
  console.log('js: get message', url, pKey, query);
}

SmartThingsGetMessage.prototype.setResourceUri = function(uri) {
  this.resourceUri = uri;
};

SmartThingsGetMessage.prototype.hasPropertyKey = function(key) {
  if (!this._propertyKey || this._propertyKey.indexOf(key) > 0) {
    return false;
  }

  return true;
};


function SmartThingsSetMessage(url, query, req) {
  this.resourceUri = url;
  this._queryRaw = query; // key1=value1?key2=value2?
  this.req = req;
  
  console.log('[IOTJS] set message', query);
  console.log('js: set message', url, query);
}


function SmartThings(config, cb) {
  console.log('js, smart thigns con');
  EventEmitter.call(this);

  var callback = cb || function() {};

  this._st = new SmartThingsNative(this, config, callback);

  process.on('exit', this.onExit.bind(this));
}

util.inherits(SmartThings, EventEmitter);

SmartThings.prototype.emitGetRequest = function(_url, _pKey, _query, req) {
  var msg = new SmartThingsGetMessage(_url, _pKey, _query);
  console.log('emit get request', _url, _pKey, _query);
  this.emit('getRequest', msg, req);

  return;
};


SmartThings.prototype.emitSetRequest = function(_url, _query, meg_req, req) {
  var msg = new SmartThingsSetMessage(_url, _query, meg_req);
  console.log('emit set request', _url, _query);
  this.emit('setRequest', msg, req);

  return;
};

SmartThings.prototype.notifyObservers = function(res) {
  this._st.notifyObservers(res);
};


SmartThings.prototype.onExit = function() { 
  console.log('js, smart thigns exit');
  if (this._st) {
    console.log('js, smart thigns exit2');
    this._st.close();
    this._st = null;
  }
};


module.exports = {
  create: function(config, cb) {
    return new SmartThings(config, cb);
  },
};
