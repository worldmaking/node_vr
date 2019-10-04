const assert = require("assert")
const openvr = require('bindings')('openvr_node.node')

assert( openvr.init(0) == 0);

setInterval(function() {
  openvr.update();
}, 100)

console.log("openvr", openvr)