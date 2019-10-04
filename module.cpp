
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
  vr::IVRCompositor * m_compositor = nullptr;

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

  int32_t apptype = 0;
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
  napi_value result;

  // Process SteamVR events
	vr::VREvent_t event;
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
        fprintf(stdout, "Other VR event: %d\n", event.eventType);
      }
    }
	}

  return result;
}

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
  
  assert(napi_ok == napi_add_env_cleanup_hook(env, cleanup, nullptr));

  //printf("\n\n****************************************************************************\n****************************************************************************\n\n*                  CPP : napi_init initializing module                     *\n\n****************************************************************************\n****************************************************************************\n\n");
              
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init_module);

