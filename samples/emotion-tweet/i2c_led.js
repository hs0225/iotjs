var I2C = require('i2c');
var i2c = new I2C();

var CMD_BRIGHTNESS = 0xE0;
var CMD_OSCILLATOR = 0x21;

var charData = {
  smile: [
    "00111100",
    "01000010",
    "10100101",
    "10000001",
    "10100101",
    "10011001",
    "01000010",
    "00111100",
  ],
  sad: [
    "00111100",
    "01000010",
    "10100101",
    "10000001",
    "10011001",
    "10100101",
    "01000010",
    "00111100",
  ],
  soso: [
    "00111100",
    "01000010",
    "10100101",
    "10000001",
    "10000001",
    "10111101",
    "01000010",
    "00111100",
  ],
};

function writeLed(wire, array) {
  var buffer = [0x00];

  for (var i = 0; i < array.length; i++) {
    buffer.push(parseInt(array[i], 2));
    buffer.push(0x00);
  }

  wire.write(buffer);
}

var wire = i2c.open({address: 0x70, device: '/dev/i2c-1'}, function(err) {
  if (err) {
    throw err;
  }

  // turn on oscillator
  wire.write([CMD_OSCILLATOR], function() {
    // wire.write([0x81]);
    wire.write([CMD_BRIGHTNESS | 1]); // adjust brightness
  });
});

function setDisplay(type) {
  writeLed(wire, charData[type]);
}

module.exports = setDisplay;
