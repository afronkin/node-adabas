var assert = require('assert');

try {
  var adabas = require('adabas');
} catch (err) {
  var adabas = require('..');
}

var db = new adabas.Adabas();
var query = new adabas.Command();

query
  .clear()
  .setCommandCode('OP')
  .setDbId(88);
db.exec(query, function(rc) {
  assert(rc === adabas.ADA_SUCCESS);

  var searchBuffer = new Buffer('AW,6,A.');
  var valueBuffer = new Buffer('READER');
  query
    .clear()
    .setCommandCode('S1')
    .setDbId(88)
    .setFileNo(12)
    .setSearchBufferLength(searchBuffer.length)
    .setSearchBuffer(searchBuffer)
    .setValueBufferLength(valueBuffer.length)
    .setValueBuffer(valueBuffer)
  db.exec(query, function(rc) {
    assert(rc === adabas.ADA_SUCCESS);

    console.error('Found records: %d', query.getIsnQuantity());

    query
      .clear()
      .setCommandCode('CL')
      .setDbId(88);
    db.exec(query, function(rc) {
      assert(rc === adabas.ADA_SUCCESS);

      db.close();
    });
  });
});
