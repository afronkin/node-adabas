{
  "target_defaults": {
    "default_configuration": "Release",
    "configurations": {
      "Debug": {
        "defines": ["DEBUG", "_DEBUG"],
        "cflags": ["-g", "-O0"],
        "msvs_settings": {
          "VCCLCompilerTool": {
            "Optimization": 0, # /Od, no optimization
            "MinimalRebuild": "false",
            "OmitFramePointers": "false",
            "BasicRuntimeChecks": 3, # /RTC1

            "StringPooling": "true", # pool string literals
            "DebugInformationFormat": 3, # Generate a PDB
            "WarningLevel": 3,
            "BufferSecurityCheck": "true",
            "ExceptionHandling": 1, # /EHsc
            "RuntimeLibrary": "2", # /MD
            "RuntimeTypeInfo": "true", # /GR
            "SuppressStartupBanner": "true",
            "WarnAsError": "false",
            "DisableSpecificWarnings": ["4345", "4506"]
          },
          "VCLinkerTool": {
            "LinkIncremental": 2, # enable incremental linking

            "GenerateDebugInformation": "true",
            "RandomizedBaseAddress": 2, # enable ASLR
            "DataExecutionPrevention": 2, # enable DEP
            "AllowIsolation": "true",
            "SuppressStartupBanner": "true",
            "target_conditions": [
              ["_type==\"executable\"",
                {
                  "SubSystem": 1 # console executable
                }
              ]
            ]
          }
        }
      },
      "Release": {
        "defines": ["NDEBUG"],
        "cflags": [
          "-O3",
          "-fstrict-aliasing",
          "-fomit-frame-pointer",
          "-fdata-sections",
          "-ffunction-sections"
        ],
        "msvs_settings": {
          "VCCLCompilerTool": {
            "Optimization": 3, # /Ox, full optimization
            "FavorSizeOrSpeed": 1, # /Ot, favour speed over size
            "InlineFunctionExpansion": 2, # /Ob2, inline anything eligible
            "WholeProgramOptimization": "true", # /GL, whole program optimization, needed for LTCG
            "OmitFramePointers": "true",
            "EnableFunctionLevelLinking": "true",
            "EnableIntrinsicFunctions": "true",

            "StringPooling": "true", # pool string literals
            "DebugInformationFormat": 3, # Generate a PDB
            "WarningLevel": 3,
            "BufferSecurityCheck": "true",
            "ExceptionHandling": 1, # /EHsc
            "RuntimeLibrary": "2", # /MD
            "RuntimeTypeInfo": "true", # /GR
            "SuppressStartupBanner": "true",
            "WarnAsError": "false",
            "DisableSpecificWarnings": ["4345", "4506"]
          },
          "VCLibrarianTool": {
            "AdditionalOptions": [
              "/LTCG" # link time code generation
            ]
          },
          "VCLinkerTool": {
            "LinkTimeCodeGeneration": 1, # link-time code generation
            "OptimizeReferences": 2, # /OPT:REF
            "EnableCOMDATFolding": 2, # /OPT:ICF
            "LinkIncremental": 1, # disable incremental linking

            "GenerateDebugInformation": "true",
            "RandomizedBaseAddress": 2, # enable ASLR
            "DataExecutionPrevention": 2, # enable DEP
            "AllowIsolation": "true",
            "SuppressStartupBanner": "true",
            "target_conditions": [
              ["_type==\"executable\"",
                {
                  "SubSystem": 1 # console executable
                }
              ]
            ]
          }
        }
      }
    }
  }
}
