var adabas = module.exports = exports = require('node_adabas.node');
var EventEmitter = require('events').EventEmitter;

inherits(adabas.Command, EventEmitter);

function inherits(target, source) {
  for (var k in source.prototype)
    target.prototype[k] = source.prototype[k];
}
