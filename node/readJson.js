let jsroot = require("jsroot");
let geom = jsroot.require('geom')
let fs = require("fs");
var zlib = require('zlibjs');

console.log('JSROOT version', jsroot.version);

// http://localhost:8090/Objects/Geometry/root.json.gz

const filename = "../test/geometry/BabyIAXO.json.gz";

fs.readFile(filename, function (err, data) {
    zlib.gunzip(data, function (err, dezipped) {
        if (err) {
            console.log(err);
        } else {
            console.log('length of data = ' + data.length);
            console.log('length of dezipped =' + dezipped.length);
            console.log(dezipped.toString("utf-8"));
        }
    });
});
