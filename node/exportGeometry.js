let jsroot = require("jsroot");
let fs = require("fs");
var zlib = require("zlib");

console.log("JSROOT version", jsroot.version);

const { Blob, FileReader } = require("vblob");
const THREE = require("three");
const Canvas = require("canvas");


function exportGeometry(obj) {

    let opt = { numfaces: 100000, numnodes: 1000, wireframe: false };
    if (d.has("all")) {
        opt.numfaces *= 100;
        opt.numnodes *= 100;
    }

    if (d.has("dflt") || d.has("dflt_colors"))
        opt.dflt_colors = true;

    let volumes = obj.fMasterVolume.fNodes.arr
    let top_volumes_names = []
    for (var i in volumes) {
        const name = volumes[i].fName
        top_volumes_names.push(name)
        console.log(i, name)
    }

    let obj3d = jsroot.GEO.build(obj, opt);

    if (!obj3d) return;

    let data = obj3d.toJSON();

    data["info"] = []
    // add additional info to json
    for (var i in top_volumes_names) {
        const name = top_volumes_names[i]
        data["info"].push(top_volumes_names[i])
    }

    const outputFileName = "geometry.three.json";

    fs.writeFileSync(outputFileName, JSON.stringify(data));
    // save also compressed
    out = fs.createWriteStream(outputFileName + ".gz");
    var gzip = zlib.createGzip();
    const input = fs.createReadStream(outputFileName);
    input.pipe(gzip).pipe(out);
    console.log("data saved to: " + outputFileName + " and " + outputFileName + ".gz");


    console.log("done!")

    // export to gltf

    global.window = global;
    global.Blob = Blob;
    global.FileReader = FileReader;
    global.THREE = THREE;
    global.document = {
        createElement: (nodeName) => {
            if (nodeName !== "canvas") throw new Error(`Cannot create node ${nodeName}`);
            const canvas = new Canvas(256, 256);
            return canvas;
        }
    };

    // https://github.com/mrdoob/three.js/issues/9562
    require("three/examples/js/exporters/GLTFExporter");

    const exporter = new THREE.GLTFExporter();
    exporter.parse(obj3d, (content) => {

        console.log(`Writing GLTF`);
        if (typeof content === "object") content = JSON.stringify(content);
        const filename = outputFileName + ".gltf"
        fs.writeFileSync(filename, content);

    });

}

function main() {
    d = jsroot.decodeUrl();
    {
        let filename = "../test/geometry/BabyIAXO.root"
        if (filename.indexOf(".root") > 0) {
            // it is a ROOT file, must include a "geo" key with the geometry (TGeoManager)
            let itemname = "geo";
            jsroot.openFile(filename)
                .then(file => file.readObject(itemname))
                .then(exportGeometry);
        }
    }
}


jsroot.require("geom").then(main);
