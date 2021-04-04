/*
    Based on the example found in https://root.cern/js/latest/api.htm#node_js_geometry_svg_src
*/

let jsroot = require("jsroot");
let fs = require("fs");
var zlib = require('zlibjs');

console.log('JSROOT version', jsroot.version);

// one could specify rendering engine via options
// r3d_img is normal webgl, create svg:image (default)
// r3d_svg uses SVGRenderer, can produce large output

// http://localhost:8090/UpdateGeometryFile/cmd.json?arg1=test/BabyIAXO.root
// http://localhost:8090/EnableSingleGeometryName/cmd.json
// http://localhost:8090/Objects/Geometry/root.json.gz
// useful options ->  const projOptions = ["", "projx", "projy", "projz"]

const projOpt = "projz"

const filename = "../test/geometry/BabyIAXO.json.gz";
// https://root.cern/js/files/geom/simple_alice.json.gz

fs.readFile(filename, function (err, data) {
    zlib.gunzip(data, function (err, dezipped) {
        if (err) {
            console.log(err);
        } else {
            console.log('length of data = ' + data.length);
            console.log('length of dezipped =' + dezipped.length);
            let json = dezipped.toString("utf-8");
            console.log("making SVG");
            jsroot.makeSVG({ object: jsroot.parse(json), width: 2000, height: 2000, option: projOpt })
                .then(svg => {
                    fs.writeFileSync("test.svg", svg);
                });
        }
    });
});


