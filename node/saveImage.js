/*
    Based on the example found in https://root.cern/js/latest/api.htm#node_js_geometry_svg_src
*/

let jsroot = require("jsroot");
let fs = require("fs");

console.log('JSROOT version', jsroot.version);

// one could specify rendering engine via options
// r3d_img is normal webgl, create svg:image (default)
// r3d_svg uses SVGRenderer, can produce large output

// http://localhost:8090/UpdateGeometryFile/cmd.json?arg1=test/BabyIAXO.root
// http://localhost:8090/EnableSingleGeometryName/cmd.json
// useful options ->  const projOptions = ["", "projx", "projy", "projz"]

const geometryName = "Geometry"
const projOpt = "projz"

jsroot.httpRequest(`http://localhost:8090/Objects/${geometryName}/root.json.gz`, 'object')
    .then(obj => jsroot.makeSVG({ object: obj, width: 2000, height: 2000, option: `${projOpt}` /*, option: "r3d_svg" */ }))
    .then(svg => { fs.writeFileSync(`${geometryName}_${projOpt}.svg`, svg); console.log(`Create SVG file with size ${svg.length}`); });
