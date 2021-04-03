# ROOT Geometry Browser

[![CI](https://github.com/lobis/geometry-browser/actions/workflows/ci.yml/badge.svg)](https://github.com/lobis/geometry-browser/actions/workflows/ci.yml)

A simple `TGeo` ([root](https://root.cern.ch/doc/master/classTGeoManager.html)) / [`GDML`](https://root.cern.ch/doc/v614/group__Geometry__gdml.html) geometry web browser using ROOT's [`THttpServer`](https://root.cern.ch/root/htmldoc/guides/HttpServer/HttpServer.html).

![GeometryBrowserScreenshot](https://user-images.githubusercontent.com/35803280/113485837-e65d6480-94af-11eb-98cd-83ffa62ca45b.PNG)

# Requirements 

Only [ROOT](https://root.cern/), the minimal version should work. It has been tested on ROOT `v6.22.08`. If you don't have ROOT installed, you can also run it in docker using the [official ROOT image](https://hub.docker.com/r/rootproject/root)

# Usage

To start the server, just run the ROOT macro.

```
root GeometryBrowser.C
```

You can access the server in http://localhost:8090. If you want to use a different port, change the `PORT` variable in the script, or define an environment variable named `GEOMETRYBROWSER_PORT` to be equal to the port you want to use.

The default browser for `THttpServer` will open.

![Geometry Browser On Startup](https://user-images.githubusercontent.com/35803280/113483324-20743980-94a3-11eb-89af-66d85cb0d798.PNG)

To open a new geometry, click on the `UpdateGeometryFile` icon (open folder). A prompt will open asking you for the file path. You can use relative or absolute path in your file system.

After selecting the file to view its necessary to click the `reload` button for the changes to be visible. If the file was valid (valid GDML or root file with `TGeoManager`) there should be a new entry under the "Objects" directory with the name of your file. If the file was not valid, an empty object will appear.

![usage](https://user-images.githubusercontent.com/35803280/113485440-bf059800-94ad-11eb-90e9-16b478ba7b94.gif)
