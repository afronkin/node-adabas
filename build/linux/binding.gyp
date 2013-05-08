{
  'targets': [
    {
      'target_name': 'node_adabas',
      'sources': [
        '../../src/node_adabas.cxx',
        '../../src/command.cxx',
        '../../src/command_init.cxx'
      ],
      'cflags_cc': ['-I$(ACLDIR)/$(ACLVERS)/inc'],
      'link_settings': {
        'libraries': [
          '$(ACLDIR)/$(ACLVERS)/lib/adabasx.o'
        ]
      }
    }
  ]
}
