
const openvr = require('bindings')('openvr_node.node')
const vr = require("./api.js")

for (let k in vr) {
  openvr[k] = vr[k]
}

module.exports = openvr