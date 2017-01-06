{
  "conditions": [
    ["OS=='win'", {
      "target_defaults": {
        "default_configuration": "Release_x64",
        "configurations": {
          "Debug_x32": {
            "msvs_configuration_platform": "Win32",
          },
          "Debug_x64": {
            "msvs_configuration_platform": "x64",
          },
          "Release_x32": {
            "msvs_configuration_platform": "Win32",
          },
          "Release_x64": {
            "msvs_configuration_platform": "x64",
          }
        }
      }
    }, {
      "target_defaults": {
        "default_configuration": "Release",
        "configurations": {
          "Debug": {
            "defines": ["DEBUG"]
          }
        }
      }
    }],
    ["OS=='linux'", {
      "target_defaults": {
        "configurations": {
          "Debug": {
            "cflags": ["-g"]
          },
          "Release": {
            "cflags": ["-O3"]
          }
        }
      }
    }],
    ["OS=='mac'", {
      "target_defaults": {
        "default_configuration": "Release",
        "configurations": {
          "Debug": {
            "xcode_settings": {
              "GCC_OPTIMIZATION_LEVEL": "0",
              "GCC_GENERATE_DEBUGGING_SYMBOLS": "YES"
            }
          },
          "Release": {
            "xcode_settings": {
              "GCC_OPTIMIZATION_LEVEL": "3",
              "GCC_GENERATE_DEBUGGING_SYMBOLS": "NO",
            }
          }
        }
      }
    }]
  ],
  "targets": [
    {
      "target_name": "boids",
      "type": "executable",
      "sources": [
        "src/boids.cc",
        "src/shaders.cc",
        "src/util.cc",
        "<(INTERMEDIATE_DIR)/boids-shaders.cc"
      ],
      "actions": [
        {
          "action_name": "pack_shaders",
          "variables": {
            "shader_files": [
              "shaders/boidvertex.vert",
              "shaders/boidgeometry.geom",
              "shaders/flatfragment.frag",
              "shaders/cellcounts.comp",
              "shaders/spreadcounts.comp",
              "shaders/index.comp",
              "shaders/spreadindex.comp",
              "shaders/move.comp"
            ]
          },
          "inputs": [
            "tools/pack-shaders.py",
            "<@(shader_files)"
          ],
          "outputs": [
            "<(INTERMEDIATE_DIR)/boids-shaders.cc",
            "<(INTERMEDIATE_DIR)/boids-shaders.h"
          ],
          "action": ["python", "tools/pack-shaders.py", "<(INTERMEDIATE_DIR)", "<@(shader_files)"]
        }
      ],
      "link_settings": {
        "libraries": [
          "-lm",
          "-lGL",
          "-lglut",
          "-lGLEW"
        ]
      },
      "include_dirs": [
        "include",
        "<(INTERMEDIATE_DIR)"
      ],
      "conditions": [
        ["OS=='linux'", {
          "cflags": ["-Wall"]
        }]
      ]
    }
  ]
}
