var adabas = require('adabas');
var async = require('async');
var fs = require('fs');
var util = require('util');

var query = new adabas.Command();
var outputFile = null;
var recNo = 0;
var startTime, currentTime, previousTime;

function readRecord(callback) {
	recNo++;

	currentTime = new Date().getTime();
	if (currentTime - previousTime >= 1000) {
		process.stderr.write('\rRecord: ' + recNo);
		previousTime = currentTime;
	}

	query.exec(function(error) {
		if (error) {
			if (query.get(adabas.RETURN_CODE) === adabas.ADA_EOF) {
				return callback();
			}
			return callback(error);
		}

		fs.write(outputFile, query.recordBuffer, 0, 10);
		fs.write(outputFile, '\n');

		process.nextTick(function() {
			readRecord(callback);
		});
	});
}

startTime = new Date().getTime();
previousTime = startTime;

async.series([
	function openDatabase(callback) {
		query.clear();
		query.set(adabas.COMMAND_CODE, 'OP');
		query.set(adabas.DB_ID, 88);
		query.recordBuffer = new Buffer('UPD=12.');
		query.set(adabas.RECORD_BUFFER, query.recordBuffer);
		query.set(adabas.RECORD_BUFFER_LENGTH, query.recordBuffer.length);

		query.exec(function(error) {
			return callback(error);
		});
	},

	function openOutputFile(callback) {
		fs.open('test.dat', 'w', 0666, function(error, file) {
			outputFile = file;
			return callback(error);;
		});
	},

	function readRecords(callback) {
		query.clear();
		query.set(adabas.COMMAND_CODE, 'L2');
		query.set(adabas.COMMAND_ID, 'EXPT');
		query.set(adabas.DB_ID, 88);
		query.set(adabas.FILE_NO, 12);

		query.formatBuffer = new Buffer('AW,10,A.');
		query.set(adabas.FORMAT_BUFFER, query.formatBuffer);
		query.set(adabas.FORMAT_BUFFER_LENGTH, query.formatBuffer.length);

		query.recordBuffer = new Buffer(250);
		query.set(adabas.RECORD_BUFFER, query.recordBuffer);
		query.set(adabas.RECORD_BUFFER_LENGTH, query.recordBuffer.length);

		readRecord(callback);
	}],

	function final(error) {
		if (error) {
			util.error(error.toString());
		}

		if (outputFile) {
			fs.close(outputFile, function(error) {
				if (error) {
					util.error(error.toString());
				}
				outputFile = null;
			});
		}

		query.clear();
		query.set(adabas.COMMAND_CODE, 'CL');
		query.set(adabas.DB_ID, 88);
		query.exec(function(error) {
				if (error) {
					util.error(error.toString());
				}
		});

		currentTime = new Date().getTime();
		var usedTime = (currentTime - startTime) / 1000;
		var usedHours = Math.floor(usedTime / 3600);
		var usedMinutes = Math.floor((usedTime - (usedHours * 3600)) / 60);
		var usedSeconds = Math.floor(usedTime - (usedHours * 3600) - (usedMinutes * 60));

		process.stderr.write('\r');
		process.stderr.write('Readed records: ' + (recNo - 1) + '\n');
		process.stderr.write('Done in ' + usedHours
			+ ':' + String("0" + usedMinutes).slice(-2)
			+ ':' + String("0" + usedSeconds).slice(-2) + '.\n');
	});

/*
query.on('exec', function(error) {
	console.log('Event received [' +  error + ']');
});
*/
