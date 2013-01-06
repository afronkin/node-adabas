{
  'targets': [
    {
      'target_name': 'node_adabas',
      'sources': [
        '../src/node_adabas.cxx',
        '../src/command.cxx',
        '../src/command_init.cxx'
      ],
      'cflags_cc': ['-I$(ADABIN)/inc'],
      'link_settings': {
        'libraries': [
          '-ladalnkx'
        ],
        'ldflags': [
          '-L$(ADABIN)/lib',
          '-Wl,-rpath=$(ADABIN)/lib'
        ]
      }
    }
  ]
}
