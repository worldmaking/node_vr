const assert = require("assert")
const openvr = require("./index.js")
const { vec3, quat, mat4 } = require("gl-matrix")

let state = {
  hmd: { pos: vec3.create(), quat: quat.create() },
  trackers: [
    { pos: vec3.create(), quat: quat.create() },
    { pos: vec3.create(), quat: quat.create() },
  ]
}

const tmpmat = mat4.create();


try {
  openvr.init(0);
} catch(e) {
  throw openvr.EVRInitError[e];
}
function getTrackingData() {
  //openvr.update();

 // let res = openvr.waitGetPoses();
  // if (res) {
  //   console.log(res)
  // }
  let trackerCount = 0;
  for (let i=0; i<8; i++) {
    let devclass = openvr.getTrackedDeviceClass(i)

    if (openvr.ETrackedDeviceClass[devclass] == "TrackedDeviceClass_GenericTracker") {
      let out = state.trackers[trackerCount]

      openvr.getLastPoseForTrackedDeviceIndex(i, tmpmat)
      mat4.getTranslation(out.pos, tmpmat)
      mat4.getRotation(out.quat, tmpmat);

      trackerCount++;
    } else if (openvr.ETrackedDeviceClass[devclass] == "TrackedDeviceClass_HMD") {
      let out = state.hmd
      openvr.getLastPoseForTrackedDeviceIndex(i, tmpmat)
      mat4.getTranslation(out.pos, tmpmat)
      mat4.getRotation(out.quat, tmpmat)
    }

  }

  //console.log(state)
  return state;
}

setInterval(getTrackingData, 100)

//console.log("openvr", openvr)

//console.log("event type", openvr.EVREventType[1707])