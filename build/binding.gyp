{
  "variables": {
      "aclsdk": "$(ACLSDK)",
      "module_name": "adabas",
  },
  "includes": ["./common.gypi"],
  "targets": [
    {
      "target_name": "adabas",
      "sources": [
        "../src/adabas.cxx",
        "../src/command.cxx",
        "../src/node_adabas.cxx"
      ],
      "conditions": [
        ["OS=='win'", {
          "include_dirs": [
            "<(aclsdk)/inc"
          ],
          "cflags_cc!": ["-fno-rtti", "-fno-exceptions"],
          "conditions": [
            ["target_arch=='ia32'", {
              "defines": [
                "_CRT_SECURE_NO_DEPRECATE",
                "_SCL_SECURE_NO_WARNINGS",
                "_HAS_ITERATOR_DEBUGGING=0",
                "_SECURE_SCL=0",
                "PLATFORM=\"win32\""
              ],
              "msvs_settings": {
                "VCLinkerTool": {
                  "AdditionalLibraryDirectories": [
                    "<(aclsdk)/lib"
                  ],
                }
              },
            }],
            ["target_arch=='x64'", {
              "defines": [
                "_CRT_SECURE_NO_DEPRECATE",
                "_SCL_SECURE_NO_WARNINGS",
                "_HAS_ITERATOR_DEBUGGING=0",
                "_SECURE_SCL=0"
              ],
              "msvs_settings": {
                "VCLinkerTool": {
                  "AdditionalLibraryDirectories": [
                    "<(aclsdk)/lib"
                  ],
                }
              }
            }]
          ],
          "libraries": [
            "adalnkx.lib"
          ]
        }],
        ["OS=='linux'", {
          "include_dirs": [
            "<(aclsdk)/inc"
          ],
          "cflags": [
            "-O3",
            "-fno-operator-names",
            "-fPIC",
            "-std=c++0x",
            "-pthread",
            "-Wno-all",
            "-Wno-extra"
          ],
          "cflags_cc!": ["-fno-rtti", "-fno-exceptions"],
          "ldflags": [
            "-L<(aclsdk)/lib",
            "-shared"
          ],
          "libraries": [
            "-ldl",
            "-ladalnkx"
          ]
        }]
      ]
    }
  ]
}
