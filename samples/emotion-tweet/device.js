var fs = require('fs');

if (process.platform === 'tizenrt') {
  var new_filepath = '/mnt/run_demo_new.js';
  var filePath = '/rom/backend.js';
} else {
  var new_filepath = 'run_demo_new.js';
  var filePath = 'backend.js';
}

/*
  1) iotjs backend.js
  2) after updating the source, run iotjs iotjs_new_file_path
*/

module.exports = {
  config: {
    new_filepath: new_filepath,
    filePath: filePath,
  },
  code: (function() {
    var selfCode = fs.readFileSync(filePath).toString();
    return selfCode;
  })(),
};
