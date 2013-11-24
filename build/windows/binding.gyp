{
  'targets': [
    {
      'target_name': 'node_adabas',
      'sources': [
        '../../src/node_adabas.cxx',
        '../../src/command.cxx',
        '../../src/command_init.cxx'
      ],
      'include_dirs': [
        '$(ACLSDK)/inc'
      ],
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ],
      'conditions': [
        ['OS=="win"', {
          'msvs_settings': {
            'VCCLCompilerTool': {
              'AdditionalOptions': [ '/EHsc' ]
            }
          },
          'conditions': [
            ['target_arch=="ia32"', {
              'libraries': [
                '$(ACLSDK)/lib32/adalnkx.lib'
              ]
            }],
            ['target_arch=="x64"', {
              'libraries': [
                '$(ACLSDK)/lib/adalnkx.lib'
              ]
            }]
          ]
        }]
      ]
    }
  ]
}
