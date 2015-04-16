var assert = require('assert');

try {
  var adabas = require('adabas');
} catch (err) {
  var adabas = require('..');
}

var db = new adabas.Adabas();
var query = new adabas.Command();

var recordBuffer = new Buffer('UPD=12.');
query
  .clear()
  .setCommandCode('OP')
  .setDbId(88)
  .setRecordBufferLength(recordBuffer.length)
  .setRecordBuffer(recordBuffer);
var rc = db.exec(query);
assert(rc === adabas.ADA_SUCCESS);

var formatBuffer = new Buffer('AO,250,A.');
var recordBuffer = new Buffer(250);
query
  .clear()
  .setCommandCode('L2')
  .setCommandId('EXPT')
  .setDbId(88)
  .setFileNo(12)
  .setFormatBufferLength(formatBuffer.length)
  .setFormatBuffer(formatBuffer)
  .setRecordBufferLength(recordBuffer.length)
  .setRecordBuffer(recordBuffer)

for (var recNo = 1; db.exec(query) === adabas.ADA_SUCCESS; recNo++) {
  if (query.getReturnCode() !== adabas.ADA_NORMAL) {
    assert(query.getReturnCode() === adabas.ADA_EOF);
    break;
  }
}

console.error('Readed records: %d', recNo - 1);

query
  .clear()
  .setCommandCode('CL')
  .setDbId(88);
var rc = db.exec(query);
assert(rc === adabas.ADA_SUCCESS);

db.close();
