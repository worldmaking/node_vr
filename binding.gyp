{
    "targets": [{
        "target_name": "openvr_node",
        "sources": ["module.cpp"],
        "include_dirs": [
            "node_modules/native-openvr-deps/headers"
        ],
        "libraries": [
            "../node_modules/native-openvr-deps/lib/win64/openvr_api.lib"
        ],
        "cflags": ["-std=c++11", "-Wall", "-pedantic"],
        "conditions":[
            ["OS=='win'", {
            "copies":[{ 
                    'destination': './build/Release',
                    'files':[
                        "node_modules/native-openvr-deps/bin/win64/openvr_api.dll"
                    ]
                }]
            }]
        ]
    }]
}