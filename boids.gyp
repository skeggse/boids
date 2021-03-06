{
  "conditions": [
    ["OS=='win'", {
      "variables": {
        # for no documented reason, gyp decides to make cpp relative, so we add
        # the /, and strip it off in python
        "cpreprocessor": ["/cl", "-EP"]
      },
      "target_defaults": {
        "default_configuration": "Release_x64",
        "configurations": {
          "Debug_x32": {
            "msvs_configuration_platform": "Win32",
            "library_dirs": [
              "dependencies/freeglut/lib/Win32",
              "dependencies/glew/lib/Release/Win32"
            ]
          },
          "Debug_x64": {
            "msvs_configuration_platform": "x64",
            "library_dirs": [
              "dependencies/freeglut/lib/x64",
              "dependencies/glew/lib/Release/x64"
            ]
          },
          "Release_x32": {
            "msvs_configuration_platform": "Win32",
            "library_dirs": [
              "dependencies/freeglut/lib/Win32",
              "dependencies/glew/lib/Release/Win32"
            ]
          },
          "Release_x64": {
            "msvs_configuration_platform": "x64",
            "library_dirs": [
              "dependencies/freeglut/lib/x64",
              "dependencies/glew/lib/Release/x64"
            ]
          }
        }
      }
    }, {
      "variables": {
        # for no documented reason, gyp decides to make cpp relative, so we add
        # the /, and strip it off in python
        "cpreprocessor": ["/cpp", "-P"]
      },
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
              "GCC_GENERATE_DEBUGGING_SYMBOLS": "NO"
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
        "<(INTERMEDIATE_DIR)/s/boids-shaders.cc"
      ],
      "actions": [
        {
          "action_name": "pack_shaders",
          # looks like this disables a hidden "feature" whereby gyp runs things
          # with cygwin
          "msvs_cygwin_shell": 0,
          "variables": {
            "shader_files": [
              "shaders/boidgeometry.geom",
              "shaders/boidvertex.vert",
              "shaders/brutemove.comp",
              "shaders/cellcounts.comp",
              "shaders/flatfragment.frag",
              "shaders/index.comp",
              "shaders/move.comp",
              "shaders/scanmove.comp",
              "shaders/spreadcounts.comp",
              "shaders/spreadindex.comp"
            ]
          },
          "inputs": [
            "tools/pack-shaders.py",
            "<@(shader_files)"
          ],
          "outputs": [
            "<(INTERMEDIATE_DIR)/s/boids-shaders.cc",
            "<(INTERMEDIATE_DIR)/s/boids-shaders.h"
          ],
          "action": ["python", "tools/pack-shaders.py", "<(cpreprocessor)", "<(INTERMEDIATE_DIR)/s", "<@(shader_files)"]
        }
      ],
      "include_dirs": [
        "<(INTERMEDIATE_DIR)/s"
      ],
      "conditions": [
        ["OS=='linux'", {
          "cflags": ["-Wall"]
        }],
        ["OS=='win'", {
          "defines": ["NOMINMAX"],
          "copies": [
            {
              "destination": "<(PRODUCT_DIR)",
              "files": [
                "dependencies/freeglut/bin/$(PlatformName)/freeglut.dll",
                "dependencies/glew/bin/Release/$(PlatformName)/glew32.dll"
              ]
            }
          ],
          "include_dirs": [
            "dependencies/freeglut/include",
            "dependencies/glew/include"
          ],
          "link_settings": {
            "libraries": [
              "-lfreeglut",
              "-lglew32",
              "-lglew32s",
              "-lopengl32",
              "-lglu32"
            ]
          }
        }, {
          "link_settings": {
            "libraries": [
              "-lm",
              "-lGL",
              "-lglut",
              "-lGLEW"
            ]
          }
        }]
      ]
    }
  ]
}
