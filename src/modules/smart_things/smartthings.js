var SmartThingsNative = native;


module.exports = {
  start: function(config) {
    console.log('start js');
    new SmartThingsNative(config);
  },
};
