const assert = require("assert")
const openvr = require("./index.js")
const { vec3, quat, mat4 } = require("gl-matrix")

const hmdmat = mat4.create();
const hmdpos = vec3.create();
const hmdquat = quat.create();


assert( openvr.init(0) == 0);



setInterval(function() {
  //openvr.update();

 // let res = openvr.waitGetPoses();
  // if (res) {
  //   console.log(res)
  // }
  let trackerCount = 0;
  for (let i=0; i<8; i++) {
    let devclass = openvr.getTrackedDeviceClass(i)

    if (openvr.ETrackedDeviceClass[devclass] == "TrackedDeviceClass_GenericTracker") {
      openvr.getLastPoseForTrackedDeviceIndex(i, hmdmat)
      mat4.getTranslation(hmdpos, hmdmat)
      mat4.getRotation(hmdquat, hmdmat);

      console.log("tracker", trackerCount, hmdpos, hmdquat)

      trackerCount++;
    }

  }
  
 // openvr.getSortedTrackedDeviceIndicesOfClass(1)
}, 100)

console.log("openvr", openvr)

//console.log("event type", vr.EVREventType[1707])