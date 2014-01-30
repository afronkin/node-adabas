{
  'targets': [
    {
      'target_name': 'adabas',
      'sources': [
        '../../src/node_adabas.cxx',
        '../../src/command.cxx',
        '../../src/command_init.cxx'
      ],
      'include_dirs': [
        '$(ACLDIR)/$(ACLVERS)/inc'
      ],
      'libraries': [
        '$(ACLDIR)/$(ACLVERS)/lib/adabasx.o'
      ]
    }
  ]
}
