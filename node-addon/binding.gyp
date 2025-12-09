{
  "targets": [
    {
      "target_name": "lingti_sdk",
      "sources": [
        "src/addon.cc"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
        "<(module_root_dir)/.."
      ],
      "defines": [
        "NAPI_DISABLE_CPP_EXCEPTIONS"
      ],
      "conditions": [
        [
          "OS=='win'",
          {
            "libraries": [
              "<(module_root_dir)/lingti_sdk.lib"
            ],
            "copies": [
              {
                "destination": "<(module_root_dir)/build/Release/",
                "files": [
                  "<(module_root_dir)/lingti_sdk.dll",
                  "<(module_root_dir)/lingtiwfp64.sys"
                ]
              }
            ]
          }
        ]
      ]
    }
  ]
}
