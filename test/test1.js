try {
  var adabas = require('adabas');
} catch (err) {
  var adabas = require('..');
}

console.log("[1] Create Adabas command.");
var query = new adabas.Command();
console.log(query.toString());

console.log("[2] Set fields of Adabas control block.");
query.setCommandCode('L1');
query.setCommandId('TEST');
query.setDbId(88);
query.setFileNo(12);
query.setReturnCode(1);
query.setIsn(10);
query.setIsnLowerLimit(20);
query.setIsnQuantity(30);
query.setFormatBufferLength(1000);
query.setRecordBufferLength(2000);
query.setSearchBufferLength(3000);
query.setValueBufferLength(4000);
query.setIsnBufferLength(5000);
query.setCommandOption1(65);
query.setCommandOption2(66);
query.setAddition1([1, 1, 1, 1, 1, 1, 1, 1]);
query.setAddition2([1, 2, 3, 4]);
query.setAddition3([0, 1, 2, 3, 4, 5, 6, 7]);
query.setAddition4([7, 6, 5, 4, 3, 2, 1, 0]);
query.setAddition5([10, 20, 30, 40, 50, 60, 70, 80]);
query.setCommandTime(12345);
query.setUserArea([1, 2, 3, 4]);
console.log(query.toString());

console.log("[3] Get fields of Adabas control block.");
console.log("Command Code     : %s", query.getCommandCode());
console.log("Command Id       : %s", query.getCommandId());
console.log("File Number      : %d", query.getFileNo());
console.log("Database Id      : %d", query.getDbId());
console.log("Response Code    : %d", query.getReturnCode());
console.log("Isn              : %d", query.getIsn());
console.log("Isn Lower Limit  : %d", query.getIsnLowerLimit());
console.log("Isn Quantity     : %d", query.getIsnQuantity());
console.log("FB Length        : %d", query.getFormatBufferLength());
console.log("RB Length        : %d", query.getRecordBufferLength());
console.log("SB Length        : %d", query.getSearchBufferLength());
console.log("VB Length        : %d", query.getValueBufferLength());
console.log("IB Length        : %d", query.getIsnBufferLength());
console.log("Command Option 1 : %s", query.getCommandOption1());
console.log("Command Option 2 : %s", query.getCommandOption2());
console.log("Additions 1      : %s", query.getAddition1());
console.log("Additions 2      : %s", query.getAddition2());
console.log("Additions 3      : %s", query.getAddition3());
console.log("Additions 4      : %s", query.getAddition4());
console.log("Additions 5      : %s", query.getAddition5());
console.log("Command Time     : %s", query.getCommandTime());
console.log("User Area        : %s", query.getUserArea());
