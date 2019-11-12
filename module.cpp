
#include <node_api.h>

#include <openvr_capi.h>
#include <openvr.h>

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <malloc.h>

#include <windows.h>



struct VRContext {
  vr::IVRSystem * m_system = nullptr;
  vr::IVRCompositor * m_compositor = nullptr; /*getLastPoseForTrackedDeviceIndex;*/

  std::string m_strDriver = "No Driver";
	std::string m_strDisplay = "No Display";

  std::string GetTrackedDeviceString( vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError *peError = NULL ) {
    uint32_t unRequiredBufferLen = m_system->GetStringTrackedDeviceProperty( unDevice, prop, NULL, 0, peError );
    if( unRequiredBufferLen == 0 )
      return "";

    char *pchBuffer = new char[ unRequiredBufferLen ];
    unRequiredBufferLen = m_system->GetStringTrackedDeviceProperty( unDevice, prop, pchBuffer, unRequiredBufferLen, peError );
    std::string sResult = pchBuffer;
    delete [] pchBuffer;
    return sResult;
  }

  void close() {
    if (m_system) {
      vr::VR_Shutdown();
      m_system = nullptr;
    }
  }
};

VRContext vrContext;

napi_value throwErrorString(napi_env env, std::string errmsg) {
  napi_value err;
  napi_create_string_utf8(env, errmsg.data(), errmsg.size(), &err);
  napi_throw(env, err);
  return nullptr;
}

#include "generated_module.cpp"



/*
  if ( eError != vr::VRInitError_None )
	{
		m_pHMD = NULL;
		char buf[1024];
		sprintf_s( buf, sizeof( buf ), "Unable to init VR runtime: %s", vr::VR_GetVRInitErrorAsEnglishDescription( eError ) );
		SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "VR_Init Failed", buf, NULL );
		return false;
	}

*/

napi_value init(napi_env env, napi_callback_info args) {
  size_t argc = 1; // how many args we want
  napi_value argv[1];
  void * userContext;  // the C++ data we can associate with a function
  assert(napi_get_cb_info(env, args, &argc, argv, NULL, (void **)&userContext) == napi_ok);
  napi_value result;

  int32_t apptype = EVRApplicationType_VRApplication_Scene; // vr::VRApplicaiton_Scene
  napi_get_value_int32(env, argv[0], &apptype);
  
  vr::EVRApplicationType eApplicationType = (vr::EVRApplicationType)apptype;
  vr::EVRInitError peError;
  vrContext.m_system = vr::VR_Init(&peError, eApplicationType);

  if (peError != vr::VRInitError_None ) {
    napi_value err;
    napi_create_int32(env, (int32_t)peError, &err);
    napi_throw(env, err);
    return nullptr;
  }

	vrContext.m_strDriver = vrContext.GetTrackedDeviceString( vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_TrackingSystemName_String );
	vrContext.m_strDisplay = vrContext.GetTrackedDeviceString( vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_SerialNumber_String );
  vrContext.m_compositor = vr::VRCompositor();

	if (!vrContext.m_compositor) return throwErrorString(env, "Compositor initialization failed. See log file for details");


	// vr::VRInput()->SetActionManifestPath( Path_MakeAbsolute( "../hellovr_actions.json", Path_StripFilename( Path_GetExecutablePath() ) ).c_str() );

	// vr::VRInput()->GetActionHandle( "/actions/demo/in/HideCubes", &m_actionHideCubes );
	// vr::VRInput()->GetActionHandle( "/actions/demo/in/HideThisController", &m_actionHideThisController);
	// vr::VRInput()->GetActionHandle( "/actions/demo/in/TriggerHaptic", &m_actionTriggerHaptic );
	// vr::VRInput()->GetActionHandle( "/actions/demo/in/AnalogInput", &m_actionAnalongInput );

	// vr::VRInput()->GetActionSetHandle( "/actions/demo", &m_actionsetDemo );

	// vr::VRInput()->GetActionHandle( "/actions/demo/out/Haptic_Left", &m_rHand[Left].m_actionHaptic );
	// vr::VRInput()->GetInputSourceHandle( "/user/hand/left", &m_rHand[Left].m_source );
	// vr::VRInput()->GetActionHandle( "/actions/demo/in/Hand_Left", &m_rHand[Left].m_actionPose );

	// vr::VRInput()->GetActionHandle( "/actions/demo/out/Haptic_Right", &m_rHand[Right].m_actionHaptic );
	// vr::VRInput()->GetInputSourceHandle( "/user/hand/right", &m_rHand[Right].m_source );
	// vr::VRInput()->GetActionHandle( "/actions/demo/in/Hand_Right", &m_rHand[Right].m_actionPose );


  napi_create_int32(env, peError, &result);
  return result;
};

napi_value close(napi_env env, napi_callback_info args) {
  size_t argc = 0; // how many args we want
  void * userContext;  // the C++ data we can associate with a function
  assert(napi_get_cb_info(env, args, &argc, nullptr, NULL, (void **)&userContext) == napi_ok);
  napi_value result;

  vrContext.close();
  

	// for( std::vector< CGLRenderModel * >::iterator i = m_vecRenderModels.begin(); i != m_vecRenderModels.end(); i++ )
	// {
	// 	delete (*i);
	// }
	// m_vecRenderModels.clear();
	
  return result;
}

	// bQuit = HandleInput();

	// 	RenderFrame();


napi_value update(napi_env env, napi_callback_info args) {
  size_t argc = 1; // how many args we want
  napi_value argv[1];
  void * userContext;  // the C++ data we can associate with a function
  assert(napi_get_cb_info(env, args, &argc, argv, NULL, (void **)&userContext) == napi_ok);
  napi_value result = nullptr;

  // Process SteamVR events
	vr::VREvent_t event;
  
  //       	uint32_t eventType; // EVREventType enum
	// TrackedDeviceIndex_t trackedDeviceIndex;
	// float eventAgeSeconds;
	// // event data must be the end of the struct as its size is variable
	// VREvent_Data_t data;

	while( vrContext.m_system->PollNextEvent( &event, sizeof( event ) ) )
	{
		switch( event.eventType ) {
      case vr::VREvent_TrackedDeviceDeactivated:{
        fprintf(stdout, "Device %u detached.\n", event.trackedDeviceIndex );
      }
      break;
      case vr::VREvent_TrackedDeviceUpdated:{
        fprintf(stdout, "Device %u updated.\n", event.trackedDeviceIndex );
      }
      break;
      default: {


        fprintf(stdout, "Other VR event: %d for device %d\n", event.eventType, event.trackedDeviceIndex);
      }
    }
	}

  return result;
}


napi_value waitGetPoses(napi_env env, napi_callback_info args) {
  size_t argc = 1; // how many args we want
  napi_value argv[1];
  void * userContext;  // the C++ data we can associate with a function
  assert(napi_get_cb_info(env, args, &argc, argv, NULL, (void **)&userContext) == napi_ok);
  napi_value result = nullptr;

  if (!vrContext.m_compositor) return result;

	/** Scene applications should call this function to get poses to render with (and optionally poses predicted an additional frame out to use for gameplay).
	* This function will block until "running start" milliseconds before the start of the frame, and should be called at the last moment before needing to
	* start rendering.
	*
	* Return codes:
	*	- IsNotSceneApplication (make sure to call VR_Init with VRApplicaiton_Scene)
	*	- DoNotHaveFocus (some other app has taken focus - this will throttle the call to 10hz to reduce the impact on that app)
	*/
	//virtual EVRCompositorError WaitGetPoses( VR_ARRAY_COUNT( unRenderPoseArrayCount ) TrackedDevicePose_t* pRenderPoseArray, uint32_t unRenderPoseArrayCount,
		//VR_ARRAY_COUNT( unGamePoseArrayCount ) TrackedDevicePose_t* pGamePoseArray, uint32_t unGamePoseArrayCount ) = 0;
  
  uint32_t unRenderPoseArrayCount = 8;
  vr::TrackedDevicePose_t renderPoseArray[8];
  uint32_t unGamePoseArrayCount = 8;
  vr::TrackedDevicePose_t gamePoseArray[8];
  vr::EVRCompositorError err = vrContext.m_compositor->WaitGetPoses(renderPoseArray, unRenderPoseArrayCount, gamePoseArray, unGamePoseArrayCount);

  /** describes a single pose for a tracked object 
  struct TrackedDevicePose_t
  {
    // right-handed system
    // +y is up
    // +x is to the right
    // -z is forward
    // Distance unit is  meters
    struct HmdMatrix34_t
    {
      float m[3][4];
    };
    
    
    HmdMatrix34_t mDeviceToAbsoluteTracking;
    HmdVector3_t vVelocity;				// velocity in tracker space in m/s
    HmdVector3_t vAngularVelocity;		// angular velocity in radians/s (?)
    ETrackingResult eTrackingResult;  // expected to be vr::TrackingResult_Running_OK
    bool bPoseIsValid;

    // This indicates that there is a device connected for this spot in the pose array.
    // It could go from true to false if the user unplugs the device.
    bool bDeviceIsConnected;
  };*/

  for (int i=0; i<8; i++) {
    vr::TrackedDevicePose_t& rpose = renderPoseArray[i];
    vr::TrackedDevicePose_t& gpose = gamePoseArray[i];

/*
TrackedControllerRole_Invalid = 0,					// Invalid value for controller type
	TrackedControllerRole_LeftHand = 1,					// Tracked device associated with the left hand
	TrackedControllerRole_RightHand = 2,				// Tracked device associated with the right hand
	TrackedControllerRole_OptOut = 3,					// Tracked device is opting out of left/right hand selection
	TrackedControllerRole_Treadmill = 4,				// Tracked device is a treadmill
	TrackedControllerRole_Max = 5*/
  // only useful for left/right hand role on hand controllers:
    vr::ETrackedControllerRole role = vrContext.m_system->GetControllerRoleForTrackedDeviceIndex( (vr::TrackedDeviceIndex_t) i );
    /*
    	TrackedDeviceClass_Invalid = 0,				// the ID was not valid.
	TrackedDeviceClass_HMD = 1,					// Head-Mounted Displays
	TrackedDeviceClass_Controller = 2,			// Tracked controllers
	TrackedDeviceClass_GenericTracker = 3,		// Generic trackers, similar to controllers
	TrackedDeviceClass_TrackingReference = 4,	// Camera and base stations that serve as tracking reference points
	TrackedDeviceClass_DisplayRedirect = 5,		// Accessories that aren't necessarily tracked themselves, but may redirect video output from other tracked devices

*/
    vr::ETrackedDeviceClass devclass = vrContext.m_system->GetTrackedDeviceClass( (vr::TrackedDeviceIndex_t) i );

    vr::HmdMatrix34_t& pose = rpose.mDeviceToAbsoluteTracking;

    float x = pose.m[0][3];
    float y = pose.m[1][3];
    float z = pose.m[2][3];

    printf("i %d -- %d %d == %d %d is %d : %d @ %f %f %f\n", i, rpose.bDeviceIsConnected, rpose.bPoseIsValid, gpose.bDeviceIsConnected, gpose.bPoseIsValid, role, devclass, x, y, z);
    //printf("poses: %d %d\n", unRenderPoseArrayCount, unGamePoseArrayCount);
  }

  return result;
}


// arguments:
// device index
// mat4
// getLastPoseForTrackedDeviceIndex(0, new Float32Array(16))
napi_value getTrackedDeviceClass(napi_env env, napi_callback_info args) {
  size_t argc = 1; // how many args we want
  napi_value argv[1];
  void * userContext;  // the C++ data we can associate with a function
  assert(napi_get_cb_info(env, args, &argc, argv, NULL, (void **)&userContext) == napi_ok);
  napi_value result = nullptr;

  if (!vrContext.m_system) return result;
  if (!vrContext.m_compositor) return result;

  uint32_t unDeviceIndex = 0;
  assert(napi_ok == napi_get_value_uint32(env, argv[0], &unDeviceIndex));

  vr::ETrackedDeviceClass devclass = vrContext.m_system->GetTrackedDeviceClass( (vr::TrackedDeviceIndex_t) unDeviceIndex );

  assert(napi_ok == napi_create_uint32(env, (uint32_t)devclass, &result));

  return result;
}


// arguments:
// device index
// mat4
// getLastPoseForTrackedDeviceIndex(0, new Float32Array(16))
napi_value getLastPoseForTrackedDeviceIndex(napi_env env, napi_callback_info args) {
  size_t argc = 2; // how many args we want
  napi_value argv[2];
  void * userContext;  // the C++ data we can associate with a function
  assert(napi_get_cb_info(env, args, &argc, argv, NULL, (void **)&userContext) == napi_ok);
  napi_value result = nullptr;

  if (!vrContext.m_system) return result;
  if (!vrContext.m_compositor) return result;

  uint32_t unDeviceIndex = 0;
  assert(napi_ok == napi_get_value_uint32(env, argv[0], &unDeviceIndex));

  float * matrix;
  napi_typedarray_type type = napi_float32_array;
  napi_value arraybuffer;
  size_t length, offset;
  assert(napi_ok == napi_get_typedarray_info(env, argv[1], &type, &length, (void **)&matrix, &arraybuffer, &offset));

  

  vr::TrackedDevicePose_t pose;
  vr::EVRCompositorError err = vrContext.m_compositor->GetLastPoseForTrackedDeviceIndex( (TrackedDeviceIndex_t) unDeviceIndex, &pose, nullptr );
  if (err == vr::VRCompositorError_None) {
    // get data:
    
    matrix[0*4+0] = pose.mDeviceToAbsoluteTracking.m[0][0];
    matrix[0*4+1] = pose.mDeviceToAbsoluteTracking.m[1][0];
    matrix[0*4+2] = pose.mDeviceToAbsoluteTracking.m[2][0];
    
    matrix[1*4+0] = pose.mDeviceToAbsoluteTracking.m[0][1];
    matrix[1*4+1] = pose.mDeviceToAbsoluteTracking.m[1][1];
    matrix[1*4+2] = pose.mDeviceToAbsoluteTracking.m[2][1];
    
    matrix[2*4+0] = pose.mDeviceToAbsoluteTracking.m[0][2];
    matrix[2*4+1] = pose.mDeviceToAbsoluteTracking.m[1][2];
    matrix[2*4+2] = pose.mDeviceToAbsoluteTracking.m[2][2];

    matrix[3*4+0] = pose.mDeviceToAbsoluteTracking.m[0][3];
    matrix[3*4+1] = pose.mDeviceToAbsoluteTracking.m[1][3];
    matrix[3*4+2] = pose.mDeviceToAbsoluteTracking.m[2][3];
  }

  return result;
}



/*
  typedef uint32_t TrackedDeviceIndex_t;

   Interface for accessing last set of poses returned by WaitGetPoses one at a time.
	* Returns VRCompositorError_IndexOutOfRange if unDeviceIndex not less than k_unMaxTrackedDeviceCount otherwise VRCompositorError_None.
	* It is okay to pass NULL for either pose if you only want one of the values. 
	virtual EVRCompositorError GetLastPoseForTrackedDeviceIndex( TrackedDeviceIndex_t unDeviceIndex, TrackedDevicePose_t *pOutputPose, TrackedDevicePose_t *pOutputGamePose ) = 0;
*/

void cleanup(void * arg) {
  vrContext.close();
}

napi_value init_module(napi_env env, napi_value exports) {
  napi_value fn;

  
  void * userContext = &vrContext; // User-provided data context. This will be passed back into the function when invoked later.
  assert(napi_create_function(env, "init", 0, init, userContext, &fn) == napi_ok);
  assert(napi_set_named_property(env, exports, "init", fn) == napi_ok);
  
  assert(napi_create_function(env, "close", 0, close, userContext, &fn) == napi_ok);
  assert(napi_set_named_property(env, exports, "close", fn) == napi_ok);
  
  
  assert(napi_create_function(env, "update", 0, update, userContext, &fn) == napi_ok);
  assert(napi_set_named_property(env, exports, "update", fn) == napi_ok);
  assert(napi_create_function(env, "waitGetPoses", 0, waitGetPoses, userContext, &fn) == napi_ok);
  assert(napi_set_named_property(env, exports, "waitGetPoses", fn) == napi_ok);
  assert(napi_create_function(env, "getTrackedDeviceClass", 0, getTrackedDeviceClass, userContext, &fn) == napi_ok);
  assert(napi_set_named_property(env, exports, "getTrackedDeviceClass", fn) == napi_ok);
  assert(napi_create_function(env, "getLastPoseForTrackedDeviceIndex", 0, getLastPoseForTrackedDeviceIndex, userContext, &fn) == napi_ok);
  assert(napi_set_named_property(env, exports, "getLastPoseForTrackedDeviceIndex", fn) == napi_ok);

  
  
  assert(napi_ok == napi_add_env_cleanup_hook(env, cleanup, nullptr));

  //printf("\n\n****************************************************************************\n****************************************************************************\n\n*                  CPP : napi_init initializing module                     *\n\n****************************************************************************\n****************************************************************************\n\n");
              
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init_module);

