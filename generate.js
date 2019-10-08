const path = require("path")
  fs = require("fs"),
  assert = require("assert")

let openvr_api = JSON.parse(fs.readFileSync(path.join(__dirname, "node_modules", "native-openvr-deps", "headers", "openvr_api.json"), "utf-8"));

let used = {}

let json = {}

let typedefs = {}
let enums = {}
let functions = {}

for (let o of openvr_api.typedefs) {
  let name = o.typedef
  typedefs[name] = o.type

  assert(!used[name], name)
  used[name] = true;
}

for (let o of openvr_api.enums) {
  let e = {}
  for (let v of o.values) {
    e[+v.value] = v.name 
  }
  let name = o.enumname
  assert(!used[name], name)
  used[name] = true;
  enums[name] = e

  let unnamespacedname = name.substr(4)
  json[unnamespacedname] = e
}

for (let o of openvr_api.consts) {
  let name = o.constname;
  assert(!used[name], name)
  used[name] = true;
  // o.consttype
  // o.constval

  
  json[name] = (typeof +o.constval == "number") ? +o.constval : o.constval
}

for (let o of openvr_api.structs) {
  let name = o.struct;
  if (name == "vr::(anonymous)") continue;
  assert(!used[name], name)
  used[name] = true;
  // o.fields = []
}

for (let o of openvr_api.methods) {
  let name = `${o.classname}_${o.methodname}`
  if (name == "vr::(anonymous)") continue;
  assert(!used[name], name)
  used[name] = true;

  let params = []
  for (let i in o.params) {
    let p = o.params[i]
    // p.paramname
    // p.paramtype
    params[i] = p
  }
  
  functions[name] = {
    classname: o.classname,
    methodname: o.methodname,
    returntype: o.returntype,
    params: params
  }

  /*
    Generate a Napi method for this function
  */

}

fs.writeFileSync("api.js", "module.exports = " + JSON.stringify(json, null, "  "), "utf-8")

//console.log((openvr_api.methods))

//console.log(enums)