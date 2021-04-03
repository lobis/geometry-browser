# ROOT Geometry Browser

[![CI](https://github.com/lobis/geometry-browser/actions/workflows/ci.yml/badge.svg)](https://github.com/lobis/geometry-browser/actions/workflows/ci.yml)

A simple `TGeo` ([root](https://root.cern.ch/doc/master/classTGeoManager.html)) / [`GDML`](https://root.cern.ch/doc/v614/group__Geometry__gdml.html) geometry web browser using ROOT's [`THttpServer`](https://root.cern.ch/root/htmldoc/guides/HttpServer/HttpServer.html).

# Requirements 

Only [ROOT](https://root.cern/), the minimal version should work. It has been tested on ROOT `v6.22.08`. If you don't have ROOT installed, you can also run it in docker using the [official ROOT image](https://hub.docker.com/r/rootproject/root)

# Usage

To start the server, just run the ROOT macro.

```
root GeometryBrowser.C
```

You can access the server in http://localhost:8090. If you want to use a different port, change the `PORT` variable in the script.

The default browser for `THttpServer` will open.

[](https://user-images.githubusercontent.com/35803280/113483324-20743980-94a3-11eb-89af-66d85cb0d798.PNG)
